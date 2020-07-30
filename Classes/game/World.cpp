#include "cocos2d.h"
#include "World.h"
#include "GameView.h"
#include "Enemy.h"

using namespace std;
using namespace cocos2d;

static CCSize s;

#define  Base_gravity 9.8f*50*s.height/960
#define  Base_gameVelocityX s.width/4
#define  Base_gameVelocityY s.height/3/2

World::World(GameView *gameView)
{
	s = CCDirector::sharedDirector()->getVisibleSize();

	_gameView = gameView;

	_gameRequireNum = GAME_NO_REQUIRE;
	_gameCollideBallDirection = GAME_COLLIDE_BALL_BOTH;
	
	_gameWorldGCharacter = GAME_G_PLUS1;
	_gameWorldACharacter = GAME_A_PLUS1;
	_gameWorldVXCharacter = GAME_VX_PLUS1;
	_gameWorldVYCharacter = GAME_VY_PLUS1;

	_gameWorldLevel = WORLD_LEVEL_1024;

	_gameWorld =  GAME_WORLD_SILENT_NORMAL;

	_gameGravity = Base_gravity;

	_gameAcceleration = 0.0f;

	_gameVelocityX = Base_gameVelocityX;//enemy

	_gameVelocityY = Base_gameVelocityY;//fly +vy


	_gameView->addChild(this);
}


World::~World(void)
{

}
WorldLevelType World::getGameWorldLevelType()
{
	return _gameWorldLevel;
}
void World::setGameWorldLevelType(WorldLevelType type)
{
	_gameWorldLevel = type;
}

#define WORLD_INFO_BG_TAG 3000

void World::showWorldChangeInfo(WorldChangeType type)
{
	CCSize s = CCDirector::sharedDirector()->getVisibleSize();

	int INFO_FRAME_NUM;
	int index,maxJ;

	CCSprite *bg = CCSprite::create("InfoBg.png");
	bg->setPosition(ccp(s.width/2,s.height/2));
	_gameView->addChild(bg,WORLD_INFO_BG_TAG,WORLD_INFO_BG_TAG);

	CCSprite *info;

	string animationPath = "";

	switch (type)
	{
	case WORLD_CHANGE_G:
		INFO_FRAME_NUM = 4;
		animationPath = animationPath + "WorldChangeGInfo_";
		index = 0;
		maxJ = 1;
		break;
	case WORLD_CHANGE_A:
		INFO_FRAME_NUM = 3;
		animationPath = animationPath + "WorldChangeAInfo_";
		index = 0;
		maxJ = 1;
		break;
	case WORLD_CHANGE_VX:
		INFO_FRAME_NUM = 4;
		animationPath = animationPath + "WorldChangeVXInfo_";
		index = 0;
		maxJ = 1;
		break;
	case WORLD_CHANGE_VY:
		INFO_FRAME_NUM = 4;
		animationPath = animationPath + "WorldChangeVYInfo_";
		index = 0;
		maxJ = 1;
		break;
	case WORLD_CHANGE_COLLIDE_TYPE:
		INFO_FRAME_NUM = 5;
		animationPath = animationPath + "WorldChangeCollideTypeInfo_";
		index = _gameCollideBallDirection;
		maxJ = 2;
		break;
	case WORLD_CHANGE_WORLD:
		/*
		INFO_FRAME_NUM = 5;
		animationPath = animationPath + "";
		index = _gameWorldGCharacter;
		*/
		break;
	}

	animationPath  = animationPath + CCString::createWithFormat("%d",(int) index)->getCString() + ".png";

	CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage(animationPath.c_str());

	CCArray* animFrames = CCArray::createWithCapacity(INFO_FRAME_NUM);

	// manually add frames to the frame cache
	for(int j=0;j<maxJ;j++)
	{
		for(int i=0;i<INFO_FRAME_NUM;i++)
		{
			CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture, CCRectMake(260*i, 160*j, 260, 160));
			animFrames->addObject(frame);
			if(i == 0 && j == 0)
			{
				info = CCSprite::createWithSpriteFrame(frame);
				info->setPosition( ccp( bg->getContentSize().width/2,bg->getContentSize().height/3 ) );
				bg->addChild(info,WORLD_INFO_BG_TAG+1);
			}

		}
	}

	CCAnimation *animation = CCAnimation::createWithSpriteFrames(animFrames, .2f);
	CCAnimate *animate = CCAnimate::create(animation);

	CCActionInterval* seq = CCSequence::create( animate,NULL);
	info->runAction(CCRepeatForever::create( seq ) );

	//timer
	ccColor3B color = {0,0,0};
	_timerCount = 5;
	CCLabelBMFont*  timerLabel;
	string scoreNum = CCString::createWithFormat("%d", _timerCount)->getCString();
	timerLabel = CCLabelBMFont::create(scoreNum.c_str(),"fonts/west_england-64.fnt",32);//
	timerLabel->setScale(0.8f);
	timerLabel->setColor(color);
	bg->addChild(timerLabel,WORLD_INFO_BG_TAG+2,WORLD_INFO_BG_TAG+2);//
	timerLabel->setPosition(ccp(bg->getContentSize().width - timerLabel->getContentSize().width - 5,
							2*bg->getContentSize().height/3 - timerLabel->getContentSize().height - 5));

	this->schedule(schedule_selector(World::updateTimer),1.0f);
	_gameView->stopGame();
}
void World::updateTimer(float dt)
{
	_timerCount--;

	CCSprite *bg = (CCSprite *)_gameView->getChildByTag(WORLD_INFO_BG_TAG);
	CCLabelBMFont*  timerLabel = (CCLabelBMFont* )bg->getChildByTag(WORLD_INFO_BG_TAG+2);
	
	string scoreNum = CCString::createWithFormat("%d", _timerCount)->getCString();

	timerLabel->setString(scoreNum.c_str());

	if(_timerCount == 0)
	{
		this->unschedule(schedule_selector(World::updateTimer));

		hideInfo();
	}
}
void World::hideInfo()
{
	CCSprite *bg = (CCSprite *)_gameView->getChildByTag(WORLD_INFO_BG_TAG);

	bg->removeAllChildrenWithCleanup(true);
	bg->removeFromParentAndCleanup(true);
	
	_gameView->resumeGame();
}

GameCollideBallDirectionType World::getGameCollideBallDirection()
{
	return _gameCollideBallDirection;
}
GameWorldGCharacterType World::getGameWorldGCharacter()
{
	return _gameWorldGCharacter;
}

GameWorldACharacterType World::getGameWorldACharacter()
{
	return _gameWorldACharacter;//
}
GameWorldVXCharacterType World::getGameWorldVXCharacter()
{
	return _gameWorldVXCharacter;//
}
GameWorldVYCharacterType  World::getGameWorldVYCharacter()
{
	return _gameWorldVYCharacter;//
}
void World::unscheduleAll()
{
	this->unschedule(schedule_selector(World::restG));
	this->unschedule(schedule_selector(World::restA));
	this->unschedule(schedule_selector(World::restVx));
	this->unschedule(schedule_selector(World::restVy));

	this->stopAllActions();
	this->removeAllChildrenWithCleanup(true);
	//_gameView->getChildByTag(WORLD_INFO_BG_TAG)->stopAllActions();
}
void World::restG(float dt)
{	
	_gameWorldGCharacter = GAME_G_PLUS1;

	_gameGravity = Base_gravity;

	_gameView->setIsChangingWorld(false);
}
void World::restA(float dt)
{
	_gameWorldACharacter = GAME_A_PLUS1;
	_gameAcceleration = 0.0f;
	_gameView->setIsChangingWorld(false);
}
void World::restVx(float dt)
{
	CCObject *pObject = NULL;

	_gameWorldVXCharacter = GAME_VX_PLUS1;
	_gameVelocityX = Base_gameVelocityX;//enemy

	CCARRAY_FOREACH(_gameView->getTheEnemies(), pObject)
	{
		//
		Enemy *enemy = (Enemy*)pObject;

		enemy->setEnemySpeedX(-_gameVelocityX);

	}
	_gameView->setIsChangingWorld(false);
}
void World::restVy(float dt)
{
	_gameWorldVYCharacter = GAME_VY_PLUS1;
	_gameVelocityY = Base_gameVelocityY;//fly +vy
	_gameView->setIsChangingWorld(false);

}
void World::restCollideType(float dt)
{
	_gameCollideBallDirection = GAME_COLLIDE_BALL_BOTH;
	_gameView->setIsChangingWorld(false);

}
void World::setGameCollideBallDirection(GameCollideBallDirectionType type,float dt)
{
	if(_gameCollideBallDirection == type)
	{
		return;
	}
	_gameCollideBallDirection = type;

	showWorldChangeInfo(WORLD_CHANGE_COLLIDE_TYPE);

	this->scheduleOnce(schedule_selector(World::restCollideType),dt);
}
void  World::setGameWorldGCharacter(GameWorldGCharacterType type,float dt)
{
	if(type == _gameWorldGCharacter)
	{
		return;
	}

	_gameWorldGCharacter = type;

	float rate;

	switch (type)
	{
	case GAME_VY_PLUS1:
		rate = 1.0f;
		break;
	case GAME_VY_PLUS2:
		rate = 2.0f;
		break;//this use for touch +v
	case GAME_VY_PLUS4:
		rate = 2.8f;
		break;
	}
	_gameGravity = rate * Base_gravity;

	if(dt == 0)
	{
		return;
	}

	showWorldChangeInfo(WORLD_CHANGE_G);

	this->scheduleOnce(schedule_selector(World::restG),dt);

}
void  World::setGameWorldACharacter(GameWorldACharacterType type,float dt)
{
	if(type == _gameWorldACharacter)
	{
		return;
	}

	_gameWorldACharacter = type;

	float rate;

	switch (type)
	{
	case GAME_VY_PLUS1:
		rate = 1.0f;
		break;
	case GAME_VY_PLUS2:
		rate = 2.0f;
		break;//this use for touch +v
	case GAME_VY_PLUS4:
		rate = 4.0f;
		break;
	}
	_gameAcceleration += rate*10;


	if(dt == 0)
	{
		return;
	}

	showWorldChangeInfo(WORLD_CHANGE_A);

	this->scheduleOnce(schedule_selector(World::restA),dt);
}
void World::setGameWorldVXCharacter(GameWorldVXCharacterType type,float dt)
{
	if(type == _gameWorldVXCharacter)
	{
		return;
	}

	_gameWorldVXCharacter = type;

	float rate;

	switch (type)
	{
	case GAME_VY_PLUS1:
		rate = 1.0f;
		break;
	case GAME_VY_PLUS2:
		rate = 2.0f;
		break;//this use for touch +v
	case GAME_VY_PLUS4:
		rate = 4.0f;
		break;
	}
	_gameVelocityX = rate * Base_gameVelocityX;

	if(dt == 0)
	{
		return;
	}

	showWorldChangeInfo(WORLD_CHANGE_VX);

	this->scheduleOnce(schedule_selector(World::restVx),dt);
}
void  World::setGameWorldVYCharacter(GameWorldVYCharacterType type,float dt)
{
	float rate;

	if(type == _gameWorldVYCharacter)
	{
		return;
	}

	_gameWorldVYCharacter = type;
	switch (type)
	{
	case GAME_VY_PLUS1:
		rate = 1.0f;
		break;
	case GAME_VY_PLUS2:
		rate = 1.5f;
		break;//this use for touch +v
	case GAME_VY_PLUS4:
		rate = 2.0f;
		break;
	}
	_gameVelocityY = rate * Base_gameVelocityY;

	if(dt == 0)
	{
		return;
	}

	showWorldChangeInfo(WORLD_CHANGE_VY);

	this->scheduleOnce(schedule_selector(World::restVy),dt);
}
float World::getGameGravity()
{
	return _gameGravity;
}
float World::getGameAcceleration()
{
	return _gameAcceleration;
}

float World::getGameVelocityX()
{
	return _gameVelocityX;
}
float World::getGameVelocityY()
{
	return _gameVelocityY;
}