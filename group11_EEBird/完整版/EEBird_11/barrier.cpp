//
//  Barrier.cpp
//  SDL
//
//  Created by Yen-hsiang Huang on 2017/12/28.
//  Copyright © 2017年 robert. All rights reserved.
//

#include "Barrier.hpp"

int xx=1440;

Barrier::Barrier()
{
}


void Barrier::move()
{
    //Move the dot left or right
    mPosX += mVelX;

    //If the dot went too far up or down//133   是障礙物寬度
    if( ( mPosX < -133 )  )
    {
        //Reinitialize the offsets
        mPosX = 5*xx/4-133;
        mPosY = -240-barrier_range/2+rand()%barrier_range;
    }
    //Move the dot up or down
    mPosY += mVelY;


}

void Barrier::render()
{
    //Show the dot
    gBarrierTexture.render( mPosX, mPosY );
}
int Barrier::locationX()//障礙物左方的邊的x座標
{
    return mPosX;
}
int Barrier::locationY()//障礙物中間空隙的正中央的y座標
{
    return mPosY+612;
}

void Barrier::reset(int x)
{
    id = x;
    //Initialize the offsets
    mPosX = xx+(id-1)*(xx/4);
    mPosY = -240-barrier_range/2+rand()%barrier_range;

    //Initialize the velocity
    mVelX = -5;
    mVelY = 0;
}
