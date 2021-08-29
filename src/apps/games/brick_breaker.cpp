
#include "./apps/games/brick_breaker.h"
// #define GIF_BG

#include <gfx_util.h>
#include <osw_app.h>
#include <osw_config.h>
#include <osw_config_keys.h>
#include <osw_hal.h>

void OswAppBrickBreaker::setup(OswHal* hal) {
#ifdef GIF_BG
  bgGif->setup(hal);
#endif
}

void OswAppBrickBreaker::loop(OswHal* hal) {
  hal->gfx()->fill(ui->getBackgroundColor());

  BrickBreaker(hal);
  hal->requestFlush();
}

void OswAppBrickBreaker::stop(OswHal* hal) {
  // hal->disableDisplayBuffer();

#ifdef GIF_BG
  bgGif->stop(hal);
#endif
}


void OswAppBrickBreaker::drawPlayer(OswHal* hal) {
  hal->gfx()->drawThickLine(playerPos-(playerWidth/2),playerY,playerPos+(playerWidth/2),playerY,playerHeight,ui->getForegroundColor(),true);
}

void OswAppBrickBreaker::drawGrid(OswHal* hal) {
  uint16_t color = 0;
  for (int i = 0;i < gridH;i++){
    for (int j = 0;j<gridW;j++){
      if(grid[i][j] == true){
        color = ui->getForegroundColor();
      } else {
        color = ui->getBackgroundColor();
      }
      hal->gfx()->fillFrame((25+(24*j)),(20+(10*i)),22,8,color);
    }
  }
}

void OswAppBrickBreaker::drawBall(OswHal* hal) {
  hal->gfx()->fillCircle(ballPosx,ballPosy,4,ui->getForegroundColor());
  //hal->gfx()->drawArc(120,120, 0, 360, 60, 80, 0.75,ui->getForegroundColor(),false);
  //hal->gfx()->drawFrame(18,18,203,250,ui->getForegroundColor());
}

void OswAppBrickBreaker::drawScore(OswHal* hal) {
  hal->gfx()->setTextCursor(100, 220);
  hal->gfx()->print(score);
}

void OswAppBrickBreaker::drawButtonHints(OswHal* hal) {
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

void OswAppBrickBreaker::BrickBreaker(OswHal* hal) {
  hal->gfx()->setTextSize(2);

  drawScore(hal);

  if (buttonControllerMode == 0) {
    accelerometerController(hal);

  } else if (buttonControllerMode == 1){
    gravityController(hal);
  } else {
    drawButtonHints(hal);
    buttonController(hal);
  }

  drawScore(hal);

  if (gameRunning) {

    if (previousTime == 0) {
      gameStart = millis();
      previousTime = gameStart;
    }

    const int currentTime = millis();

    deltaSeconds += (currentTime - previousTime) / (1000.0);

    previousTime = currentTime;

    //game lost
   // if (!coordsInGame(xTarget, yTarget) || touchItself()) {
    //  hal->gfx()->setTextSize(2);
     // hal->gfx()->setTextCursor(95, 15);
     // hal->gfx()->print(score);

     // delay(1000);
    //  resetGame();
    //}

    drawGrid(hal);
    moveBall();
    drawBall(hal);
    drawPlayer(hal);
  } else {
    waitingRoom(hal);
  }
  //drawGameState(hal);
}

void OswAppBrickBreaker::waitingRoom(OswHal* hal) {
  initGrid();
  drawGrid(hal);
  drawBall(hal);
  drawPlayer(hal);
  hal->gfx()->setTextSize(2);
  hal->gfx()->setTextCursor(150, 48);
  hal->gfx()->print("Start");

  hal->gfx()->setTextCursor(240 - 180 - 30, 240 - 48);
  switch (buttonControllerMode){
    case 0:
      hal->gfx()->print("Accelerometer");
      break;
    case 1:
      hal->gfx()->print("Gravity");
      break;
    case 2:
      hal->gfx()->print("Buttons");
      break;
  }

  if (hal->btnHasGoneDown(BUTTON_1)) {
    buttonControllerMode = buttonControllerMode + 1;
    if (buttonControllerMode > 2){
      buttonControllerMode = 0;
    }
  }

  if (hal->btnHasGoneDown(BUTTON_3)) {
    score = 0;
    resetGame();
    gameRunning = true;
  }
}

void OswAppBrickBreaker::initGrid(){
  for (int i = 0; i < 4; i++){
    for (int j = 0; j < 8;j++){
      grid[i][j] = newGrid[i][j];
    }
  }
}

void OswAppBrickBreaker::hitPlayer() {
    pHitPosition = ballPosx - playerPos;
    absspd = sqrt((ballSpdx*ballSpdx)+(ballSpdy*ballSpdy));
    pHitAngle = atan(-ballSpdy/ballSpdx);
    posAngle = asin(pHitPosition/24);
    angleVar = pHitAngle + posAngle;
    angleVar = (PI + angleVar*2);
    angleout = pHitAngle - angleVar;
    angleout2 = PI + angleout;
    if (pHitPosition >= 0){
        if (ballSpdx >= 0){
            ballSpdx = cos(angleout2)*absspd;
            ballSpdy = -sin(angleout2)*absspd;
        } else {
            ballSpdx = -cos(angleout2)*absspd;
            ballSpdy = sin(angleout2)*absspd;
        }
    } else {
        if (ballSpdx >= 0){
            ballSpdx = cos(angleout2)*absspd;
            ballSpdy = -sin(angleout2)*absspd;
        } else {
            ballSpdx = -cos(angleout2)*absspd;
            ballSpdy = sin(angleout2)*absspd;
        }    
    }
}

void OswAppBrickBreaker::hitWall() {

  wallPosx = ballPosx - 120;
  wallPosy = -(ballPosy - 120);
  absspd = sqrt((ballSpdx*ballSpdx)+(ballSpdy*ballSpdy));
    
  pHitAngle = atan(-ballSpdy/ballSpdx);
  posAngle = asin(wallPosy/116);
  angleVar = pHitAngle + posAngle;

  if (wallPosx <= 0){
    angleVar = pHitAngle + posAngle;
  } else {
    angleVar = pHitAngle - posAngle;
  }
  angleVar = angleVar*2;
  angleout = (pHitAngle - angleVar);
    
  if (wallPosx <= 0 && wallPosy >= 0) {
    //quadrant 2 (top left) /
    if (ballSpdx >= 0 && ballSpdy <= 0) {
      //quadrant 2 x+ y- /
      ballSpdx = -cos(angleout)*absspd;
      ballSpdy = sin(angleout)*absspd;
    } else if (ballSpdx <= 0 && ballSpdy <= 0) {
      //quadrant 2 x- y- /
      ballSpdx = cos(angleout)*absspd;
      ballSpdy = -sin(angleout)*absspd;
    } else { 
      //quadrant 2 x- y+ /
      ballSpdx = -cos(angleout)*absspd;
      ballSpdy = -sin(angleout)*absspd;
    }
    } else if (wallPosx <= 0 && wallPosy <= 0) {
      //quadrant 3 (bottom left) /
      if (ballSpdx <= 0 && ballSpdy >=0) {
        //quadrant 3 x- y+ /
        ballSpdx = cos(angleout)*absspd;
        ballSpdy = -sin(angleout)*absspd;
      } else if (ballSpdx >= 0 && ballSpdy >= 0) {
        //quadrant 3 x+ y+ /
        ballSpdx = -cos(angleout)*absspd;
        ballSpdy = sin(angleout)*absspd;
      } else if (ballSpdx <= 0 && ballSpdy <= 0) {
        //quadrant 3 x- y- /
        ballSpdx = -sin(angleout)*absspd;
        ballSpdy = cos(angleout)*absspd;
      }
      } else if (wallPosx >= 0 && wallPosy >= 0){
        //quadrant 1 (top right) /
        if (ballSpdx >= 0 && ballSpdy <=0) {
            //quadrant 1 x+ y- /
            ballSpdx = -cos(angleout)*absspd;
            ballSpdy = -sin(angleout)*absspd;
        } else if (ballSpdx >= 0 && ballSpdy >= 0) {
            ////quadrant 1 x+ y+
            ballSpdx = -cos(angleout)*absspd;
            ballSpdy = sin(angleout)*absspd;
        } else if (ballSpdx <= 0 && ballSpdy <= 0) {
            //quadrant 1 x- y- /
            angleVar = pHitAngle - posAngle;
            angleVar = (angleVar*2);
            angleout = (pHitAngle - angleVar);
            ballSpdx = cos(angleout)*absspd;
            ballSpdy = -sin(angleout)*absspd;
        }
    } else {
        //quadrant 4 (bottom right) /
        if (ballSpdx <= 0 && ballSpdy >=0) {
            //quadrant 4 x- y+ /
            ballSpdx = cos(angleout)*absspd;
            ballSpdy = -sin(angleout)*absspd;
        } else if (ballSpdx >= 0 && ballSpdy <= 0) {
            //quadrant 4 x+ y- /
            ballSpdx = -cos(angleout)*absspd;
            ballSpdy = sin(angleout)*absspd;
        } else if (ballSpdx >= 0 && ballSpdy >= 0) {
            //quadrant 4 x+ y+ /
            ballSpdx = -cos(angleout)*absspd;
            ballSpdy = sin(angleout)*absspd;
        }
    }
}

void OswAppBrickBreaker::resetGame() {

  gameRunning = false;
  previousTime = 0;
  ballPosx = 160;
  ballPosy = 120;
  ballSpdx = -2;
  ballSpdy = 3;
  playerPos = 120;
  hasbounced = 0;
  playerSpd = 0;
  initGrid();

  gameStart = 0;
}

void OswAppBrickBreaker::buttonController(OswHal* hal) {
  // Bottom right
  if (hal->btnHasGoneDown(BUTTON_2)) {
    playerPos = playerPos + 15;
      if (playerPos >= 207) {
        playerPos = 207;
      }

  }
  // Bottom left
  else if (hal->btnHasGoneDown(BUTTON_1)) {
    playerPos = playerPos - 15;
      if (playerPos <= 30) {
        playerPos = 30;
      }
  }
}

void OswAppBrickBreaker::accelerometerController(OswHal* hal) {
  float xAcceleration = hal->getAccelerationX();
  playerPos = -(xAcceleration * 100)+120;
    if (playerPos <= 30) {
      playerPos = 30;
    }
    if (playerPos >= 207) {
      playerPos = 207;
    }

}

void OswAppBrickBreaker::gravityController(OswHal* hal) {
  float xAcceleration = hal->getAccelerationX();
  float realAcceleration = -xAcceleration * 59.3346774;
  playerSpd = playerSpd + (realAcceleration * (millis()-lastpos));
  playerPos = playerPos + (playerSpd * (millis()-lastpos));
  if (playerPos <= 30) {
    playerPos = 30;
  }
  if (playerPos >= 207) {
    playerPos = 207;
  }

  lastpos = (millis());
}


void OswAppBrickBreaker::moveBall() {

  if (ballPosy >= playerY-7 && ballPosx <= playerPos + (playerWidth/2)+playerHeight && ballPosx >= playerPos - (playerWidth/2)-playerHeight){
    hitPlayer();
  }  else if (ballPosy > playerY-7){
    resetGame();
  }

   if (((((ballPosx - 120)*(ballPosx - 120)) + ((ballPosy - 120)*(ballPosy - 120))) >= 13456)){
     hitWall();
  //if (ballPosx <= 22 || ballPosx >= 218){
  //  ballSpdx = -ballSpdx;
  //}
  //if (ballPosy <= 22){
  //  ballSpdy = -ballSpdy;
  }
  for (int j = 0;j<4;j++){

    if (ballPosy <= (28 + (10*j)) && ballPosy > (20+(10*j))){
        for (int i = 0; i < 8; i++){
          if (ballSpdx < 0){ //right brick hit
            if (ballPosx <= (51 + (i*24)) && ballPosx >= (48 + (i*24)) && grid[j][i] == true){
              ballSpdx = -ballSpdx;
              score = score + 1;
              grid[j][i] = false;
            }
          } else { //left brick hit
            if (ballPosx >= (21 + (i*24)) && ballPosx <= (24 + (i*24)) && grid[j][i] == true){
              ballSpdx = -ballSpdx;
              score = score + 1;
              grid[j][i] = false;
            }
          }
          
      }
    }
    if (ballPosy <= (32 + (10*j)) && ballPosy > (28+(10*j)) && ballSpdy < 0){
      for (int i = 0; i < 8; i++){
        if (ballPosx >= (25 + (i*24)) && ballPosx <= (47 + (i*24)) && grid[j][i] == true){ //bottom brick hit
          ballSpdy = -ballSpdy;
          score = score + 1;
          grid[j][i] = false;
        }
      }
    }
    if (ballPosy >= (16 + (10*j)) && ballPosy < (19+(10*j)) && ballSpdy > 0){  //top brick hit
      for (int i = 0; i < 8; i++){
        if (ballPosx >= (25 + (i*24)) && ballPosx <= (47 + (i*24)) && grid[j][i] == true){
          ballSpdy = -ballSpdy;
          score = score + 1;
          grid[j][i] = false;
        }
      }
    }
  }


  if (millis() >= lastmove + 33){
    ballPosy = ballPosy + ballSpdy * spd;
    ballPosx = ballPosx + ballSpdx * spd;
    lastmove = millis();
    // if (hasbounced >= 1){
    //   hasbounced++;
    // }
    // if (hasbounced >= 10){
    //   hasbounced = 0;
    //}  


  }

}
