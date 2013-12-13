#include "HelloWorldScene.h"
#include "AppMacros.h"
USING_NS_CC;
#include "CCControlButton.h"
using namespace cocos2d;

CCScene* HelloWorld::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    HelloWorld *layer = HelloWorld::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool HelloWorld::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

 /*   /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    CCMenuItemImage *pCloseItem = CCMenuItemImage::create(
                                        "CloseNormal.png",
                                        "CloseSelected.png",
                                        this,
                                        menu_selector(HelloWorld::menuCloseCallback));
    
	pCloseItem->setPosition(ccp(origin.x + visibleSize.width - pCloseItem->getContentSize().width/2 ,
                                origin.y + pCloseItem->getContentSize().height/2));

    // create menu, it's an autorelease object
    CCMenu* pMenu = CCMenu::create(pCloseItem, NULL);
    pMenu->setPosition(CCPointZero);
    this->addChild(pMenu, 1);
*/
    /////////////////////////////
    // 3. add your codes below...
    //enable touch
	setTouchEnabled( true );
    //set projection to 2D (default is 3D). if use 3D projection, may cause tiny projection error, in some situation, if the error accumulated across frames, may cause effect wrong.
    CCDirector::sharedDirector()->setProjection(kCCDirectorProjection2D);
    //sinSize
    CCSize winSize=CCDirector::sharedDirector()->getWinSize();//winSize equals to designResolutionSize
    //show frame rate info
    CCDirector::sharedDirector()->setDisplayStats(true);


    //lightingSprite shader
    ClightingSprite::initProgram();
    

    

    
  
    //----menu0
    {
        CCSize screenSize = CCDirector::sharedDirector()->getWinSize();
        
        CCLabelTTF* pLabel0 = CCLabelTTF::create("", "Arial", 30);
        
        pMenuItem0 = CCMenuItemImage::create("button_selectscene1_001-hd.png","button_selectscene1_001-hd.png",this,menu_selector(HelloWorld::menu0Callback));
        pMenuItem0->setPosition(CCPointZero);
        pMenuItem0->addChild(pLabel0,0);
        pLabel0->setPosition(ccp(pMenuItem0->getContentSize().width/2,pMenuItem0->getContentSize().height/2));
        
        pMenu0 = CCMenu::create(pMenuItem0,NULL);
        pMenu0->setPosition(ccp(screenSize.width/2, screenSize.height/2));
        this->addChild(pMenu0, 0);
    }
    //centerPoint
    CCPoint centerPoint=pMenuItem0->convertToWorldSpaceAR(ccp(0,0));
    //startPoint and endPoint
    const float R=650.0/2;
    const float r=pMenuItem0->getContentSize().width/2;
    CCPoint startPoint=centerPoint-CCPoint(R,0);
    CCPoint endPoint=centerPoint+CCPoint(R,0);
    
      //lightingSprit
    {
        lightingSprite=new ClightingSprite();
        lightingSprite->autorelease();
        lightingSprite->init();
        lightingSprite->genLighting();
        this->addChild(lightingSprite,1);
        lightingSprite->setDOpacity(1000);
        lightingSprite->setIsDoJitter(true);
        lightingSprite->setK_offset(0.20);//0.18
        lightingSprite->setGenBranchProbabilityFade(0.7);
    }
    //lightingSprit2
    {
        lightingSprite2=new ClightingSprite();
        lightingSprite2->autorelease();
        lightingSprite2->init();
        lightingSprite2->genLighting();
        this->addChild(lightingSprite2,1);
        lightingSprite2->setDOpacity(1000);
        lightingSprite2->setIsDoJitter(true);
        lightingSprite2->setK_offset(0.20);
        lightingSprite2->setGenBranchProbabilityFade(0.7);
        
    }

    
    //position will be the scale/rotate center
    //and note: before set startPoint and endPoint, we must setPosition first
    //because local space startPoint and endPoint which will be used inner lightingSprite is calculated base on position
    lightingSprite->setPosition(centerPoint);
    lightingSprite2->setPosition(centerPoint);
    
    lightingSprite->setStartPoint(startPoint);
    lightingSprite->setEndPoint(endPoint);
    
    lightingSprite2->setStartPoint(startPoint);
    lightingSprite2->setEndPoint(endPoint);
    
    lightingSprite->setScale(r/R);
    lightingSprite2->setScale(r/R);
    lightingSprite->setRotation(45);
    lightingSprite2->setRotation(45);
    

    
   

    
    // author info
    {
        CCLabelTTF* pLabel = CCLabelTTF::create("by yang chao (wantnon) 2013-12-13", "Arial", 30);
        pLabel->setPosition(ccp(origin.x + visibleSize.width/2,
                                origin.y + visibleSize.height - pLabel->getContentSize().height-60));
        this->addChild(pLabel, 1);
    }
    
    // add a label shows "Hello World"
    // create and initialize a label
 
    CCLabelTTF* pLabel = CCLabelTTF::create("lightning Bolt Callback", "Arial", 45);
    
    // position the label on the center of the screen
    pLabel->setPosition(ccp(origin.x + visibleSize.width/2,
                            origin.y + visibleSize.height - pLabel->getContentSize().height));

    // add the label as a child to this layer
    this->addChild(pLabel, 1);
  
    
   
    
    return true;
}
void HelloWorld::rotItCallBack(CCNode* sender){
    //http://blog.csdn.net/bill_man/article/details/7218545
    float duration=1;
    CCOrbitCamera *orbitCameraAction = CCOrbitCamera::create(duration, 1, 0, 0, 360, 0, 0);
    pMenuItem0->runAction(CCSequence::create(orbitCameraAction, NULL));
    
}
void HelloWorld::menu0Callback(CCNode* sender){
    {
        lightingSprite->setVisible(true);
        CCFadeOut*fadeOut=CCFadeOut::create(0.5);
        Caction_lightingSprite_blinkSomeTime*blinkSomeTime=Caction_lightingSprite_blinkSomeTime::createWithDuration(1.2);
        CCCallFuncN *rotIt = CCCallFuncN::create(this, callfuncN_selector(HelloWorld::rotItCallBack));
        lightingSprite->runAction(CCSequence::create(blinkSomeTime,fadeOut,rotIt,NULL));
    }
    {
        lightingSprite2->setVisible(true);
        CCFadeOut*fadeOut=CCFadeOut::create(0.5);
        const float delayTime=255.0/lightingSprite2->getDOpacity()/2;
        CCDelayTime *delay = CCDelayTime::create(delayTime);
        Caction_lightingSprite_blinkSomeTime*blinkSomeTime=Caction_lightingSprite_blinkSomeTime::createWithDuration(1.2-delayTime);
        lightingSprite2->runAction(CCSequence::create(delay,blinkSomeTime,fadeOut,NULL));
        
    }

    
}
void HelloWorld::menuCloseCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT) || (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)
	CCMessageBox("You pressed the close button. Windows Store Apps do not implement a close button.","Alert");
#else
    CCDirector::sharedDirector()->end();
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    exit(0);
#endif
#endif
}


void HelloWorld::ccTouchesEnded(CCSet* touches, CCEvent* event)
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    CCSetIterator it;
    CCTouch* touch;
    
    for( it = touches->begin(); it != touches->end(); it++)
    {
        touch = (CCTouch*)(*it);
        
        if(!touch)
            break;
        
        CCPoint loc_winSpace = touch->getLocationInView();
        CCPoint loc_GLSpace = CCDirector::sharedDirector()->convertToGL(loc_winSpace);
        
      /*  lightingSprite->setEndPoint(loc_GLSpace);
        lightingSprite2->setEndPoint(loc_GLSpace);
        break;//because setEndPoint cost, so we should break early*/
        
    }
}
void HelloWorld::ccTouchesMoved(cocos2d::CCSet* touches , cocos2d::CCEvent* event)
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    CCSetIterator it;
    CCTouch* touch;
    for( it = touches->begin(); it != touches->end(); it++)
    {
        touch = (CCTouch*)(*it);
        
        if(!touch)
            break;
        
        CCPoint loc_winSpace = touch->getLocationInView();
        CCPoint loc_GLSpace = CCDirector::sharedDirector()->convertToGL(loc_winSpace);


      /*  lightingSprite->setEndPoint(loc_GLSpace);
        lightingSprite2->setEndPoint(loc_GLSpace);
        break;//because setEndPoint cost, so we should break early*/
    
        
		
    }
    
}

void HelloWorld::ccTouchesBegan(CCSet* touches, CCEvent* event)
{
    CCSize winSize = CCDirector::sharedDirector()->getWinSize();
    
    CCSetIterator it;
    CCTouch* touch;
	for( it = touches->begin(); it != touches->end(); it++)
    {
        touch = (CCTouch*)(*it);
        if(!touch)
            break;
        
        CCPoint loc_winSpace = touch->getLocationInView();
        CCPoint loc_GLSpace = CCDirector::sharedDirector()->convertToGL(loc_winSpace);
        //CCLOG("loc_GLSpace:%f,%f",loc_GLSpace.x,loc_GLSpace.y);
    /*    lightingSprite->setEndPoint(loc_GLSpace);
        lightingSprite2->setEndPoint(loc_GLSpace);
        break;//because setEndPoint cost, so we should break early*/

        
    }
}

