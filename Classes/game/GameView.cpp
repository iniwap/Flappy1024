#include "GameView.h"
#include "MainView.h"
#include "Enemy.h"
#include "Props.h"
#include "SimpleAudioEngine.h"


#include "PlatformInterface.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
#include "GKHWrapperCpp.h"
#endif

using namespace CocosDenshion;

USING_NS_CC;
using namespace std;

static float HERO_SIZE;
static int GameNumArray[] = {2,4,8,16,32,64,128,256,512,1024,2048,4096,8192};//1024

#define MAX_CHANGE_TYPE_NUM 4
static int ChangeType[] = {   WORLD_CHANGE_G,
							WORLD_CHANGE_VX,
							WORLD_CHANGE_VY,
							WORLD_CHANGE_COLLIDE_TYPE,
						};

#define BASIC_CHANGE_CONTINUE 10.0f

enum 
{
	REPLAY_BUTTON = 1,
	SHARE_BUTTON = 2,
};
typedef enum{
	BackGroundTag = 1000,
	BackGroundTag_Near = 1001,
	BackGroundTag_Far = 1002,
}BackGroundTagType;

GameView::GameView()
{

}

GameView::~GameView()
{
	//_theHero->release();
	_enemies->release();
    _props->release();

	/*
	if(_gameWorld)
	{
		delete _gameWorld;
		_gameWorld = NULL;
	}
	*/
}

CCScene* GameView::scene()
{
	CCScene *scene = CCScene::create();
	GameView *layer = GameView::create();

	scene->addChild(layer);


	return scene;
}

bool GameView::init()
{
	if ( !CCLayer::init() )
	{
		return false;
	}
	CC_UNUSED CCSize s = CCDirector::sharedDirector()->getVisibleSize();

    _isGameStart = false;
    
    _isPlayingCollideWithScoreBoard = false;
	_gameScore = 0;
	//Game Num
	_gameNum = 4;//begin from 4

	_theHero = NULL;

	this->initGameWorld();

	this->initEnemys();
    
	this->initBgTileMap();
    
    this->initProps();//

	this->initTapGoInfo();

	this->setTouchEnabled(true);

	//SimpleAudioEngine::sharedEngine()->playBackgroundMusic("GameBgMusic.mp3", true);
    
    PlatformInterface::hideADView();//hide ad
   
    
    //this->schedule(schedule_selector(GameView::addProps), 2.0f);//disable props
    
	return true;
}

void GameView::initGameWorld()
{
	_isChangingWorld = false;

	WorldLevelType level = (WorldLevelType)CCUserDefault::sharedUserDefault()->getIntegerForKey("WorldLevel");

	_gameWorld = new World(this);

	_gameWorld->setGameWorldLevelType(level);

	if(level != WORLD_LEVEL_1024)
	{
		_gameScore = CCUserDefault::sharedUserDefault()->getIntegerForKey("GameScore");
		//
		_gameWorld->setGameWorldVXCharacter(GAME_VX_PLUS2,0);
		_gameWorld->setGameWorldVYCharacter(GAME_VY_PLUS2,0);
		setGameNum(pow(2,(int)level)*512);

	}
}
#ifndef WIN_FRAME_NUM
#define WIN_FRAME_NUM 4
#define WIN_TAG 1001
#endif

void GameView::initGameWinAnimation()
{
	CCSize s = CCDirector::sharedDirector()->getVisibleSize();

	CCSprite *winInfo;

	CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage("Win.png");

	CCArray* animFrames = CCArray::createWithCapacity(WIN_FRAME_NUM);

	// manually add frames to the frame cache
	for(int i=0;i<WIN_FRAME_NUM;i++)
	{
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture, CCRectMake(450*i, 0, 450, 350));
		animFrames->addObject(frame);
		if(i == 0)
		{
			winInfo = CCSprite::createWithSpriteFrame(frame);
			winInfo->setPosition( ccp( s.width/2,s.height/2 ) );
			this->addChild(winInfo,HERO_ZORDER,WIN_TAG);
		}

	}

	CCAnimation *animation = CCAnimation::createWithSpriteFrames(animFrames, 0.4f);
	CCAnimate *animate = CCAnimate::create(animation);

	CCActionInterval* seq = CCSequence::create( animate,NULL);
	winInfo->runAction(CCRepeatForever::create( seq ) );

	this->schedule(schedule_selector(GameView::hideWinAnimation), 3.0f);
}

void GameView::hideWinAnimation(float dt)
{
	this->unschedule(schedule_selector(GameView::hideWinAnimation));

	this->getChildByTag(WIN_TAG)->runAction(CCFadeOut::create(0.5f));
	this->showGameScoreBord(false);
}
void GameView::gameWin()
{
	//show ad
	PlatformInterface::showAdView();

	SimpleAudioEngine::sharedEngine()->stopAllEffects();//stop effect

	//////////////////////////////////////////////////////////////////////////
	this->unscheduleUpdate();
	//this->unschedule(schedule_selector(GameView::addProps));

	this->setTouchEnabled(false);//disable touch

	//stop all enemy update
	CCObject *pObject = NULL;
	CCARRAY_FOREACH(_enemies,pObject)
	{
		//
		Enemy *enemy = (Enemy *)pObject;
		enemy->unscheduleUpdate();
	}

	this->showGameOverOrWinView(false);

	SimpleAudioEngine::sharedEngine()->playEffect("Win.wav");//
}

#ifndef INFO_FRAME_NUM
#define INFO_FRAME_NUM 5
#define TAP_GO_INFO_BG_TAG 2000
#define TAP_GO_INFO_TAG 2001
#define TAP_GO_TAG 2002
#define GET_READY_TAG 2003
#endif

void GameView::initTapGoInfo()
{
	CCSize s = CCDirector::sharedDirector()->getVisibleSize();

	CCSprite *bg = CCSprite::create("GameBg.png");
	bg->setPosition(ccp(s.width/2,s.height/2));
	this->addChild(bg,-BackGroundTag_Far,TAP_GO_INFO_BG_TAG);

	CCSprite *tapGoInfo;

	CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage("TapGoInfo.png");

	CCArray* animFrames = CCArray::createWithCapacity(INFO_FRAME_NUM);

	// manually add frames to the frame cache
	for(int j=0;j<2;j++)
	{
		for(int i=0;i<INFO_FRAME_NUM;i++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture, CCRectMake(320*i, 275*j, 320, 275));
			animFrames->addObject(frame);
			if(i == 0 && j == 0)
			{
				tapGoInfo = CCSprite::createWithSpriteFrame(frame);
				tapGoInfo->setPosition( ccp( s.width/2,s.height/2 ) );
				bg->addChild(tapGoInfo,HERO_ZORDER,TAP_GO_INFO_TAG);
			}

		}
	}

	CCAnimation *animation = CCAnimation::createWithSpriteFrames(animFrames, .5f);
	CCAnimate *animate = CCAnimate::create(animation);

	CCActionInterval* seq = CCSequence::create( animate,NULL);
	tapGoInfo->runAction(CCRepeatForever::create( seq ) );


	CCSprite *tapToGo = CCSprite::create("TapToGo.png");
	CCSprite *getReady = CCSprite::create("GetReady.png");
	bg->addChild(tapToGo,HERO_ZORDER,TAP_GO_TAG);
	bg->addChild(getReady,HERO_ZORDER,GET_READY_TAG);

	tapToGo->setPosition(ccp(s.width/2,
						s.height/2  - tapGoInfo->getContentSize().height/2 - tapToGo->getContentSize().height));

	getReady->setPosition(ccp(s.width/2,
						s.height/2  + tapGoInfo->getContentSize().height/2 + getReady->getContentSize().height));
}
void GameView::initScoreBoard()
{
	ccColor3B color = {255,255,0};
	CCSize s = CCDirector::sharedDirector()->getVisibleSize();

	CCSprite *scoreBoard = CCSprite::create("ScoreBoard.png");
	scoreBoard->setPosition(ccp(s.width/2,s.height - scoreBoard->getContentSize().height/2));
	this->addChild(scoreBoard,1000);

	string scoreNum = CCString::createWithFormat("%d", _gameNum)->getCString();
	_gameNumLabel = CCLabelBMFont::create(scoreNum.c_str(),"fonts/west_england-64.fnt",32);//
	_gameNumLabel->setScale(0.9f);
	scoreBoard->addChild(_gameNumLabel);//
	_gameNumLabel->setColor(color);
	_gameNumLabel->setPosition(scoreBoard->getContentSize().width/2 + 15,scoreBoard->getContentSize().height/4);
	this->setFontSize(scoreNum.length());
}
void GameView::showGameScoreBord(bool isOver)
{
	CCSize s = CCDirector::sharedDirector()->getVisibleSize();

	CCSize views = _gameOverView->getContentSize();
	string play,playSelected;

	if(_gameWorld->getGameWorldLevelType() != WORLD_LEVEL_8192
		&&!isOver)
	{
		play = "NextButtonNormal.png";
		playSelected = "NextButtonSelected.png";
	}
	else
	{
		play = "ReplayButtonNormal.png";
		playSelected = "ReplayButtonSelected.png";
	}

	//menu
	//replay
	CCMenuItemImage *pRePlayButton = CCMenuItemImage::create(
		play.c_str(),
		playSelected.c_str(),
		NULL,
		this,
		menu_selector(GameView::menuClickCallback));
/*
	//share
	CCMenuItemImage *pShareButton = CCMenuItemImage::create(
		"ShareButtonNormal.png",
		"ShareButtonSelected.png",
		NULL,
		this,
		menu_selector(GameView::menuClickCallback));
*/
	_rePlayAndShareMenu = CCMenu::create(pRePlayButton,/*pShareButton,*/NULL);

	pRePlayButton->setTag(REPLAY_BUTTON);
	//pShareButton->setTag(SHARE_BUTTON);

	_rePlayAndShareMenu->alignItemsHorizontallyWithPadding(100);
	_rePlayAndShareMenu->setPosition(ccp(views.width/2,10));
	_gameOverView->addChild(_rePlayAndShareMenu);



	/////score
	int highestScore = CCUserDefault::sharedUserDefault()->getIntegerForKey("HighestScore");
	if(highestScore < _gameScore)
	{
		CCUserDefault::sharedUserDefault()->setIntegerForKey("HighestScore", _gameScore);//save highest score
		CCUserDefault::sharedUserDefault()->flush();
		highestScore = _gameScore;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		///////game center///////
		GKHWrapperCpp gkh;
		gkh.submitScoreToCatagory(highestScore, "Flappy1024");
#endif

	}

	ccColor3B color = {0,0,0};

	string scoreNum = CCString::createWithFormat("%d", _gameScore)->getCString();
	_scoreLabel = CCLabelBMFont::create(scoreNum.c_str(),"fonts/west_england-64.fnt");
	//_scoreLabel->setScale(0.8f);
	_gameOverView->addChild(_scoreLabel);
	_scoreLabel->setPosition(ccp(_gameOverView->getContentSize().width/2 + _scoreLabel->getContentSize().width/2 + 32,
		_gameOverView->getContentSize().height/3));
	_scoreLabel->setColor(color);

	scoreNum = CCString::createWithFormat("%d", highestScore)->getCString();
	_bestScoreLabel = CCLabelBMFont::create(scoreNum.c_str(),"fonts/west_england-64.fnt");
	//_bestScoreLabel->setScale(0.8f);
	_bestScoreLabel->setPosition(ccp(_gameOverView->getContentSize().width/2 + _bestScoreLabel->getContentSize().width/2 + 32,
		_gameOverView->getContentSize().height/5));
	_gameOverView->addChild(_bestScoreLabel);
	_bestScoreLabel->setColor(color);

	this->addChild(_gameOverView,10000);
	_gameOverView->setPosition(ccp(s.width/2,0));

	_gameOverView->runAction(CCMoveTo::create(1.0f,ccp(s.width/2,s.height/2)));////to be fixed

	//for retain
	_gameOverView->release();
}
void GameView::showGameOverOrWinView(bool isOver)
{
	CCSize s = CCDirector::sharedDirector()->getVisibleSize();
	
	_gameWorld->unscheduleAll();

	this->removeAllChildrenWithCleanup(true);

	CCSprite *bg = CCSprite::create("GameBg.png");
	this->addChild(bg,-1);
	bg->setPosition(ccp(s.width/2,s.height/2));

	//
	_gameOverView = CCSprite::create("GameOverViewBg.png");
	_gameOverView->retain();

	if(isOver)
	{
		this->showGameScoreBord(true);
		//
		CCSprite *gameOverSp = CCSprite::create("GameOver.png");
		this->addChild(gameOverSp,10000);
		gameOverSp->setPosition(ccp(s.width/2,s.height + 100));
		gameOverSp->runAction(CCMoveTo::create(1.0f,ccp(s.width/2,
			s.height/2 + _gameOverView->getContentSize().height/2 + gameOverSp->getContentSize().height/2 + 20)));


		//to be fixed if game over rest level
		CCUserDefault::sharedUserDefault()->setIntegerForKey("WorldLevel",0);//rest world level
		CCUserDefault::sharedUserDefault()->setIntegerForKey("GameScore", 0);
		CCUserDefault::sharedUserDefault()->flush();
	}
	else
	{
		//
		CCSprite *gameWinSp = CCSprite::create("WinInfo.png");
		this->addChild(gameWinSp,10000);
		gameWinSp->setPosition(ccp(s.width/2,s.height + 100));
		gameWinSp->runAction(CCMoveTo::create(1.0f,ccp(s.width/2,
			s.height/2 + _gameOverView->getContentSize().height/2 + gameWinSp->getContentSize().height/2 + 20)));

		if(_gameWorld->getGameWorldLevelType() != WORLD_LEVEL_8192)
		{
			CCUserDefault::sharedUserDefault()->setIntegerForKey("WorldLevel", _gameWorld->getGameWorldLevelType() + 1);

			//save score for next level
			CCUserDefault::sharedUserDefault()->setIntegerForKey("GameScore", _gameScore);
			CCUserDefault::sharedUserDefault()->flush();

			this->showGameScoreBord(false);
		}
		else
		{
			CCUserDefault::sharedUserDefault()->setIntegerForKey("WorldLevel",0);//rest world level
			CCUserDefault::sharedUserDefault()->setIntegerForKey("GameScore", 0);
			CCUserDefault::sharedUserDefault()->flush();

			this->initGameWinAnimation();
		}
	}
}
void GameView::initEnemys()
{
	srand((unsigned int)(time(0)));

	_enemies = CCArray::create();//six enemies
	_enemies->retain();
#ifndef ENEMY_NUM
#define ENEMY_NUM 9
#endif
	for (int i =1;i<=ENEMY_NUM;i++)
	{

		Enemy *enemy = Enemy::initEnemyWithGameView(this);
		enemy->setEnemyID(i);
		_enemies->addObject(enemy);

	}
}
void GameView::initProps()
{
	_props = CCArray::create();//
	_props->retain();
}
void GameView::initBgTileMap()
{
    CCSize s = CCDirector::sharedDirector()->getVisibleSize();
    
	//far
	m_pBgSprite = CCSprite::create("GameViewBg.jpg");
	m_pBgSpriteRe = CCSprite::create("GameViewBg.jpg");
	m_pBgSprite->setAnchorPoint(CCPointZero);
	m_pBgSpriteRe->setAnchorPoint(CCPointZero);
	this->addChild(m_pBgSprite, -BackGroundTag_Far,BackGroundTag_Far);
	this->addChild(m_pBgSpriteRe, -BackGroundTag_Far,BackGroundTag_Far);

	//near
	// Map
	m_pTileMap = CCTMXTiledMap::create("GameViewBg.tmx");
	m_pTileMapRe = CCTMXTiledMap::create("GameViewBg.tmx");
	m_pTileMap->setAnchorPoint(CCPointZero);
	m_pTileMapRe->setAnchorPoint(CCPointZero);    
	this->addChild(m_pTileMap, -BackGroundTag_Near, BackGroundTag_Near);
	this->addChild(m_pTileMapRe, -BackGroundTag_Near, BackGroundTag_Near);

	//
	m_fBgWidth = m_pBgSprite->boundingBox().size.width;
	m_fTileMapWidth = m_pTileMap->getMapSize().width * m_pTileMap->getTileSize().width;

	m_pBgSpriteRe->setPosition(ccp(m_fBgWidth + s.width, 0));
	m_pTileMapRe->setPosition(ccp(m_fTileMapWidth + s.width, 0));
	m_pBgSprite->setPosition(ccp(s.width, 0));
	m_pTileMap->setPosition(ccp(s.width, 0));	
	
	//init
	m_fBgVelocity = -s.width/16;//
	m_fTileMapVelocity = -s.width/8;//
}
void GameView::menuClickCallback(CCObject* pSender)
{
	CCNode *node = (CCNode *)pSender;
	int tag = node->getTag();
	switch (tag)
	{
	case REPLAY_BUTTON:
		SimpleAudioEngine::sharedEngine()->stopAllEffects();//stop effect
		CCDirector::sharedDirector()->replaceScene(
			CCTransitionFade::create(1, MainView::scene()));
		break;
	case SHARE_BUTTON:
        {
            int sns;
            if(CCApplication::sharedApplication()->getCurrentLanguage() == kLanguageChinese)
            {
                /*
                sns = CCRANDOM_0_1()*3;
                if(sns == 3)
                {
                    sns = 0;
                }
                 */
                sns = 0;
            }
            else
            {
                sns = CCRANDOM_0_1()*2 + 3;
                if (sns == 5)
                {
                    sns = 4;
                }
            }
            
            PlatformInterface::share(sns);
        }
        
		//share
		break;
	}
}
void GameView::backToMainView(float dt)
{
	this->unschedule(schedule_selector(GameView::backToMainView));
	CCDirector::sharedDirector()->replaceScene(CCTransitionFade::create(1, MainView::scene()));
}
bool GameView::gameOver()
{
	//show ad
	PlatformInterface::showAdView();

	//////////////////////////////////////////////////////////////////////////
	this->unscheduleUpdate();
	//this->unschedule(schedule_selector(GameView::addProps));

	this->setTouchEnabled(false);//disable touch

	//stop all enemy update
	CCObject *pObject = NULL;
	CCARRAY_FOREACH(_enemies,pObject)
	{
		//
		Enemy *enemy = (Enemy *)pObject;
		enemy->unscheduleUpdate();
	}

	this->showGameOverOrWinView(true);

	return true;
}

void GameView::update(float dt)
{
	CCSize s = CCDirector::sharedDirector()->getVisibleSize();
	//first move tap info bg
	CCSprite *tapInfo = (CCSprite *)this->getChildByTag(TAP_GO_INFO_BG_TAG);
	if (tapInfo)
	{
		tapInfo->setPosition(ccp(tapInfo->getPositionX() + m_fBgVelocity * dt,
							tapInfo->getPositionY()));
		if(tapInfo->getPositionX()<= -s.width/2)
		{
			tapInfo->removeFromParent();
		}
	}


	//
	CCArray *bg = this->getChildren();
	CCObject *obj = NULL;

	CCARRAY_FOREACH(bg, obj)
	{
		CCNode *bgNode = (CCNode*) obj;
		int tag = bgNode->getTag();

		if (tag == BackGroundTag_Far) {
			//far bg
			bgNode->setPositionX(bgNode->getPositionX() + m_fBgVelocity * dt);
			if (bgNode->getPositionX() < -m_fBgWidth) {
				bgNode->setPositionX(bgNode->getPositionX() + m_fBgWidth * 2 - 3);
			}
		}
		else if (tag == BackGroundTag_Near) {
			//near bg
			bgNode->setPositionX(bgNode->getPositionX() + m_fTileMapVelocity * dt);
			if (bgNode->getPositionX() < -m_fTileMapWidth) {
				bgNode->setPositionX(bgNode->getPositionX() + m_fTileMapWidth * 2 - 3);
			}
		}
	}
    
	//if collide score board
	if(_theHero->getHeroSprite()->getPositionY() == s.height - _theHero->getHeroSprite()->getContentSize().height/2)
	{
        if(!_isPlayingCollideWithScoreBoard)
        {
            SimpleAudioEngine::sharedEngine()->playEffect("OhNoSound.wav");//
            _isPlayingCollideWithScoreBoard = true;
        }
	}
    else
    {
        if(_isPlayingCollideWithScoreBoard)
        {
            _isPlayingCollideWithScoreBoard = false;
        }
    }
    
    
    //
   // this->updateProps(dt);//disable props
	if(_isGameStart)
	{
		//this->refreshEnemyNum();
	}
}

void GameView::refreshEnemyNum()
{
	CCSize s = CCDirector::sharedDirector()->getVisibleSize();
	int i = 0;
	Enemy *enemy = NULL;
	for (i = 0;i<ENEMY_NUM;i++)
	{
		enemy = (Enemy *)_enemies->objectAtIndex(i);
		if(enemy->getTheWall()->getPosition().x >= 3*s.width/4
			&&enemy->getTheWall()->getPosition().x <= 5*s.width/4)
		{
			//
			WorldLevelType type =  _gameWorld->getGameWorldLevelType();

			int NUM = 10 + type;

			int num1 = CCRANDOM_0_1()*3;

			int num2r = (int)(CCRANDOM_0_1()*NUM);
			int num3r = (int)(CCRANDOM_0_1()*NUM);

			int num2 = GameNumArray[num2r==NUM?NUM-1:num2r];//
			int num3 = GameNumArray[num3r==NUM?NUM-1:num3r];//
			
			if(num1 == 0)
			{
				//+0
				enemy->enemyRefreshNum(_theHero->getHeroNum());
				//+1
				enemy = (Enemy *)_enemies->objectAtIndex(i + 1);
				enemy->enemyRefreshNum(num2);
				//+2
				enemy = (Enemy *)_enemies->objectAtIndex(i + 2);
				enemy->enemyRefreshNum(num3);
			}
			else if(num1 == 1)
			{
				enemy->enemyRefreshNum(num2);
				
				enemy = (Enemy *)_enemies->objectAtIndex(i + 1);
				enemy->enemyRefreshNum(_theHero->getHeroNum());

				enemy = (Enemy *)_enemies->objectAtIndex(i + 2);
				enemy->enemyRefreshNum(num3);
			}
			else 
			{
				enemy->enemyRefreshNum(num2);

				enemy = (Enemy *)_enemies->objectAtIndex(i + 1);
				enemy->enemyRefreshNum(num3);

				enemy = (Enemy *)_enemies->objectAtIndex(i + 2);
				enemy->enemyRefreshNum(_theHero->getHeroNum());
			}
			break;
		}
	}
	//
}
void GameView::updateProps(float dt)
{
    CCObject *pObject = NULL;
    
    //add props if need
    //this->addProps();
    
    CCARRAY_FOREACH_REVERSE(_props, pObject)
    {
        Props *props = (Props*)pObject;
        
        CCPoint newPos = props->getTheProps()->getPosition();

        newPos.x += m_fTileMapVelocity*dt;//may be use y for good game

        if(newPos.x < 0)
        {
            props->getTheProps()->removeFromParent();
            
            _props->removeObject(props);
        }
        else
        {
            props->getTheProps()->setPosition(newPos);
            
            if(checkIfCollosionPropsAndHero(props->getTheProps()))
            {
                //may be animate here
                
                props->getTheProps()->removeFromParent();
                
                _props->removeObject(props);
                
                //random num front
                CCObject *enemyObject = NULL;
                CCARRAY_FOREACH(_enemies, enemyObject)
                {
                    //
                    Enemy *enemy = (Enemy *)enemyObject;
                    enemy->randomNumForProps();
                }
            }
        }
    }
    
}
bool GameView::checkIfCollosionPropsAndHero(CCSprite *props)
{
    CCRect propsRect = props->boundingBox();
    CCRect hero = _theHero->getHeroSprite()->boundingBox();
    
    if(propsRect.intersectsRect(hero)) return true;
    else return false;
}
void GameView::addProps(float dt)
{
    CCPoint pos;
    CCSize s = CCDirector::sharedDirector()->getVisibleSize();
    
    int sNum = _theHero->getHeroSNum();
    int num = _theHero->getHeroNum();
    
    if(sNum <= 0)return;
    if(num <=16)return;
    
    CCObject *pObject = NULL;
    
    CCARRAY_FOREACH_REVERSE(_props, pObject)
    {
        //
        Props *props = (Props*)pObject;
        if(props->getPropsType() == RANDOM_PROPS)
        {
            return;//if have one random props not add
        }
    }
    
    float random = num / sNum;
    int ifPropsOccur = CCRANDOM_0_1()*random;

    if(ifPropsOccur == 0)
    {
        //add random props
        Props *props = Props::initPropsWithType(this,RANDOM_PROPS);
        _props->addObject(props);
        
        //random pos
        float xpos = CCRANDOM_0_1()/2;
        float ypos = CCRANDOM_0_1()>=0.5?0:1;
        
        pos.x = s.width - s.width/2*xpos;
        float h = ((s.height - 6*HERO_SIZE)/4+2*HERO_SIZE);
        
        pos.y = ypos==0?(s.height - h):h;
        
        props->getTheProps()->setPosition(pos);
    }
}
void GameView::ccTouchesBegan(CCSet *pTouches,CCEvent *pEvent)
{
    CC_UNUSED CCSize s = CCDirector::sharedDirector()->getVisibleSize();
    
    if(_isGameStart)
    {
		if(_theHero->getVy() < 0)
		{
			 _theHero->setVy(-_gameWorld->getGameVelocityY() + _theHero->getVy());
		}
		else
		{
			 _theHero->setVy(-_gameWorld->getGameVelocityY());//
		}
        
        _theHero->setG(_gameWorld->getGameGravity());

        SimpleAudioEngine::sharedEngine()->playEffect("Fly.wav");
    }
    else
    {
		CCSprite *tapInfo = (CCSprite *)this->getChildByTag(TAP_GO_INFO_BG_TAG);

		tapInfo->removeChildByTag(TAP_GO_INFO_TAG);
		tapInfo->removeChildByTag(TAP_GO_TAG);
		tapInfo->removeChildByTag(GET_READY_TAG);
		
		_theHero = Hero::initHeroWithGameView(this);
		HERO_SIZE = _theHero->getHeroSprite()->boundingBox().size.height;

		this->initScoreBoard();

		//start game

        this->start();
        
		SimpleAudioEngine::sharedEngine()->playEffect("Begin.wav");//
    }
}
void GameView::start()
{
    _isGameStart = true;
    _theHero->start();
    
    CCObject *pObject = NULL;
    
    CCARRAY_FOREACH(_enemies, pObject)
    {
        //
        Enemy *enemy = (Enemy*)pObject;
        
        enemy->start();
    }
	//
	this->scheduleUpdate();
}

void GameView::stopGame()
{
	_isGameStart = false;
	this->setTouchEnabled(false);
	_theHero->stop();

	CCObject *pObject = NULL;

	CCARRAY_FOREACH(_enemies, pObject)
	{
		//
		Enemy *enemy = (Enemy*)pObject;

		enemy->stop();
	}
	//
	this->unscheduleUpdate();

}
void GameView::resumeGame()
{
	this->setTouchEnabled(true);
	start();
}

//////////////////////////////////////////////////////////////////////////

void GameView::updateWorld()
{

	int size = _changeAtIndex.size();
	if(size == 0)
	{
		return;
	}
	for (int i = 0;i<size;i++)
	{
		if (_changeAtIndex[i] == 0)
		{
			continue;
		}
		if(_theHero->getHeroNum() == GameNumArray[_changeAtIndex[i]])	
		{
			_delayChangeIndex = i;
			_changeAtIndex[i] = 0;
			this->scheduleOnce(schedule_selector(GameView::delayChangeWorld),1.0f);
			//_changeAtIndex.erase(_changeAtIndex.begin()+i);

			break;
		}
	}
	/*
	switch (ChangeType[1])
	{
	case WORLD_CHANGE_G:
		changeGameWorldGravity();
		break;
	case WORLD_CHANGE_VX:
		changeGameWorldVx();
		break;
	case WORLD_CHANGE_VY:
		changeGameWorldVy();
		break;
	case WORLD_CHANGE_COLLIDE_TYPE:
		changeGameWorldCollideType();
		break;
	}
	*/
}
void GameView::delayChangeWorld(float dt)
{
	changeWorld(_delayChangeIndex);
}
bool GameView::getIfChange(int level)
{
	//9 num

	int num;
	bool result = false;
	num = CCRANDOM_0_1() * 8 + 2;

	switch (level)
	{
	case 16:
		if(num<=2)
		{
			result = true;
		}
		else
		{
			result = false;
		}
		break;
	case 32:
		if(num<=3)
		{
			result = true;
		}
		else
		{
			result = false;
		}
		break;
	case 64:
		if(num<=4)
		{
			result = true;
		}
		else
		{
			result = false;
		}
		break;
	case 128:
		if(num<=5)
		{
			result = true;
		}
		else
		{
			result = false;
		}
		break;
	case 256:
		if(num<=6)
		{
			result = true;
		}
		else
		{
			result = false;
		}
		break;
	case 512:
		if(num<=7)
		{
			result = true;
		}
		else
		{
			result = false;
		}
		break;
	case 1024:
		if(num<=8)
		{
			result = true;
		}
		else
		{
			result = false;
		}
		break;
	case 2048:
		if(num<=9)
		{
			result = true;
		}
		else
		{
			result = false;
		}
		break;
	case 4096:
		result = true;

		break;
	default:
		result = false;
		break;
	}
	if(result)
	{
		SimpleAudioEngine::sharedEngine()->playEffect("WindSound.wav");
	}

	return result;
}
void GameView::shuffleNum()
{
	for (int i = MAX_CHANGE_TYPE_NUM - 1;i>=0;i--)
	{
		int p = CCRANDOM_0_1() * i;//bug
		int  tmp = ChangeType[i];
		ChangeType[i] = ChangeType[p];
		ChangeType[p] = tmp;
	}

}
int GameView::getBasicChange()
{
	if(_theHero->getHeroNum() > _gameNum / 4)//if > 1/4 will *2
	{
		return 2;
	}
	else
	{
		return 3;
	}
}
float GameView::getBasicChangeTime()
{
	return (_gameWorld->getGameWorldLevelType()/2 + 2.0f);
}

//////////////////////////////////////////////////////////////////////////
void GameView::changeWorld(int index)
{
	int num ;
	//change world

	_isChangingWorld = true;

	switch (_gameNum)
	{
	case 32:
		//release
		num = CCRANDOM_0_1()*2 + 1;
		if(num == 1)
		{
			//UP
			changeGameWorldCollideType(GAME_COLLIDE_BALL_UP);
		}
		else
		{
			//down
			changeGameWorldCollideType(GAME_COLLIDE_BALL_DOWN);
		}
		break;
	case 64:
		changeGameWorldVy(GAME_VY_PLUS2);
		break;
	case 128:
		changeGameWorldVy(GAME_VY_PLUS4);
		break;
	case 256:
		if (index == 0)
		{
			changeGameWorldGravity(GAME_G_PLUS2);
		}
		else if (index == 1)
		{
			changeGameWorldGravity(GAME_G_PLUS4);
		}
		break;
	case 512:
		if (index == 0)
		{
			changeGameWorldVx(GAME_VX_PLUS2);
		}
		else if (index == 1)
		{
			changeGameWorldGravity(GAME_G_PLUS4);
		}
		else
		{
			changeGameWorldVx(GAME_VX_PLUS4);
		}
		break;
	case 1024:
		if (index == 0)
		{
			changeGameWorldVy(GAME_VY_PLUS4);
		}
		else if (index == 1)
		{
			changeGameWorldGravity(GAME_G_PLUS4);
		}
		else
		{
			changeGameWorldVx(GAME_VX_PLUS4);
		}
		break;
	case 2048:
		if (index == 0)
		{
			changeGameWorldVy(GAME_VY_PLUS4);
		}
		else if (index == 1)
		{
			changeGameWorldGravity(GAME_G_PLUS4);
		}
		else
		{
			changeGameWorldVx(GAME_VX_PLUS4);
		}
		break;
	case 4096:
		if (index == 0)
		{
			changeGameWorldVy(GAME_VY_PLUS4);
		}
		else if (index == 1)
		{
			changeGameWorldGravity(GAME_G_PLUS4);
		}
		else
		{
			changeGameWorldVx(GAME_VX_PLUS4);
		}
		//may be more diffcult
		break;
	}
}
void GameView::setIsChangingWorld(bool isChanging)
{
	_isChangingWorld = isChanging;

	refreshEnemyNum();
}
bool GameView::getIsChangingWorld()
{
	return _isChangingWorld;
}
void GameView::changeGameWorldVx(GameWorldVXCharacterType type)
{
	CCObject *pObject = NULL;

	_gameWorld->setGameWorldVXCharacter(type,BASIC_CHANGE_CONTINUE*getBasicChangeTime());


	CCARRAY_FOREACH(_enemies, pObject)
	{
		//
		Enemy *enemy = (Enemy*)pObject;

		enemy->setEnemySpeedX(-_gameWorld->getGameVelocityX());
	}
}
void GameView::changeGameWorldVy(GameWorldVYCharacterType type)
{
	_gameWorld->setGameWorldVYCharacter(type,BASIC_CHANGE_CONTINUE*getBasicChangeTime());
}
void GameView::changeGameWorldGravity(GameWorldGCharacterType type)
{

	_gameWorld->setGameWorldGCharacter(type,BASIC_CHANGE_CONTINUE*getBasicChangeTime());
}
void GameView::changeGameWorldCollideType(GameCollideBallDirectionType type)
{
	_gameWorld->setGameCollideBallDirection(type,BASIC_CHANGE_CONTINUE*getBasicChangeTime());
}
//////////////////////////////////////////////////////////////////////////
void GameView::ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
	
}
void GameView::ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent)
{
}

void GameView::setGameScore(int score)
{
	_gameScore = score;
}
int GameView::getGameScore()
{
	return _gameScore;
}
int GameView::getGameNum()
{
	return _gameNum;
}
void GameView::setGameNum(int num)
{
	int index;

	_gameNum = num;

	if(_gameNum>=32)
	{
		_changeAtIndex.clear();

		//change world//no 0 index
		switch (_gameNum)
		{
		case 32:
			//
			index = CCRANDOM_0_1()*3+1;
			if(index == 4)
			{
				index = 3;
			}
			_changeAtIndex.push_back(index);

			break;
		case 64:
			//
			index = CCRANDOM_0_1()*4+1;
			if(index == 5)
			{
				index = 4;
			}
			_changeAtIndex.push_back(index);
			break;
		case 128:
			//
			index = CCRANDOM_0_1()*5+1;
			if(index == 6)
			{
				index = 5;
			}
			_changeAtIndex.push_back(index);
			break;
		case 256:
			//
			index = CCRANDOM_0_1()*3+1;
			if(index == 4)
			{
				index = 3;
			}
			_changeAtIndex.push_back(index);

			//
			index = CCRANDOM_0_1()*3+4;
			if(index == 7)
			{
				index = 6;
			}
			_changeAtIndex.push_back(index);
			break;
		case 512:
			//
			index = CCRANDOM_0_1()*2+1;
			if(index == 3)
			{
				index = 2;
			}
			_changeAtIndex.push_back(index);

			//
			index = CCRANDOM_0_1()*3+3;
			if(index == 6)
			{
				index = 5;
			}
			_changeAtIndex.push_back(index);

			//
			index = CCRANDOM_0_1()*2+6;
			if(index == 8)
			{
				index = 7;
			}
			_changeAtIndex.push_back(index);

			break;
		case 1024:
			//
			index = CCRANDOM_0_1()*2+1;
			if(index == 3)
			{
				index = 2;
			}
			_changeAtIndex.push_back(index);

			//
			index = CCRANDOM_0_1()*3+3;
			if(index == 6)
			{
				index = 5;
			}
			_changeAtIndex.push_back(index);

			//
			index = CCRANDOM_0_1()*3+6;
			if(index == 9)
			{
				index = 8;
			}
			_changeAtIndex.push_back(index);
			break;
		case 2048:
			//
			index = CCRANDOM_0_1()*2+1;
			if(index == 3)
			{
				index = 2;
			}
			_changeAtIndex.push_back(index);

			//
			index = CCRANDOM_0_1()*3+3;
			if(index == 6)
			{
				index = 5;
			}
			_changeAtIndex.push_back(index);

			//
			index = CCRANDOM_0_1()*3+7;
			if(index == 10)
			{
				index = 9;
			}
			_changeAtIndex.push_back(index);
			break;
		case 4096:
			//
			index = CCRANDOM_0_1()*3+1;
			if(index == 4)
			{
				index = 3;
			}
			_changeAtIndex.push_back(index);

			//
			index = CCRANDOM_0_1()*4+4;
			if(index == 8)
			{
				index = 7;
			}
			_changeAtIndex.push_back(index);

			//
			index = CCRANDOM_0_1()*3+8;
			if(index == 11)
			{
				index = 10;
			}
			_changeAtIndex.push_back(index);
			break;
		}
	}
}
void GameView::setGameNumLabel()
{
	//set label
	string gameNum = CCString::createWithFormat("%d", _gameNum)->getCString();

	_gameNumLabel->setString(gameNum.c_str());
	this->setFontSize(gameNum.length());
}
void GameView::setFontSize(int charLen)
{
	if (charLen == 1)
	{
		_gameNumLabel->setScale(0.9f);
	}
	else if (charLen == 2)
	{
		_gameNumLabel->setScale(0.7f);
	}
	else if (charLen == 3)
	{
		_gameNumLabel->setScale(0.5f);
	}
	else if (charLen == 4)
	{
		_gameNumLabel->setScale(0.4f);
	}
}