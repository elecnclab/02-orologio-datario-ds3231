/*
 * ============================================================
 *  OROLOGIO DIGITALE CON DATARIO – 7 SEGMENTI
 *  Arduino Nano + RTC + Encoder Rotativo
 * ============================================================
 *
 *  Progetto e firmware realizzati da:
 *  Giuseppe – Electronic & CNC Lab
 *
 *  Canale YouTube:
 *  https://www.youtube.com/@Electronic.CNCLab
 *
 *  Questo sketch fa parte di un progetto di autocostruzione
 *  documentato passo-passo sul canale YouTube.
 *
 *  Video di riferimento:
 *
 *  ▶️ Video #118
 *  "Orologio 7 segmenti con datario – autocostruito"
 *  https://youtu.be/fMx0qyd0LHk
 *
 *  ▶️ Video #121
 *  "Orologio con datario, dalla breadboard alla PCB finale"
 *  https://youtu.be/LtEdSlX1n-0
 *
 *  Il progetto nasce come prototipo con cablaggio su shield
 *  con morsettiere e viene poi evoluto in una versione
 *  definitiva con PCB dedicata.
 *
 *  Questo codice è pensato per essere studiato, modificato,
 *  migliorato e riutilizzato.
 *
 * ============================================================
 */


// ===============================================
// OROLOGIO DS3231 + DOPPIO TM1637 (6 cifre x 2)
// OROLOGIO DS1307 + DOPPIO TM1637 (6 cifre x 2)
// Display 1: ORA  -> HH.MM.SS
// Display 2: DATA -> GG.MM.AA
// Regolazione ORA + DATA con encoder rotativo
// Lampeggio del campo selezionato
// Arduino Nano
// ===============================================

#include <Wire.h>
#include <RTClib.h>
#include <TM1637TinyDisplay6.h>

// -----------------------
// OPZIONE: inizializza RTC da PC
// -----------------------
// -----------------------
// OPZIONE: inizializzare l'RTC con data/ora del PC
// Metti a 1 SET_RTC_FROM_COMPILE_TIME, carica lo sketch una volta,
// poi rimetti a 0 e ricarica per non sovrascrivere sempre l'ora.
#define SET_RTC_FROM_COMPILE_TIME 0

// -----------------------
// PIN DEFINITIONS
// -----------------------
#define TM_CLK_TIME 2
#define TM_DIO_TIME 3

#define TM_CLK_DATE 7
#define TM_DIO_DATE 8

#define ENC_CLK 4
#define ENC_DT 5
#define ENC_SW 6

// -----------------------
// OGGETTI GLOBALI
// -----------------------
TM1637TinyDisplay6 displayTime(TM_CLK_TIME, TM_DIO_TIME);
TM1637TinyDisplay6 displayDate(TM_CLK_DATE, TM_DIO_DATE);
// Scommentare l' RTC utilizzato
// RTC_DS1307 rtc;  // meno preciso
RTC_DS3231 rtc;     // più preciso

// -----------------------
// MODALITÀ
// -----------------------
enum Mode {
  MODE_RUN,
  MODE_SET_HH,
  MODE_SET_MM,
  MODE_SET_SS,
  MODE_SET_DD,
  MODE_SET_MO,
  MODE_SET_YY
};

Mode mode = MODE_RUN;

// Valori in editing
int setHH, setMM, setSS;
int setDD, setMO, setYY2;

// Valori correnti RTC
int curHH, curMM, curSS;
int curDD, curMO, curYY2;

// Blink
unsigned long lastBlink = 0;
bool blinkOn = true;
const unsigned long BLINK_INTERVAL = 400;

// Encoder
volatile int8_t encDelta = 0;
int lastEncCLK = HIGH;
int lastEncSW = HIGH;
int stableEncSW = HIGH;
unsigned long lastButtonChange = 0;
unsigned long buttonPressStart = 0;
bool longPressHandled = false;
const unsigned long LONG_PRESS_MS = 700;

// Update flag
bool needUpdate = true;
int lastSecond = -1;

// -----------------------
// FUNZIONI SUPPORTO
// -----------------------
bool isLeapYear(int yy2) {
  int y = 2000 + yy2;
  if (y % 400 == 0) return true;
  if (y % 100 == 0) return false;
  return (y % 4 == 0);
}

int daysInMonth(int m, int yy2) {
  switch (m) {
    case 2: return isLeapYear(yy2) ? 29 : 28;
    case 4: case 6: case 9: case 11: return 30;
    default: return 31;
  }
}

void normalizeTime() {
  if (setHH < 0) setHH = 23;
  if (setHH > 23) setHH = 0;
  if (setMM < 0) setMM = 59;
  if (setMM > 59) setMM = 0;
  if (setSS < 0) setSS = 59;
  if (setSS > 59) setSS = 0;
}

void normalizeDate() {
  if (setMO < 1) setMO = 12;
  if (setMO > 12) setMO = 1;
  if (setYY2 < 0) setYY2 = 99;
  if (setYY2 > 99) setYY2 = 0;

  int dim = daysInMonth(setMO, setYY2);
  if (setDD < 1) setDD = dim;
  if (setDD > dim) setDD = 1;
}

void readRTC() {
  DateTime now = rtc.now();
  curHH = now.hour();
  curMM = now.minute();
  curSS = now.second();
  curDD = now.day();
  curMO = now.month();
  curYY2 = now.year() % 100;

  if (mode == MODE_RUN && curSS != lastSecond) {
    lastSecond = curSS;
    needUpdate = true;
  }
}

void writeRTC() {
  rtc.adjust(DateTime(2000 + setYY2, setMO, setDD, setHH, setMM, setSS));
}

// -----------------------
// ENCODER
// -----------------------
void setupEncoder() {
  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT, INPUT_PULLUP);
  pinMode(ENC_SW, INPUT_PULLUP);
  lastEncCLK = digitalRead(ENC_CLK);
  lastEncSW = digitalRead(ENC_SW);
  stableEncSW = lastEncSW;
}

void enterEditMode() {
  setHH = curHH; setMM = curMM; setSS = curSS;
  setDD = curDD; setMO = curMO; setYY2 = curYY2;
}

void saveAndExitEditMode() {
  normalizeTime();
  normalizeDate();
  writeRTC();
  mode = MODE_RUN;
}

void handleShortPress() {
  switch (mode) {
    case MODE_SET_HH: mode = MODE_SET_MM; break;
    case MODE_SET_MM: mode = MODE_SET_SS; break;
    case MODE_SET_SS: saveAndExitEditMode(); break;
    case MODE_SET_DD: mode = MODE_SET_MO; break;
    case MODE_SET_MO: mode = MODE_SET_YY; break;
    case MODE_SET_YY: saveAndExitEditMode(); break;
    default: break;
  }
  needUpdate = true;
}

void handleLongPress() {
  if (mode == MODE_RUN) {
    enterEditMode();
    mode = MODE_SET_HH;
  } else if (mode >= MODE_SET_HH && mode <= MODE_SET_SS) {
    mode = MODE_SET_DD;
  }
  needUpdate = true;
}

void updateEncoder() {
  int clk = digitalRead(ENC_CLK);
  int dt = digitalRead(ENC_DT);
  int sw = digitalRead(ENC_SW);

  if (clk != lastEncCLK) {
    // Conta solo su un fronte del CLK per evitare doppio step per scatto.
    if (clk == LOW) {
      encDelta += (dt != clk) ? 1 : -1;
    }
    lastEncCLK = clk;
  }

  if (sw != lastEncSW) {
    lastButtonChange = millis();
    lastEncSW = sw;
  }

  if (millis() - lastButtonChange > 50) {
    if (sw != stableEncSW) {
      stableEncSW = sw;

      if (stableEncSW == LOW) {
        buttonPressStart = millis();
        longPressHandled = false;
      } else {
        if (!longPressHandled) handleShortPress();
      }
    }

    if (stableEncSW == LOW && !longPressHandled &&
        (millis() - buttonPressStart >= LONG_PRESS_MS)) {
      handleLongPress();
      longPressHandled = true;
    }
  }
}

void applyEncoderDelta() {
  if (!encDelta || mode == MODE_RUN) return;
  int d = encDelta;
  encDelta = 0;

  switch (mode) {
    case MODE_SET_HH: setHH += d; normalizeTime(); break;
    case MODE_SET_MM: setMM += d; normalizeTime(); break;
    case MODE_SET_SS: setSS += d; normalizeTime(); break;
    case MODE_SET_DD: setDD += d; normalizeDate(); break;
    case MODE_SET_MO: setMO += d; normalizeDate(); break;
    case MODE_SET_YY: setYY2 += d; normalizeDate(); break;
    default: break;
  }
  needUpdate = true;
}

void updateBlink() {
  if (mode == MODE_RUN) {
    blinkOn = true;
    return;
  }
  if (millis() - lastBlink >= BLINK_INTERVAL) {
    lastBlink = millis();
    blinkOn = !blinkOn;
    needUpdate = true;
  }
}

// -----------------------
// DISPLAY
// -----------------------
void show6WithDots(TM1637TinyDisplay6 &d, const char *buf, uint8_t dp1, uint8_t dp2) {
  uint8_t seg[6];
  for (int i = 0; i < 6; i++) {
    seg[i] = (buf[i] >= '0' && buf[i] <= '9') ? d.encodeDigit(buf[i] - '0') : 0x00;
  }
  seg[dp1] |= 0x80;
  seg[dp2] |= 0x80;
  d.setSegments(seg);
}

void updateDisplays() {
  int hh = (mode == MODE_RUN) ? curHH : setHH;
  int mm = (mode == MODE_RUN) ? curMM : setMM;
  int ss = (mode == MODE_RUN) ? curSS : setSS;
  int dd = (mode == MODE_RUN) ? curDD : setDD;
  int mo = (mode == MODE_RUN) ? curMO : setMO;
  int yy = (mode == MODE_RUN) ? curYY2 : setYY2;

  char bufTime[7], bufDate[7];
  sprintf(bufTime, "%02d%02d%02d", hh, mm, ss);
  sprintf(bufDate, "%02d%02d%02d", dd, mo, yy);

  if (mode != MODE_RUN && !blinkOn) {
    if (mode <= MODE_SET_SS) {
      int i = (mode - MODE_SET_HH) * 2;
      bufTime[i] = bufTime[i + 1] = ' ';
    } else {
      int i = (mode - MODE_SET_DD) * 2;
      bufDate[i] = bufDate[i + 1] = ' ';
    }
  }

  show6WithDots(displayTime, bufTime, 1, 3);
  show6WithDots(displayDate, bufDate, 1, 3);
}

// -----------------------
// SETUP
// -----------------------
void setup() {
  Wire.begin();
  displayTime.begin();
  displayDate.begin();
  displayTime.setBrightness(7);
  displayDate.setBrightness(7);

  rtc.begin();
#if SET_RTC_FROM_COMPILE_TIME
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
#endif

  setupEncoder();
}

// -----------------------
// LOOP
// -----------------------
void loop() {
  static unsigned long lastRead = 0;

  if (millis() - lastRead > 200) {
    lastRead = millis();
    readRTC();
  }

  updateEncoder();
  applyEncoderDelta();
  updateBlink();

  if (needUpdate) {
    needUpdate = false;
    updateDisplays();
  }
}
