#ifndef   ARDUINO_H
#define   ARDUINO_H

#include <Arduino.h> //being a bit lazy and using arduino random function in here

#endif

typedef enum {
  NONE,
  LEFT,
  UP,
  RIGHT,
  DOWN
} Direction;


class Game {
    private:
      bool gameWon = false;
      bool animating = false;
      bool couldMove = false;
      Direction direction = NONE;

    protected:
        int* board;
        bool* blockMerge;

        void setPosition(int x, int y, int value){
            board[x+(4*y)] = value;
        }

        void setBlockMerge(int x, int y, bool value){
            blockMerge[x+(4*y)] = value;
        }

        bool getBlockMerge(int x, int y){
            return blockMerge[x+(4*y)];
        }

        void addRandom(){
          int x, y;

          bool canPlace = false;
          for (int i = 0; i < 16; i++){
            if (board[i] == 0) {
                canPlace = true;
                break;
            }
          }
          
          if (!canPlace){
            return;
          }
          
          do {
            x = random(4);
            y = random(4);
          } while (Tile(x,y) != 0);

          int n = random(100);
          
          if (n < 10){
             setPosition(x, y, 4);
             return;
          }
          
          setPosition(x, y, 2);
        }

    public:
        Game() {
            board = new int[16]{};
            blockMerge = new bool[16]{};
        }

        bool Animating(){
          return animating;
        }

        int Tile(int x, int y){
            return board[x+4*y];
        }

        void New(){
            gameWon = false;
            for (int i = 0; i < 16; i++) { board[i] = 0; }
            addRandom();
            addRandom();
        }

        bool GameWon(){
          return gameWon;
        }

        void PlayMove(Direction moveDirection){
          for (int i = 0; i < 16; i++) { blockMerge[i] = false; }
          couldMove = false;
          direction = moveDirection;
        }

        bool TileEmpty(int x, int y){
          return Tile(x,y) == 0;
        }

        bool TilesCanMerge(int x, int y, int dx, int dy){
          return Tile(x+dx, y+dy) == Tile(x,y) && !(getBlockMerge(x+dx, y+dy) || getBlockMerge(x, y));
        }

        bool MovesAvailable(){
            for(int i = 0; i < 16; i++){
              if (board[i] == 0) {
                return true;
              }
            }
          
            for(int y = 0; y < 4; y++){
              for (int x = 1; x < 4; x++) {
                if (TilesCanMerge(x,y,-1,0)){
                  return true;
                };
              }
            }
          
            for(int y = 0; y < 4; y++){
              for (int x = 2; x >= 0; x--) {
                if (TilesCanMerge(x,y,1,0)){
                  return true;
                };
              }
            }
          
            for(int x = 0; x < 4; x++){
              for (int y = 1; y < 4; y++) {
                if (TilesCanMerge(x,y,0,-1)){
                  return true;
                };
              }
            }
                 
            for(int x = 0; x < 4; x++){
              for (int y = 2; y >= 0; y--) {
                if (TilesCanMerge(x,y,0,1)){
                  return true;
                };
              }
            }

            return false;
        }

        void MoveTile(int x, int y, int dx, int dy){
          if (TileEmpty(x,y)) {
            return;
          }

          if (TileEmpty(x+dx, y+dy)){
            setPosition(x+dx, y+dy, Tile(x,y));
            setPosition(x,y, 0);
            setBlockMerge(x+dx, y+dy, getBlockMerge(x,y));
            setBlockMerge(x,y, false);
            couldMove = true;
            animating = true;
            return;
          }

          if (TilesCanMerge(x, y, dx, dy)){
            setPosition(x+dx, y+dy, Tile(x,y)*2);
            setPosition(x,y, 0);
            setBlockMerge(x+dx, y+dy, true);
            setBlockMerge(x,y, false);
            couldMove = true;
            animating = true;

            if (Tile(x+dx,y+dy) == 2048) {
              gameWon = true;
            }
            return;
          }
        }

        void Step(){
          animating = false;

          if (direction == NONE){
            return;
          }

          if (direction == RIGHT) {
            for(int y = 0; y < 4; y++){
              for (int x = 1; x < 4; x++) {
                MoveTile(x,y,-1,0);
              }
            }
          }

          if (direction == LEFT) {
            for(int y = 0; y < 4; y++){
              for (int x = 2; x >= 0; x--) {
                MoveTile(x,y,1,0);
              }
            }
          }

          if (direction == DOWN) {
            for(int x = 0; x < 4; x++){
              for (int y = 1; y < 4; y++) {
                MoveTile(x,y,0,-1);
              }
            }
          }

          if (direction == UP) {            
            for(int x = 0; x < 4; x++){
              for (int y = 2; y >= 0; y--) {
                MoveTile(x,y,0,1);
              }
            }
          }

          if (!animating && couldMove) {
            direction = NONE;
            addRandom();
          }
        }
};