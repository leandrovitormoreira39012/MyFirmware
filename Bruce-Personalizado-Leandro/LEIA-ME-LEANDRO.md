# Bruce Personalizado — pacote completo de código + Joker Leandro

Perfil da placa: **ESP32-S3 N16R8, ST7789 240×320 em paisagem, inversão desativada**, touch XPT2046, SD e navegação JY050. Ambiente PlatformIO: `bruce-joker-s3-marauder` (nome histórico; o driver deste ambiente é ST7789).

Este pacote contém o código-fonte completo necessário ao projeto: `src`, `include`, `lib`, `boards`, scripts, configurações PlatformIO, recursos embarcados, testes e arquivos para SD. As personalizações das etapas anteriores já estão aplicadas; **não é um pacote de patches**. Extraia em uma pasta nova para não misturar com versões antigas. A base é a branch `2`, revisão `78a7902c7faeef76f5e18521e75aec01f67656df`, acrescida das alterações locais desta personalização. Não houve publicação automática no GitHub.

## 1. Instalar e selecionar o tema pelo cartão SD

1. Desligue a placa e conecte o cartão ao computador.
2. Dentro deste projeto, abra `sd_files/themes` e copie a pasta **JokerLeandro** para uma pasta **themes** na raiz do cartão.
3. Confirme esta estrutura no cartão:

```text
/themes/JokerLeandro/
    theme.json
    boot.gif
    wifi.png
    ble.png
    rf.png
    rfid.png
    ir.png
    gps.png
    nrf.png
    files.png
    config.png
    ethernet.png
    fm.png
    lora.png
    interpreter.png
    others.png
    clock.png
    connect.png
```

4. Recoloque o SD e ligue o Bruce completo.
5. Abra **Configurações → Tema da interface → Cartão SD**.
6. No seletor de arquivos, entre em **themes → JokerLeandro** e escolha **theme.json**. O tema é escolhido por esse arquivo; não precisa editar o código nem cadastrar um novo item de menu.
7. A seleção é salva pelo Bruce. Reinicie para ver a abertura **Custom by Leandro**.
8. Se a animação não aparecer, desative **Início rápido** nas configurações de inicialização. Esse recurso pula a abertura. Um botão/toque durante a reprodução também pode pulá-la.

O cartão deve permanecer inserido para carregar as imagens. Preserve os nomes dos arquivos e mantenha o JSON junto dos PNGs e do GIF; seus caminhos são relativos. Não selecione cada PNG individualmente. Não é necessário copiar o MP4 para o cartão. A seleção do tema não acontece automaticamente: escolha o JSON uma vez pelo menu.

Para voltar ao visual original, abra o mesmo menu de tema e selecione **Padrão**. A pasta `tmp` que pode aparecer ao lado das imagens é o cache PNG criado pelo próprio Bruce.

## 2. Firmware pronto

O arquivo **`firmware/BRUCE_COMPLETO_JOKER_LEANDRO.bin`**, incluído no pacote de entrega, foi compilado com o código deste ZIP. É o aplicativo completo do Bruce com bootloader e partições, não o programa de diagnóstico.

Flash Download Tool: **ESP32-S3, endereço 0x0, DIO, 80 MHz, flash 16 MB, velocidade 115200**. Depois da gravação, reinicie. Este binário adiciona reprodução da abertura selecionada uma vez, com os tempos do GIF, na tela física de 320×240. O GIF fornecido tem 6 segundos nominais; o tempo real depende da leitura/decodificação do SD. O tempo de inicialização anterior à animação não faz parte desses 6 segundos.

Os binários antigos podem carregar as imagens de menu, mas mantêm o limite anterior da abertura. Para a sequência completa, use o binário deste pacote ou compile os fontes atualizados.

**A pinagem não mudou em relação ao último firmware completo ST7789.** Ela é diferente da ligação dos primeiros testes isolados. Confira [a tabela completa da placa](boards/bruce-joker-s3/MARAUDER.md) antes de energizar: LED11, RESET12, SCK6, MOSI7, MISO5, CS10, DC9; touchCS4 e SDCS8. JY050 continua com sete contatos/pull-ups internos; COM no GND.

## 3. Compilar os fontes

Instale Python 3.10 ou superior e PlatformIO; a compilação deste pacote foi validada com **Python 3.13 e PlatformIO 6.2.0**. A plataforma e as bibliotecas estão declaradas no projeto e serão baixadas na primeira compilação, portanto é necessário acesso à internet.

Na pasta que contém `platformio.ini`:

```sh
python -m pip install platformio==6.2.0
python -m platformio run -e bruce-joker-s3-marauder
```

Se o seu computador usa o comando `python3`, substitua `python` por `python3`. Resultado mesclado na raiz: **Bruce-bruce-joker-s3-marauder.bin**, para gravar em `0x0`. O arquivo `.pio/build/.../firmware.bin` é somente o aplicativo e não substitui o mesclado.

Os cabeçalhos gerados de mquickjs existentes no projeto são reutilizados quando válidos. Se alterações exigirem regeneração, o gerador precisa de compilador C com suporte a 32 bits; consulte a mensagem do script no seu sistema. Não copie `.pio` ou ferramentas do sandbox para o seu computador.

## 4. O que está configurado

- Mapa dedicado de GPIO para tela, touch, SD, CC1101, NRF24, IR, GPS, PN532 e JY050.
- SPI compartilhado com CS separados e uma única instância usada pelos dispositivos.
- Iluminação e reset da tela por GPIO; ST7789 e cores normais.
- 680 entradas da interface em PT-BR, incluindo menus e mensagens já traduzidos. Web, scripts e algumas telas operacionais ainda podem ter inglês.
- Tema Joker Leandro: 16 imagens com interações diferentes, nomes em português, cores RGB565 e abertura GIF já indicada em `theme.json`.
- Abertura específica deste perfil reproduz o GIF selecionado uma vez a partir de(0,0), sem o corte da área útil do menu. Demais perfis seguem o comportamento anterior.

O tema não adiciona funções aos módulos nem garante o funcionamento elétrico. OLED secundário e medição da bateria pelo ESP continuam pendentes; o voltímetro é independente. Compilei/verifiquei em software; SD, cor, touch, abertura e cada módulo ainda precisam ser confirmados na placa.

## 5. Conteúdo e verificações do pacote

`MANIFEST-SHA256.txt` relaciona os arquivos e seus hashes. `CONTEUDO-DO-PACOTE.json` registra a revisão-base e as inclusões/exclusões da entrega. Caches `.pio`, histórico `.git`, ambientes virtuais e credenciais locais não são necessários e não acompanham o ZIP.

Os arquivos grandes da galeria `media/` e dos projetos CAD/Gerber `pcbs/` das placas originais não participam desta compilação e ficam fora deste pacote de código. Seus caminhos são relacionados no manifesto de conteúdo; nenhum arquivo de `src`, `include`, `lib`, `boards` ou recurso embarcado foi omitido. O arquivo LICENSE e os documentos originais estão preservados.
