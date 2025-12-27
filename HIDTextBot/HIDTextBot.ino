#include "ShapeStepper.h"
#include "HIDTextBot.h"

HIDTextBot bot(5, 7);
ShapeStepper mouseShape;
int mouseSizePx = 100;

void setup() {
  randomSeed(analogRead(A0));
  bot.beginNotePad();
  mouseShape.begin(mouseSizePx, false);
  mouseShape.setExplicitShape(SH_PENTAGRAM);
}

void loop() {
  bot.doAction();
  mouseShape.move();
}
