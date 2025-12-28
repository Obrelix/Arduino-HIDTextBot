#pragma once
#include <Arduino.h>
#include <Mouse.h>

enum Shape { SH_SQUARE,
             SH_TRIANGLE,
             SH_CIRCLE,
             SH_PENTAGRAM };

class ShapeStepper {
public:

  int stepDelayMs = 3;
  int sizePx = 30;
  int pauseMs = 5;

  unsigned long nextTickMs = 0;
  unsigned long pauseStartMs = 0;
  bool paused = false;
  bool oneShapeOnly = false;
  ShapeStepper();

  void setExplicitShape(Shape s);
  void setShape(Shape s);
  Shape getShape() const;

  // Initialize the currently selected shape with a size (pixels/steps)
  void init(int size);

  // Returns true when the current shape is finished.
  // Outputs next movement step in dx/dy (typically -1/0/1 per tick).
  bool nextStep(int &dx, int &dy);

  // Convenience: cycle Square -> Triangle -> Circle -> Square...
  void advanceShape();
  void begin(int size, bool beginMouse = true);
  void move();
private:
  struct LineState {
    int x0, y0, x1, y1;
    int dx, dy, sx, sy, err;
    bool active;
  } lineState;

  Shape currentShape;
  Shape explicitShape;

  // Square
  int squareSide, squareSeg, squareRem;

  // Triangle
  int triangleSide, triangleH, triangleEdge;

  // Circle
  int circleRadius, circleStepsTotal, circleStepsDone;
  int circlePrevXi, circlePrevYi;
  float circleX, circleY, circleC, circleS;

  void startLine(int x1, int y1);
  bool lineNextStep(int &outDx, int &outDy);

  void initSquare(int side);
  bool squareNextStep(int &dx, int &dy);

  void initTriangle(int side);
  bool triangleNextStep(int &dx, int &dy);

  void initCircle(int radius);
  bool circleNextStep(int &dx, int &dy);
  
  // Pentagram
  int pentRadius = 0;
  uint8_t pentEdge = 0;

  struct Pt { int x; int y; };
  Pt pentV[5];

  void initPentagram(int size);
  bool pentagramNextStep(int &dx, int &dy);
};
