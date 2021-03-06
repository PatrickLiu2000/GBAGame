// Patrick Liu
#include <stdio.h>
#include <stdlib.h>
#include "gba.h"
#include "game.h"
                    /* TODO: */
// Include any header files for title screen or exit
// screen images generated by nin10kit. Example for the provided garbage
// image:
// #include "images/garbage.h"
#include "images/startScreen.h"
#include "images/gameBackground.h"
#include "images/heart.h"

                    /* TODO: */
// Add any additional states you need for your app.
typedef enum {
    DRAWSTART,
    START,
    PLAY,
    LIFELOST,
    WIN,
    DRAWLOSE,
    LOSE,
} GBAState;


int main(void) {
                    /* TODO: */
    // Manipulate REG_DISPCNT here to set Mode 3. //
    REG_DISPCNT = MODE3 | BG2_ENABLE;

    // Save current and previous state of button input.
    u32 previousButtons = BUTTONS;
    u32 currentButtons = BUTTONS;

    // Load initial game state
    GBAState state = DRAWSTART;
    struct paddle {
        int x;
        int y;
        int width;
        int height;
    } paddle, prevPaddle;

    struct ball {
        int x;
        int y;
        int size;
        int deltaY;
        int deltaX;
    } ball, prevBall;

    struct levelBlock {
        int x;
        int y;
        int width;
        int height;
        int outerLayer;
    };

    paddle.x = WIDTH / 2 - 13;
    paddle.y = HEIGHT - 30;
    paddle.width = 30;
    paddle.height = 5;

    ball.x = WIDTH / 2 - 35;
    ball.y = HEIGHT - 50;
    ball.size = 6;
    ball.deltaX = 1;
    ball.deltaY = 1;
    struct levelBlock blockArr[64];
    int numLives = 5;
    int numBricksLeft = 64;
    while (1) {
        currentButtons = BUTTONS; // Load the current state of the buttons


                    /* TODO: */
        // Manipulate the state machine below as needed //
        // NOTE: Call waitForVBlank() before you draw
        waitForVBlank();
        switch(state) {
            case DRAWSTART:
                drawFullScreenImageDMA(startBackground);
                state = START;
                break;
            case START:
                if (KEY_DOWN(BUTTON_A, currentButtons)) {
                    state = PLAY;
                    fillScreenDMA(BLACK);
                    int index = 0;
                    for (int j = 1; j < HEIGHT / 4; j+= 5) {
                        for (int i = 0; i < WIDTH; i+=30) {
                            drawRectDMA(i, j, 29, 4, WHITE);
                            blockArr[index].x = i;
                            blockArr[index].y = j;
                            blockArr[index].width = 29;
                            blockArr[index].height = 4;
                            blockArr[index].outerLayer = 0;
                            if (index >= 56) {
                                blockArr[index].outerLayer = 1;
                            }
                            index++;
                        }
                    }
                    ball.x = WIDTH / 2 - 35;
                    ball.y = HEIGHT - 50;
                    ball.size = 6;
                    ball.deltaX = 1;
                    ball.deltaY = 1;

                    paddle.x = WIDTH / 2 - 13;
                    paddle.y = HEIGHT - 30;
                    paddle.width = 30;
                    paddle.height = 5;
                    numLives = 5;
                    numBricksLeft = 64;
                }
                break;
            case PLAY:
                prevPaddle = paddle;
                prevBall = ball;
                // drawLevel(); // should be random
                if (ball.y <= HEIGHT / 4) {
                    for (int i = 0; i < 64; i++) {
                        if (blockArr[i].outerLayer == 1) {
                            if (ball.y <= blockArr[i].y + blockArr[i].height && ball.x + ball.size > blockArr[i].x && ball.x < blockArr[i].x + blockArr[i].width) {
                                ball.y = blockArr[i].y + blockArr[i].height;
                                ball.deltaY = -ball.deltaY;
                                blockArr[i].outerLayer = 0;
                                numBricksLeft--;
                                if (i >= 8) {
                                    blockArr[i - 8].outerLayer = 1;
                                }
                                drawRectDMA(blockArr[i].x, blockArr[i].y, blockArr[i].width, blockArr[i].height, BLACK);
                            } else {
                                if (ball.y <= blockArr[i].y + blockArr[i].height) {
                                    
                                    if (ball.x + (2 * ball.size) == blockArr[i].x){ // left side
                                        ball.x = ball.x + (2 * ball.size);
                                        ball.deltaX = -ball.deltaX;
                                        blockArr[i].outerLayer = 0;
                                        numBricksLeft--;
                                        if (i >= 8) {
                                            blockArr[i - 8].outerLayer = 1;
                                        }
                                        drawRectDMA(blockArr[i].x, blockArr[i].y, blockArr[i].width, blockArr[i].height, BLACK);
                                    }
                                    if (ball.x == blockArr[i].x + blockArr[i].width) { // right side
                                        ball.x = blockArr[i].x + blockArr[i].width;
                                        ball.deltaX = -ball.deltaX;
                                        blockArr[i].outerLayer = 0;
                                        numBricksLeft--;
                                        if (i >= 8) {
                                            blockArr[i - 8].outerLayer = 1;
                                        }
                                        drawRectDMA(blockArr[i].x, blockArr[i].y, blockArr[i].width, blockArr[i].height, BLACK);
                                    }
                                }
                            }
                        }
                    }
                }
                if (KEY_DOWN(BUTTON_LEFT,currentButtons) && paddle.x > 1) {
                    paddle.x-=2;
                }
                if (KEY_DOWN(BUTTON_RIGHT, currentButtons) && paddle.x < WIDTH - paddle.width - 1) {
                    paddle.x+=2;
                }

                ball.x += ball.deltaX;
                ball.y += ball.deltaY;
                
                if (ball.x >= WIDTH-ball.size) { // right
                    ball.x = WIDTH-ball.size;
                    ball.deltaX = -ball.deltaX;
                }
                if (ball.y >= HEIGHT-ball.size) { // bottom
                    ball.y = HEIGHT-ball.size;
                    ball.deltaY = -ball.deltaY;
                }
                if (ball.x < 0) {
                    ball.x = 0;
                    ball.deltaX = -ball.deltaX;
                }
                if (ball.y < 0) { // top
                    ball.y = 0;
                    ball.deltaY= -ball.deltaY;
			    }
                // bounce if hits paddle, added user control depending on which direction paddle is moving
                if (ball.y >= paddle.y - (ball.size / 2) && ball.x >= paddle.x && ball.x <= paddle.x + paddle.width && ball.y <= paddle.y + paddle.height) {
                    ball.y = paddle.y - ball.size;
                    ball.deltaY = -ball.deltaY;
                    if ((KEY_DOWN(BUTTON_LEFT,currentButtons) && ball.deltaX > 0) || (KEY_DOWN(BUTTON_RIGHT, currentButtons) && ball.deltaX < 0)) {
                        ball.deltaX = -ball.deltaX;
                    }
                }
                erase(prevPaddle.x, prevPaddle.y, prevPaddle.width, prevPaddle.height);
                erase(prevBall.x, prevBall.y, prevBall.size, prevBall.size);
                erase(WIDTH / 2 + 20, HEIGHT - 20, 6, 8);
                erase(5, HEIGHT - 20, 50, 8);

                drawBall(ball.x, ball.y, ball.size, ball.size);
                drawPaddle(paddle.x, paddle.y, paddle.width, paddle.height);
                char buffer[64];
                snprintf(buffer, 64, "Score:%d", 64 - numBricksLeft);
                drawString(WIDTH / 2 - 60, HEIGHT - 20, "Lives: ", WHITE);
                drawString(5, HEIGHT - 20, buffer, WHITE);
                for (int i = 0; i < numLives; i++) {
                    drawImageDMA(WIDTH / 2 - 20 + (i * 15), HEIGHT - 20, 10, 10, heart);
                }
                if (ball.y == HEIGHT - ball.size) {
                    numLives--;
                    drawRectDMA(WIDTH / 2 - 20 + (numLives * 15), HEIGHT - 20, 10, 10, BLACK);
                    state = LIFELOST;
                }
                if (numBricksLeft == 0) {
                    fillScreenDMA(BLACK);
                    drawFullScreenImageDMA(gameBackground);
                    drawCenteredString(60, HEIGHT / 2 + 10, 120, 20, "You Win!",WHITE);
                    drawCenteredString(60, HEIGHT / 2 + 35, 120, 20, "Press A to Restart",WHITE);
                    state = WIN;
                }
                if (numLives == 0) {
                    state = DRAWLOSE;
                }
                break;
            case LIFELOST:
                drawRectDMA(ball.x, ball.y, ball.size, ball.size, BLACK);
                drawRectDMA(paddle.x, paddle.y, paddle.width, paddle. height, BLACK);
                drawCenteredString(WIDTH / 2 - 50, HEIGHT - 60,100, 10, "Lost 1 Life! Press A to Continue", RED);
                if (KEY_DOWN(BUTTON_A, currentButtons)) {
                    state = PLAY;
                    ball.x = WIDTH / 2 - 35;
                    ball.y = HEIGHT - 50;
                    ball.deltaX = 1;
                    ball.deltaY = 1;
                    paddle.x = WIDTH / 2 - 13;
                    drawRectDMA(0, HEIGHT - 60, WIDTH, 10, BLACK);
                }
                break;
            case WIN:
                if (KEY_DOWN(BUTTON_A, currentButtons)) {
                    state = DRAWSTART;
                }
                break;
            case DRAWLOSE:
                fillScreenDMA(BLACK);
                drawFullScreenImageDMA(gameBackground);
                drawCenteredString(60, HEIGHT / 2 + 10, 120, 20, "Press A to Restart!",WHITE);
                state = LOSE;
                break;
            case LOSE:
                drawRectDMA(ball.x, ball.y, ball.size, ball.size, BLACK);
                if (KEY_JUST_PRESSED(BUTTON_A, currentButtons, previousButtons)) {
                    ball.x = WIDTH / 2 - 35;
                    ball.y = HEIGHT - 50;
                    ball.deltaX = 1;
                    ball.deltaY = 1;
                    paddle.x = WIDTH / 2 - 13;
                    state = DRAWSTART;
                    numLives = 5;
                }
                break;
        }
        if (KEY_JUST_PRESSED(BUTTON_SELECT, currentButtons, previousButtons)) {
            fillScreenDMA(BLACK);
            state = DRAWSTART;
        }
        previousButtons = currentButtons; // Store the current state of the buttons
    }
    return 0;
}

void drawPaddle(int x, int y, int width, int height) {
    drawRectDMA(x, y, width, height, WHITE);
}
void erase(int x, int y, int width, int height) {
    drawRectDMA(x, y, width, height, BLACK);
}

void drawBall(int x, int y, int width, int height) {
    drawRectDMA(x,y,width, height, YELLOW);
}