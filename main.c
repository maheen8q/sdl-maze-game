#include <SDL3/SDL.h>
#include <time.h>
#include <stdlib.h>
#include<stdio.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>



#define ROWS 31
#define COLUMNS 31
#define MAX_COINS 7

typedef struct {
    int x;
    int y;
    int collected; 
} Coin;

Coin coins[MAX_COINS];


typedef struct {
    int x;
    int y;
    int steps;
    int coins;
    int totalCoins;
    int key;
    time_t startTime;
} Player;

typedef enum {
    STATE_MENU,
    STATE_RULES,
    STATE_PLAY,
    STATE_HIGHSCORE,
    STATE_GAMEOVER,
    STATE_QUIT
} GameState;


typedef struct {
    SDL_FRect rect;
    char text[20];
} Button;

int dx[4] = { -2, 2, 0, 0 };
int dy[4] = { 0, 0, -2, 2 };

char message[100] = "";
Uint32 messageTime = 0;


void drawMaze(SDL_Renderer *renderer, char maze[ROWS][COLUMNS], Player *player, SDL_Window *window,
              SDL_Texture *playerTex, SDL_Texture *keyTex, SDL_Texture *doorTex, SDL_Texture *wallTex, SDL_Texture *floorTex, SDL_Texture *coinTex ) 
{
    int size = 24;
    int playersize = 24;
    int itemsize = 28;
    

   float centerOffsetPlayer = (playersize - size) / 2.0f;
   float centerOffsetItem   = (itemsize   - size) / 2.0f;

   int windowW, windowH;
   SDL_GetWindowSize(window, &windowW, &windowH);



  int offsetX = (windowW - (COLUMNS * size)) / 2;
  int offsetY = (windowH - (ROWS * size)) / 2;

    SDL_FRect rect;
    rect.w = rect.h = size;

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLUMNS; j++) {
            rect.x = j * size + offsetX;
            rect.y = i * size + offsetY;

            // Draw floor first if it's a path
            if (maze[i][j] == ' ') {
                SDL_RenderTexture(renderer, floorTex, NULL, &rect);
            }
            // Draw wall
            else if (maze[i][j] == '#') {
                SDL_RenderTexture(renderer, wallTex, NULL, &rect);
            }
            // Draw key
            else if (maze[i][j] == 'K') {
                SDL_RenderTexture(renderer, floorTex, NULL, &rect); 
                SDL_FRect keyRect = {
                rect.x - centerOffsetItem,
                 rect.y - centerOffsetItem,
                 itemsize,
                 itemsize
                 };
                
                SDL_RenderTexture(renderer, keyTex, NULL, &keyRect);
            }
            // Draw door
            else if (maze[i][j] == 'D') {
                SDL_RenderTexture(renderer, floorTex, NULL, &rect);
                 
                SDL_FRect doorRect = {
                 rect.x - centerOffsetItem,
                 rect.y - centerOffsetItem,
                 itemsize,
                 itemsize
                 };
                SDL_RenderTexture(renderer, doorTex, NULL, &doorRect);
            }
        }
    }

    for (int i = 0; i < MAX_COINS; i++) {
    if (!coins[i].collected) {
        SDL_FRect coinRect = {
            coins[i].y * size + offsetX - centerOffsetItem,
            coins[i].x * size + offsetY - centerOffsetItem,
            itemsize,
            itemsize
        };
        SDL_RenderTexture(renderer, coinTex, NULL, &coinRect);
    }
}

    // Draw player on top
    rect.x = player->y * size + offsetX;
    rect.y = player->x * size + offsetY;

    SDL_FRect playerRect = {
    player->y * size + offsetX - centerOffsetPlayer,
    player->x * size + offsetY - centerOffsetPlayer,
    playersize,
    playersize
   };
    SDL_RenderTexture(renderer, playerTex, NULL, &playerRect);
}

    

void placeExit(char maze[ROWS][COLUMNS]) {
    for (int i = ROWS - 2; i > 0; i--) {
        for (int j = COLUMNS - 2; j > 0; j--) {
            if (maze[i][j] == ' ') {
                maze[i][j] = 'D';
                return;
            }
        }
    }
}

void placeKey(char maze[ROWS][COLUMNS]) {
    while (1) {
        int r = 1 + rand() % (ROWS - 2);
        int c = 1 + rand() % (COLUMNS - 2);
        if (maze[r][c] == ' ') {
            maze[r][c] = 'K';
            return;
        }
    }
}

void shuffle(int *arr, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int t = arr[i];
        arr[i] = arr[j];
        arr[j] = t;
    }
}

void carveMaze(char maze[ROWS][COLUMNS], int x, int y) {
    int dirs[4] = {0, 1, 2, 3};
    shuffle(dirs, 4);

    for (int i = 0; i < 4; i++) {
        int d = dirs[i];
        int nx = x + dx[d];
        int ny = y + dy[d];

        if (nx > 0 && nx < ROWS - 1 && ny > 0 && ny < COLUMNS - 1) {
            if (maze[nx][ny] == '#') {
                maze[(x + nx) / 2][(y + ny) / 2] = ' ';
                maze[nx][ny] = ' ';
                carveMaze(maze, nx, ny);
            }
        }
    }
}

void generateMaze(char maze[ROWS][COLUMNS]) {
    
    for(int i=0;i<ROWS;i++)
        for(int j=0;j<COLUMNS;j++)
            maze[i][j] = '#';

    int startX = 1, startY = 1;
    maze[startX][startY] = ' ';


     maze[1][1] = ' ';
    carveMaze(maze, 1, 1);

    maze[1][1]=' ';
    placeExit(maze);
    placeKey(maze);
}

void placeCoins(char maze[ROWS][COLUMNS], Player player, Coin coins[], int maxCoins) {
    int i = 0;
    while (i < maxCoins) {
        int x = 1 + rand() % (ROWS - 2);
        int y = 1 + rand() % (COLUMNS - 2);

        if (maze[x][y] == ' ' && !(x == player.x && y == player.y)) {
            int conflict = 0;
            for (int j = 0; j < i; j++) {
                if (coins[j].x == x && coins[j].y == y) {
                    conflict = 1;
                    break;
                }
            }
            if (!conflict) {
                coins[i].x = x;
                coins[i].y = y;
                coins[i].collected = 0;
                i++;
            }
        }
    }
}

void checkPlayerCollision(Player *player, Coin coins[], int maxCoins) {
    for (int i = 0; i < maxCoins; i++) {
        if (!coins[i].collected && player->x == coins[i].x && player->y == coins[i].y) {
            coins[i].collected = 1;
            player->coins++;
            player->totalCoins++;
            sprintf(message, "Collected a coin! Total coins: %d\n", player->coins);
            messageTime = SDL_GetTicks();
        }
    }
}

void drawButton(SDL_Renderer *renderer, TTF_Font *font, Button *btn) {
    SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
    SDL_RenderFillRect(renderer, &btn->rect);

    SDL_Color white = {255,255,255,255};
    SDL_Surface *s = TTF_RenderText_Blended(font, btn->text, strlen(btn->text), white);
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);

    SDL_FRect textRect = {
        btn->rect.x + (btn->rect.w - s->w) / 2,
        btn->rect.y + (btn->rect.h - s->h) / 2,
        s->w, s->h
    };

    SDL_RenderTexture(renderer, t, NULL, &textRect);

    SDL_DestroySurface(s);
    SDL_DestroyTexture(t);
}


int isClicked(Button *btn, float x, float y) {
    return x >= btn->rect.x &&
           x <= btn->rect.x + btn->rect.w &&
           y >= btn->rect.y &&
           y <= btn->rect.y + btn->rect.h;
}

int getHighScore(char *bestUser) {
    strcpy(bestUser, "Player1"); // placeholder
    return 100;                  // placeholder score
}

int calculateSCore(Player *player) {
    return player->coins * 10;   // simple score formula
}

void renderGameOver(SDL_Renderer *renderer, TTF_Font *font, int finalScore, int highScore) {
    char text[100];
    snprintf(text, sizeof(text), "Game Over! Score: %d, High Score: %d", finalScore, highScore);

    SDL_Color yellow = {255, 255, 0, 255};
    SDL_Surface *s = TTF_RenderText_Blended(font, text, strlen(text), yellow);
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);
    SDL_FRect rect = {200, 300, s->w, s->h};
    SDL_RenderTexture(renderer, t, NULL, &rect);
    SDL_DestroySurface(s);
    SDL_DestroyTexture(t);
}

void renderHighScore(SDL_Renderer *renderer, TTF_Font *font) {
    char bestUser[30] = "";  // <-- INITIALIZE THE ARRAY
    int highScore = getHighScore(bestUser);

    char text[100];
    snprintf(text, sizeof(text),
             "High Score: %d by %s", highScore, bestUser);

    SDL_Color yellow = {255,255,0,255};
    SDL_Surface *s = TTF_RenderText_Blended(font, text, strlen(text), yellow);
    SDL_Texture *t = SDL_CreateTextureFromSurface(renderer, s);

    SDL_FRect rect = {200, 250, s->w, s->h};
    SDL_RenderTexture(renderer, t, NULL, &rect);

    SDL_DestroySurface(s);
    SDL_DestroyTexture(t);
}


SDL_Texture* loadTexture(SDL_Renderer *renderer, const char *path)
{
    SDL_Texture *texture = IMG_LoadTexture(renderer, path);
    if (!texture) {
        printf("Failed to load texture %s: %s\n", path, SDL_GetError());
        return NULL;
    }
    return texture;
}

void renderRules(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color yellow = {255, 255, 0, 255};
    
    int yOffset = 80;
    int lineHeight = 35;

    // Title
    SDL_Surface *titleSurf = TTF_RenderText_Blended(font, "GAME RULES", strlen("GAME RULES"), yellow);
    if (titleSurf) {
        SDL_Texture *titleTex = SDL_CreateTextureFromSurface(renderer, titleSurf);
        if (titleTex) {
            SDL_FRect titleRect = {250, yOffset, (float)titleSurf->w, (float)titleSurf->h};
            SDL_RenderTexture(renderer, titleTex, NULL, &titleRect);
            SDL_DestroyTexture(titleTex);
        }
        SDL_DestroySurface(titleSurf);
    }

    yOffset += 60;

    // Rule 1
    const char *rule1 = "1. Use W/A/S/D keys to move around the maze";
    SDL_Surface *rule1Surf = TTF_RenderText_Blended(font, rule1, strlen(rule1), white);
    if (rule1Surf) {
        SDL_Texture *rule1Tex = SDL_CreateTextureFromSurface(renderer, rule1Surf);
        if (rule1Tex) {
            SDL_FRect rule1Rect = {50, yOffset, (float)rule1Surf->w, (float)rule1Surf->h};
            SDL_RenderTexture(renderer, rule1Tex, NULL, &rule1Rect);
            SDL_DestroyTexture(rule1Tex);
        }
        SDL_DestroySurface(rule1Surf);
    }
    yOffset += lineHeight;

    // Rule 2
    const char *rule2 = "2. Collect all coins scattered throughout the maze";
    SDL_Surface *rule2Surf = TTF_RenderText_Blended(font, rule2, strlen(rule2), white);
    if (rule2Surf) {
        SDL_Texture *rule2Tex = SDL_CreateTextureFromSurface(renderer, rule2Surf);
        if (rule2Tex) {
            SDL_FRect rule2Rect = {50, yOffset, (float)rule2Surf->w, (float)rule2Surf->h};
            SDL_RenderTexture(renderer, rule2Tex, NULL, &rule2Rect);
            SDL_DestroyTexture(rule2Tex);
        }
        SDL_DestroySurface(rule2Surf);
    }
    yOffset += lineHeight;

    // Rule 3
    const char *rule3 = "3. Find the KEY to unlock the DOOR";
    SDL_Surface *rule3Surf = TTF_RenderText_Blended(font, rule3, strlen(rule3), white);
    if (rule3Surf) {
        SDL_Texture *rule3Tex = SDL_CreateTextureFromSurface(renderer, rule3Surf);
        if (rule3Tex) {
            SDL_FRect rule3Rect = {50, yOffset, (float)rule3Surf->w, (float)rule3Surf->h};
            SDL_RenderTexture(renderer, rule3Tex, NULL, &rule3Rect);
            SDL_DestroyTexture(rule3Tex);
        }
        SDL_DestroySurface(rule3Surf);
    }
    yOffset += lineHeight;

    // Rule 4
    const char *rule4 = "4. Reach the exit door to win the game";
    SDL_Surface *rule4Surf = TTF_RenderText_Blended(font, rule4, strlen(rule4), white);
    if (rule4Surf) {
        SDL_Texture *rule4Tex = SDL_CreateTextureFromSurface(renderer, rule4Surf);
        if (rule4Tex) {
            SDL_FRect rule4Rect = {50, yOffset, (float)rule4Surf->w, (float)rule4Surf->h};
            SDL_RenderTexture(renderer, rule4Tex, NULL, &rule4Rect);
            SDL_DestroyTexture(rule4Tex);
        }
        SDL_DestroySurface(rule4Surf);
    }
    yOffset += lineHeight;

    // Rule 5
    const char *rule5 = "5. Your score is based on coins collected";
    SDL_Surface *rule5Surf = TTF_RenderText_Blended(font, rule5, strlen(rule5), white);
    if (rule5Surf) {
        SDL_Texture *rule5Tex = SDL_CreateTextureFromSurface(renderer, rule5Surf);
        if (rule5Tex) {
            SDL_FRect rule5Rect = {50, yOffset, (float)rule5Surf->w, (float)rule5Surf->h};
            SDL_RenderTexture(renderer, rule5Tex, NULL, &rule5Rect);
            SDL_DestroyTexture(rule5Tex);
        }
        SDL_DestroySurface(rule5Surf);
    }
    yOffset += 50;

    // Instructions
    const char *instruction = "Press ENTER or click to continue...";
    SDL_Surface *instSurf = TTF_RenderText_Blended(font, instruction, strlen(instruction), yellow);
    if (instSurf) {
        SDL_Texture *instTex = SDL_CreateTextureFromSurface(renderer, instSurf);
        if (instTex) {
            SDL_FRect instRect = {150, yOffset, (float)instSurf->w, (float)instSurf->h};
            SDL_RenderTexture(renderer, instTex, NULL, &instRect);
            SDL_DestroyTexture(instTex);
        }
        SDL_DestroySurface(instSurf);
    }
}

//main function
int main(int argc, char *argv[])
{ 
    SDL_Init(SDL_INIT_VIDEO);


    if (TTF_Init()==-1) {
    printf("TTF_Init error: %s\n", SDL_GetError());
    SDL_Quit();
    return 1;
}

TTF_Font *font = TTF_OpenFont("assets/arial.ttf", 24);
if (!font) {
    printf("Failed to load font: %s\n", SDL_GetError());
    TTF_Quit();
    SDL_Quit();
    return 1;
}


    SDL_Window *window = SDL_CreateWindow(
    "Maze game",
    800, 600,
    SDL_WINDOW_RESIZABLE

);

if (!window) {
        printf("Failed to create window: %s\n", SDL_GetError());
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

//create renderer
 SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);

 if (!renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }
//loads images
 SDL_Texture *playerTex = loadTexture(renderer, "assets/player.png");
 SDL_Texture *keyTex    = loadTexture(renderer, "assets/key.png");
 SDL_Texture *doorTex   = loadTexture(renderer, "assets/door.png");
 SDL_Texture *wallTex  = loadTexture(renderer, "assets/wall.png");
 SDL_Texture *floorTex = loadTexture(renderer, "assets/floor.png");
 SDL_Texture *coinTex = loadTexture(renderer, "assets/coin.png");

 

if (!playerTex || !keyTex || !doorTex || !wallTex || !floorTex || !coinTex) {
        printf("Fatal: One or more textures failed to load. Check your assets folder.\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_CloseFont(font);
        TTF_Quit();
        SDL_Quit();
        return 1; 
    }

//maze
 char maze[ROWS][COLUMNS];
    generateMaze(maze);
    srand(time(NULL));
    

    Player player = {1,1,0,0,0,0,0};
    placeCoins(maze, player, coins, MAX_COINS);

    Button playBtn  = {{300, 200, 200, 50}, "Play"};
    Button scoreBtn = {{300, 270, 200, 50}, "High Score"};
    Button quitBtn  = {{300, 340, 200, 50}, "Quit"};


    GameState currentState = STATE_MENU;
    int finalScore=0;
    int highScore=0;
    char bestUser[30]= "";
    
    char playerName[30] = ""; 
    int nameLen = 0;

    int running = 1;

    SDL_StartTextInput(window);

    //Main game loop
    while (running) {
        SDL_Event event;
        //Handle all events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT)
                running = 0;
            if(currentState==STATE_MENU){
                if (event.type == SDL_EVENT_TEXT_INPUT){
            // Check if event.text.text is valid AND the character is valid
                   if (event.text.text && event.text.text[0] != '\0'){
                      // Don't add special characters or control codes
                       char c = event.text.text[0];
                       if ((c >= 32 && c <= 126) && nameLen < 29)  // Printable ASCII, leave room for null terminator
                     {
                       playerName[nameLen] = c;
                       nameLen++;
                       playerName[nameLen] = '\0';
                     }
               }
           }
            else if (event.type == SDL_EVENT_KEY_DOWN) {
               if (event.key.scancode == SDL_SCANCODE_BACKSPACE && nameLen > 0) {
                nameLen--;
                playerName[nameLen] = '\0';
                }
           }
            else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN){
                float mx = event.button.x;
                float my = event.button.y;

                if (isClicked(&playBtn, mx, my)) {
                    if (strlen(playerName) == 0) {
                    strcpy(playerName, "Player1");
                 }
                    currentState = STATE_RULES;
                }

                else if (isClicked(&scoreBtn, mx, my)) {
                    currentState = STATE_HIGHSCORE;
                }

                else if (isClicked(&quitBtn, mx, my)) {
                    running = 0;}
                }
            }

            //GamePLay
            else if (currentState == STATE_RULES && event.type == SDL_EVENT_KEY_DOWN) {
                 if (event.key.scancode == SDL_SCANCODE_RETURN) {
                     currentState = STATE_PLAY;
                         }
            }
            else if (currentState == STATE_RULES && event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                    currentState = STATE_PLAY;
            }
            
            else if (currentState == STATE_PLAY && event.type == SDL_EVENT_KEY_DOWN) {
               
                    int nx = player.x;
                    int ny = player.y;

                    SDL_Scancode sc = event.key.scancode;  // <- use scancode

                   if (sc == SDL_SCANCODE_W) nx--;
                   if (sc == SDL_SCANCODE_S) nx++;
                   if (sc == SDL_SCANCODE_A) ny--;
                   if (sc == SDL_SCANCODE_D) ny++;

    // Check collision
                  if (nx >= 0 && nx < ROWS && ny >= 0 && ny < COLUMNS && maze[nx][ny] != '#') {

                 int oldX = player.x;
                 int oldY = player.y;

                 player.x = nx;
                 player.y = ny;

                if (maze[nx][ny] == 'K') {
                  player.key = 1;          
                  maze[nx][ny] = ' ';     
                  sprintf(message, "Key collected!");
                  messageTime = SDL_GetTicks();
                  }
                checkPlayerCollision(&player, coins, MAX_COINS);
       
                if (maze[nx][ny] == 'D') {
                    if (player.key) {
                        //victory
                      finalScore = calculateSCore(&player);
                      highScore= getHighScore(bestUser);
                      currentState = STATE_GAMEOVER;

                      snprintf(message, sizeof(message), "You reached the exit! Congratulations!\n");
                      messageTime = SDL_GetTicks();
                     }

                    else {
                          snprintf(message, sizeof(message),"Door is locked! Find the key.\n");
                          messageTime = SDL_GetTicks();
                          player.x = oldX;
                          player.y = oldY;
                   }
            }
        }
    }

      if (event.type == SDL_EVENT_KEY_DOWN &&
         event.key.scancode == SDL_SCANCODE_ESCAPE) {

         if (currentState == STATE_PLAY || currentState == STATE_HIGHSCORE)
             currentState = STATE_MENU;
         }

}

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
         if (currentState == STATE_MENU) {
            drawButton(renderer, font, &playBtn);
            drawButton(renderer, font, &scoreBtn);
            drawButton(renderer, font, &quitBtn);

           SDL_Color white = {255,255,255,255};
           
           SDL_Surface *promptSurf = TTF_RenderText_Blended(font, "Enter your name:", 
                strlen("Enter your name:"), white);
            if (promptSurf) {
                SDL_Texture *promptTex = SDL_CreateTextureFromSurface(renderer, promptSurf);
                if (promptTex) {
                    SDL_FRect promptRect = {150, 100, (float)promptSurf->w, (float)promptSurf->h};
                    SDL_RenderTexture(renderer, promptTex, NULL, &promptRect);
                    SDL_DestroyTexture(promptTex);
                }
                SDL_DestroySurface(promptSurf);
            }

             SDL_FRect inputBox = {
                             150,        // same X as prompt
                             135,        // slightly below prompt
                             300,        // width
                             40          // height
                        };

                      SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); 
                      SDL_RenderRect(renderer, &inputBox);

            // Player name
            char displayName[32];
            snprintf(displayName, sizeof(displayName), "%s_", playerName);
            
            SDL_Surface *nameSurf = TTF_RenderText_Blended(font, displayName, 
                strlen(displayName), white);
            if (nameSurf) {
                SDL_Texture *nameTex = SDL_CreateTextureFromSurface(renderer, nameSurf);
                if (nameTex) {
                    SDL_FRect nameRect = {150, 140, (float)nameSurf->w, (float)nameSurf->h};
                    SDL_RenderTexture(renderer, nameTex, NULL, &nameRect);
                    SDL_DestroyTexture(nameTex);
                }
                SDL_DestroySurface(nameSurf);
            }
        
        }
        
        else if (currentState == STATE_RULES) {
             // Render rules screen
             renderRules(renderer, font);
           } 

        else if (currentState == STATE_PLAY) {

            drawMaze(renderer, maze, &player, window,
                     playerTex, keyTex, doorTex,
                     wallTex, floorTex, coinTex);

            // HUD
            SDL_Color yellow = {255,255,0,255};
            char hud[50];
            snprintf(hud, sizeof(hud), "Coins: %d | Steps: %d", player.coins, player.steps);

            SDL_Surface *hudSurf = TTF_RenderText_Blended(font, hud, strlen(hud), yellow);
            if (hudSurf) {
                SDL_Texture *hudTex = SDL_CreateTextureFromSurface(renderer, hudSurf);
                if (hudTex) {
                    SDL_FRect hudRect = {10, 10, (float)hudSurf->w, (float)hudSurf->h};
                    SDL_RenderTexture(renderer, hudTex, NULL, &hudRect);
                    SDL_DestroyTexture(hudTex);
                }
                SDL_DestroySurface(hudSurf);
            }
            
            if (SDL_GetTicks() - messageTime < 2000) {
                SDL_Color white = {255, 255, 255, 255};
                SDL_Surface *msgSurf = TTF_RenderText_Blended(font, message, strlen(message), white);
                if (msgSurf) {
                    SDL_Texture *msgTex = SDL_CreateTextureFromSurface(renderer, msgSurf);
                    if (msgTex) {
                        SDL_FRect msgRect = {10, 50, (float)msgSurf->w, (float)msgSurf->h};
                        SDL_RenderTexture(renderer, msgTex, NULL, &msgRect);
                        SDL_DestroyTexture(msgTex);
                    }
                    SDL_DestroySurface(msgSurf);
                }
            }
    }

    else if (currentState == STATE_HIGHSCORE) {
            renderHighScore(renderer, font);
        }

    else if (currentState == STATE_GAMEOVER) {
            renderGameOver(renderer, font, finalScore, highScore);
        }


       SDL_RenderPresent(renderer);
       SDL_Delay(16);
    }

    SDL_StopTextInput(window);

    SDL_DestroyTexture(playerTex);
    SDL_DestroyTexture(keyTex);
    SDL_DestroyTexture(doorTex);
    SDL_DestroyTexture(wallTex);
    SDL_DestroyTexture(floorTex);
    SDL_DestroyTexture(coinTex);


    //IMG_Quit();
    TTF_CloseFont(font);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}