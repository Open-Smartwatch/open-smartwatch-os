
#include "./apps/tools/snake_game.h"
// #define GIF_BG

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

void OswAppSnakeGame::setup(OswHal* hal) {
#ifdef GIF_BG
  bgGif->setup(hal);
#endif
}

void OswAppSnakeGame::loop(OswHal* hal) {
  hal->gfx()->fill(ui->getBackgroundColor());

  snakeGame(hal);
  hal->requestFlush();
}

void OswAppSnakeGame::stop(OswHal* hal) {
  // hal->disableDisplayBuffer();

#ifdef GIF_BG
  bgGif->stop(hal);
#endif
}

void OswAppSnakeGame::drawDirection(OswHal* hal, const int xDirection, const int yDirection) {
  switch (xDirection) {
    case -1:
      drawDirectionArrow(hal, LEFT);
      break;
    case 1:
      drawDirectionArrow(hal, RIGHT);
      break;
  }

  switch (yDirection) {
    case -1:
      drawDirectionArrow(hal, UP);
      break;
    case 1:
      drawDirectionArrow(hal, DOWN);
      break;
  }
}

void OswAppSnakeGame::drawDirectionArrow(OswHal* hal, const int direction) {
  if (direction == UP || direction == DOWN) {
    const int x = 120;
    const int height = 8;
    const int yUp = 5;
    const int yDown = yUp + height;

    hal->getCanvas()->drawLine(x, yUp, x, yDown, WHITE);

    if (direction == UP) {
      hal->getCanvas()->drawTriangle(x - 1, yUp, x + 1, yUp, x, yUp - 1, WHITE);
    } else {
      hal->getCanvas()->drawTriangle(x - 1, yDown, x + 1, yDown, x, yDown + 1, WHITE);
    }

  } else {
    const int length = 8;
    const int xLeft = 116;
    const int xRight = xLeft + length;
    const int y = 9;

    hal->getCanvas()->drawLine(xLeft, y, xRight, y, WHITE);
    if (direction == LEFT) {
      hal->getCanvas()->drawTriangle(xLeft, y - 1, xLeft, y + 1, xLeft - 1, y, WHITE);
    } else {
      hal->getCanvas()->drawTriangle(xRight, y - 1, xRight, y + 1, xRight + 1, y, WHITE);
    }
  }
}

void OswAppSnakeGame::drawPlayer(OswHal* hal) {
  if (gameRunning) {
    uint16_t green = rgb565(10, 200, 40);
    for (int i = 1; i < snakeLength - 1; i++) {
      if (snake[i][0] != 0 && snake[i][1] != 0) {
        if (snake[i + 1][0] != 0 && snake[i + 1][1] != 0) {
          int xSize = snake[i][0] - snake[i + 1][0];
          int ySize = snake[i][1] - snake[i + 1][1];

          hal->getCanvas()->fillRoundRect(min(snake[i][0], snake[i + 1][0]) * cellSize + 2,  //
                                          min(snake[i][1], snake[i + 1][1]) * cellSize + 2,  //
                                          cellSize * (1 + abs(xSize)) - 3,                   //
                                          cellSize * (1 + abs(ySize)) - 3,                   //
                                          3,                                                 //
                                          green);
        } else {
          hal->getCanvas()->fillRoundRect(snake[i][0] * cellSize + 2,  //
                                          snake[i][1] * cellSize + 2,  //
                                          cellSize - 3,                //
                                          cellSize - 3,                //
                                          3,                           //
                                          green);
        }
      }
    }
    hal->getCanvas()->fillRoundRect(snake[0][0] * cellSize,  //
                                    snake[0][1] * cellSize,  //
                                    cellSize,                //
                                    cellSize,                //
                                    5,                       //
                                    rgb565(0, 150, 0));
  }
}

void OswAppSnakeGame::drawGameState(OswHal* hal) {
  if (gameRunning) {
    hal->getCanvas()->drawTriangle(140, 5,   //
                                   140, 15,  //
                                   150, 10,  //
                                   WHITE);
  } else {
    hal->getCanvas()->fillRect(140, 5, 2, 10, WHITE);
    hal->getCanvas()->fillRect(143, 5, 2, 10, WHITE);
  }
}

void OswAppSnakeGame::drawLunch(OswHal* hal) {
  hal->getCanvas()->fillRoundRect(mealXCoord * cellSize + 2,  //
                                  mealYCoord * cellSize + 2,  //
                                  cellSize - 2,               //
                                  cellSize - 2,               //
                                  3,                          //
                                  rgb565(200, 10, 40));
}

void OswAppSnakeGame::drawGrid(OswHal* hal) {
  for (size_t x = 0; x < gameWidth; x++) {
    for (size_t y = 0; y < yCells; y++) {
      int xTarget = x * cellSize;
      int yTarget = y * cellSize + 20;

      if (coordsInGame(xTarget, yTarget)) {
        hal->getCanvas()->drawRect(x * cellSize, 20 + y * cellSize, cellSize, cellSize, WHITE);
      }
    }
  }
}

void OswAppSnakeGame::drawScore(OswHal* hal) {
  hal->gfx()->setTextCursor(95, 15);
  hal->gfx()->print(score);
}

void OswAppSnakeGame::snakeGame(OswHal* hal) {
  hal->gfx()->setTextSize(1);

  bool fastGame = false;

#ifdef demo
  autoController();
  fastGame = true;
#else
  accelerometerController(hal);
#endif

  drawGrid(hal);
  drawScore(hal);

  if (gameRunning) {
    drawDirection(hal, xDirection, yDirection);

    if (previousTime == 0) {
      gameStart = millis();
      previousTime = gameStart;
    }

    const int currentTime = millis();

    deltaSeconds += (currentTime - previousTime) / (1000.0);

    proceedSnakeCoords(fastGame);

    previousTime = currentTime;

    const int xTarget = snake[0][0] * cellSize;
    const int yTarget = snake[0][1] * cellSize;

    if (!coordsInGame(xTarget, yTarget) || touchItself()) {
      delay(500);
      resetGame();
    }

    drawLunch(hal);
    drawPlayer(hal);
    proceedEating();
  } else {
    waitingRoom(hal);
  }
  drawGameState(hal);
}

void OswAppSnakeGame::waitingRoom(OswHal* hal) {
  hal->gfx()->setTextCursor(180, 48);
  hal->gfx()->print("Start");

  if (hal->btnHasGoneDown(BUTTON_3)) {
    resetGame();
    gameRunning = true;
  }
}

void OswAppSnakeGame::resetGame() {
  for (int i = 1; i < snakeLength; i++) {
    snake[i][0] = 0;
    snake[i][1] = 0;
  }

  snake[0][0] = 10;
  snake[0][1] = 10;

  score = 1;
  gameRunning = false;
  previousTime = 0;

  lastDirection = DOWN;

  xDirection = 0;
  yDirection = 1;

  gameStart = 0;
  spawnEat();
}

void OswAppSnakeGame::proceedEating() {
  if (!scoreUpdated && snake[0][0] == mealXCoord && snake[0][1] == mealYCoord) {
    mealXCoord = -1;
    mealYCoord = -1;

    score = score + 1;
    scoreUpdated = true;

    spawnEat();

    if (score == snakeLength) {
      delay(500);
      resetGame();
    }
  }
}

void OswAppSnakeGame::spawnEat() {
  scoreUpdated = false;

  mealXCoord = rand() % gameWidth;
  mealYCoord = rand() % gameWidth;
  if (!coordsInGame(mealXCoord * cellSize, mealYCoord * cellSize)) {
    spawnEat();
  }
}

bool OswAppSnakeGame::coordsInGame(const int xCoord, const int yCoord) {
  return pow(120 - (xCoord + halfSize), 2) + pow(120 - (yCoord + halfSize), 2) <= squaredWidth && yCoord > 20;
}

void OswAppSnakeGame::accelerometerController(OswHal* hal) {
  float xAcceleration = hal->getAccelerationX();
  float yAcceleration = hal->getAccelerationY();

  if (xAcceleration >= -1 * xSensitivity && xAcceleration <= 1 * xSensitivity) {
    xDirection = 0;

    if (yAcceleration >= 1 * ySensitivity) {
      yDirection = 1;

    } else if (yAcceleration <= -1 * ySensitivity) {
      yDirection = -1;
    }

  } else if (yAcceleration >= -1 * ySensitivity && yAcceleration <= 1 * ySensitivity) {
    yDirection = 0;

    if (xAcceleration >= 1 * xSensitivity) {
      xDirection = 1;

    } else if (xAcceleration <= -1 * xSensitivity) {
      xDirection = -1;
    }
  } else {
    if (lastDirection == UP) {
      xDirection = 0;
      yDirection = -1;
    } else if (lastDirection == DOWN) {
      xDirection = 0;
      yDirection = 1;
    } else if (lastDirection == LEFT) {
      xDirection = -1;
      yDirection = 0;
    } else if (lastDirection == RIGHT) {
      xDirection = 1;
      yDirection = 0;
    }
  }
}

void OswAppSnakeGame::autoController() {
  int snakeX = snake[0][0];
  int snakeY = snake[0][1];

  if (lastDirection != UP && snakeY < mealYCoord) {
    xDirection = 0;
    yDirection = 1;

  } else if (lastDirection != RIGHT && snakeX > mealXCoord) {
    xDirection = -1;
    yDirection = 0;

  } else if (lastDirection != DOWN && snakeY > mealYCoord) {
    xDirection = 0;
    yDirection = -1;

  } else if (lastDirection != LEFT && snakeX < mealXCoord) {
    xDirection = 1;
    yDirection = 0;
  }
}

void OswAppSnakeGame::proceedSnakeCoords(const bool fastGame) {
  if (fastGame || isTimeFinish()) {
    for (int i = score - 1; i > 0; i--) {
      snake[i][0] = snake[i - 1][0];
      snake[i][1] = snake[i - 1][1];
    }

    if (xDirection != 0) {
      lastDirection = xDirection == -1 ? LEFT : RIGHT;
      snake[0][0] += xDirection;
    } else if (yDirection != 0) {
      lastDirection = yDirection == -1 ? UP : DOWN;
      snake[0][1] += yDirection;
    }

    deltaSeconds = 0;
  }
}

bool OswAppSnakeGame::touchItself() {
  const int xCoord = snake[0][0];
  for (int i = 1; i < score; i++) {
    if (xCoord == snake[i][0] && snake[0][1] == snake[i][1]) {
      return true;
    }
  }
  return false;
}
