# Diagnóstico da tela — ESP32-S3 N16R8

Este programa testa somente tela, touch e cartão SD. Não é o firmware Bruce completo. Há duas versões, com drivers ILI9341 e ST7789; ambas usam inversão de cores.

O perfil `boards/marauder-touch/pins_arduino.h` do repositório usa ILI9341, LED ativo em nível alto e reset por GPIO. O INI fornecido seleciona esse arquivo, mas não contém essas definições. Isso justifica comparar os dois drivers; não identifica com certeza o chip da sua tela.

## Ligação para este teste

Desligue a alimentação antes de mudar os fios. Desconecte JY050, rádios, IR, GPS, PN532 e OLED para este teste. GPIO13, GPIO14 e GPIO18 têm outras funções no firmware completo anterior.

A ligação LED → GPIO13 reproduz o controle lógico do módulo de tela que você informou já ter usado com o Marauder. Ela pressupõe que o pino LED desse mesmo módulo tem circuito de acionamento/limitação adequado; não é uma ligação para LED de iluminação avulso sem resistor. O software não limita a corrente da carga.

Use somente a alimentação USB da placa no teste. Mantenha VCC da tela na alimentação já utilizada e compatível com esse módulo; não aplique 5 V aos sinais do ESP32-S3. Todos os GND devem estar interligados.

| Pino escrito na tela | ESP32-S3 / destino |
| --- | --- |
| GND | GND |
| VCC | Alimentação compatível com o módulo, conforme acima |
| LED | GPIO13 |
| RESET | GPIO14 — **remova a ligação anterior ao 3V3** |
| CS | GPIO10 |
| DC | GPIO9 |
| SCK | GPIO12 |
| SDI (MOSI) | GPIO11 |
| SDO (MISO) | GPIO18 |
| T_CLK | GPIO6 |
| T_DIN | GPIO7 |
| T_DO | GPIO5 |
| T_CS | GPIO4 |
| T_IRQ | Sem ligação; não ligar ao EN |
| SD_SCK | GPIO6, junto de T_CLK |
| SD_MOSI | GPIO7, junto de T_DIN |
| SD_MISO | GPIO5, junto de T_DO |
| SD_CS | GPIO8 |

GPIO é o número impresso na placa, não a posição do terminal. GPIO32 do ESP32 clássico não está disponível nessa placa S3 N16R8. O reset acima é o RESET da tela; o EN/RST da placa ESP32 continua como está.

## Gravação

1. Comece com `teste-tela-ili9341-invertido.bin`, correspondente ao driver do perfil Marauder consultado.
2. No Flash Download Tool selecione **ESP32-S3**, arquivo no endereço **0x0**, **DIO**, **80 MHz**, flash **16 MB** e velocidade **115200**. Estes binários já incluem bootloader e partições. Não use o endereço 0x10000.
3. Grave e reinicie a placa. Não apague toda a flash para este teste; o programa não usa as configurações salvas do Bruce.
4. Se a iluminação acender, mas não houver imagem correta, repita com `teste-tela-st7789-invertido.bin`, mantendo exatamente os mesmos fios e parâmetros.

A biblioteca instalada fornece bootloader DIO de 80 MHz; por isso estes arquivos usam 80 MHz, não os 40 MHz usados no seu teste anterior. A comunicação com a tela é de 10 MHz, independente da frequência da flash.

**Não grave os binários antigos do Bruce com esta nova ligação.** Eles não controlam LED13/reset14 e reservam esses pinos para outros módulos. Ao concluir o diagnóstico, a pinagem definitiva e o firmware completo precisarão ser ajustados conforme o resultado.

## Resultado esperado

- Iluminação ligada continuamente, título do driver e três faixas: vermelho, verde e azul, nessa ordem.
- Contador `Ativo` avançando a cada segundo.
- Coordenadas e ponto amarelo ao tocar. A calibração é provisória; informe se o ponto ficar invertido ou deslocado.
- `SD: montado` com um cartão compatível inserido antes de ligar. O teste não formata nem grava arquivos; a montagem pode falhar por cartão, formato ou ligação.
- Pela porta USB nativa do S3, um monitor serial a 115200 pode mostrar o driver e linhas `ALIVE`, `SD mount` e `TOUCH`. A porta pode reaparecer após o reset.

Se continuar totalmente apagada, com o multímetro em tensão contínua meça LED/GPIO13 em relação ao GND após o boot (esperado próximo de 3,3 V) e VCC da tela em relação ao GND. Informe as duas leituras e se o monitor mostra `ALIVE`. Não faça ponte de LED para 5 V.

Envie qual arquivo testou, se a iluminação acendeu, se as cores estão corretas, e o resultado de touch/SD. A compilação não comprova o funcionamento elétrico da montagem.

## Compilação a partir dos fontes

Aplique o pacote de fontes sobre a branch `2`, revisão `78a7902c7faeef76f5e18521e75aec01f67656df`, usando o patch cumulativo; ou use o patch incremental se já aplicou o pacote PT-BR etapa 2. Não aplique ambos.

```sh
pio run -e bruce-joker-test-ili9341 -e bruce-joker-test-st7789
```

Os arquivos mesclados são gerados na raiz como `Bruce-bruce-joker-test-ili9341.bin` e `Bruce-bruce-joker-test-st7789.bin`.
