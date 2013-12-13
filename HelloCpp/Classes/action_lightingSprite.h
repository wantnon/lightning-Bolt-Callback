//
//  action_lightingSprite.h
//  HelloCpp
//
//  Created by ck02-084 on 13-12-12.
//
//

#ifndef __HelloCpp__action_lightingSprite__
#define __HelloCpp__action_lightingSprite__

#include <iostream>
#include "cocos2d.h"
using namespace cocos2d;
#include "lightingSprite.h"
class Caction_lightingSprite_blinkSomeTime:public CCActionInterval
{
public:
    virtual bool isDone(void){
        return CCActionInterval::isDone();
    }
    virtual void step(float dt){
        CCActionInterval::step(dt);
    }
    virtual void update(float time){//the time actually is percentage, namely eclipsedTime/totalDuration
        //the t in action's update(t) and sprite's update(t) is different meaning
        //t in action's update(t) means the progress percentage, see CCActionInterval::step()
        //t in sprite's update(t) means the deltaTime between current frame and previous frame, see CCDirector::drawScene()
        timeFoe=timeCur;
        timeCur=time*getDuration();
        m_pTarget->update(timeCur-timeFoe);
    }
    virtual void startWithTarget(CCNode *pTarget){
        CCAction::startWithTarget(pTarget);
        //other init code
        timeCur=timeFoe=0;
    }
    bool initWithDuration(float d){
        setDuration(d);
        return true;
    }
    static Caction_lightingSprite_blinkSomeTime* createWithDuration(float d){
        Caction_lightingSprite_blinkSomeTime* pAction = new Caction_lightingSprite_blinkSomeTime();
        pAction->initWithDuration(d);
        pAction->autorelease();
        return pAction;
    }
private:
    float timeCur,timeFoe;

};
#endif /* defined(__HelloCpp__action_lightingSprite__) */
