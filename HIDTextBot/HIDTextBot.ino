#include "ShapeStepper.h"
#include "HIDTextBot.h"

HIDTextBot bot(5, 7);
ShapeStepper mouseShape;
int mouseSizePx = 200;

void setup() {
  randomSeed(analogRead(A0));
  // bot.begin();
  // bot.beginNotePad();
  bot.beginCode();
  mouseShape.begin(mouseSizePx, false);
  mouseShape.setExplicitShape(SH_PENTAGRAM);
}

void loop() {
  bot.doAction();
  mouseShape.move();
}
