//
//  Start.cpp
//  SDL
//
//  Created by LIN YU-HSUAN on 2018/1/1.
//  Copyright © 2018年 robert. All rights reserved.
//

#include "Start.h"

bool StartScene( SDL_Event& e )
{
    bool start;
    if(infoAppear) InfoEvent( &e );
    else start=handleEvent( &e );
    return start;
}

bool handleEvent( SDL_Event* e )
{
    //If mouse event happened
	if( e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP )
	{
		//Get mouse position
		int x, y;
		SDL_GetMouseState( &x, &y );

		//button position
		int start_lx = StartB_PosX, start_rx = StartB_PosX+StartB_WIDTH ;
		int start_uy = StartB_PosY, start_dy = StartB_PosY+StartB_HEIGHT ;
		int info_lx = StartB_PosX+StartB_WIDTH+40, info_rx = StartB_PosX+StartB_WIDTH*2+40 ;
		int info_uy = StartB_PosY, info_dy = StartB_PosY+StartB_HEIGHT ;

		//Mouse is on the left button
		if( x>start_lx && x<start_rx )
		{
			if( y>start_uy && y<start_dy )
            {
                if( e->type == SDL_MOUSEBUTTONDOWN ) return true;
            }
		}
		//Mouse is on the right button
		if( x>info_lx && x<info_rx )
		{
			if( y>info_uy && y<info_dy )
            {
                if( e->type == SDL_MOUSEBUTTONDOWN ) infoAppear=true;// return 1;
            }
		}
	}
	return false;
}

void InfoEvent( SDL_Event* e )
{
    //If mouse event happened
	if( e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP )
	{
		//Get mouse position
		int x, y;
		SDL_GetMouseState( &x, &y );

		int Left_lx = InfoPosX, Left_rx = InfoPosX+30 ;
		int Left_uy = InfoPosY+InfoBox_HEIGHT/2-30, Left_dy = InfoPosY+InfoBox_HEIGHT/2+30 ;
		int Right_lx = InfoPosX+InfoBox_WIDTH-30, Right_rx = InfoPosX+InfoBox_WIDTH ;
		int Right_uy = InfoPosY+InfoBox_HEIGHT/2-30, Right_dy = InfoPosY+InfoBox_HEIGHT/2+30 ;
		int Close_lx = InfoPosX+InfoBox_WIDTH, Close_rx = InfoPosX+InfoBox_WIDTH+25 ;
		int Close_uy = InfoPosY-25, Close_dy = InfoPosY ;

		//Mouse is on the left button
		if( x>Left_lx && x<Left_rx )
		{
			if( y>Left_uy && y<Left_dy )
            {
                if( e->type == SDL_MOUSEBUTTONDOWN ) CurrentInfo = (CurrentInfo==0) ? 0 : CurrentInfo-1 ;
                //printf("left,%x\n",CurrentInfo);
            }
		}
		//Mouse is on the right button
		if( x>Right_lx && x<Right_rx )
		{
			if( y>Right_uy && y<Right_dy )
            {
                if( e->type == SDL_MOUSEBUTTONDOWN ) CurrentInfo = (CurrentInfo==2) ? 2 : CurrentInfo+1 ;
                //printf("right,%x\n",CurrentInfo);
            }
		}
		//Mouse is on the close button
		if( x>Close_lx && x<Close_rx )
		{
			if( y>Close_uy && y<Close_dy )
            {
                if( e->type == SDL_MOUSEBUTTONDOWN ) {CurrentInfo=0; infoAppear=false ;}
                //printf("right,%x\n",CurrentInfo);
            }
		}
	}
}

