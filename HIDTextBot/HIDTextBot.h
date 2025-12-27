#pragma once
#include <Arduino.h>
#include <Keyboard.h>
#include <Mouse.h>

class HIDTextBot {
public:
  HIDTextBot(uint8_t ledPin = 5, uint8_t btnPin = 7);

  void begin(bool beginKeyboard = true, bool beginMouse = true);
  void beginNotePad(bool beginKeyboard = true, bool beginMouse = true, bool switchLayout = true);

  void setDelays(int keyMinMs, int keyMaxMs,
                 long stepMinMs, long stepMaxMs,
                 long mouseMinMs, long mouseMaxMs);

  void setMouseJitterMax(int maxJitter);
  bool isRunning() const;
  bool notepadOpened = false;

  // Call this often from loop()
  void tick();
  void doAction();
  void switchLayout_WinSpace();
  void openNotepad();

private:
  enum RunState { STOPPED,
                  RUNNING };

  uint8_t _ledPin;
  uint8_t _btnPin;

  // Settings (defaults from your code)
  int keyDelayMinMs = 24;
  int keyDelayMaxMs = 187;

  long stepDelayMinMs = 3300;
  long stepDelayMaxMs = 8400;

  long mouseDelayMinMs = 7000;
  long mouseDelayMaxMs = 15000;

  int mouseJitterMax = 35;

  // State
  RunState runState = RUNNING;

  unsigned long nextActionMs = 0;
  unsigned long nextMouseMs = 0;
  unsigned long nextKeyMs = 0;
  unsigned long btnDownMs = 0;
  unsigned long now = 0;

  bool btnPrev = HIGH;  // following your original (kept as-is)
  bool longPressFired = false;

  // Helpers
  long randBetween(long a, long b);

  void keyTap(uint8_t k);
  void comboTap(uint8_t mod, uint8_t k);


  void doAltTab();
  void clearAll();
  void deleteLastWord();
  void typeRandomLine();

  void smallMouseJitter();
  void doOneAction();
  void setNextSchedules();

  void handleButton();
};
