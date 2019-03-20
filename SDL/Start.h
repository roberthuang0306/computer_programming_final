//  Start.h
//  SDL
//
//  Created by LIN YU-HSUAN on 2018/1/1.
//  Copyright © 2018年 robert. All rights reserved.
//

#ifndef Start_h
#define Start_h
#include "Header.h"
#include <stdio.h>
#include <string>
#include <stdio.h>
#include <cstdlib>
#include <ctime>
#include "LTexture.hpp"
extern bool infoAppear;
extern const int StartB_WIDTH;
extern const int StartB_HEIGHT;
extern const int StartB_PosX;
extern const int StartB_PosY;
extern const int InfoBox_WIDTH;
extern const int InfoBox_HEIGHT;
extern const int InfoPosX;
extern const int InfoPosY;
extern int CurrentInfo;

bool handleEvent( SDL_Event* e );
void InfoEvent( SDL_Event* e );
bool StartScene( SDL_Event& e );

#endif /* Start_h */
