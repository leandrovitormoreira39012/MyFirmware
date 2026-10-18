# Bruce completo — ST7789 no ESP32-S3 N16R8 (cores normais)

Ambiente: `bruce-joker-s3-marauder`. Usa o driver **ST7789**, com **inversão de cores desativada** após o teste físico informado, tela 240×320 em paisagem, touch XPT2046, cartão SD e os sete contatos do JY050. Inclui as traduções PT-BR já implementadas (680 entradas); scripts externos, web e algumas telas operacionais ainda podem ter inglês. O tema Joker Leandro e a abertura estão disponíveis em `sd_files/themes/JokerLeandro`; instale e selecione conforme `LEIA-ME-LEANDRO.md`. Mensagens do OLED ainda não foram implementadas.

A pinagem desta versão é idêntica à do último firmware completo ILI9341. Ela difere dos testes isolados.

Este é o Bruce completo, não o programa isolado de diagnóstico. A compatibilidade física ainda precisa ser confirmada na sua montagem. No teste físico, ILI9341 ficou vertical/cortado e ST7789 ficou horizontal. O diagnóstico deveria mostrar fundo preto/letras brancas, mas o usuário observou o negativo. Por isso esta versão seleciona ST7789 e envia INVOFF (0x20). A correção das cores ainda precisa de confirmação visual. O nome do ambiente foi mantido por compatibilidade com os comandos anteriores.

## O que mudou em relação aos arquivos enviados

- ESP32 clássico de 4 MB → ESP32-S3, flash de 16 MB e PSRAM Octal de 8 MB.
- Pinos remapeados para os disponíveis no S3 N16R8, evitando USB, PSRAM e pinos de inicialização.
- Tela, touch, SD e rádios usam **o mesmo SPI de hardware**, com CS separados e a mesma instância de software. A tela inicia esse barramento. O touch é consultado na tarefa chamadora, sem tarefa paralela de leitura.
- LED no GPIO11, reset no GPIO12; iluminação liga no boot. Como no Marauder enviado, brilho acima de 5 liga e até 5 desliga: não há ajuste gradual por PWM.
- Inversão desativada na inicialização da biblioteca e reafirmada após carregar configurações antigas. O perfil anterior usava INVON (0x21).
- JY050 usa contatos com pull-ups internos. O bloco `WAVESENTRY` do arquivo enviado é para encoder rotativo e não corresponde ao módulo fotografado.
- Comunicação SPI da tela em 20 MHz, mais conservadora que os 40 MHz do perfil original; isso é independente dos 80 MHz da flash.

## Ligação — use somente esta tabela para este binário

Desligue antes de alterar os fios. Os números são GPIO, não a posição do terminal. **Esta ligação difere tanto do firmware inicial quanto dos dois testes isolados.** Retire qualquer ligação RESET → 3V3 antes de ligar RESET → GPIO12.

Comece apenas com ESP32, tela, touch e JY050, alimentando a placa por USB. Mantenha GND comum e alimentação VCC compatível com o módulo de tela que já funcionou. Não aplique 5 V aos sinais do ESP32.

LED → GPIO11 pressupõe o mesmo módulo que você já usou com controle de LED por GPIO no Marauder, com seu circuito de acionamento/limitação. Não é ligação para LEDs avulsos sem limitação de corrente; o firmware não limita a corrente da carga.

### Tela, touch e leitor SD

| Terminal do módulo | Conexão |
| --- | --- |
| VCC | Alimentação compatível com o módulo |
| GND | GND comum |
| LED | GPIO11 |
| RESET | GPIO12 |
| CS da tela | GPIO10 |
| DC | GPIO9 |
| SCK da tela + T_CLK + SD_SCK | GPIO6 |
| SDI/MOSI da tela + T_DIN + SD_MOSI | GPIO7 |
| SDO/MISO da tela + T_DO + SD_MISO | GPIO5 |
| T_CS | GPIO4 |
| SD_CS | GPIO8 |
| T_IRQ | Sem ligação; não conectar ao EN |

Unir SCK/MOSI/MISO significa ligar cada sinal ao GPIO comum correspondente, incluindo os terminais separados do leitor SD. **Não una os CS.** O compartilhamento de MISO requer que os módulos liberem a linha quando seu CS estiver alto. Se algum módulo impedir isso, valide-o isoladamente antes de conectar todos.

### JY050

| Terminal | GPIO / função |
| --- | --- |
| COM | GND |
| UP | 38 / cima |
| DWN | 39 / baixo |
| LET | 40 / esquerda, anterior |
| RHT | 41 / direita, próximo |
| MID | 42 / confirmar |
| SET | 13 / confirmar alternativo |
| RST | 47 / voltar; não é reset do ESP32 |

Não precisa de resistores externos de 10 kΩ para esses contatos. Confira com continuidade, desligado, que cada botão fecha seu contato com COM. O primeiro acionamento desperta a tela se ela estiver em repouso; o seguinte navega. O touch também continua disponível.

### Demais módulos — conectar depois do primeiro teste

| Sinal | GPIO |
| --- | --- |
| CC1101 e NRF24: SCK / MOSI / MISO | 6 / 7 / 5 |
| CC1101: CSN / GDO0 | 14 / 15 |
| CC1101: GDO2 | Sem ligação |
| NRF24: CSN / CE | 16 / 48 |
| NRF24: IRQ | Sem ligação |
| PN532 (modo I²C): SDA / SCL | 1 / 2 |
| OLED: SDA / SCK (I²C), reserva sem driver ativo | 1 / 2 |
| GPS TX → ESP RX | 17 |
| GPS RX ← ESP TX | 43 |
| IR emissor: sinal | 18 |
| IR receptor: sinal | 21 |

Os pinos dos periféricos e do JY050 são os mesmos da configuração completa inicial. Confira a alimentação própria de cada módulo antes de conectar. GPIO48 também pode chegar ao LED RGB integrado; não habilite o driver desse LED. USB usa GPIO19/20; 35/36/37 são reservados à PSRAM. Logs usam USB nativo, deixando 43 para GPS. O voltímetro e a chave de alimentação permanecem independentes do firmware; não há medição de bateria pelo ESP.

## Gravação e teste

Arquivo de entrega: `firmware/BRUCE_COMPLETO_JOKER_LEANDRO.bin`, já mesclado com bootloader e partições.

- Flash Download Tool: **ESP32-S3, DIO, 80 MHz, flash 16 MB, endereço 0x0, velocidade 115200**.
- Não misture outros binários/endereços com este arquivo. Não é necessário apagar toda a flash como primeiro passo.
- Reinicie e confira iluminação, imagem em paisagem e cores normais (preto não deve aparecer branco e vice-versa; vermelho não deve aparecer ciano).
- Teste os sete contatos do JY050, toque nos quatro cantos e leitura de cartão inserido antes de ligar. A calibração do touch é inicial e pode precisar de ajuste.
- Pelo USB nativo, log a 115200 deve informar `[PSRAM] init=1 found=1` e aproximadamente 8 MB. Se ficar apagada, meça LED/GPIO11 para GND e VCC para GND e informe o log.
- Adicione SD e depois os demais módulos um por vez; a compilação não valida consumo, comunicação elétrica nem funcionamento dos rádios.

Não volte aos binários antigos sem restaurar a tabela correspondente. Para os testes isolados anteriores, LED era 13, RESET 14 e SDO 18: **essa não é a ligação deste firmware completo**.

## Fontes e compilação

O pacote completo já contém os fontes alterados; não aplique patches anteriores sobre ele. Extraia em uma pasta nova e abra a raiz que contém `platformio.ini`. Consulte `LEIA-ME-LEANDRO.md` para instalar o tema no SD.

```sh
pio run -e bruce-joker-s3-marauder
```

O projeto requer a ferramenta Python/PlatformIO compatível com sua plataforma fixada (neste ambiente, Python 3.13 e PlatformIO 6.2). O resultado mesclado é `Bruce-bruce-joker-s3-marauder.bin`, na raiz. O workflow Manual Build também inclui o ambiente `bruce-joker-s3-marauder`; publicação e execução remota não foram feitas nesta etapa.
