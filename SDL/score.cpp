//
//  score.cpp
//  SDL
//
//  Created by Yen-hsiang Huang on 2017/12/28.
//  Copyright © 2017年 robert. All rights reserved.
//

#include "score.h"
void Score()
{
    int birdX=BIRD_XLOCATION;
    static int bl1=0,bl2=0,bl3=0,bl4=0,bl5=0;
    /*if(bl1+133>=birdX&&(barrier1.locationX()+133)<birdX) score+=1;  //----------這裡改了
    if(bl2+133>=birdX&&(barrier2.locationX()+133)<birdX) score+=1;
    if(bl3+133>=birdX&&(barrier3.locationX()+133)<birdX) score+=1;
    if(bl4+133>=birdX&&(barrier4.locationX()+133)<birdX) score+=1;
    if(bl5+133>=birdX&&(barrier5.locationX()+133)<birdX) score+=1;
    bl1=barrier1.locationX();
    bl2=barrier2.locationX();
    bl3=barrier3.locationX();
    bl4=barrier4.locationX();
    bl5=barrier5.locationX();*/
    if(bl1+133>=birdX&&(barrier[0].locationX()+133)<birdX) score+=1;
    if(bl2+133>=birdX&&(barrier[1].locationX()+133)<birdX) score+=1;
    if(bl3+133>=birdX&&(barrier[2].locationX()+133)<birdX) score+=1;
    if(bl4+133>=birdX&&(barrier[3].locationX()+133)<birdX) score+=1;
    if(bl5+133>=birdX&&(barrier[4].locationX()+133)<birdX) score+=1;
    bl1=barrier[0].locationX();
    bl2=barrier[1].locationX();
    bl3=barrier[2].locationX();
    bl4=barrier[3].locationX();
    bl5=barrier[4].locationX();     //------------------------------------到這
}
string scorestring(int x)
{
    int l=scorelength(x);
    char* scorechar;
    scorechar=new char[l+1];
    for(int i=0;i<l+1;i++)
    {
        scorechar[i]='\0';
    }
    for(int i=l-1 ,j=0;i>=0;i--,j++)
    {
        int y=pow(10,i);
        scorechar[j]=(x/y)+48;
        x%=y;
    }
    string r=scorechar;
    delete []scorechar;
    return r;

}
int scorelength(int x)
{
    int i=0;
    do
    {
        x/=10;
        i++;
    }while(x>0);

    return i;
}
