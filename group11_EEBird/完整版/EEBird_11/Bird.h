//
//  Bird.hpp
//  SDL
//
//  Created by  on 2017/12/28.
//  Copyright © 2017年 robert. All rights reserved.
//

#ifndef Bird_h
#define Bird_h
#include "Header.h"
#include <stdio.h>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include "LTexture.hpp"

extern const int SCREEN_WIDTH ;
extern const int SCREEN_HEIGHT ;
extern const int barrier_range ;
extern const int BIRD_XLOCATION ;

extern LTexture gBirdTexture[2];

struct Point
{
    void setPos(int,int);
    int x, y;

};

//The dot that will move around on the screen
class Bird
{
    public:
		//The dimensions of the dot
		static const int BIRD_WIDTH = 100;
		static const int BIRD_HEIGHT = 100;

		//Initializes the variables
		Bird();

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e );

		//Moves the dot
		void move();

		//Shows the dot on the screen
		void render(bool);

		//reset position
		void resetPos();


		//check collision
		bool collision(int,int);

    private:
		//The X and Y offsets of the dot
		int mPosX, mPosY;

		//The velocity of the dot
		float mVelX, mVelY;
    //死掉的鳥可以動
        int xx;
};

#endif /* Bird_h */
