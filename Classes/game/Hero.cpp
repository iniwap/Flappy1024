#include "Hero.h"
#include "GameView.h"
#include "Enemy.h"
#include "SimpleAudioEngine.h"
#include "World.h"


using namespace CocosDenshion;

using namespace std;

//#define HERO_SIZE 50


Hero::Hero(void)
{
	//_heroSprite = NULL;
}


Hero::~Hero(void)
{
}
Hero* Hero::initHeroWithGameView(GameView* game)
{
	Hero *pRet = new Hero();
	if (pRet && pRet->initHero(game))
	{
		return pRet;
	}
	else
	{
		delete pRet;
		pRet = NULL;
		return NULL;
	}
}
bool Hero::initHero(GameView* gameView)
{
	bool bRet = false;
	do 
	{
		CCSize s = gameView->getContentSize();

		_theGameView = gameView;

		_heroLoseNumCount = 0;
        _prevLoseID = 0;

		this->initHeroAnimation();
		this->initHeroNum();

		_heroNPCSprite = CCSprite::create("DeadCat.png");
		this->addChild(_heroNPCSprite);
		_heroNPCSprite->setPosition(ccp(s.width/2,s.height + _heroNPCSprite->getContentSize().height));

		//init hero param
		_vx = _vx0 = 0.0f;//set it
		_vy = _vy0 = 1.0f;//set it

		_g = _theGameView->getGameWorld()->getGameGravity();//
		_a = 0.0f;

		_theGameView->addChild(this,HERO_ZORDER);

		bRet = true;
	} while (0);

	return bRet;
}
void Hero::initHeroNum()
{
	CCPoint pos = _heroSprite->getPosition();
	float HERO_SIZE = _heroSprite->boundingBox().size.height;//width or height

	ccColor3B color = {0,0,0};

	_heroNum = 2;
	string heroNum = CCString::createWithFormat("%d", _heroNum)->getCString();

	_heroLabel = CCLabelBMFont::create(heroNum.c_str(),"fonts/west_england-64.fnt");//font to be fixed
	_heroLabel->setScale(0.8f);

	_heroLabel->setColor(color);

	_heroLabel->setPosition(ccp(HERO_SIZE/2,HERO_SIZE/4));

	_heroSprite->addChild(_heroLabel);
}
void Hero::initHeroAnimation()
{

	CCSize s = _theGameView->getContentSize();

	CCTexture2D *texture = CCTextureCache::sharedTextureCache()->addImage("HeroFrames.png");
#ifndef HERO_FRAME_NUM
#define HERO_FRAME_NUM 6
#endif

	CCArray* animFrames = CCArray::createWithCapacity(HERO_FRAME_NUM);

	// manually add frames to the frame cache
	for(int i=0;i<HERO_FRAME_NUM;i++)
	{
		CCSpriteFrame *frame = CCSpriteFrame::createWithTexture(texture, CCRectMake(100*i, 100*0, 100, 100));
		animFrames->addObject(frame);
		if(i == 0)
		{
			_heroSprite = CCSprite::createWithSpriteFrame(frame);
			_heroSprite->setPosition( ccp( s.width/2,s.height/2 ) );
			_theGameView->addChild(_heroSprite,HERO_ZORDER);
		}

	}

	CCAnimation *animation = CCAnimation::createWithSpriteFrames(animFrames, 0.2f);
	CCAnimate *animate = CCAnimate::create(animation);

	CCActionInterval* seq = CCSequence::create( animate,NULL);

	//fix img//remove later
	//float HERO_SIZE = s.width/8;//width or height

	//_heroSprite->setScale(HERO_SIZE/132.0f);
	//_heroSprite->setFlipX(true);

	_heroSprite->runAction(CCRepeatForever::create( seq ) );
}


int Hero::getHeroSNum()
{
	return _heroLoseNumCount;
}
void Hero::setScore(ScoreType type)
{
	CCSize s = CCDirector::sharedDirector()->getVisibleSize();

	// +1
	int num = _heroLoseNumCount + 1;
	//if(num == 0) num += 1;

	int score = 0;

	switch (type)
	{
	case UP_LEVEL_SCORE:
		score = _theGameView->getGameNum()*2;
		break;
	case UP_NUM_SCORE:
		score = _heroNum;
		break;
	case DOWN_NUM_SCORE:
		score = _heroNum/2;
		break;
	case WIN_SCORE:
		score = _heroNum*4;
		break;
	}

	if (_theGameView->getIsChangingWorld())
	{
		score *= 2;

		CCLabelBMFont *doubleScoreLabel = CCLabelBMFont::create("Score X 2","fonts/west_england-64.fnt");//
		doubleScoreLabel->setScale(1.0f);
		doubleScoreLabel->setPosition(ccp(s.width/2,s.height/2));

		this->addChild(doubleScoreLabel,10001);

		CCAction *sqAction = CCSequence::create(
			CCFadeOut::create(1.5f),
			CCFadeIn::create(1.0f),
			CCCallFuncND::create(this, callfuncND_selector(Hero::removeDoubleScoreInfo),(void *)doubleScoreLabel),
			NULL);
		doubleScoreLabel->runAction(sqAction);

	}

	_theGameView->setGameScore(score/num + _theGameView->getGameScore());

	_heroLoseNumCount = 0;//reset
    _prevLoseID = 0;
}

void Hero::removeDoubleScoreInfo(CCNode *pTarget,void *data)
{
	CCSprite *sp = (CCSprite *)data;
	if(!sp) return;

	sp->removeFromParentAndCleanup(true);
}
bool Hero::checkIfCollideDirectionIsRight()
{
	bool result = true;

	GameCollideBallDirectionType type;

	type = _theGameView->getGameWorld()->getGameCollideBallDirection();

	if(type == GAME_COLLIDE_BALL_BOTH)
	{
		result =  true;
	}
	else if(type == GAME_COLLIDE_BALL_DOWN)
	{
		if(_vy > 20)
		{
			result = true;
		}
		else
		{
			result = false;
		}
	}
	else if(type == GAME_COLLIDE_BALL_UP)
	{
		if(_vy > 20)
		{
			result = false;
		}
		else
		{
			result = true;
		}
	}
	return result;
}
void Hero::update(float dt)
{
	float Sx,Sy;

	if(0 != _heroSprite->retainCount())
	{
		float HERO_SIZE = _heroSprite->boundingBox().size.height;//width or height

		CCPoint pos = _heroSprite->getPosition();

		//v0*t+1/2*at^2
		Sx = _vx*dt + .5*_a*dt*dt;
		Sy = _vy*dt + .5*_g*dt*dt;

		//
		_vx += _a * dt;
		_vy += _g * dt;

		//
		if(_vy > 0)
		{
			_g += _theGameView->getGameWorld()->getGameGravity()/50;//
		}

		//
		pos.x += Sx;
		pos.y -= Sy;

		if(_theGameView->boundingBox().size.height - pos.y <= _heroSprite->boundingBox().size.height/2)
		{
			//
			pos.y = _theGameView->boundingBox().size.height - _heroSprite->boundingBox().size.height/2;
			_vy0 = 1.0f;
            _vy = -_vy;
		}

		_heroSprite->setPosition(pos);

		//_heroLabel->setPosition(ccp(pos.x,pos.y - HERO_SIZE/4));

		//_heroLabel->setRotation(CC_RADIANS_TO_DEGREES(atan2(_vy, 200)));
		//_heroSprite->setRotation(CC_RADIANS_TO_DEGREES(atan2(_vy, 200)));


		//////////////////////////////////////////////////////////////////////
		CCObject *pObject = NULL;
		CCARRAY_FOREACH(_theGameView->getTheEnemies(), pObject)
		{
			Enemy *enemy = (Enemy*)pObject;

			//

			CCPoint enemyLabelPos = enemy->getEnemyLabel()->getPosition();
			CCRect enemyRect = enemy->getTheWall()->boundingBox();
			CCRect labelEnemyRect = CCRectMake(enemyLabelPos.x - HERO_SIZE/2,enemyLabelPos.y - HERO_SIZE/2,HERO_SIZE,HERO_SIZE);
			CCRect heroRect = _heroSprite->boundingBox();


			CCSize enemySize = enemyRect.size;
			CCSize labelEnemySize = labelEnemyRect.size;

			CCPoint enemyOrign = enemyRect.origin;
			CCPoint labelEnemyOrign = labelEnemyRect.origin;

			enemyRect.setRect(enemyOrign.x + enemySize.width/8,
				enemyOrign.y + enemySize.height/8,
				enemySize.width - enemySize.width/8 * 2,
				enemySize.height - enemySize.height/8 * 2);// 1/8 will intersect
			labelEnemyRect.setRect(labelEnemyOrign.x + labelEnemySize.width/4,
				labelEnemyOrign.y + labelEnemySize.height/4,
				labelEnemySize.width - labelEnemySize.width/4 * 2,
				labelEnemySize.height - labelEnemySize.height/4 * 2);// 1/4 will intersect

			//碰撞检测
			if(heroRect.intersectsRect(enemyRect))
			{
				this->heroDead();
			}
			else if (!enemy->getBalloonHasCollide()
				&& heroRect.intersectsRect(labelEnemyRect))//是否与数字相碰且相同
			{
				enemy->setHasJudgeLoseCount(true);

				enemy->setBalloonHasCollide(true);//set has check collision

				//
				if(_heroNum == enemy->getNumOfLabel())//是否相同//乘以2
				{

					//////////////////////////////////////////////////////////////////////////
					if(!this->checkIfCollideDirectionIsRight())
					{
						//SimpleAudioEngine::sharedEngine()->playEffect("Fail.wav");//
						//this->heroDead();//姿势不对死
						break;
					}
					//////////////////////////////////////////////////////////////////////////

					if(_heroNum == 256 * pow(2,int(_theGameView->getGameWorld()->getGameWorldLevelType())))
					{
						//
						//1024 通关//这不科学
						_heroNum *= 2;//512

						this->setScore(WIN_SCORE);
						//here my be animinate
						this->unscheduleUpdate();

						this->runNumUpAction(true);

					}
					else
					{
						_heroNum *= 2;
						//
						if(_theGameView->getGameNum() == _heroNum)
						{
							//up level
							_heroNum = 2;
							string heroNum = CCString::createWithFormat("%d", _heroNum)->getCString();
							_heroLabel->setString(heroNum.c_str());
							setFontSize(heroNum.length(),_heroLabel);

							SimpleAudioEngine::sharedEngine()->playEffect("CatSoundGood.wav");//
							this->runNumUpAction(false);

							this->setScore(UP_LEVEL_SCORE);

						}
						else
						{
							string heroNum = CCString::createWithFormat("%d", _heroNum)->getCString();
							_heroLabel->setString(heroNum.c_str());
							setFontSize(heroNum.length(),_heroLabel);

							SimpleAudioEngine::sharedEngine()->playEffect("CollideBalloon.wav");//

							this->setScore(UP_NUM_SCORE);
						}

						//////////////////////////////////////////////////////////////////////////
						if(_theGameView->getGameNum() >= 32)
						{
							_theGameView->updateWorld();
						}
						//////////////////////////////////////////////////////////////////////////
					}

				}
				else//判罚
				{

					if(_heroNum > enemy->getNumOfLabel())
					{
						_heroNum /= enemy->getNumOfLabel();
						string heroNum = CCString::createWithFormat("%d", _heroNum)->getCString();
						_heroLabel->setString(heroNum.c_str());

						setFontSize(heroNum.length(),_heroLabel);

						SimpleAudioEngine::sharedEngine()->playEffect("CatSoundBad.wav");//

						this->setScore(DOWN_NUM_SCORE);
					}
					else
					{
						SimpleAudioEngine::sharedEngine()->playEffect("Fail.wav");//

						//dead
						this->heroDead();
					}
				}
				break;
			}
		}

		//死亡
		if(pos.y <= HERO_SIZE/2)
		{
			//
			//this->unscheduleUpdate();
			//_theGameView->gameOver();
			this->heroDead();
		}
	}
}
void Hero::fallDown(float dt)
{
	CCSize s = CCDirector::sharedDirector()->getVisibleSize();

	SimpleAudioEngine::sharedEngine()->playEffect("CollideEnemy.wav");

	float HERO_SIZE = _heroSprite->boundingBox().size.height;//width or height

	this->unschedule(schedule_selector(Hero::fallDown));

	_heroSprite->setRotation(180.0f);

	CCAction *sqAction = CCSequence::create(
		CCMoveTo::create(.5f,CCPointMake(s.width/2,HERO_SIZE/2)),
		CCDelayTime::create(1.0f),
		CCCallFunc::create(this, callfunc_selector(Hero::gameOver)),
		NULL);

	_heroSprite->runAction(sqAction);
	//_heroLabel->runAction(sqActionLabel);


}
void Hero::heroDead()
{
	//碰撞
	this->unscheduleUpdate();
	CCDirector::sharedDirector()->getRunningScene()->runAction(CCShaky3D::create(0.5, CCSizeMake(1,1), 4, false));

	//执行跌落动画//动画结束则游戏结束

	this->schedule(schedule_selector(Hero::fallDown), .2f);
}
void Hero::gameOver()
{
	//_heroSprite->stopAllActions();//may be it not dead
	_theGameView->gameOver();
}
void Hero::start()
{
	this->scheduleUpdate();//frame call

}
void Hero::stop()
{
	this->unscheduleUpdate();//frame call

}
void Hero::draw()
{
    /*
#ifndef COCOS2D_DEBUG
	float HERO_SIZE = _heroSprite->boundingBox().size.height;//width or height
	CCSize heroSize = _heroSprite->boundingBox().size;//to be fixed for 50*50
	CCPoint pos = _heroSprite->getPosition();
	CCPoint orign = ccp(pos.x - HERO_SIZE/2,pos.y + HERO_SIZE/2);
	CCPoint dest = ccp(pos.x + HERO_SIZE/2,pos.y + 3*HERO_SIZE/2);

	ccColor4F color = {0.8f,0,0,0.8f};

	CC_UNUSED ccColor4F color1 = {0,0,0,1.0f};

	ccDrawSolidRect(orign,dest,color);


	ccDrawRect(_heroSprite->boundingBox().origin,
		ccp(_heroSprite->boundingBox().getMaxX(),_heroSprite->boundingBox().getMaxY()));


#endif // COCOS2D_DEBUG
*/
}
void Hero::setFontSize(int charLen,CCLabelBMFont *numLabel)
{
	float scale = 0.9f;
	if (charLen == 1)
	{
		scale = 0.9f;
	}
	else if (charLen == 2)
	{
		scale = 0.7f;
	}
	else if (charLen == 3)
	{
		scale = 0.5f;
	}
	else if(charLen == 4)
	{
		scale = 0.4f;
	}

	numLabel->setScale(scale);
}
void Hero::runNumUpAction(bool isWin)
{
	static bool boolWin;
	boolWin = isWin;

	float HERO_SIZE = _heroSprite->boundingBox().size.height;//width or height
	CCSize s = CCDirector::sharedDirector()->getVisibleSize();
	ccColor3B c = {255,255,0};

	CCSprite *sp = CCSprite::create("Balloon.png");
	sp->setPosition(_heroSprite->getPosition());
	_theGameView->addChild(sp,1000);


	string num = CCString::createWithFormat("%d", _theGameView->getGameNum())->getCString();

	//update game num
	_theGameView->setGameNum(_theGameView->getGameNum()*2);


	CCLabelBMFont *numLabel = CCLabelBMFont::create(num.c_str(),"fonts/west_england-64.fnt");
	numLabel->setColor(c);

	this->setFontSize(num.length(),numLabel);
	numLabel->setPosition(ccp(sp->getContentSize().width/2,sp->getContentSize().height/2));

	sp->addChild(numLabel,1000);

	CCAction *sqAction = CCSequence::create(
		CCMoveTo::create(2.0f,CCPointMake(s.width/2 + 15,s.height - 3*sp->getContentSize().height/4)),
		CCFadeOut::create(0.5f),
		CCCallFuncND::create(this, callfuncND_selector(Hero::GameNumUp),(void *)sp),
		CCDelayTime::create(1.0f),
		CCCallFuncND::create(this, callfuncND_selector(Hero::GameIfWin),(void *)sp),
		NULL);


	sp->setUserData((void*)&boolWin);

	sp->runAction(sqAction);
}
void Hero::GameNumUp(CCNode *pTarget,void *data)
{
	CCSprite *sp = (CCSprite *)data;
	if(!sp) return;
	sp->removeAllChildrenWithCleanup(true);

	_theGameView->setGameNumLabel();
}
void Hero::GameIfWin(CCNode *pTarget,void *data)
{
	CCSprite *sp = (CCSprite *)data;
	if(!sp) return;
	bool isWin = *((bool*)sp->getUserData());

	sp->removeFromParentAndCleanup(true);

	if(isWin)
	{
		_theGameView->gameWin();
	}
}
void Hero::setHeroLoseNumCount(int count,int enemyID)
{
	CCSize s = CCDirector::sharedDirector()->getVisibleSize();

	ccColor3B c = {255,0,0};

    if(!getIfAddLoseNum(enemyID)) return;
    
	if (count >= 4)
	{
		//
		_theGameView->stopGame();
		CCAction *sqAction = CCSequence::create(
			CCMoveTo::create(0.5,ccp(s.width/2,_heroSprite->getPositionY() - _heroSprite->getContentSize().height/2)),
			CCCallFunc::create(this, callfunc_selector(Hero::heroDead)),
			NULL);

		_heroNPCSprite->runAction(sqAction);
	}

	else
	{
        
        _prevLoseID = enemyID;
		_heroLoseNumCount = count;

		string str = CCString::createWithFormat("%d", _heroLoseNumCount )->getCString();
        string str1 = CCString::createWithFormat("%d", _heroLoseNumCount + 1)->getCString();
		str = "Lose + " + str + "(Score/" + str1 + ")";

		CCLabelBMFont *label = CCLabelBMFont::create(str.c_str(),"fonts/west_england-64.fnt");//
		label->setScale(0.8f);
		label->setPosition(ccp(s.width/2,s.height/2));
		label->setColor(c);

		this->addChild(label,10001);
		
		CCPoint pos;
        
        pos = ccp(s.width/2,s.height/2 + 2*label->getContentSize().height);

		CCAction *sqAction = CCSequence::create(
			CCMoveTo::create(0.5f,pos),
			CCFadeOut::create(.8f),
            CCFadeIn::create(.5f),
            CCFadeOut::create(.2f),
			CCCallFuncND::create(this, callfuncND_selector(Hero::removeLoseNumInfo),(void *)label),
			NULL);

		label->runAction(sqAction);
	}
}
bool Hero::getIfAddLoseNum(int enemyID)
{
    bool nRet = true;
    
    if((_prevLoseID <=3&&_prevLoseID>0&&enemyID<=3&enemyID>0))
    {
        nRet = false;
    }
    else if((_prevLoseID <=6&&_prevLoseID>3&&enemyID<=6&enemyID>3))
    {
        nRet = false;
    }
    else if((_prevLoseID <=9&&_prevLoseID>6&&enemyID<=9&enemyID>6))
    {
        nRet = false;
    }
    
    
    return nRet;
}
int Hero::getLoseID()
{
    return _prevLoseID;
}
void Hero::setLoseID()
{
    _prevLoseID = 0;
}

int Hero::getHeroLoseNumCount()
{
	return _heroLoseNumCount;
}

void Hero::removeLoseNumInfo(CCNode *pTarget,void *data)
{
	CCSprite *sp = (CCSprite *)data;
	if(!sp) return;

	sp->removeFromParentAndCleanup(true);
}