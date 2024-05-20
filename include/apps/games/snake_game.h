#ifndef OSW_APP_SNAKE_GAME_H
#define OSW_APP_SNAKE_GAME_H

#include <osw_hal.h>
#include <osw_ui.h>

#include <OswAppV1.h>

class OswAppSnakeGame : public OswApp {
  public:
    OswAppSnakeGame(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppSnakeGame() {};

  private:
    OswUI* ui;

#define cellSize 10
#define snakeLength 30
    bool buttonControllerMode = false;
    //#define demo 1

    // Change these values if sensitivity is too much/low
    const float xSensitivity = 0.75f;
    const float ySensitivity = 0.75f;

    int score = 1;
    int snake[snakeLength][2] = {{10, 10}};

    bool gameRunning = false;
    const int gameWidth = 30;

    enum direction_t: int {
        UP = 0,
        RIGHT = 1,
        DOWN = 2,
        LEFT = 3
    };

    int lastDirection = direction_t::DOWN;

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

    void drawDirection(const int xDirection, const int yDirection);
    void drawDirectionArrow(const int direction, const int topLeftX = 120, const int topLeftY = 13);
    void drawPlayer();
    void drawGameState();
    void drawLunch();
    void drawGrid();
    void drawScore();
    void drawButtonHints();

    // Game logics
    void snakeGame();
    void resetGame();
    void proceedEating();
    void spawnEat();
    bool coordsInGame(const int xCoord, const int yCoord);

    // Controls
    void buttonController();
    void accelerometerController();
    void autoController();
    void useLastDirection();

    void waitingRoom();

    void proceedSnakeCoords(const bool fastGame);
    bool touchItself();
    bool isTimeFinish() {
        return deltaSeconds * score > 1;
    }
};

#endif
