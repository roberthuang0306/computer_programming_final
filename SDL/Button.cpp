//
//  Button.cpp
//  SDL
//
//  Created by LIN YU-HSUAN on 2017/12/28.
//  Copyright © 2017年 robert. All rights reserved.
//

#include "Button.h"

Button::Button ( int n )  //-----------這裡改了
{
    id = n;
    resetPos();
    select = 2 ;
    stop = false;
    getButton = false;
}    //----------------------------------到這

void Button::handleEvent( SDL_Event& e ,int caseN) //-------------這裡改了
{
    switch( caseN )
    {
        case 1 :
            if( e.type == SDL_KEYDOWN )
            {
                switch( e.key.keysym.sym )
                {
                    case SDLK_RIGHT :
                        select++;
                        if( select > 3 ) select = 1 ;
                        break;

                    case SDLK_LEFT :
                        select--;
                        if( select < 1 ) select = 3 ;
                        break;

                    case SDLK_SPACE :
                        if( id == select ) getButton = true ;
                        select = 2 ;
                        setPos((SCREEN_WIDTH/2 - (BUTTON_WIDTH*3/2+40) + (BUTTON_WIDTH+40)*(id-1)) ,mPosY);
                        break;
                }
            }
            break;
        case 2 :
            
            if( e.type == SDL_KEYDOWN )
            {
                switch( e.key.keysym.sym )
                {
                    case SDLK_RIGHT :
                        select++;
                        if( select > 3 ) select = 2 ;
                        break;

                    case SDLK_LEFT :
                        select--;
                        if( select < 2 ) select = 3 ;
                        break;

                    case SDLK_SPACE :
                        if( id == select ) getButton = true ;
                        select = 2 ;
                        setPos((SCREEN_WIDTH/2 - (BUTTON_WIDTH*3/2+40) + (BUTTON_WIDTH+40)*(id-1)) ,mPosY);
                        break;
                }
            }
    }
} //-----------------------------------------------------到這

void Button::move()
{
    if( id == select && !stop )
    {
        mPosY -= BUTTON_VEL ;
        stop = true;
    }
    else if( id != select )
    {
        resetPos();
        stop = false;
    }
}

void Button::setPos(int x,int y) //-----------------加了這個
{
    mPosX = x;
    mPosY = y;
}

void Button::resetPos()
{
    mPosX = SCREEN_WIDTH/2 - (BUTTON_WIDTH*3/2+40) + (BUTTON_WIDTH+40)*(id-1) ;
    mPosY = SCREEN_HEIGHT/2 - BUTTON_HEIGHT/2 ;
}

void Button::render(int caseN)  //---------------------------這裡改了
{
    //Show the Button
    switch( caseN )
    {
        case 1 :
            setPos((SCREEN_WIDTH/2 - (BUTTON_WIDTH*3/2+40) + (BUTTON_WIDTH+40)*(id-1)) ,mPosY);
            gButtonTexture[id-1].render( mPosX, mPosY );
            break;

        case 2 :
            if(id==1)break;
            else
            {
            setPos((SCREEN_WIDTH/2 - (BUTTON_WIDTH+40) + (BUTTON_WIDTH+80)*(id-2)) , mPosY);
            gButtonTexture[id-1].render( mPosX, mPosY-50-move_restart);
            break;
            }
    }
} //------------------------------------------------------到這

bool Button::getButtonflag()
{
    if( getButton == true )
    {
        getButton = false;
        return true;
    }
    else return false;
}
