#ifndef OSW_APP_BRICK_BREAKER_H
#define OSW_APP_BRICK_BREAKER_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppBrickBreaker : public OswApp {
 public:
  OswAppBrickBreaker(void) { ui = OswUI::getInstance(); };
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppBrickBreaker(){};

 private:
  OswUI* ui;

#define cellSize 10
#define snakeLength 30
  uint8_t buttonControllerMode = 0;

  // Change these values if sensitivity is too much/low
  const int playerHeight = 3;
  const int playerWidth = 24;
  const int playerY = 184;
  const int gridW = 8;
  const int gridH = 4;
  const float xSensitivity = 0.75;
  const float ySensitivity = 0.75;
  const bool newGrid[4][8] = {
      {0, 0, 1, 1, 1, 1, 0, 0}, {0, 1, 1, 1, 1, 1, 1, 0}, {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}};

  int score = 0;
  int playerPos = 120;
  bool gameRunning = false;
  int lastmove = 0;
  int lastpos = 0;

  double ballPosx = 160;
  double ballPosy = 120;
  double ballSpdx = -2;
  double ballSpdy = 4;
  double absspd = 0;
  double angleVar = 0;
  double angleout = 0;
  double angleout2 = 0;
  double pHitPosition = 0;
  double pHitAngle = 0;
  double posAngle = 0;
  double wallPosx = 0;
  double wallPosy = 0;
  double playerSpd = 0;

  bool scoreUpdated = false;
  bool grid[4][8] = {};
  int hasbounced = 0;

  int previousTime = 0;
  int gameStart = 0;
  double spd = 1;

  float deltaSeconds = 0;

  void drawDirection(OswHal* hal, const int xDirection, const int yDirection);
  void drawDirectionArrow(OswHal* hal, const int direction, const int topLeftX = 120, const int topLeftY = 13);
  void drawPlayer(OswHal* hal);
  void blockHit(OswHal* hal);
  void drawGrid(OswHal* hal);
  void drawScore(OswHal* hal);
  void drawButtonHints(OswHal* hal);
  void drawBall(OswHal* hal);
  void initGrid();
  void moveBall();
  void hitPlayer();
  void hitWall();

  // Game logics
  void BrickBreaker(OswHal* hal);
  void blockHit(OswHal* hal, const int x, const int y);
  bool coordsInGame(const int xCoord, const int yCoord);
  void resetGame(void);

  // Controls
  void buttonController(OswHal* hal);
  void accelerometerController(OswHal* hal);
  void gravityController(OswHal* hal);

  void waitingRoom(OswHal* hal);
};

#endif
