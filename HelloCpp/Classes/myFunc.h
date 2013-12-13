//
//  myFunc.h
//  HelloCpp
//
//  Created by apple on 13-10-26.
//
//

#ifndef __HelloCpp__myFunc__
#define __HelloCpp__myFunc__

#include <iostream>
using namespace std;
#include "cocos2d.h"
#include "cocos-ext.h"
using namespace cocos2d;
CCTexture2D* createCCTexture2DWithSize(const CCSize&size,CCTexture2DPixelFormat eFormat,float r,float g,float b,float a);
float random01();
float randomInRange(float min,float max);
CCPoint rotateVec(const CCPoint &vec,float A);
CCPoint rotateVec(const CCPoint &vec,float cosA,float sinA);
bool catchProbability(float p);
float calAngleFromCosAndSin(float cosA,float sinA);
bool pointEqual(const CCPoint&point1,const CCPoint&point2,float eps);
void cross(const float v1[3],const float v2[3], float rs[3]);
CCPoint calculateAngleFromVec1ToVec2(const CCPoint&vec1,const CCPoint&vec2);
float nPow(float a,int n);
#endif /* defined(__HelloCpp__myFunc__) */
