#include "ShapeStepper.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ShapeStepper::ShapeStepper()
  : currentShape(SH_SQUARE),
    squareSide(0), squareSeg(0), squareRem(0),
    triangleSide(0), triangleH(0), triangleEdge(0),
    circleRadius(0), circleStepsTotal(0), circleStepsDone(0),
    circlePrevXi(0), circlePrevYi(0),
    circleX(0), circleY(0), circleC(0), circleS(0) {
  lineState = { 0, 0, 0, 0, 0, 0, 0, 0, 0, false };
}

void ShapeStepper::setExplicitShape(Shape s) {
  oneShapeOnly = true;
  explicitShape = s;
}
void ShapeStepper::setShape(Shape s) {
  currentShape = s;
}
Shape ShapeStepper::getShape() const {
  return currentShape;
}

void ShapeStepper::startLine(int x1, int y1) {
  lineState.x0 = 0;
  lineState.y0 = 0;
  lineState.x1 = x1;
  lineState.y1 = y1;
  lineState.dx = abs(x1);
  lineState.sx = (0 < x1) ? 1 : -1;
  lineState.dy = -abs(y1);
  lineState.sy = (0 < y1) ? 1 : -1;
  lineState.err = lineState.dx + lineState.dy;
  lineState.active = true;
}

bool ShapeStepper::lineNextStep(int &outDx, int &outDy) {
  if (!lineState.active) {
    outDx = 0;
    outDy = 0;
    return true;
  }

  if (lineState.x0 == lineState.x1 && lineState.y0 == lineState.y1) {
    lineState.active = false;
    outDx = 0;
    outDy = 0;
    return true;
  }

  int e2 = lineState.err << 1;
  int nx = lineState.x0, ny = lineState.y0;

  if (e2 >= lineState.dy) {
    lineState.err += lineState.dy;
    nx += lineState.sx;
  }
  if (e2 <= lineState.dx) {
    lineState.err += lineState.dx;
    ny += lineState.sy;
  }

  outDx = nx - lineState.x0;
  outDy = ny - lineState.y0;
  lineState.x0 = nx;
  lineState.y0 = ny;

  if (lineState.x0 == lineState.x1 && lineState.y0 == lineState.y1) {
    lineState.active = false;
    return true;
  }
  return false;
}

void ShapeStepper::initSquare(int side) {
  squareSide = side;
  squareSeg = 0;
  squareRem = side;
}

bool ShapeStepper::squareNextStep(int &dx, int &dy) {
  if (squareSeg >= 4) {
    dx = 0;
    dy = 0;
    return true;
  }

  if (squareSeg == 0) {
    dx = 1;
    dy = 0;
  } else if (squareSeg == 1) {
    dx = 0;
    dy = 1;
  } else if (squareSeg == 2) {
    dx = -1;
    dy = 0;
  } else {
    dx = 0;
    dy = -1;
  }

  squareRem--;
  if (squareRem <= 0) {
    squareSeg++;
    squareRem = squareSide;
  }

  return (squareSeg >= 4);
}

void ShapeStepper::initTriangle(int side) {
  triangleSide = side;
  triangleH = (int)lroundf(side * 0.8660254f);  // ~sqrt(3)/2
  triangleEdge = 0;
  startLine(triangleSide, 0);
}

bool ShapeStepper::triangleNextStep(int &dx, int &dy) {
  bool doneLine = lineNextStep(dx, dy);
  if (!doneLine) return false;

  triangleEdge++;
  if (triangleEdge == 1) startLine(-triangleSide / 2, triangleH);
  else if (triangleEdge == 2) startLine(-triangleSide / 2, -triangleH);
  else return true;

  return false;
}

void ShapeStepper::initCircle(int radius) {
  circleRadius = radius;
  const int angleStepDeg = 10;
  const float stepRad = angleStepDeg * (float)M_PI / 180.0f;

  circleC = cosf(stepRad);
  circleS = sinf(stepRad);

  circleStepsTotal = 360 / angleStepDeg;
  circleStepsDone = 0;

  circleX = (float)circleRadius;
  circleY = 0.0f;

  circlePrevXi = (int)lroundf(circleX);
  circlePrevYi = (int)lroundf(circleY);
}

bool ShapeStepper::circleNextStep(int &dx, int &dy) {
  if (circleStepsDone >= circleStepsTotal) {
    dx = 0;
    dy = 0;
    return true;
  }

  float nx = circleX * circleC - circleY * circleS;
  float ny = circleX * circleS + circleY * circleC;
  circleX = nx;
  circleY = ny;

  int xi = (int)lroundf(circleX);
  int yi = (int)lroundf(circleY);

  dx = xi - circlePrevXi;
  dy = yi - circlePrevYi;

  circlePrevXi = xi;
  circlePrevYi = yi;

  circleStepsDone++;
  return (circleStepsDone >= circleStepsTotal);
}

void ShapeStepper::initPentagram(int size) {
  // Pick radius so overall footprint is similar to your circle init (circle uses size/2)
  pentRadius = size / 2;
  pentEdge = 0;

  // Build 5 outer vertices of a regular pentagon, but start with the point DOWN.
  // Mouse coordinates: +Y is down, so we use y = -sin(angle) * R to flip.
  const float startDeg = -90.0f;  // makes first vertex point down on screen coords
  const float stepDeg = 72.0f;    // 360/5
  const float deg2rad = (float)M_PI / 180.0f;

  for (int i = 0; i < 5; i++) {
    float a = (startDeg + stepDeg * i) * deg2rad;
    int x = (int)lroundf(pentRadius * cosf(a));
    int y = (int)lroundf(-pentRadius * sinf(a));  // flip for Mouse Y direction
    pentV[i] = { x, y };
  }

  // Translate so we START at vertex 0 (like your triangle starts at (0,0))
  // This makes the first point (pentV[0]) become origin.
  int ox = pentV[0].x;
  int oy = pentV[0].y;
  for (int i = 0; i < 5; i++) {
    pentV[i].x -= ox;
    pentV[i].y -= oy;
  }

  // Star connection order: 0->2->4->1->3->0
  // We'll start with edge 0: from v0 to v2
  startLine(pentV[2].x - pentV[0].x, pentV[2].y - pentV[0].y);
}

bool ShapeStepper::pentagramNextStep(int &dx, int &dy) {
  // Edge sequence for a 5-point star (pentagram)
  static const uint8_t seq[6] = { 0, 2, 4, 1, 3, 0 };

  // Step along current line segment
  bool doneLine = lineNextStep(dx, dy);
  if (!doneLine) return false;

  // Finished one segment, advance to next
  pentEdge++;
  if (pentEdge >= 5) {
    dx = 0;
    dy = 0;
    return true;  // all 5 segments complete
  }

  uint8_t a = seq[pentEdge];
  uint8_t b = seq[pentEdge + 1];

  startLine(pentV[b].x - pentV[a].x, pentV[b].y - pentV[a].y);
  return false;
}

void ShapeStepper::begin(int size, bool beginMouse) {
  if (beginMouse) Mouse.begin();
  setShape(SH_PENTAGRAM);
  init(size);
}

void ShapeStepper::init(int size) {
  sizePx = size;
  if (oneShapeOnly) {
    if (explicitShape == SH_SQUARE) initSquare(size);
    else if (explicitShape == SH_TRIANGLE) initTriangle(size);
    else if (explicitShape == SH_PENTAGRAM) initPentagram(size);
    else initCircle(size / 2);
  } else {
    if (currentShape == SH_SQUARE) initSquare(size);
    else if (currentShape == SH_TRIANGLE) initTriangle(size);
    else if (currentShape == SH_PENTAGRAM) initPentagram(size);
    else initCircle(size / 2);
  }
}

bool ShapeStepper::nextStep(int &dx, int &dy) {
    if (currentShape == SH_SQUARE) return squareNextStep(dx, dy);
    if (currentShape == SH_TRIANGLE) return triangleNextStep(dx, dy);
    if (currentShape == SH_CIRCLE) return circleNextStep(dx, dy);
    return pentagramNextStep(dx, dy);
}

void ShapeStepper::advanceShape() {
  if (oneShapeOnly) {
    if (explicitShape == SH_SQUARE) currentShape = SH_SQUARE;
    else if (explicitShape == SH_TRIANGLE) currentShape = SH_TRIANGLE;
    else if (explicitShape == SH_CIRCLE) currentShape = SH_CIRCLE;
    else  currentShape = SH_PENTAGRAM;
  } else {
    if (currentShape == SH_SQUARE) currentShape = SH_TRIANGLE;
    else if (currentShape == SH_TRIANGLE) currentShape = SH_CIRCLE;
    else if (currentShape == SH_CIRCLE) currentShape = SH_PENTAGRAM;
    else currentShape = SH_SQUARE;
  }
}

void ShapeStepper::move() {
  unsigned long now = millis();
  if (now < nextTickMs) return;
  nextTickMs = now + stepDelayMs;

  if (paused) {
    if (now - pauseStartMs >= (unsigned long)pauseMs) {
      paused = false;
      advanceShape();
      init(sizePx);
    }
    return;
  }

  int dx, dy;
  bool done = nextStep(dx, dy);
  Mouse.move(dx, dy);

  if (done) {
    paused = true;
    pauseStartMs = now;
  }
}
