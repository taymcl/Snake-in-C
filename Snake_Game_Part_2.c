/*
CS355 Project: Part 2
Programmers: Taylor McLean and Pema Sherpa
*/
#include <ncurses.h>
#include <curses.h>
#include <stdio.h>
#include <unistd.h>     // for usleep()
#include <stdlib.h>     // for atoi()
#include <time.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <string.h>
#include <pthread.h>
#define UP     1
#define DOWN  -1
#define LEFT   2
#define RIGHT -2


typedef struct snakes{
        int row;		//Row
        int col;		// column
        int num;
        struct snakes* next;
}snake,*psnake;


struct point{
    int x;
    int y;
};

// track the coordinate of each node of the snake
struct snakeNode {
    struct point node;
};


//prototype methods
void initTrophies();
void snakeGrow();
int randomNum(int min, int max);
void addNode();
void initSnake();
void initScr();
void deleteSnake();
int whetherSnakeNode(int i,int j);
int whetherSnakeFood(int i,int j);
int ifSnakeDie();
void initNcure();
void moveSnake();
void* refreshGameFace();
void* changeDirection();
char randDirection();
void gameInterFace();
void turn (int direction);
int get_terminal_width();
int get_terminal_length();
int pthread_cancel(pthread_t thread);

//global variables
int x, y;
WINDOW * win;
pthread_t k1, k2;	// linux thread logo
int snakeLength;
int perimeter;

struct snakeNode body[1000]; //an array to save each node

psnake head=NULL;		// Global variable snake head
psnake tail=NULL;		// Global variable snake tail

int key;
int dir; //keeps track of direction input by user
snake food; //define trophies


int main(int ac, char *av[]) {  
        pthread_t k1, k2;	// linux threads
         
        printf("Welcome to snake, press any key to continue...");  
        initNcure();	// Initialization function
        initSnake();  // Initialize the snake
        gameInterFace();	
        pthread_create(&k1,NULL,refreshGameFace,NULL);	// Thread 1, refresh interface
        pthread_create(&k2,NULL,changeDirection,NULL);	// Thread 2, change direction

        while(1);	// Main thread
        getch();
        endwin();
        return 0;
}

//Taylor Mclean
//Generate a random direction for the snake to start moving
char randDirection()
{
    int randNumber = rand();
    int x = (randNumber % 4) + 1;

    if (x == 1)
        return UP;
    else if (x == 2)
        return DOWN;
    else if (x == 3)
        return LEFT;
    else
        return RIGHT;
}

//Taylor Mclean and Pema Sherpa
//Initalize the snake's body
void initSnake(){
        snakeLength = 3;
        psnake p=NULL;
        dir= randDirection();
        while(head!=NULL){
                p=head;
                head=head->next;
                free(p);	// Release space after re-starting
        }
        initTrophies();
        head=(psnake)malloc(sizeof(snake));
        head->row=1;
        head->col=1;
        head->next=NULL;
        tail=head;

        /* Increase the initial length of the snake body */
        addNode();			
        addNode();
}

//Taylor Mclean
//Increase the snake's size by adding nodes
void addNode(){
        snakeLength++;
        psnake new=NULL;
        new=(psnake)malloc(sizeof(snake));
        new->next=NULL;
        switch(dir){	// Control the top of the snake
                case UP :
                        new->row=tail->row-1;
                        new->col=tail->col;
                
                        break;
                case DOWN:
                        new->row=tail->row+1;
                        new->col=tail->col;
                        
                        break;
                case LEFT :
                        new->row=tail->row;
                        new->col=tail->col-1;
                        
                        break;
                case RIGHT :
                        new->row=tail->row;
                        new->col=tail->col+1;
                       
                        break;
        }
        tail->next=new;
        tail=new;
}

//Taylor Mclean
//Delete a header node 
void deleteSnake(){
        snakeLength--;
        psnake p=NULL;
        p=head;
        head=head->next;
        free(p);		// Release space to avoid spatial leakage
}
//Taylor McLean
//Check whether a specific node is part of the snake
int whetherSnakeNode(int i,int j){ 
        psnake p=head;
        while(p!=NULL){
                if(p->row==i && p->col==j){
                        return 1;
                }
                        p=p->next;
        }
                        return 0;
}

//Taylor Mclean
// Determine the food coordinate node 
int whetherSnakeFood(int i,int j){
 
        if(food.row==i && food.col==j){
                return 1;
        }
                return 0;
}
//Taylor Mclean
//Increase the size of the snake when trophy has been eaten
void snakeGrow()
{
  for(int i = 0; i < food.num;i++){         //increase the size
                    addNode();
                }
}

//Taylor Mclean and Pema Sherpa
//Check the coordinate of the snake's movement for any triggered scenarios
void moveSnake(){

        addNode();	// Add a node

        if(whetherSnakeFood(tail->row,tail->col)){	// Judgment if there is food
                snakeGrow();
                food.num = randomNum(1, 9);
                initTrophies();

        }
        else{
                deleteSnake();// Delete a node and realize the snake body
        }

        ifSnakeDie();     //check if scenarios have been met
       
}

//Pema Sherpa
//The end game scenarios of the game
int ifSnakeDie()
{
        psnake p=NULL;
        p=head;
        perimeter = (((COLS-1)*2) + ((LINES-1)*2));

  //end game scenarios
        if(tail->row == 0 || tail->col==0 || tail->row == LINES -1 || tail->col== COLS -1){	// Judgment whether the wall
                system("clear");
                 printf("Game Over! You hit the wall!\n");
                pthread_cancel(k1);                   //cancel all threads
                 pthread_cancel(k2);
                 
        }
           for(int i = 0; i < snakeLength; i++){                                                   // if the snake hits itself
                 if(tail->row == body[i].node.x && tail->col == body[i].node.y){	
                        system("clear");
                        printf("Game Over! You ran into yourself!\n");
                         pthread_cancel(k1);              //cancel all threads
                        pthread_cancel(k2);
            }
         }

        if(snakeLength == perimeter){     //user wins
                system("clear");
                printf("You Win!\n");
                   pthread_cancel(k1);                   //cancel all threads
                   pthread_cancel(k2);
                
            }
      
    return 0;
}


//Taylor Mclean
void* refreshGameFace(){
        while(1){
                //int speed = sizeof(tail) * 2;
                moveSnake();	// Mobile snake body
                gameInterFace();
                refresh();		// Refresh the interface
                usleep(100000); // In order not to report, finally add <unistd.h> header file
        }
}

//Pema Sherpa
//Initalizes the curses window for the game
void initNcure(){
        initscr();
        move(0, 0);
         win = initscr();  
         getchar();              // pause the program to wait for a keyboard input

        clear(); // clear the screen

        refresh();  
        // getch();
        keypad(stdscr, 1);
        noecho();		//Prevent garbled
  
}
//Taylor Mclean
//Turn the snake's body smoothly
void turn (int direction){
        if(abs(dir) != abs(direction)){
                dir=direction;
        }
 
}

//Taylor Mclean
// Keeps track of the direction inputed by the user
void* changeDirection(){
        while(1){
                move(head->row,head->col);
                key=getch();	// Waiting for the input key value
                switch(key){
                        case KEY_DOWN:
                                turn(DOWN);
                                break;
                        case KEY_UP:
                                turn(UP);
                                break;
                        case KEY_LEFT:
                                turn(LEFT);
                                break;
                        case KEY_RIGHT:
                                turn(RIGHT);
                                break;
                }
        }
}
//Pema Sherpa
//Initializes the trophies
void initTrophies(){
        int x=rand()%get_terminal_length();	
        int y=rand()%get_terminal_width();
        food.row=x;
        food.col=y;

// Our attempt at a timer
        /*int interval = randomNum(1, 9);

        time_t start, end;
        double elapsed;
 
        time(&start);  // start the timer 
 
     do{
        
         time(&end);
        
         elapsed = difftime(end, start);

         if(eatenCheck == TRUE)
         break;


     } while(elapsed <= interval);

        if(eatenCheck == FALSE){

        move(food.row,food.col); // move to previous spot

        addch(' ');   // delete expired trophy
    
        refresh();
        }*/

        //int trophy = randomNum(1,9);
       // move(x, y);
        //printw("%d", trophy);
        //return trophy;
}

 
//Pema Sherpa
//generates random numbers for the trophies and coordinates
int randomNum(int min, int max){
   int num = min + rand() / (RAND_MAX / (max - min + 1) + 1);
   if (num == 0)
   {
     return num + 1;
   }
   else 
   {
     return num;
   }
}


//Taylor Mclean and Pema Sherpa
// Creates the snake pit 
 void gameInterFace(){
         int row = LINES;		// Define the line
        int col = COLS;		// Definition column
        int bodyCounter = 0;
        move(0,0);		// Move the cursor to refresh the interface
        for(row=0;row < LINES;row++){
                 if(row==0){
                         for(col=0;col<COLS;col++){
                                 printw("-");
                         }
                         printw("\n");
                 }
                 else if(row > 0 && row < (LINES - 1)){

                         for(col=0;col<COLS;col++){

                                 if(col == 0 || col==COLS - 1){
                                         printw("|");
                                      
                                 }
                            
                                else if(whetherSnakeNode(row,col)){	//Scan output
                                    printw("*");
                                       body[bodyCounter].node.x = row;
                                    body[bodyCounter].node.y = col;
                                    bodyCounter++;
                                }
                                else if(whetherSnakeFood(row,col)){
                                    printw("%d", food.num);
                                }
                                else{
                                    printw(" ");
           
                                }
                          }
                  }
                  if(row==(LINES-1)){
                        for(col=0;col<COLS-1;col++){
                                printw("--");
                        }
                  }
         }
 }
                
        
