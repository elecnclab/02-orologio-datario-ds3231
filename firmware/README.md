# Firmware - Orologio Datario DS3231

Questa cartella contiene il firmware Arduino del progetto **Orologio con datario a 7 segmenti**.

## Struttura
- `orologio-datario-ds3231/`
- `orologio-datario-ds3231/orologio-datario-ds3231.ino`

## Requisiti
- Arduino IDE (consigliata versione 2.x)
- Scheda: Arduino Nano (ATmega328P)
- Librerie richieste:
  - `RTClib`
  - `TM1637TinyDisplay6`

## Pin utilizzati (da sketch)
- Display ORA (TM1637 #1)
  - `TM_CLK_TIME` -> D2
  - `TM_DIO_TIME` -> D3
- Display DATA (TM1637 #2)
  - `TM_CLK_DATE` -> D7
  - `TM_DIO_DATE` -> D8
- Encoder rotativo
  - `ENC_CLK` -> D4
  - `ENC_DT`  -> D5
  - `ENC_SW`  -> D6
- RTC (DS3231)
  - I2C su A4 (SDA), A5 (SCL) su Arduino Nano

## Come compilare/caricare
1. Apri Arduino IDE.
2. Apri `orologio-datario-ds3231/orologio-datario-ds3231.ino`.
3. Seleziona scheda e porta corretta.
4. Installa le librerie richieste se mancanti.
5. Compila e carica.

## Nota su inizializzazione RTC
Nel file `.ino` è presente:
- `#define SET_RTC_FROM_COMPILE_TIME 0`

Per inizializzare l'RTC con data/ora del PC:
1. Imposta temporaneamente a `1`.
2. Carica lo sketch una volta.
3. Riporta a `0`.
4. Ricarica lo sketch.

## Stato
Firmware importato in una struttura pronta per versionamento Git/GitHub.
## Ultime modifiche firmware
- Gestione encoder aggiornata con pressione lunga/corta:
  - pressione lunga da RUN: entra in regolazione ORARIO
  - pressioni brevi: HH -> MM -> SS -> salva/esce
  - seconda pressione lunga in regolazione orario: passa a regolazione DATA
  - pressioni brevi: GG -> MM -> AA -> salva/esce
- Corretto doppio incremento encoder: ora un singolo scatto incrementa/decrementa di 1.

