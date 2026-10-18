# Bruce Personalizado — ESP32-S3 N16R8

> Para a adaptação completa do perfil Marauder com LED/reset por GPIO, use
> [MARAUDER.md](MARAUDER.md). A pinagem abaixo pertence ao perfil anterior
> `bruce-joker-s3`; não misture as tabelas.

Primeira etapa da branch `2`: configuração dedicada `bruce-joker-s3`, tela
ST7789V 240×320 em paisagem (320×240), touch compatível com XPT2046 e JY050.
Esta configuração ainda precisa de teste físico. Não é o tema final nem a
tradução completa para PT-BR.

## O que está incluído

- Flash Quad SPI de 16 MB e PSRAM Octal SPI de 8 MB (`qio_opi`).
- Tela em HSPI separado do barramento FSPI dos periféricos.
- Inversão de cores ativada para a ST7789V confirmada pelo proprietário.
  Aplicada desde a inicialização e reafirmada após ler configurações antigas.
- Touch usando o driver XPT2046 já presente no Bruce, sem fio de interrupção.
  Entradas são consultadas na tarefa chamadora (`BRUCE_INPUT_SYNC`), evitando
  leitura de touch em segundo plano durante transações dos rádios/SD.
- Sete entradas JY050 com pull-ups internos, repetição de navegação e despertar da tela.
- SD e reservas de pinos para CC1101, NRF24L01, PN532, GPS e IR, aproveitando os
  drivers existentes. Esses módulos ainda não foram validados nesta montagem.
- I²C reservado para PN532 e OLED. As mensagens do OLED serão implementadas
  na próxima etapa; não há driver de OLED ativado nesta versão.
- Sem leitura de bateria pelo firmware: o voltímetro externo é independente.

## Antes da montagem

Os números abaixo são **GPIO**, não posições no conector. Confira a serigrafia.
Todos os sinais do ESP32 são de 3,3 V. Faça a primeira montagem sem bateria,
com alimentação USB, e conecte apenas tela + touch + JY050. Não una uma saída
externa de 5 V ao USB sem confirmar o isolamento da sua placa.

Os GPIO 35, 36 e 37 pertencem à PSRAM nesta variante; 19/20 ficam para USB;
0/3/45/46 são evitados por suas funções de inicialização; 44 fica livre para
não receber disputa elétrica do TX do conversor USB-UART da placa. O GPIO48
será usado para CE do NRF24 e também pode chegar ao LED RGB integrado: o driver
desse LED fica desativado e o LED pode mudar de estado durante o uso do rádio. Referência:
[Espressif, datasheet do ESP32-S3-WROOM-1](https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf),
tabelas de variantes e definição dos pinos.

### Tela ST7789V — primeira montagem

| Terminal da tela | ESP32 / conexão |
| --- | --- |
| GND | GND comum |
| VCC | Alimentação conforme especificação do módulo; iniciar com 3V3 e conferir a tensão após eventual regulador da tela |
| SCK | GPIO12 |
| SDI / MOSI | GPIO11 |
| CS | GPIO10 |
| DC | GPIO9 |
| RESET | 3V3; o driver executa reset por comando SPI |
| SDO / MISO da tela | Não conectar |
| LED | **Não deixar solto.** A iluminação exige uma ligação neste terminal; confirmar o circuito antes da alimentação direta. Ver teste abaixo. |

O terminal LED só pode receber alimentação direta se a placa da tela já
possuir limitação de corrente apropriada. Caso seja uma ligação direta aos
LEDs, é necessário resistor/driver dimensionado. Esta versão não oferece
controle físico de brilho; o menu de brilho do Bruce não terá efeito no
backlight. O desligamento por software também não corta a alimentação:
use a chave física.

### Diagnóstico: tela totalmente apagada

T_IRQ e SDO/MISO podem ficar desconectados nesta montagem; **LED não é opcional**.
O firmware não controla esse terminal por GPIO (`TFT_BL=-1`). Se LED ficou solto,
resolva primeiro a iluminação antes de repetir o flash ou mudar o driver.

Teste provisório com limitação de corrente, usando um resistor de 10 kΩ:

1. Desligue o USB antes de mexer nos fios.
2. Ligue **3V3 do ESP32 → resistor de 10 kΩ → LED da tela**; mantenha o GND comum.
3. Religue o USB e observe se surge alguma iluminação, mesmo fraca.

Esse resistor limita a corrente fornecida por essa ligação a no máximo cerca de
0,33 mA com 3,3 V. Se o terminal alimentar os LEDs diretamente, a luz pode ser
muito fraca ou imperceptível: ausência de luz nesse teste não prova defeito.
Se o terminal controlar um transistor, o resultado depende do circuito da placa.
Esta é uma ligação de diagnóstico, não o dimensionamento final do backlight.
Não remova o resistor nem use 5 V no terminal LED sem confirmar o circuito.
Para definir a ligação definitiva, obtenha foto nítida dos componentes e trilhas
próximos a LED, Q1, R5 e R6, ou o esquema da placa exata.

### Controle por GPIO e identificação dos sinais

O resistor de 10 kΩ acima é apenas um teste provisório; não é uma exigência de
software. É possível controlar iluminação e RESET por GPIO, com pinagem e
firmware correspondentes. Para ligar LED diretamente a um GPIO, confirme que
ele aciona um circuito de backlight da própria placa. Se alimentar os LEDs
diretamente, será necessário um estágio de acionamento apropriado.

O mapa atual **não habilita LED nem RESET por GPIO** (`TFT_BL=-1`, `TFT_RST=-1`).
Não conecte esses terminais a pinos escolhidos de outra placa sem atualizar o
mapa. GPIO32 não está exposto no ESP32-S3-WROOM-1; o número usado em firmwares
para outras variantes ESP32 não pode ser copiado para este módulo.

- **SDI/MOSI da tela**: é necessário; neste firmware vai ao GPIO11.
- **SDO/MISO da tela**: não é usado pelo desenho da interface nesta configuração.
- **T_DO do touch** e **SD_MISO do cartão**: são sinais distintos; ambos usam
  GPIO5 quando seus dispositivos estão conectados.
- **T_IRQ**: permanece solto porque o firmware consulta o touch por SPI. Não
  ligar ao **EN** do ESP32: EN é a entrada de habilitação/reset do processador.
- **RESET da tela**: é distinto de T_IRQ e EN. O driver atual envia reset por
  comando SPI; com GPIO dedicado, pode executar também o pulso de reset físico.

Referência do módulo e função de EN:
[Espressif — ESP32-S3-WROOM-1/1U, pinagem](https://www.espressif.com/sites/default/files/documentation/esp32-s3-wroom-1_wroom-1u_datasheet_en.pdf).

### Touch — primeira montagem

| Terminal | GPIO |
| --- | --- |
| T_CLK | 6 |
| T_DIN | 7 |
| T_DO | 5 |
| T_CS | 4 |
| T_IRQ | Não conectar |

A tela foi confirmada pelo proprietário como ST7789V. O touch está configurado
como XPT2046; a identificação inicial “XPT4056” ainda não foi confirmada por
leitura do chip. Se o touch não responder, use o JY050 e confirme o controlador.
A calibração inicial é a do driver CYD do repositório; alinhamento, inversão
dos eixos e cantos precisam ser conferidos na tela real.

### JY050 — primeira montagem

O JY050 da foto é um conjunto de contatos direcionais, não um encoder rotativo.
Com a alimentação desligada, use o multímetro para confirmar continuidade
entre COM e cada contato somente quando o respectivo botão for pressionado.

| Terminal JY050 | Conexão | Função |
| --- | --- | --- |
| COM | GND | Comum |
| UP | GPIO38 | Cima |
| DWN | GPIO39 | Baixo |
| LET | GPIO40 | Esquerda / anterior |
| RHT | GPIO41 | Direita / próximo |
| MID | GPIO42 | Confirmar |
| RST | GPIO47 | Voltar; **não** conectar ao reset do ESP32 |
| SET | GPIO13 | Confirmar alternativo |

Os pull-ups internos dispensam seus resistores de 10 kΩ para esses contatos.
Não alimente COM com 5 V. Se precisar substituir o módulo por chaves tácteis,
ligue cada uma entre GND e o GPIO correspondente. Botões ausentes ficam
soltos, mantendo o pull-up interno; não precisam ser simulados por resistores.
Touch e botões podem coexistir; um botão pressionado tem prioridade.

### Reserva dos demais módulos — conectar em etapas futuras

| Módulo / sinal | GPIO |
| --- | --- |
| SD_SCK (conector separado da tela) | 6 |
| SD_MOSI | 7 |
| SD_MISO | 5 |
| SD_CS | 8 |
| CC1101 SCK / MOSI / MISO | 6 / 7 / 5 |
| CC1101 CSN / GDO0 | 14 / 15 |
| CC1101 GDO2 | Não conectar |
| NRF24 SCK / MOSI / MISO | 6 / 7 / 5 |
| NRF24 CSN / CE | 16 / 48 |
| NRF24 IRQ | Não conectar |
| PN532 SDA / SCL, selecionado em I²C | 1 / 2 |
| OLED SDA / SCK (I²C) | 1 / 2 |
| GPS TX → ESP RX | 17 |
| GPS RX ← ESP TX | 43 |
| Emissor IR, entrada de sinal | 18 |
| Receptor IR, saída de sinal | 21 |

SD, touch e rádios compartilham SCK/MOSI/MISO com seleções individuais;
não unir os sinais CS. O OLED compartilha I²C com PN532, sujeito à confirmação
do controlador e dos endereços. GPS inicia em 9600 baud. Os GPIO43/17 ficam
reservados ao GPS, portanto os logs devem usar USB nativo, não UART0.
A alimentação dos módulos e o orçamento de corrente serão conferidos antes
de conectar o conjunto completo; saídas de sinais não podem exceder 3,3 V.

## Compilação

Use Python 3.13 (a versão da plataforma exige Python 3.10 ou superior) e
PlatformIO. Na raiz do repositório:

```sh
pio run -e bruce-joker-s3
```

A compilação herda as versões e scripts do projeto. Quando os cabeçalhos
mquickjs precisarem ser regenerados, o script atual exige compilador C com
suporte a 32 bits. Cabeçalhos existentes com assinatura válida são reutilizados.
Consulte o erro da ferramenta se o host não tiver esse suporte instalado. O script `build.py` gera o arquivo
completo `Bruce-bruce-joker-s3.bin`; ele inclui bootloader e partições e deve
ser gravado no endereço `0x0`. O arquivo `.pio/build/bruce-joker-s3/firmware.bin`
é apenas o aplicativo e não substitui esse arquivo completo.

O workflow **Manual Build (Select Environment)** inclui `bruce-joker-s3`.
Depois de incorporar os arquivos no GitHub, use `target_branch: 2` e esse
ambiente. Nenhum workflow remoto foi executado automaticamente por esta alteração.

## Primeiro teste físico

1. Confirmar o mapa GPIO e testar continuidade do JY050 com tudo desligado.
2. Ligar tela, touch e JY050. Deixar os demais módulos desconectados.
3. Gravar o binário completo apenas depois de uma compilação bem-sucedida.
4. Conferir no log USB `[PSRAM] init=1 found=1`, total próximo de 8 MB.
5. Conferir imagem em 320×240, cores e ausência de reinicializações.
6. Testar esquerda/direita no menu, cima/baixo nos submenus e teclado,
   MID/SET para confirmar e RST para voltar; testar toque nos quatro cantos.
7. Registrar foto da tela e eventuais erros do log para ajustar cor/calibração.
8. Adicionar SD, depois os demais periféricos, um por vez.

## Próximas etapas

- Validar sinais e alimentação na montagem real.
- Implementar mensagens de estado no OLED após confirmar seu controlador.
- Traduzir a interface completa para PT-BR, verificando acentos e cortes.
- Criar tema inspirado no Coringa (roxo, verde, branco, vermelho e preto),
  com sorriso, “Ha Ha Ha” e animação. Artes aguardam as referências do proprietário.

## Interface PT-BR — primeira etapa

Este ambiente ativa `BRUCE_UI_PT_BR`. O catálogo em `include/ui_text.h` traduz
os títulos principais, os submenus de configuração, opções de tela/cores,
relógio, energia, pinos, credenciais e mensagens comuns dessas telas.
Os rodapés do touch e a opção compartilhada de retorno ao menu também estão em PT-BR.
Nomes de protocolos (WiFi, BLE, GPS etc.) são mantidos.

Esta etapa **não traduz todo o firmware**: telas internas dos módulos, informações
detalhadas do aparelho, ferramentas de rede, scripts e interface web ainda podem
aparecer em inglês. Imagens e animação do tema Joker serão feitas separadamente.
A opção de idioma do teclado configura o teclado, não o idioma da interface.

O catálogo é aplicado explicitamente aos textos da interface. Identificadores
salvos (por exemplo, `Config`, `Clock` e `JS Interpreter`), nomes de arquivos,
comandos, credenciais e entradas do usuário continuam intactos.
A fonte GLCD recebe glifos para acentos portugueses; corte, centralização e rolagem
dos menus contam caracteres UTF-8. A inversão ST7789 permanece ativada.

Teste nativo da tradução e do tratamento de texto:

```sh
python3 tests/ui_localization/run.py
```

Compilação: use o mesmo ambiente `pio run -e bruce-joker-s3` descrito acima.
Valide no aparelho os acentos em **Configurações**, **Relógio** e **Não**, a rolagem
de itens longos, o touch e os botões JY050. Sem acesso à placa física, esses testes
visuais e elétricos continuam pendentes.

## Interface PT-BR — menus dos módulos

A segunda etapa amplia a tradução para os menus de WiFi, BLE, RF, IR, RFID/NFC,
GPS, NRF24, arquivos, conexão, LoRa, FM e utilitários. Inclui opções internas de
leitura/gravação, controles de mídia, gerenciador de arquivos e mensagens comuns
de conexão, armazenamento e detecção dos módulos. Protocolos, marcas, comandos
DuckyScript, SSIDs, URLs e formatos dos arquivos permanecem como no original.

A tradução é aplicada nos pontos de exibição; não há substituição automática de
textos recebidos dos módulos ou escritos pelo usuário. Telas de operação com
textos próprios, scripts externos e a interface web ainda podem conter inglês.
Esta versão não adiciona as imagens ou a animação Joker.
