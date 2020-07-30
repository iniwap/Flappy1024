#include "MainView.h"
#include "../util/GlobalSingleton.h"
#include "GameView.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "GKHWrapperCpp.h"
#endif

#include "SimpleAudioEngine.h"
using namespace CocosDenshion;


USING_NS_CC;

typedef enum{
	BUTTON_PLAY = 1,
	BUTTON_SCORE  = 2,
	BUTTON_ABOUT = 3,
}ButtonType;

CCScene* MainView::scene()
{
    // 'scene' is an autorelease object
    CCScene *scene = CCScene::create();
    
    // 'layer' is an autorelease object
    MainView *layer = MainView::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool MainView::init()
{
    //////////////////////////////
    // 1. super init first
    if ( !CCLayer::init() )
    {
        return false;
    }
    
    CCSize visibleSize = CCDirector::sharedDirector()->getVisibleSize();
    CC_UNUSED CCPoint origin = CCDirector::sharedDirector()->getVisibleOrigin();

	//
	SimpleAudioEngine::sharedEngine()->preloadEffect("Begin.wav");//
	SimpleAudioEngine::sharedEngine()->preloadEffect("Fly.wav");//
	SimpleAudioEngine::sharedEngine()->preloadEffect("CatSoundGood.wav");//
	SimpleAudioEngine::sharedEngine()->preloadEffect("CatSoundBad.wav");//
	SimpleAudioEngine::sharedEngine()->preloadEffect("CollideBalloon.wav");//
	SimpleAudioEngine::sharedEngine()->preloadEffect("CollideEnemy.wav");//
	SimpleAudioEngine::sharedEngine()->preloadEffect("WindSound.wav");//
	SimpleAudioEngine::sharedEngine()->preloadEffect("Win.wav");//
	SimpleAudioEngine::sharedEngine()->preloadEffect("Fail.wav");//
	SimpleAudioEngine::sharedEngine()->preloadEffect("OhNoSound.wav");//

//	SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("GameBgMusic.mp3");
//	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("MenuBgMusic.mp3", true);

	CCSprite *menuBg = CCSprite::create("MenuBg.png");
	menuBg->setPosition(ccp(visibleSize.width/2,visibleSize.height/2));
	this->addChild(menuBg);

	//menu
	//play
	CCMenuItemImage *pPlayButton = CCMenuItemImage::create(
		"PlayButtonNormal.png",
		"PlayButtonSelected.png",
		NULL,
		this,
		menu_selector(MainView::menuClickCallback));

	//score
	CCMenuItemImage *pScoreButton = CCMenuItemImage::create(
		"HighScoresButtonNormal.png",
		"HighScoresButtonSelected.png",
		NULL,
		this,
		menu_selector(MainView::menuClickCallback));

	//about
	/*
	CCMenuItemImage *pAboutButton = CCMenuItemImage::create(
		"AboutButtonNormal.png",
		"AboutButtonSelected.png",
		NULL,
		this,
		menu_selector(MainView::menuClickCallback));
		*/
	CCMenu *pMenu = CCMenu::create(pPlayButton,pScoreButton,NULL);


	//ÉèÖÃTAG¡¢Ãªµã¡¢Î»ÖÃ
	pPlayButton->setTag(BUTTON_PLAY);
	pScoreButton->setTag(BUTTON_SCORE);
	//pAboutButton->setTag(BUTTON_ABOUT);

	pMenu->alignItemsHorizontallyWithPadding(100);

	this->addChild(pMenu);
	pMenu->setPosition(visibleSize.width/2,visibleSize.height/2 - 250);//to be fixed

	return true;
}


void MainView::menuClickCallback(CCObject* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    GKHWrapperCpp gkh;
#endif
	CCNode *node = (CCNode *)pSender;
	int tag = node->getTag();
	switch (tag)
	{
	case BUTTON_PLAY:
	//	CCDirector::sharedDirector()->pushScene(CCDirector::sharedDirector()->getRunningScene());
		CCDirector::sharedDirector()->replaceScene(
			CCTransitionFade::create(1, GameView::scene()));
		break;
	case BUTTON_SCORE:
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        gkh.showLeaderBoard();
#endif
		
        break;
	case BUTTON_ABOUT:
		break;
	default:
		break;
	}
}
