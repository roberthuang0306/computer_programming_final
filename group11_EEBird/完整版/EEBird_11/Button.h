//  Button.h
//  SDL
//
//  Created by LIN YU-HSUAN on 2017/12/28.
//  Copyright © 2017年 robert. All rights reserved.
//

#ifndef Button_h
#define Button_h
#include "Header.h"
#include <stdio.h>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include "LTexture.hpp"

extern const int SCREEN_WIDTH ;
extern const int SCREEN_HEIGHT ;
extern LTexture gButtonTexture[3];
extern int move_restart;
extern Mix_Chunk *gclick;
//The dimensions of the button
static const int BUTTON_WIDTH = 150;
static const int BUTTON_HEIGHT = 87;

//Maximum axis velocity of the button
static const int BUTTON_VEL = 30;
//buttons
class Button
{
    public:


		//Initializes the variables
		Button( int );

		//Takes key presses and adjusts the dot's velocity
		void handleEvent( SDL_Event& e ,int);

		//Moves the button
		void move();

		//Shows the button on the screen
		void render(int);

		//get flag
		bool getButtonflag();

		//set button position
		void setPos(int,int);

    private:
        //button id
        int id;

		//The X and Y offsets of the button
		int mPosX, mPosY;

		//reset button position
		void resetPos();

		//button id selected
        int select ;

        //stop button from keep moving
        bool stop = false;

        //flag for getting button
        bool getButton = false;

};

#endif /* Button_h */

