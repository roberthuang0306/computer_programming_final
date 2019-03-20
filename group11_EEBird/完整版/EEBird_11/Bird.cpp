//
//  Bird.cpp
//  SDL
//
//  Created by  on 2017/12/28.
//  Copyright © 2017年 robert. All rights reserved.
//
#include "Bird.h"

Bird::Bird()
{
    resetPos();
    xx=0;
}

void Point::setPos(int x_, int y_)
{
    x = x_ ; y = y_ ;
}

void Bird::handleEvent( SDL_Event& e )
{
    //If a key was pressed
	if( e.type == SDL_KEYDOWN && e.key.repeat == 0 )
    {
        //Adjust the velocity
        switch( e.key.keysym.sym )
        {
            case SDLK_UP: mVelY = -10; break;
        }
    }
}

void Bird::move()
{

    mVelY+=0.5;//acceleration

    //Move the dot up or down
    mPosY += mVelY;

    //If the dot went too far up or down
    if( ( mPosY + 80 > SCREEN_HEIGHT ) )
    {
        //Move back
        mPosY =SCREEN_HEIGHT-80;
    }
    else if (mPosY<-20)
    {
        mPosY=-20;
    }
}

void Bird::render(bool x)
{
    //Show the dot
    if (x==1)gBirdTexture[1].render( mPosX-(xx+=4), mPosY );
    else if (mVelY>=0){gBirdTexture[1].render( mPosX, mPosY );xx=0;}
    else if (mVelY< 0){gBirdTexture[0].render( mPosX, mPosY );xx=0;}
}

void Bird::resetPos()
{
    //Initialize the offsets
    mPosX = 200;
    mPosY = 0;

    //Initialize the velocity
    mVelX = 0;
    mVelY = 0;
}

bool Bird::collision(int x,int y)
{
    Point birdCenter;
    birdCenter.setPos(mPosX+BIRD_WIDTH/2 , mPosY+BIRD_WIDTH/2);
    Point BR[4] ;   //topleft,topright,bottomleft,bottomright
    BR[0].setPos(x,y-131); BR[1].setPos(x+108,y-131); BR[2].setPos(x,y+131); BR[3].setPos(x+108,y+131);

    bool collide;
    collide=false;
    //圓形鳥version
/*
    float distance, birdRadius;
    birdRadius = 30*sqrt(2);

    if(mPosY<=0) collide=true;
    else if(mPosY+80==SCREEN_HEIGHT) collide=true;
    else if((x<=BIRD_XLOCATION+BIRD_WIDTH) && (x>=BIRD_XLOCATION-40))
    {
        //鳥有超過一半的部分在barrier 之外
        if(x>=BIRD_XLOCATION+BIRD_WIDTH/2 || x<=BIRD_XLOCATION-barrier_range/2)
        {
            //撞到barrier 左邊
            if(birdCenter.y-BIRD_HEIGHT/2<y-132 || birdCenter.y+BIRD_HEIGHT/2>y+132)
            {
                if(birdCenter.x+BIRD_WIDTH/2<=x) collide = true;
            }
            //撞到barrier開口的角
            for(int i=0;i<4;i++)
            {
                distance = sqrt( pow((BR[i].x-birdCenter.x),2) + pow((BR[i].y-birdCenter.y),2) ) ;
                if(distance<birdRadius) {collide = true;}
            }
        }
        else //鳥有一半以上的部分進入barrier的範圍
        {
            //鳥頂端撞到barrier
            if(birdCenter.y-BIRD_HEIGHT/2<y-132 || birdCenter.y+BIRD_HEIGHT/2>y+132) {collide = true;}
        }
    }
 */
    //方形鳥version
    if (mPosY+20<=0) collide=true;
    if (mPosY+80>=SCREEN_HEIGHT) collide=true;
    if(birdCenter.x>=(BR[0].x-30)&&birdCenter.x<=(BR[1].x+30))
    {
        if ((birdCenter.y>=(BR[2].y-30))||(birdCenter.y<=(BR[0].y+30)))collide=true;
    }
    return collide;
}



