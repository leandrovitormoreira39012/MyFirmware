"""Host regression checks: python3 tests/joker_theme/run.py (requires g++)."""
from pathlib import Path
import json
import re
import struct
import subprocess
import tempfile

repo = Path(__file__).resolve().parents[2]
source = (repo / 'src/main.cpp').read_text()
start = source.index('#if defined(BRUCE_JOKER_MARAUDER) && !defined(LITE_VERSION)', source.index('**  Function: boot_screen_anim'))
end = source.index('\nvoid boot_screen_anim()', start)
helper = source[start:end]
route_start = source.index('void boot_screen_anim() {', end) + len('void boot_screen_anim() {')
route_end = source.index('    boot_screen();', route_start)
route = source[route_start:route_end]
mocks = r'''
#include <cassert>
#include <algorithm>
#include <cctype>
#include <string>
struct String : std::string {
    using std::string::string;
    String(std::string v) : std::string(v) {}
    int lastIndexOf(char c) const { auto p=rfind(c); return p==npos ? -1 : int(p); }
    String substring(int n) const { return n<0 ? String("") : String(substr(n)); }
    void toLowerCase() { std::transform(begin(),end(),begin(),[](unsigned char c){return std::tolower(c);}); }
};
struct FS {} sd;
struct Paths { String boot_img=""; };
struct Theme { bool boot_img=false; Paths paths; };
struct Config { Theme theme; int bgColor=0; FS *themeFS(){return &sd;} } bruceConfig;
int fills=0,calls=0,fallback=0;
bool imageOk=true;
struct TFT {void fillScreen(int color){assert(color==0);++fills;}} tft;
bool drawImg(FS &fs,const String &path,int x,int y,bool center,int duration) {
    ++calls; assert(&fs==&sd); assert(path==bruceConfig.theme.paths.boot_img);
    assert(x==0 && y==0 && !center && duration==0); return imageOk;
}
'''
checks = r'''
int main(){
    bootRoute();assert(fallback==1 && calls==0);
    bruceConfig.theme.boot_img=true;
    bruceConfig.theme.paths.boot_img="/themes/JokerLeandro/boot.png";
    bootRoute();assert(fallback==2 && calls==0);
    bruceConfig.theme.paths.boot_img="/themes/JokerLeandro/boot.GIF";
    imageOk=false;
    bootRoute();
#if defined(BRUCE_JOKER_MARAUDER) && !defined(LITE_VERSION)
    assert(calls==1 && fallback==3 && fills==1);
#else
    assert(calls==0 && fallback==3 && fills==0);
#endif
    imageOk=true;
    bootRoute();
#if defined(BRUCE_JOKER_MARAUDER) && !defined(LITE_VERSION)
    assert(calls==2 && fallback==3 && fills==2);
#else
    assert(calls==0 && fallback==4 && fills==0);
#endif
}
'''
with tempfile.TemporaryDirectory(prefix='joker-boot-test-') as tmp:
    path=Path(tmp)/'test.cpp'
    path.write_text(mocks+helper+'\nvoid bootRoute(){'+route+'\n++fallback;\n}\n'+checks)
    for name,flags in [('joker',['-DBRUCE_JOKER_MARAUDER']),('legacy',[]),('lite',['-DBRUCE_JOKER_MARAUDER','-DLITE_VERSION'])]:
        out=Path(tmp)/name
        subprocess.run(['g++','-std=c++17',*flags,str(path),'-o',str(out)],check=True)
        subprocess.run([str(out)],check=True)
print('PASS: actual boot helper/routing, one-pass full LCD, missing/non-GIF fallback, legacy/LITE isolation')

folder=repo/'sd_files/themes/JokerLeandro'
config=json.loads((folder/'theme.json').read_text())
keys=set(re.findall(r'\{"(\w+)"\s*,\s*&theme\.',(repo/'src/core/theme.cpp').read_text()))
assets={k:v for k,v in config.items() if k in keys}
assert len(assets)==17 and assets['boot_img']=='boot.gif'
for key,name in assets.items():
    assert '/' not in name and '..' not in name
    data=(folder/name).read_bytes()
    if key!='boot_img':
        assert data[:8]==b'\x89PNG\r\n\x1a\n'
        assert struct.unpack('>II',data[16:24])==(306,186)
        assert 64<len(data)<500*1024
for k in ['priColor','secColor','bgColor']:assert 0<=int(config[k],16)<=65535
# Parse the GIF container: do not mistake compressed image bytes for control blocks.
gif=(folder/'boot.gif').read_bytes()
assert gif[:6] in (b'GIF87a',b'GIF89a') and struct.unpack_from('<HH',gif,6)==(320,240)
pos=13
if gif[10]&0x80:pos+=3*(2**((gif[10]&7)+1))
frames=duration=0
def skip_blocks(pos):
    while gif[pos]:pos+=1+gif[pos]
    return pos+1
while gif[pos]!=0x3b:
    tag=gif[pos];pos+=1
    if tag==0x21:
        label=gif[pos];pos+=1
        if label==0xf9:assert gif[pos]==4;duration+=struct.unpack_from('<H',gif,pos+2)[0]*10
        pos=skip_blocks(pos)
    elif tag==0x2c:
        packed=gif[pos+8];pos+=9
        if packed&0x80:pos+=3*(2**((packed&7)+1))
        pos+=1;pos=skip_blocks(pos);frames+=1
    else:raise AssertionError(('Unexpected GIF block',tag,pos))
assert frames==120 and duration==6000
assert config['label'] is False
print('PASS: 16 selectable menu assets + boot path, PNG dimensions, GIF120 frames/6000ms, colors and names')
