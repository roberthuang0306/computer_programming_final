//
//  Barrier.hpp
//  SDL
//
//  Created by Yen-hsiang Huang on 2017/12/28.
//  Copyright © 2017年 robert. All rights reserved.
//

#ifndef Barrier_hpp
#define Barrier_hpp
#include "Header.h"
#include <stdio.h>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include "LTexture.hpp"

extern const int SCREEN_WIDTH ;
extern const int SCREEN_HEIGHT ;
extern const int barrier_range ;
extern LTexture gBarrierTexture;

class Barrier
{
public:
    //The dimensions of the dot
    static const int DOT_WIDTH = 20;
    static const int DOT_HEIGHT = 20;

    //Maximum axis velocity of the dot
    static const int DOT_VEL = 10;

    //Initializes the variables
    Barrier();

    //Takes key presses and adjusts the dot's velocity
    void handleEvent( SDL_Event& e );

    //Moves the dot
    void move();

    //Shows the dot on the screen
    void render();

    // 障礙物回報
    int locationX();//障礙物左方的邊的x座標
    int locationY();//障礙物中間空隙的正中央的y座標

    //reset position
    void reset(int);

private:
    //barrier id
    int id;

    //The X and Y offsets of the dot
    int mPosX, mPosY;

    //The velocity of the dot
    int mVelX, mVelY;
};

#endif /* Barrier_hpp */
