#include "HIDTextBot.h"
#include <string.h>
#include <avr/pgmspace.h>

// ---- your charset + phrases (kept as-is) ----
static const char charset[] = {
  // a-z
  'a',
  'b',
  'c',
  'd',
  'e',
  'f',
  'g',
  'h',
  'i',
  'j',
  'k',
  'l',
  'm',
  'n',
  'o',
  'p',
  'q',
  'r',
  's',
  't',
  'u',
  'v',
  'w',
  'x',
  'y',
  'z',
  // A-Z
  'A',
  'B',
  'C',
  'D',
  'E',
  'F',
  'G',
  'H',
  'I',
  'J',
  'K',
  'L',
  'M',
  'N',
  'O',
  'P',
  'Q',
  'R',
  'S',
  'T',
  'U',
  'V',
  'W',
  'X',
  'Y',
  'Z',
  // 0-9
  '0',
  '1',
  '2',
  '3',
  '4',
  '5',
  '6',
  '7',
  '8',
  '9',
  // specials
  ' ',
  '!',
  '"',
  '#',
  '$',
  '%',
  '&',
  '\'',
  '(',
  ')',
  '*',
  '+',
  ',',
  '-',
  '.',
  '/',
  ':',
  ';',
  '<',
  '=',
  '>',
  '?',
  '@',
  '[',
  '\\',
  ']',
  '^',
  '_',
  '`',
  '{',
  '|',
  '}',
  '~',
};
static const int charsetCount = sizeof(charset) / sizeof(charset[0]);

static const char p0[] PROGMEM = "Uelv na systhuv ston kosmo.";
static const char p1[] PROGMEM = "Eimai o Eunikow Star.";
static const char p2[] PROGMEM = "To epvnymo moy exei allajei.";
static const char p3[] PROGMEM = "Exasa th mhtera moy.";
static const char p4[] PROGMEM = "H giagia moy me megalvse apo eji hmervn.";
static const char p5[] PROGMEM = "Mana einai ayth poy megalvnei ena paidi.";
static const char p6[] PROGMEM = "Me yiouethse dyo xronia prin fygei apo th zvh.";
static const char p7[] PROGMEM = "Htan para poly dyskolo na diaxeiristv ena terastio penuow.";
static const char p8[] PROGMEM = "Perasa tragika.";
static const char p9[] PROGMEM = "Xvriw na to katalabv me phre apo to mnhma thw manaw moy.";
static const char p10[] PROGMEM = "Ton agaphsa kai ton ebala ena skalopati pio katv apo th mhtera moy.";
static const char p11[] PROGMEM = "Egv exv dextei kakh symperifora apo ton syzygo moy, alla.. ton latreyv.";
static const char p12[] PROGMEM = "Phra thn zvh sta xeria moy.";
static const char p13[] PROGMEM = "Se hlikia 18 xronvn katalaba thn prosvpikh moy sejoyalikh diaforetikothta.";
static const char p14[] PROGMEM = "Eixa thn dynamh kai ton tsampoyka na sthrijv thn sejoyalikh moy protimhsh.";
static const char p15[] PROGMEM = "Den agaphuhka oyte 5 lepta kata thn diarkeia thw zvhw moy.";
static const char p16[] PROGMEM = "Uelei poly megalh dynamh gia na to antejei kaneiw.";
static const char p17[] PROGMEM = "Mporei na jerei kaneiw ti dynamh cyxhw xreiazetai;";
static const char p18[] PROGMEM = "Mphka vw kallitexnhw kai oxi vw soyrgelo.";
static const char p19[] PROGMEM = "Eimai ena prosvpo poy poyloysa kai poylav poly.";
static const char p20[] PROGMEM = "Anagkasmenow gia na bgalv to cvmi moy ekatsa se ayth thn ekpomph.";
static const char p21[] PROGMEM = "Huela kapoy na doylecv gia na zhsv.";
static const char p22[] PROGMEM = "Parallhla huela na perasv kapoia mhnymata koinvnika.";
static const char p23[] PROGMEM = "To mono poy fvnaza htan \"kyrie dhmarxe kante kati\".";
static const char p24[] PROGMEM = "Ola ayta ta apodeiknyei h istoria.";
static const char p25[] PROGMEM = "Hmoyn enaw anurvpow oxi askhnouethstow alla alhuinow.";
static const char p26[] PROGMEM = "Moy edvsan jana onoma kai taytothta.";
static const char p27[] PROGMEM = "Den ua jana kanv pote tipota allo pera apo to na tragoydv.";
static const char p28[] PROGMEM = "Breuhka apo ton oyrano sthn gh.";
static const char p29[] PROGMEM = "Thn giagia moy thn agapv perissotero apo th zvh moy.";

static const char* const phrases[] PROGMEM = {
  p0, p1, p2, p3, p4, p5, p6, p7, p8, p9,
  p10, p11, p12, p13, p14, p15, p16, p17, p18, p19,
  p20, p21, p22, p23, p24, p25, p26, p27, p28, p29
};

static const uint16_t phrasesCount = sizeof(phrases) / sizeof(phrases[0]);

// ---- class implementation ----

static void loadPhraseFromProgmem(uint16_t idx, char* out, size_t outSize) {
  if (outSize == 0) return;
  out[0] = '\0';

  // Read the pointer (which itself is stored in PROGMEM)
  const char* p = (const char*)pgm_read_ptr(&phrases[idx]);

  // Copy the string from PROGMEM into RAM
  strncpy_P(out, p, outSize - 1);
  out[outSize - 1] = '\0';
}

HIDTextBot::HIDTextBot(uint8_t ledPin, uint8_t btnPin)
  : _ledPin(ledPin), _btnPin(btnPin) {}

void HIDTextBot::begin(bool beginKeyboard, bool beginMouse) {
  pinMode(_ledPin, OUTPUT);
  digitalWrite(_ledPin, LOW);

  pinMode(_btnPin, INPUT_PULLUP);

  if (beginKeyboard) Keyboard.begin();
  if (beginMouse) Mouse.begin();

  runState = STOPPED;
  notepadOpened = false;

  now = millis();
  nextActionMs = now;
  nextMouseMs = now;
  nextKeyMs = now;
}


void HIDTextBot::beginNotePad(bool beginKeyboard, bool beginMouse, bool switchLayout) {
  pinMode(_ledPin, OUTPUT);
  digitalWrite(_ledPin, LOW);

  pinMode(_btnPin, INPUT_PULLUP);

  if (beginKeyboard) Keyboard.begin();
  if (beginMouse) Mouse.begin();

  runState = STOPPED;
  notepadOpened = false;

  now = millis();
  nextActionMs = now;
  nextMouseMs = now;
  nextKeyMs = now;

  delay(random(5000, 10000));
  openNotepad();
  notepadOpened = true;
  delay(random(600, 2500));
  if (switchLayout) switchLayout_WinSpace();
}

void HIDTextBot::setDelays(int keyMinMs, int keyMaxMs,
                           long stepMinMs, long stepMaxMs,
                           long mouseMinMs, long mouseMaxMs) {
  keyDelayMinMs = keyMinMs;
  keyDelayMaxMs = keyMaxMs;
  stepDelayMinMs = stepMinMs;
  stepDelayMaxMs = stepMaxMs;
  mouseDelayMinMs = mouseMinMs;
  mouseDelayMaxMs = mouseMaxMs;
}

void HIDTextBot::setMouseJitterMax(int maxJitter) {
  mouseJitterMax = maxJitter;
}

bool HIDTextBot::isRunning() const {
  return runState == RUNNING;
}

long HIDTextBot::randBetween(long a, long b) {
  return random(a, b + 1);
}

void HIDTextBot::keyTap(uint8_t k) {
  Keyboard.releaseAll();
  delay(random(8, 15));
  Keyboard.press(k);
  delay(random(20, 30));
  Keyboard.release(k);
  delay(random(8, 15));
  Keyboard.releaseAll();
  delay(random(8, 15));
}

void HIDTextBot::comboTap(uint8_t mod, uint8_t k) {
  Keyboard.press(mod);
  Keyboard.press(k);
  delay(random(30, 50));
  Keyboard.release(k);
  Keyboard.release(mod);
  Keyboard.releaseAll();
  delay(random(10, 20));
}

void HIDTextBot::switchLayout_WinSpace() {
  Keyboard.press(KEY_LEFT_GUI);
  delay(random(10, 30));
  Keyboard.press(' ');
  delay(random(30, 50));
  Keyboard.release(' ');
  delay(random(8, 15));
  Keyboard.release(KEY_LEFT_GUI);
  Keyboard.releaseAll();
  delay(random(70, 90));
}

void HIDTextBot::openNotepad() {
  Keyboard.press(KEY_LEFT_GUI);
  Keyboard.press('r');
  delay(random(60, 100));
  Keyboard.releaseAll();
  delay(random(180, 280));

  Keyboard.print("notepad");
  delay(random(90, 150));
  keyTap(KEY_RETURN);
  delay(random(552, 750));
}

void HIDTextBot::doAltTab() {
  Keyboard.press(KEY_LEFT_ALT);

  delay(random(10, 30));
  Keyboard.press(KEY_TAB);
  delay(random(30, 50));
  Keyboard.release(KEY_TAB);
  delay(random(8, 15));

  delay(random(800, 2400));

  delay(random(10, 30));
  Keyboard.press(KEY_TAB);
  delay(random(30, 50));
  Keyboard.release(KEY_TAB);
  delay(random(8, 15));

  delay(random(1200, 3400));

  delay(random(10, 30));
  Keyboard.press(KEY_LEFT_SHIFT);
  delay(random(30, 50));
  Keyboard.press(KEY_TAB);
  delay(random(30, 50));
  Keyboard.release(KEY_TAB);
  Keyboard.release(KEY_LEFT_SHIFT);
  delay(random(8, 15));

  delay(random(800, 2400));

  delay(random(10, 30));
  Keyboard.press(KEY_LEFT_SHIFT);
  delay(random(30, 50));
  Keyboard.press(KEY_TAB);
  delay(random(30, 50));
  Keyboard.release(KEY_TAB);
  Keyboard.release(KEY_LEFT_SHIFT);
  delay(random(8, 15));

  delay(random(800, 2400));

  Keyboard.release(KEY_LEFT_ALT);
  Keyboard.releaseAll();
  delay(random(70, 90));
}

void HIDTextBot::clearAll() {
  comboTap(KEY_LEFT_CTRL, 'a');
  delay(random(800, 2400));
  keyTap(KEY_BACKSPACE);
}

void HIDTextBot::deleteLastWord() {
  comboTap(KEY_LEFT_CTRL, KEY_BACKSPACE);
  delay(random(30, 50));
}

void HIDTextBot::typeRandomLine() {
  int rWord = random(0, 101);
  // const char* p = phrases[random(0, phrasesCount)];
  // int n = (int)strlen(p);
  char pbuf[160];  // adjust size if you add longer phrases
  loadPhraseFromProgmem((uint16_t)random(0, phrasesCount), pbuf, sizeof(pbuf));

  const char* p = pbuf;
  int n = (int)strlen(p);
  for (int i = 0; i < n;) {
    int rChar = random(0, 1001);
    now = millis();

    if ((long)(now - nextKeyMs) >= 0) {
      char ch = p[i];

      if (rChar < 980) {
        Keyboard.print(ch);
        nextKeyMs = now + (unsigned long)random(keyDelayMinMs, keyDelayMaxMs);
        i++;
      } else if (rChar < 995) {
        char charSetChar = charset[random(0, charsetCount)];
        Keyboard.print(charSetChar);
        delay((unsigned long)random(keyDelayMinMs, keyDelayMaxMs * 3));
        keyTap(KEY_BACKSPACE);
        delay((unsigned long)random(keyDelayMinMs, keyDelayMaxMs * 5));
        Keyboard.print(ch);
        nextKeyMs = millis() + (unsigned long)random(keyDelayMinMs, keyDelayMaxMs);
        i++;
      } else {
        keyTap(KEY_BACKSPACE);
        nextKeyMs = now + (unsigned long)random(keyDelayMinMs, keyDelayMaxMs);
        i--;
      }
    }

    if (i < 0) i = 0;
  }

  if (rWord > 98) deleteLastWord();
  keyTap(KEY_RETURN);
}

void HIDTextBot::smallMouseJitter() {
  int dx = (int)randBetween(-mouseJitterMax, mouseJitterMax);
  int dy = (int)randBetween(-mouseJitterMax, mouseJitterMax);
  if (dx != 0 || dy != 0) Mouse.move(dx, dy, 0);
}

void HIDTextBot::doOneAction() {
  int r = random(0, 101);
  if (r < 90) {
    typeRandomLine();
  } else if (r < 96) {
    doAltTab();
  } else {
    smallMouseJitter();
  }
}

void HIDTextBot::setNextSchedules() {
  nextActionMs = now + (unsigned long)randBetween(stepDelayMinMs, stepDelayMaxMs);
  nextMouseMs = now + (unsigned long)randBetween(mouseDelayMinMs, mouseDelayMaxMs);
  nextKeyMs = now + (unsigned long)randBetween(keyDelayMinMs, keyDelayMaxMs);
}

void HIDTextBot::handleButton() {
  bool btn = (digitalRead(_btnPin) == LOW);

  if (btn && !btnPrev) {
    btnDownMs = now;
    longPressFired = false;
  }

  if (btn && !longPressFired) {
    if (now - btnDownMs >= 1200) {
      clearAll();
      longPressFired = true;
    }
  }

  if (!btn && btnPrev) {
    if (!longPressFired) {
      runState = (runState == RUNNING) ? STOPPED : RUNNING;
      digitalWrite(_ledPin, runState == RUNNING ? HIGH : LOW);

      // Optional: open Notepad once when starting
      if (runState == RUNNING && !notepadOpened) {
        openNotepad();
        notepadOpened = true;
      }

      setNextSchedules();
    }
  }

  btnPrev = btn;
}

void HIDTextBot::tick() {
  now = millis();

  handleButton();

  if (runState != RUNNING) return;

  if ((long)(now - nextMouseMs) >= 0) {
    smallMouseJitter();
    nextMouseMs = now + (unsigned long)randBetween(mouseDelayMinMs, mouseDelayMaxMs);
  }

  if ((long)(now - nextActionMs) >= 0) {
    doOneAction();
    nextActionMs = now + (unsigned long)randBetween(stepDelayMinMs, stepDelayMaxMs);
  }
}

void HIDTextBot::doAction() {
  now = millis();
  if ((long)(now - nextActionMs) >= 0) {
    doOneAction();
    nextActionMs = now + (unsigned long)randBetween(stepDelayMinMs, stepDelayMaxMs);
  }
}
