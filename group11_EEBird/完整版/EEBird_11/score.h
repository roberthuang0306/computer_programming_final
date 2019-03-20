//
//  score.hpp
//  SDL
//
//  Created by Yen-hsiang Huang on 2017/12/28.
//  Copyright © 2017年 robert. All rights reserved.
//

#ifndef score_hpp
#define score_hpp
#include "Header.h"
#include <stdio.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include "LTexture.hpp"
#include "Barrier.hpp"
extern Barrier barrier[5];
extern const int BIRD_XLOCATION;
extern int score;
void Score();
string scorestring(int x);
int scorelength(int x);
#endif /* score_hpp */
