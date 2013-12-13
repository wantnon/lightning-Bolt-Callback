#ifndef __HELLOWORLD_SCENE_H__
#define __HELLOWORLD_SCENE_H__

#include "cocos2d.h"
#include "cocos-ext.h"
#include "myDef.h"
#include "GLProgramWithMyUnifos.h"
#include "lightingSprite.h"
#include "action_lightingSprite.h"
using namespace cocos2d;
using namespace cocos2d::extension;
class HelloWorld : public cocos2d::CCLayer
{
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    
    // a selector callback
    void menuCloseCallback(CCObject* pSender);
    
    // implement the "static node()" method manually
    CREATE_FUNC(HelloWorld);
public:
    HelloWorld(){
    }
    virtual ~HelloWorld(){}
public:
    cocos2d::extension::CCControlButton* m_controlButton;
    void drawBrightLine(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent);
    cocos2d::extension::CCControlButton* m_controlButton2;
    void swingMove(CCObject *senderz, cocos2d::extension::CCControlEvent controlEvent);
public:
    ClightingSprite* lightingSprite;
    ClightingSprite* lightingSprite2;

public:
    //touch
	virtual void ccTouchesBegan(cocos2d::CCSet* touches , cocos2d::CCEvent* event);
	virtual void ccTouchesMoved(cocos2d::CCSet* touches , cocos2d::CCEvent* event);
	virtual void ccTouchesEnded(cocos2d::CCSet* touches , cocos2d::CCEvent* event);

public:
    CCMenu* pMenu0;
    CCMenuItemImage *pMenuItem0;
    void menu0Callback(CCNode* sender);
    void rotItCallBack(CCNode* sender);
    void hideLightingCallBack(CCNode* sender);


};

#endif // __HELLOWORLD_SCENE_H__
