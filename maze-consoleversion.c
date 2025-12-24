#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<conio.h>
#include<windows.h>

//define rows and coloums
#define ROWS 7
#define COLUMNS 15

//define maze characteristics
#define WALL '#'
#define PLAYER '&'
#define EXIT 'D'
#define PATH ' '
#define KEY 'K'
#define COIN 'o'

//Gave ANSI colours
#define COLOR_RESET "\033[0m"
#define COLOR_WALL "\033[1;47m"
#define COLOR_PLAYER "\033[1;33;44m"
#define COLOR_EXIT "\033[1;32;41m"
#define COLOR_KEY "\033[1;36m"
#define COLOR_COIN "\033[1;33m"
#define COLOR_PATH "\033[0;40m"

//Player Structure
typedef struct {
    int x;
    int y;
    int steps;
    int coins;
    int totalCoins;
    int key;
    time_t startTime;
} Player;

//FUNCTIONS

//function1
void clearScreen(){
    #ifdef _WIN32
        system("cls");
    #else 
        system("clear");
    #endif
}

//function2
void flushInput (){
    int c;
    while((c=getchar())!='\n' && c!=EOF);
}

//function3
 void printMaze(char maze [ROWS][COLUMNS], Player *p){
    for(int i=0;i<ROWS;i++){
        for(int j=0;j<COLUMNS;j++){
            char cell = maze[i][j];
            if (cell == WALL){
                printf("%s%c%s",COLOR_WALL,219,COLOR_RESET);
                }
            else if(cell == PLAYER){
                printf("%s%c%s",COLOR_PLAYER,cell,COLOR_RESET);
                }  
            else if(cell == EXIT){
                printf("%s%c%s",COLOR_EXIT,cell,COLOR_RESET);
            }
            else if(cell ==KEY){
                printf("%s%c%s",COLOR_KEY,cell,COLOR_RESET);
            }
            else if(cell == COIN){
                printf("%s%c%s",COLOR_COIN,cell,COLOR_RESET);
            }
            else{
                printf("%s%c%s",COLOR_PATH,cell,COLOR_RESET);
            }
            
        }
        printf("%s", COLOR_RESET);
        printf("  ");
        if (i==0) printf("%sStats:%s\n", COLOR_PLAYER, COLOR_RESET);
        else if (i==1) printf(" Steps: %d\n", p->steps);
        else if (i==2) printf(" %sCoins: %d/%d%s\n", COLOR_COIN, p->coins, p->totalCoins, COLOR_RESET);
        else if (i==3) printf(" %s Key: %s%s\n", COLOR_KEY, p->key ? "Yes": "No", COLOR_RESET);
        else printf("\n");
    }
 }

//function4

int countCoins(char maze[ROWS][COLUMNS]){
    int count = 0;

for(int i=0;i<ROWS;i++){
    for(int j=0;j<COLUMNS;j++){
        if(maze[i][j]==COIN)
        count ++;
    }
}
return count;
}

//function5
void initPlayer(Player *p,char maze[ROWS][COLUMNS]){
    p->steps=0;
    p->coins=0;
    p->key=0;
    p->totalCoins=countCoins(maze);
    p->startTime=time(NULL);
    
    for(int i=0;i<ROWS;i++){
        for(int j=0;j<COLUMNS;j++){
            if(maze[i][j]==PLAYER){
                p->x=i;
                p->y=j;
                return;
            }
        }
    }
}

//function6

int playMaze(char maze[ROWS][COLUMNS],int playerNum){
    Player player;
    char move;
    int collected=0;

    initPlayer(&player,maze);
   

    while (1){
        clearScreen();
        printf("====================================\n");
        printf("         Player %d Maze             \n", playerNum);
        printf("====================================\n");
        printMaze(maze, &player);



        if(maze[player.x][player.y]==EXIT&&player.coins>=player.totalCoins&&player.key){
                int timeTaken=(int)difftime(time(NULL), player.startTime);
                printf("\n%s Player %d opened the door and escaped! %s\n", COLOR_EXIT, playerNum, COLOR_RESET);
                printf(" Time=%d seconds\n", timeTaken);
                printf(" Steps = %d\n", player.steps);
                printf("Coins = %d/%d\n", player.coins, player.totalCoins);
                break;
        }
        
        printf("\n Move (W/A/S/D) or Q to quit: ");
       // fflush(stdout); //ensure prompt is displayed
        
       move=(char)_getch();
    
       
       if(move=='q'||move=='Q'){
        printf("\nplayer %d quits the game. \n", playerNum);
        return -1;
       }

       int newX=player.x;
       int newY=player.y;

       if(move=='w'||move=='W')
       newX--;
       else if(move=='s'||move=='S')
       newX++;
       else if(move=='a'||move=='A')
       newY--;
       else if(move=='d'||move=='D')
       newY++;
       else {
        printf("Invalid move! Use W/A/S/D\n");
         Sleep(1000);
         continue;
       }

       if(maze[newX][newY]==WALL){
        printf("Can't walk through the walls!\n");
         Sleep(1000);
         continue;
       }

       if(maze[newX][newY]==EXIT&& !player.key){
        printf("\n%sThe door is locked! You need the key!%s\n",COLOR_KEY,COLOR_RESET);
        printf("\nPress Enter to continue...");
         getchar();
        continue;}
    

       if(maze[player.x][player.y] != EXIT){
        maze[player.x][player.y]=PATH;
       }
    //update player position
       player.x=newX;
       player.y=newY;
       player.steps++;

       //handle item collection
       collected=0;

       if(maze[player.x][player.y]==KEY){
        player.key=1;
        printf("%sKey collected!%s\n",COLOR_KEY,COLOR_RESET);
        collected =1;
        maze[player.x][player.y] = PATH;
       }

       if(maze[player.x][player.y]==COIN){
        player.coins++;
        printf("%s coin collected! Total: %d/%d%s\n", COLOR_COIN, player.coins, player.totalCoins, COLOR_RESET);
        printf("\a");
        collected=1;  
          maze[player.x][player.y] = PATH;
       }

       //update maze with player position
       if(maze[player.x][player.y]!=EXIT){
        maze[player.x][player.y]=PLAYER;
       }

       if(collected){
        printf("\nPress Enter to continue");
        getchar();
      
       }
       
    }
    return player.steps;

}

int main(){

    char maze1[ROWS][COLUMNS] = {
        "###############",
        "#&  o   #  o  #",
        "# ###   ### # #",
        "#   # K #   # #",
        "# # #   # # # #",
        "# o       o  D#",
        "###############"
    };

    char maze2[ROWS][COLUMNS] = {
        "###############",
        "#&o   #     o #",
        "# #   ### ### #",
        "# #   # K   # #",
        "# ### #   # # #",
        "# o         oD#",
        "###############"
    };

    printf("====================================\n");
    printf("     ASCII MAZE EXPLORER v3.0      \n");
    printf("====================================\n");
    printf("Collect the key %s%c%s to unlock door\n", COLOR_KEY, KEY, COLOR_RESET);
    printf("Gather ALL coins %s%c%s before exit\n", COLOR_COIN, COIN, COLOR_RESET);
    printf("Reach %s%c%s (door) to escape\n", COLOR_EXIT, EXIT, COLOR_RESET);
    printf("Player with FEWEST steps wins!\n");
    printf("Navigate the Player %s%c%s with W/A/S/D keys\n", COLOR_PLAYER, PLAYER, COLOR_RESET);
    printf("W for upward\n D for forward\n S for downward\n A for backward\n");
    printf("====================================\n\n");

   int steps1=0, steps2=0;
printf("Player 1's Turn!\nPress Enter to Start....");
flushInput();
getchar();

steps1 = playMaze(maze1, 1);

if(steps1 == -1){
    printf("\nGame Ended\n");
    return 0;
}

printf("Player 2's Turn!\nPress Enter to Start....");
flushInput();
getchar();

steps2 = playMaze(maze2, 2);

if(steps2 == -1){
    printf("\nGame Ended\n");
    return 0;
}

    clearScreen();
    printf("\n====================================\n");
    printf("          FINAL RESULTS             \n");
    printf("====================================\n");
    printf("Player 1 Steps: %d\n", steps1);
    printf("Player 2 Steps: %d\n", steps2);
    printf("====================================\n");

    if(steps1<steps2){
        printf("\n%sPlayer 1 Wins!(fewer steps:%d) %s\n", COLOR_PLAYER, steps1, COLOR_RESET);
    }
    else if(steps2<steps1){
        printf("\n%sPlayer 2 Wins!(fewer steps:%d) %s\n", COLOR_PLAYER, steps2, COLOR_RESET);
    }
    else{
        printf("\n%sIt is a TIE! (Both took %d steps)%s\n", COLOR_PLAYER, steps1, COLOR_RESET);
    }

    printf("\nThanks for playing ASCII Maze Explorer!\n");
    printf("====================================\n");
    return 0;
}





