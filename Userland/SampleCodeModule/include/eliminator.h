#ifndef ELIMINATOR_H
#define ELIMINATOR_H

#include <userlib.h>
#include <colors.h>

#define WIDTH 80
#define HEIGHT 60
#define MAXDIM 100

#define PIXELWIDTH (get_scrWidth() / WIDTH)
#define PIXELHEIGHT (get_scrHeight() / HEIGHT)
#define MAX_BUFF 254

#define KEY_UP_P1 'w'
#define KEY_DOWN_P1 's'
#define KEY_LEFT_P1 'a'
#define KEY_RIGHT_P1 'd'

#define KEY_UP_P2 'i'
#define KEY_DOWN_P2 'k'
#define KEY_LEFT_P2 'j'
#define KEY_RIGHT_P2 'l'


typedef struct Player PlayerType;

void draw(PlayerType* players);
void checkInput(PlayerType* players);
int checkCollision(PlayerType* players, int gameStatus[HEIGHT][WIDTH]);
int eliminator(int playerCount);
int gameOverScreen(int loser);
void updatePlayerPosition(PlayerType* players, int gameStatus[HEIGHT][WIDTH]);
void updateGameStatus(PlayerType* players, int gameStatus[HEIGHT][WIDTH]);


#endif /* ELIMINATOR_H */