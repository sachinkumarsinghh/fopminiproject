#include <SDL.h>
#include <stdlib.h>
#include <math.h>   
#include <time.h>
static void drawCircle(SDL_Renderer *renderer, int cx, int cy, int r) {
    for (int w = 0; w < r * 2; w++) {
        for (int h = 0; h < r * 2; h++) {
            int dx = r - w;  
            int dy = r - h;  
            if ((dx*dx + dy*dy) <= (r * r)) {
                SDL_RenderDrawPoint(renderer, cx + dx, cy + dy);
            }
        }
    }
}
static void drawDigit(SDL_Renderer *renderer, int x, int y, int size, int d) {
    int segW = size/6;
    int segL = size;
    SDL_Rect seg;
    int segOn[10][7] = {
        {1,1,1,0,1,1,1}, /*0*/
        {0,0,1,0,0,1,0}, /*1*/
        {1,0,1,1,1,0,1}, /*2*/
        {1,0,1,1,0,1,1}, /*3*/
        {0,1,1,1,0,1,0}, /*4*/
        {1,1,0,1,0,1,1}, /*5*/
        {1,1,0,1,1,1,1}, /*6*/
        {1,0,1,0,0,1,0}, /*7*/
        {1,1,1,1,1,1,1}, /*8*/
        {1,1,1,1,0,1,1}  /*9*/
    };
    if (segOn[d][0]) {
        seg.x = x + segW; seg.y = y; seg.w = segL; seg.h = segW;
        SDL_RenderFillRect(renderer, &seg);
    }
    if (segOn[d][1]) {
        seg.x = x; seg.y = y + segW; seg.w = segW; seg.h = segL;
        SDL_RenderFillRect(renderer, &seg);
    }
    if (segOn[d][2]) {
        seg.x = x + segW + segL; seg.y = y + segW; seg.w = segW; seg.h = segL;
        SDL_RenderFillRect(renderer, &seg);
    }
    if (segOn[d][3]) {
        seg.x = x + segW; seg.y = y + segW + segL; seg.w = segL; seg.h = segW;
        SDL_RenderFillRect(renderer, &seg);
    }
    if (segOn[d][4]) {
        seg.x = x; seg.y = y + segW + segL + segW; seg.w = segW; seg.h = segL;
        SDL_RenderFillRect(renderer, &seg);
    }
    if (segOn[d][5]) {
        seg.x = x + segW + segL; seg.y = y + segW + segL + segW; seg.w = segW; seg.h = segL;
        SDL_RenderFillRect(renderer, &seg);
    }
    if (segOn[d][6]) {
        seg.x = x + segW; seg.y = y + segW + segL + segW + segL; seg.w = segL; seg.h = segW;
        SDL_RenderFillRect(renderer, &seg);
    }
}
static void drawNumber(SDL_Renderer *renderer, int centerX, int y, int size, int num) {
    int tens = num / 10;
    int ones = num % 10;
    int digitWidth = size + size/3 + size/6; /* segL + spacing + segW */
    if (num >= 10) {
        drawDigit(renderer, centerX - digitWidth/2 - (digitWidth/2), y, size, tens);
        drawDigit(renderer, centerX + digitWidth/2 - (digitWidth/2), y, size, ones);
    } else {
        drawDigit(renderer, centerX - digitWidth/2, y, size, ones);
    }
}
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define PADDLE_HEIGHT 80
#define PADDLE_WIDTH 10
#define BALL_SIZE 10

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Pong",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          0);
    if (!window) {
        SDL_Log("Could not create window: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED |
                                                SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        SDL_Log("Could not create renderer: %s", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    int leftPaddleY = SCREEN_HEIGHT/2 - PADDLE_HEIGHT/2;
    int rightPaddleY = SCREEN_HEIGHT/2 - PADDLE_HEIGHT/2;
    int ballX = SCREEN_WIDTH/2;
    int ballY = SCREEN_HEIGHT/2;
    int ballDirX = 4;
    int ballDirY = 4;
    int scoreLeft = 0;
    int scoreRight = 0;
    const int winScore = 11;
    int gameOver = 0; 
    int winner = 0; 

    srand((unsigned)time(NULL));

    SDL_bool running = SDL_TRUE;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                running = SDL_FALSE;
            } else if (e.type == SDL_KEYDOWN) {
                if (e.key.keysym.sym == SDLK_ESCAPE) {
                    running = SDL_FALSE;
                }
                if (gameOver && e.key.keysym.sym == SDLK_SPACE) {
                    scoreLeft = 0;
                    scoreRight = 0;
                    gameOver = 0;
                    winner = 0;
                    ballX = SCREEN_WIDTH/2;
                    ballY = SCREEN_HEIGHT/2;
                    ballDirX = (rand() & 1) ? 4 : -4;
                    ballDirY = (rand() & 1) ? 3 : -3;
                }
            }
        }
        const Uint8 *state = SDL_GetKeyboardState(NULL);
        if (!gameOver) {
            if (state[SDL_SCANCODE_W]) leftPaddleY -= 5;
            if (state[SDL_SCANCODE_S]) leftPaddleY += 5;
            if (state[SDL_SCANCODE_UP]) rightPaddleY -= 5;
            if (state[SDL_SCANCODE_DOWN]) rightPaddleY += 5;
        }
        if (leftPaddleY < 0) leftPaddleY = 0;
        if (leftPaddleY > SCREEN_HEIGHT - PADDLE_HEIGHT)
            leftPaddleY = SCREEN_HEIGHT - PADDLE_HEIGHT;
        if (rightPaddleY < 0) rightPaddleY = 0;
        if (rightPaddleY > SCREEN_HEIGHT - PADDLE_HEIGHT)
            rightPaddleY = SCREEN_HEIGHT - PADDLE_HEIGHT;

        if (!gameOver) {
            ballX += ballDirX;
            ballY += ballDirY;
        }

        if (ballY - BALL_SIZE/2 <= 0 || ballY + BALL_SIZE/2 >= SCREEN_HEIGHT)
            ballDirY = -ballDirY;
        if (ballX - BALL_SIZE/2 <= 30 &&
            ballY >= leftPaddleY &&
            ballY <= leftPaddleY + PADDLE_HEIGHT)
            ballDirX = -ballDirX;

        if (ballX + BALL_SIZE/2 >= SCREEN_WIDTH - 40 &&
            ballY >= rightPaddleY &&
            ballY <= rightPaddleY + PADDLE_HEIGHT)
            ballDirX = -ballDirX;

        if (!gameOver) {
            if (ballX - BALL_SIZE/2 <= 0) {
                scoreRight++;
                if (scoreRight >= winScore) { gameOver = 1; winner = 2; }
                ballX = SCREEN_WIDTH/2; ballY = SCREEN_HEIGHT/2;
                ballDirX = (rand() & 1) ? 4 : -4;
            } else if (ballX + BALL_SIZE/2 >= SCREEN_WIDTH) {
                scoreLeft++;
                if (scoreLeft >= winScore) { gameOver = 1; winner = 1; }
                ballX = SCREEN_WIDTH/2; ballY = SCREEN_HEIGHT/2;
                ballDirX = (rand() & 1) ? 4 : -4;
            }
        }
        SDL_SetRenderDrawColor(renderer, 25, 100, 200, 255); /* blueish sky */
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
        for(int y = 0; y < SCREEN_HEIGHT; y += 20) {
            SDL_RenderDrawLine(renderer, SCREEN_WIDTH/2, y,
                               SCREEN_WIDTH/2, y + 10);
        }

        SDL_SetRenderDrawColor(renderer, 240, 240, 240, 255);
        int headRadius = PADDLE_HEIGHT/2; 
        int handleLength = 20;
        int handleWidth = 8;

        int leftHeadX = 20 + headRadius;
        int leftHeadY = leftPaddleY + PADDLE_HEIGHT/2;
        drawCircle(renderer, leftHeadX, leftHeadY, headRadius);
        SDL_Rect leftHandle = {leftHeadX, leftHeadY - handleWidth/2,
                                handleLength, handleWidth};
        SDL_RenderFillRect(renderer, &leftHandle);

        
        int rightHeadX = SCREEN_WIDTH - 30 - headRadius;
        int rightHeadY = rightPaddleY + PADDLE_HEIGHT/2;
        drawCircle(renderer, rightHeadX, rightHeadY, headRadius);
        SDL_Rect rightHandle = {rightHeadX - handleLength, rightHeadY - handleWidth/2,
                                 handleLength, handleWidth};
        SDL_RenderFillRect(renderer, &rightHandle);

        SDL_SetRenderDrawColor(renderer, 255, 80, 80, 255);
        drawCircle(renderer, ballX, ballY, BALL_SIZE/2);

        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        drawNumber(renderer, SCREEN_WIDTH/4, 30, 18, scoreLeft);
        drawNumber(renderer, 3*SCREEN_WIDTH/4, 30, 18, scoreRight);

        if (gameOver) {
            /* draw a semi-transparent overlay */
            SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 160);
            SDL_Rect overlay = {SCREEN_WIDTH/4, SCREEN_HEIGHT/3, SCREEN_WIDTH/2, SCREEN_HEIGHT/3};
            SDL_RenderFillRect(renderer, &overlay);
            /* draw winner large number */
            SDL_SetRenderDrawColor(renderer, 255, 215, 0, 255);
            if (winner == 1) drawNumber(renderer, SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 - 20, 30, scoreLeft);
            else drawNumber(renderer, SCREEN_WIDTH/2 - 40, SCREEN_HEIGHT/2 - 20, 30, scoreRight);
            /* hint to restart */
            SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            /* small bar representing "Press SPACE to restart" */
            SDL_Rect hint = {SCREEN_WIDTH/2 - 80, SCREEN_HEIGHT/2 + 40, 160, 6};
            SDL_RenderFillRect(renderer, &hint);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(30);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}