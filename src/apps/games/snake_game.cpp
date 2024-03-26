
#include "./apps/games/snake_game.h"
// #define GIF_BG

#include <gfx_util.h>
#include <OswAppV1.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

void OswAppSnakeGame::setup() {

}

void OswAppSnakeGame::loop() {
    snakeGame();
}

void OswAppSnakeGame::stop() {

}

void OswAppSnakeGame::drawDirection(const int xDirection, const int yDirection) {
    switch (xDirection) {
    case -1:
        drawDirectionArrow(LEFT);
        break;
    case 1:
        drawDirectionArrow(RIGHT);
        break;
    }

    switch (yDirection) {
    case -1:
        drawDirectionArrow(UP);
        break;
    case 1:
        drawDirectionArrow(DOWN);
        break;
    }
}

void OswAppSnakeGame::drawDirectionArrow(const int direction, const int topLeftX, const int topLeftY) {
    OswHal* hal = OswHal::getInstance();
    const int length = 8;

    if (direction == UP || direction == DOWN) {
        const int yDown = topLeftY + length;

        hal->getCanvas()->drawLine(topLeftX, topLeftY, topLeftX, yDown, ui->getForegroundColor());

        if (direction == UP) {
            hal->getCanvas()->drawTriangle(topLeftX - 1, topLeftY, topLeftX + 1, topLeftY, topLeftX, topLeftY - 1,
                                           ui->getForegroundColor());
        } else {
            hal->getCanvas()->drawTriangle(topLeftX - 1, yDown, topLeftX + 1, yDown, topLeftX, yDown + 1,
                                           ui->getForegroundColor());
        }

    } else {
        const int xLeft = topLeftX - length / 2;
        const int xRight = xLeft + length;
        const int y = topLeftY + length / 2;

        hal->getCanvas()->drawLine(xLeft, y, xRight, y, ui->getForegroundColor());
        if (direction == LEFT) {
            hal->getCanvas()->drawTriangle(xLeft, y - 1, xLeft, y + 1, xLeft - 1, y, ui->getForegroundColor());
        } else {
            hal->getCanvas()->drawTriangle(xRight, y - 1, xRight, y + 1, xRight + 1, y, ui->getForegroundColor());
        }
    }
}

void OswAppSnakeGame::drawPlayer() {
    OswHal* hal = OswHal::getInstance();
    if (gameRunning) {
        for (int i = 1; i < snakeLength - 1; i++) {
            if (snake[i][0] != 0 && snake[i][1] != 0) {
                if (snake[i + 1][0] != 0 && snake[i + 1][1] != 0) {
                    int xSize = snake[i][0] - snake[i + 1][0];
                    int ySize = snake[i][1] - snake[i + 1][1];

                    hal->getCanvas()->fillRFrame(min(snake[i][0], snake[i + 1][0]) * cellSize + 2,  //
                                                 min(snake[i][1], snake[i + 1][1]) * cellSize + 2,  //
                                                 cellSize * (1 + abs(xSize)) - 3,                   //
                                                 cellSize * (1 + abs(ySize)) - 3,                   //
                                                 3,                                                 //
                                                 ui->getSuccessColor());
                } else {
                    hal->getCanvas()->fillRFrame(snake[i][0] * cellSize + 2,  //
                                                 snake[i][1] * cellSize + 2,  //
                                                 cellSize - 3,                //
                                                 cellSize - 3,                //
                                                 3,                           //
                                                 ui->getSuccessColor());
                }
            }
        }
        hal->getCanvas()->fillRFrame(snake[0][0] * cellSize,  //
                                     snake[0][1] * cellSize,  //
                                     cellSize,                //
                                     cellSize,                //
                                     5,                       //
                                     ui->getSuccessColor());
    }
}

void OswAppSnakeGame::drawGameState() {
    OswHal* hal = OswHal::getInstance();
    if (gameRunning) {
        hal->getCanvas()->drawTriangle(140, 5,   //
                                       140, 15,  //
                                       150, 10,  //
                                       ui->getForegroundColor());
    } else {
        hal->getCanvas()->fillFrame(140, 5, 2, 10, ui->getForegroundColor());
        hal->getCanvas()->fillFrame(143, 5, 2, 10, ui->getForegroundColor());
    }
}

void OswAppSnakeGame::drawLunch() {
    OswHal::getInstance()->getCanvas()->fillRFrame(mealXCoord * cellSize + 2,  //
            mealYCoord * cellSize + 2,  //
            cellSize - 2,               //
            cellSize - 2,               //
            3,                          //
            ui->getWarningColor());
}

void OswAppSnakeGame::drawGrid() {
    for (int x = 0; x < gameWidth; x++) {
        for (int y = 0; y < yCells; y++) {
            int xTarget = x * cellSize;
            int yTarget = y * cellSize + 20;

            if (coordsInGame(xTarget, yTarget)) {
                OswHal::getInstance()->getCanvas()->drawFrame(x * cellSize, 20 + y * cellSize, cellSize, cellSize, ui->getForegroundDimmedColor());
            }
        }
    }
}

void OswAppSnakeGame::drawScore() {
    OswHal::getInstance()->gfx()->setTextCursor(95, 15);
    OswHal::getInstance()->gfx()->print(score);
}

void OswAppSnakeGame::drawButtonHints() {
    OswHal* hal = OswHal::getInstance();
    const int diameter = 4;
    hal->getCanvas()->drawArc(200, 240 - 48 + diameter, diameter + 1, diameter, 270, 360, ui->getDangerColor());
    hal->getCanvas()->drawTriangle(               //
        diameter + 200 - 2, 240 + diameter - 48,  //
        diameter + 200 + 2, 240 + diameter - 48,  //
        diameter + 200, 240 + diameter - 48 + 2,  //
        ui->getDangerColor());

    hal->getCanvas()->drawArc(240 - 200, 240 - 48 + diameter, diameter + 1, diameter, 180, 270, ui->getDangerColor());
    hal->getCanvas()->drawTriangle(              //
        40 - 2 - diameter, 240 + diameter - 48,  //
        40 + 2 - diameter, 240 + diameter - 48,  //
        40 - diameter, 240 + diameter - 48 + 2,  //
        ui->getDangerColor());                   //
}

void OswAppSnakeGame::snakeGame() {
    OswHal::getInstance()->gfx()->setTextSize(1);

    bool fastGame = false;

    drawGrid();
    drawScore();

#ifdef demo
    autoController();
    fastGame = true;
#else
    if (buttonControllerMode) {
        drawButtonHints();
        buttonController();
        useLastDirection();
    } else {
        accelerometerController();
    }
#endif

    drawGrid();
    drawScore();

    if (gameRunning) {
        drawDirection(xDirection, yDirection);

        if (previousTime == 0) {
            gameStart = millis();
            previousTime = gameStart;
        }

        const int currentTime = millis();

        deltaSeconds += (currentTime - previousTime) / 1000.0f;

        proceedSnakeCoords(fastGame);

        previousTime = currentTime;

        const int xTarget = snake[0][0] * cellSize;
        const int yTarget = snake[0][1] * cellSize;

        if (!coordsInGame(xTarget, yTarget) || touchItself()) {
            OswHal::getInstance()->gfx()->setTextSize(2);
            OswHal::getInstance()->gfx()->setTextCursor(95, 15);
            OswHal::getInstance()->gfx()->print(score);

            delay(1000);
            resetGame();
        }

        drawLunch();
        drawPlayer();
        proceedEating();
    } else {
        waitingRoom();
    }
    drawGameState();
}

void OswAppSnakeGame::waitingRoom() {
    OswHal* hal = OswHal::getInstance();
    hal->gfx()->setTextSize(2);
    hal->gfx()->setTextCursor(150, 48);
    hal->gfx()->print("Start");

    hal->gfx()->setTextCursor(240 - 180 - 30, 240 - 48);
    hal->gfx()->print(buttonControllerMode ? "Button" : "Accelerometer");

    if (hal->btnHasGoneDown(BUTTON_1)) {
        buttonControllerMode = !buttonControllerMode;
    }

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

    mealXCoord = random(gameWidth);
    mealYCoord = random(gameWidth);
    if (!coordsInGame(mealXCoord * cellSize, mealYCoord * cellSize)) {
        spawnEat();
    }
}

bool OswAppSnakeGame::coordsInGame(const int xCoord, const int yCoord) {
    return pow(120 - (xCoord + halfSize), 2) + pow(120 - (yCoord + halfSize), 2) <= squaredWidth &&
           yCoord > 20;
}

void OswAppSnakeGame::buttonController() {
    // Bottom right
    if (OswHal::getInstance()->btnHasGoneDown(BUTTON_2)) {
        lastDirection++;
    }
    // Bottom left
    else if (OswHal::getInstance()->btnHasGoneDown(BUTTON_1)) {
        lastDirection--;
    }

    if (lastDirection < 0) {
        lastDirection += 4;
    } else if (lastDirection > 3) {
        lastDirection -= 4;
    }
}

void OswAppSnakeGame::accelerometerController() {
    float xAcceleration = OswHal::getInstance()->environment()->getAccelerationX();
    float yAcceleration = OswHal::getInstance()->environment()->getAccelerationY();

    if (xAcceleration >= -1 * xSensitivity && xAcceleration <= 1 * xSensitivity) {
        xDirection = 0;

        if (yAcceleration >= 1 * ySensitivity && lastDirection != UP) {
            yDirection = 1;

        } else if (yAcceleration <= -1 * ySensitivity && lastDirection != DOWN) {
            yDirection = -1;
        } else {
            useLastDirection();
        }

    } else if (yAcceleration >= -1 * ySensitivity && yAcceleration <= 1 * ySensitivity) {
        yDirection = 0;

        if (xAcceleration >= 1 * xSensitivity && lastDirection != RIGHT) {
            xDirection = -1;
        } else if (xAcceleration <= -1 * xSensitivity && lastDirection != LEFT) {
            xDirection = 1;
        } else {
            useLastDirection();
        }
    }
}

void OswAppSnakeGame::useLastDirection() {
    switch(lastDirection) {
    case direction_t::UP:
        xDirection = 0;
        yDirection = -1;
        break;
    case direction_t::DOWN:
        xDirection = 0;
        yDirection = 1;
        break;
    case direction_t::LEFT:
        xDirection = -1;
        yDirection = 0;
        break;
    case direction_t::RIGHT:
        xDirection = 1;
        yDirection = 0;
        break;
    default:
        throw std::logic_error("Invalid direction");
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
