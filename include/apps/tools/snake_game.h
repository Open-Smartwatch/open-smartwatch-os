#ifndef OSW_APP_SNAKE_GAME_H
#define OSW_APP_SNAKE_GAME_H

#include <osw_hal.h>
#include <osw_ui.h>

#include "osw_app.h"

class OswAppSnakeGame : public OswApp {
 public:
  OswAppSnakeGame(void) { ui = OswUI::getInstance(); };
  void setup(OswHal* hal);
  void loop(OswHal* hal);
  void stop(OswHal* hal);
  ~OswAppSnakeGame(){};

 private:
  OswUI* ui;

#define cellSize 10
#define snakeLength 30
  bool buttonControllerMode = false;
  //#define demo 1

  // Change these values if sensitivity is too much/low
  const float xSensitivity = 0.75;
  const float ySensitivity = 0.75;

  int score = 1;
  int snake[snakeLength][2] = {{10, 10}};

  bool gameRunning = false;
  const int gameWidth = 30;

  const int UP = 0;
  const int RIGHT = 1;
  const int DOWN = 2;
  const int LEFT = 3;

  int lastDirection = DOWN;

  int xDirection = 0;
  int yDirection = 1;

  int mealXCoord = 10;
  int mealYCoord = 10;

  bool scoreUpdated = false;

  int nbTurns = 0;
  int previousTime = 0;
  int gameStart = 0;

  const int yCells = gameWidth - 30 / cellSize;
  const int squaredWidth = 110 * 110;
  const int halfSize = cellSize / 2;

  float deltaSeconds = 0;

  void drawDirection(OswHal* hal, const int xDirection, const int yDirection);
  void drawDirectionArrow(OswHal* hal, const int direction, const int topLeftX = 120, const int topLeftY = 13);
  void drawPlayer(OswHal* hal);
  void drawGameState(OswHal* hal);
  void drawLunch(OswHal* hal);
  void drawGrid(OswHal* hal);
  void drawScore(OswHal* hal);
  void drawButtonHints(OswHal* hal);

  // Game logics
  void snakeGame(OswHal* hal);
  void resetGame();
  void proceedEating();
  void spawnEat();
  bool coordsInGame(const int xCoord, const int yCoord);

  // Controls
  void buttonController(OswHal* hal);
  void accelerometerController(OswHal* hal);
  void autoController();
  void useLastDirection();

  void waitingRoom(OswHal* hal);

  void proceedSnakeCoords(const bool fastGame);
  bool touchItself();
  bool isTimeFinish() { return deltaSeconds * score > 1; }
};

#endif
