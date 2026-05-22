https://wokwi.com/projects/464355330381886465

Descrição do Projeto:

O sistema realiza o monitoramento contínuo da temperatura e da umidade de uma vinheria utilizando:

- Sensor DHT22
- Display LCD I2C 16x2
- RTC DS1307 (Relógio de Tempo Real)
- EEPROM para armazenamento de eventos críticos
- Botão físico para navegação entre telas

O objetivo é garantir condições adequadas para conservação dos vinhos, registrando automaticamente situações fora dos limites definidos.

Funcionalidades

- Leitura de temperatura e umidade
- Exibição das informações no display LCD
- Navegação entre telas através de botão
- Exibição de data e hora em tempo real
- Registro automático de eventos críticos na EEPROM
- Indicação visual com LED embutido
- Sistema de debounce para evitar múltiplos cliques falsos

Componentes Utilizados

| Componente | Função |
|---|---|
| Arduino Uno | Controle principal |
| DHT22 | Sensor de temperatura e umidade |
| LCD 16x2 I2C | Exibição das informações |
| RTC DS1307 | Controle de data e hora |
| Botão | Troca de telas |
| EEPROM | Armazenamento de logs |
| LED embutido | Indicação de registro |

Bibliotecas Necessárias

Instale as seguintes bibliotecas na IDE Arduino:

- LiquidCrystal_I2C
- RTClib
- Wire
- EEPROM
- DHT sensor library

Configuração dos Pinos

| Componente | Pino |
|---|---|
| DHT22 | D2 |
| Botão | D5 |
| LCD I2C | SDA/SCL |
| RTC DS1307 | SDA/SCL |
| LED | LED_BUILTIN |

Faixas de Operação

Temperatura

| Situação | Valor |
|---|---|
| BAIXA | abaixo de 20°C |
| BOA | entre 20°C e 30°C |
| ALTA | acima de 30°C |

Umidade

| Situação | Valor |
|---|---|
| BAIXA | abaixo de 30% |
| BOA | entre 30% e 60% |
| ALTA | acima de 60% |

Manual de Operação

Inicialização

Ao ligar o sistema:

1. O LCD exibirá:
   - Next Level
   - Inicializando

2. O RTC será sincronizado com a data e hora da compilação do código.

3. O sensor DHT22 iniciará a leitura ambiental.

Navegação das Telas

O botão conectado ao pino D5 permite alternar entre 3 telas.

Tela 1 — Temperatura

Exibe:

- Temperatura atual
- Status da temperatura

Exemplo:

Temperatura
24.5°C BOA

Tela 2 — Umidade

Exibe:

- Umidade atual
- Status da umidade

Exemplo:

Umidade
45.2% BOA

Tela 3 — Data e Hora

Exibe:

- Data atual
- Hora atual

Exemplo:

22/05/2026
14:32:10

Sistema de Logs

O sistema registra automaticamente eventos críticos quando:

- Temperatura estiver fora da faixa ideal
- Umidade estiver fora da faixa ideal

Funcionamento do Registro

A cada minuto:

1. O sistema verifica os valores do sensor
2. Caso algum valor esteja fora do padrão:
   - O LED embutido pisca
   - Os dados são gravados na EEPROM

Dados Armazenados

| Informação | Tipo |
|---|---|
| Timestamp | Data/Hora |
| Temperatura | Valor em °C |
| Umidade | Valor em % |

Estrutura da EEPROM

| Configuração | Valor |
|---|---|
| Máximo de registros | 100 |
| Tamanho por registro | 8 bytes |
| Tipo de gravação | Circular |

Quando a memória atinge o limite, os registros antigos começam a ser sobrescritos.

Comunicação Serial

O sistema possui duas opções configuráveis:

#define LOG_OPTION 1
#define SERIAL_OPTION 0

LOG_OPTION

Quando ativado (1):

- Exibe no Serial Monitor todos os registros salvos na EEPROM.

SERIAL_OPTION

Quando ativado (1):

- Exibe continuamente:
  - Temperatura
  - Status da temperatura
  - Umidade
  - Status da umidade

Exemplo:

Temp: 24.5 C | BOA | Umid: 45.3% | BOA
Autor

Projeto desenvolvido para monitoramento inteligente de vinheria utilizando Arduino.
