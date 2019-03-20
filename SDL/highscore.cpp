//
//  highscore.cpp
//  SDL
//
//  Created by Yen-hsiang Huang on 2018/1/7.
//  Copyright © 2018年 robert. All rights reserved.
//

#include <stdio.h>
#include "highscore.h"
void sort()
{
    for(int i=9;i>0;i--)
    {
        if (highscore[i]>highscore[i-1])
        {
            int x=highscore[i];
            highscore[i]=highscore[i-1];
            highscore[i-1]=x;
        }
        else break;
    }
}
