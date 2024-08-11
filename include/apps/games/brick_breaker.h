#pragma once
#include OSW_TARGET_PLATFORM_HEADER
#if OSW_PLATFORM_ENVIRONMENT_ACCELEROMETER == 1

#include <osw_hal.h>
#include <osw_ui.h>

#include <OswAppV1.h>

class OswAppBrickBreaker : public OswApp {
  public:
    OswAppBrickBreaker(void) {
        ui = OswUI::getInstance();
    };
    virtual void setup() override;
    virtual void loop() override;
    virtual void stop() override;
    ~OswAppBrickBreaker() {};

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
    const float xSensitivity = 0.75f;
    const float ySensitivity = 0.75f;
    const bool newGrid[4][8] = {
        {0, 0, 1, 1, 1, 1, 0, 0}, {0, 1, 1, 1, 1, 1, 1, 0}, {1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1}
    };

    int score = 0;
    int playerPos = 120;
    bool gameRunning = false;
    unsigned long lastmove = 0;
    int lastpos = 0;

    float ballPosx = 160;
    float ballPosy = 120;
    float ballSpdx = -2;
    float ballSpdy = 4;
    float absspd = 0;
    float angleVar = 0;
    float angleout = 0;
    float angleout2 = 0;
    float pHitPosition = 0;
    float pHitAngle = 0;
    float posAngle = 0;
    float wallPosx = 0;
    float wallPosy = 0;
    float playerSpd = 0;

    bool scoreUpdated = false;
    bool grid[4][8] = {};
    int hasbounced = 0;

    int previousTime = 0;
    int gameStart = 0;
    float spd = 1;

    float deltaSeconds = 0;

    void drawDirection(const int xDirection, const int yDirection);
    void drawDirectionArrow(const int direction, const int topLeftX = 120, const int topLeftY = 13);
    void drawPlayer();
    void blockHit();
    void drawGrid();
    void drawScore();
    void drawButtonHints();
    void drawBall();
    void initGrid();
    void moveBall();
    void hitPlayer();
    void hitWall();

    // Game logics
    void BrickBreaker();
    void blockHit(const int x, const int y);
    bool coordsInGame(const int xCoord, const int yCoord);
    void resetGame(void);

    // Controls
    void buttonController();
    void accelerometerController();
    void gravityController();

    void waitingRoom();
};
#endif
