#include "Enemy.h"
#include "GameView.h"
#include "Hero.h"
#include "World.h"

using namespace std;

static CCSize viewSize;


static ccColor4F NumLabelColor[10] = {  {0.8f,0,0,0.8f},//2
									{0.4f,0.2f,0,0.8f},//4
									{0,0,0.8f,0.8f},//8
									{0.1f,0,0.5f,0.8f},//16
									{0.0f,0.5f,0.5f,0.8f},//32
									{0.0f,0.4f,0,0.8f},//64
									{0.8f,0,0.4f,0.8f},//128
									{0.8f,0.2f,0.5f,0.8f},//256
									{0.2f,0.5f,0,0.8f},//512
									{0.1f,0.7f,0,0.8f},//1024
									};//Color


Enemy::Enemy(void)
{

}


Enemy::~Enemy(void)
{

}

Enemy* Enemy::initEnemyWithGameView(GameView* gameView)
{
	Enemy *pRet = new Enemy();
	if (pRet && pRet->initEnemy(gameView))
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
bool Enemy::initEnemy(GameView* gameView)
{
	bool bRet = false;
	do 
	{
		CCSize s = gameView->getContentSize();

		viewSize = CCDirector::sharedDirector()->getVisibleSize();

		_theGameView = gameView;
		_enemySpeedX = - _theGameView->getGameWorld()->getGameVelocityX();

		_enemyA = 0.0f;//no a

		_enemySpeedY = 0.0f;//this is enmey speed not the world speed

		_enemyID = 0;//no

		_balloonHasCollide = false;
		_hasJudgeLoseCount = false;

		_theGameView->addChild(this,ENEMY_ZORDER);

		_enemyType = ENEMY_ENEMY;
		this->initEnemyParam();

		bRet = true;

	} while (0);

	return bRet;
}

void Enemy::start()
{
    this->scheduleUpdate();
}
void Enemy::stop()
{
	this->unscheduleUpdate();
}
void Enemy::update(float dt)
{
	CCPoint pos = _theWall->getPosition();

	CCSize ws = _theWall->boundingBox().size;

	pos.x += dt * _enemySpeedX;
	pos.y += dt * _enemySpeedY;

	if(pos.x <= -viewSize.width/4)
	{
		//reset

		//here update mid enemy to random pos

		_balloonHasCollide = false;
		_hasJudgeLoseCount = false;

        if(_theGameView->getTheHero()->getLoseID() == _enemyID)
        {
            _theGameView->getTheHero()->setLoseID();
        }
        
		//random mid
		_randomMid = (CCRANDOM_0_1()>0.5?1:-1);
		float h = (viewSize.height - 6*ws.height)/2;
		float tmpY = viewSize.height - (ws.height*3 + h);

		if (_theGameView->getGameWorld()->getGameWorldLevelType() == WORLD_LEVEL_4096)
		{
			//4096
			int upOrDown = (CCRANDOM_0_1()>0.5?1:-1);

			float rate = CCRANDOM_0_1();

			tmpY += upOrDown * rate * h;
		}

		if(_enemyID == 1 || _enemyID == 4 || _enemyID == 7)
		{
			_theWall->setPosition(ccp(viewSize.width + viewSize.width/4 ,pos.y));
			_enemyLabel->setPosition(ccp(viewSize.width + viewSize.width/4 ,pos.y - ws.height));
			_theBalloon->setPosition(ccp(viewSize.width + viewSize.width/4 ,pos.y - ws.height));
		}
		else if(_enemyID == 2 || _enemyID == 5 || _enemyID == 8)//mid
		{
			_theWall->setPosition(ccp(viewSize.width + viewSize.width/4 ,tmpY + ws.height/2*_randomMid));
			_enemyLabel->setPosition(ccp(viewSize.width + viewSize.width/4 ,tmpY - ws.height/2*_randomMid));
			_theBalloon->setPosition(ccp(viewSize.width + viewSize.width/4 ,tmpY - ws.height/2*_randomMid));
		}
		else if(_enemyID == 3 || _enemyID == 6 || _enemyID == 9)
		{
			_theWall->setPosition(ccp(viewSize.width + viewSize.width/4 ,pos.y));
			_enemyLabel->setPosition(ccp(viewSize.width + viewSize.width/4 ,pos.y - ws.height));
			_theBalloon->setPosition(ccp(viewSize.width + viewSize.width/4 ,pos.y - ws.height));
		}

		//
		///数字标签
		string num;
		ccColor3B c = {0,0,0};
		_numOfLabel =  getLabelNum();
		num	= CCString::createWithFormat("%d", _numOfLabel)->getCString();
		_enemyLabel->setString(num.c_str());
		_enemyLabel->setColor(c);

		this->setFontSize(num.length());
	}
	else
	{
		//
		ccColor3B c = {255,255,255};
		if (!_hasJudgeLoseCount && !_theGameView->getIsChangingWorld())
		{
			if(pos.x <= viewSize.width/2 - ws.width)
			{
				_hasJudgeLoseCount = true;
				if(_numOfLabel == _theGameView->getTheHero()->getHeroNum())
				{
					_enemyLabel->setColor(c);
					_theGameView->getTheHero()->setHeroLoseNumCount(_theGameView->getTheHero()->getHeroLoseNumCount() + 1,_enemyID);
				}
			}
		}


		//here update 8192 level to move the mid enemy

		_theWall->setPosition(pos);

		if(_enemyID == 1 || _enemyID == 4 || _enemyID == 7)
		{
			_enemyLabel->setPosition(ccp(pos.x,pos.y - ws.height));
			_theBalloon->setPosition(ccp(pos.x,pos.y - ws.height));
		}
		else if(_enemyID == 2 || _enemyID == 5 || _enemyID == 8)
		{
			//
			if (_theGameView->getGameWorld()->getGameWorldLevelType() == WORLD_LEVEL_8192)
			{
				float h = (viewSize.height - 6*ws.height)/2;
				float tmpY = viewSize.height - (ws.height*3 + h);

				if(pos.y >= tmpY + h || pos.y <= tmpY - h)
				{
					_enemySpeedY = -_enemySpeedY;
				}
			}

			_enemyLabel->setPosition(ccp(pos.x,pos.y - ws.height*_randomMid));
			_theBalloon->setPosition(ccp(pos.x,pos.y - ws.height*_randomMid));
		}
		else if(_enemyID == 3 || _enemyID == 6 || _enemyID == 9)
		{
			_enemyLabel->setPosition(ccp(pos.x,pos.y + ws.height));
			_theBalloon->setPosition(ccp(pos.x,pos.y + ws.height));
		}
	}

}
void Enemy::draw()
{
    /*
#ifndef COCOS2D_DEBUG

	CCSize wallSize = _theWall->boundingBox().size;
	CCPoint pos,orign,dest;
	pos = _enemyLabel->getPosition();

	if(_enemyID == 1 || _enemyID == 4 || _enemyID == 7)
	{
		orign = ccp(pos.x - wallSize.width/2,pos.y - wallSize.height/2);
		dest = ccp(pos.x + wallSize.width/2,pos.y + wallSize.height/2);
	}
	else if(_enemyID == 2 || _enemyID == 5 || _enemyID == 8)
	{
		orign = ccp(pos.x - wallSize.width/2,pos.y - wallSize.height/2);
		dest = ccp(pos.x + wallSize.width/2,pos.y + wallSize.height/2);
	}

	else if(_enemyID == 3 || _enemyID == 6 || _enemyID == 9)
	{
		orign = ccp(pos.x - wallSize.width/2,pos.y - wallSize.height/2);
		dest = ccp(pos.x + wallSize.width/2,pos.y + wallSize.height/2);
	}

	ccDrawSolidRect(orign,dest,NumLabelColor[getNumLabelColorIndex()]);

#endif // COCOS2D_DEBUG
     */
}
void  Enemy::initEnemyParam()
{
	switch (_enemyType)
	{
	case ENEMY_WALL_DEAD:
		_Vx = _Vy = _Ax = _Ay = _S = 0.0f;

		break;
	case ENEMY_WALL_LIVE:
		_Vx = _Vy = _Ax = _Ay = _S = 0.0f;

		_Vy = 10.0f;
		_Ay = 0.0f;

		break;
	case ENEMY_MOVED:
		_Vx = _Vy = _Ax = _Ay = _S = 0.0f;

		_Vx = 10.0f;
		_Vy = 10.0f;
		_Ax = 0.0f;
		_Ay = 0.0f;

		break;
	case ENEMY_ATTACK:
		_Vx = _Vy = _Ax = _Ay = _S = 0.0f;

		_Vx = 10.0f;
		_Vy = 10.0f;
		_Ax = 10.0f;
		_Ay = 10.0f;

		break;
	default:
		_Vx = _Vy = _Ax = _Ay = _S = 0.0f;
		break;
	}
}

int Enemy::getLabelNum()
{
	Hero *hero = _theGameView->getTheHero();
	//2,4,8,16,32,64,128,256,512,1024,2048//4096,8192
	WorldLevelType type =  _theGameView->getGameWorld()->getGameWorldLevelType();

	int NUM = 9 + type;

	if(CCRANDOM_0_1()*NUM <= NUM/9)//1/9//large than other *2
	{
		if (hero != NULL)
		{
			return hero->getHeroNum();
		}
		else
		{
			return 2;
		}
	}
	else
	{
		int num = CCRANDOM_0_1()*NUM + 1;
		return pow(2,num);
	}
}
int Enemy::getNumLabelColorIndex()
{
	switch (_numOfLabel)
	{
	case 2:
		return 0;
	case 4:
		return 1;
	case 8:
		return 2;
	case 16:
		return 3;
	case 32:
		return 4;
	case 64:
		return 5;
	case 128:
		return 6;
	case 256:
		return 7;
	case 512:
		return 8;
	case 1024:
		return 9;
	}
	return 0;
}
void Enemy::setEnemyID(int enemyID)
{
	CCPoint pos;
	ccColor3B color = {0,0,0};

	CCSize s = CCDirector::sharedDirector()->getVisibleSize();

	_enemyID = enemyID;

	_theWall = CCSprite::create("Enemy.png");
	this->addChild(_theWall,ENEMY_ZORDER);

	_theBalloon = CCSprite::create("Balloon.png");
	this->addChild(_theBalloon,ENEMY_ZORDER);

	///数字标签
	string num;
	_numOfLabel = getLabelNum();
	num	= CCString::createWithFormat("%d",  _numOfLabel)->getCString();
	
	_enemyLabel = CCLabelBMFont::create(num.c_str(),"fonts/west_england-64.fnt");//font to be fixed
	this->setFontSize(num.length());

	_enemyLabel->setColor(color);
	this->addChild(_enemyLabel,ENEMY_ZORDER);

	CCSize ws = _theWall->boundingBox().size;
	float h = (s.height - 6*ws.height)/2;

	switch (_enemyID)
	{
	case 1:
		pos = ccp(5*s.width/4 + viewSize.width,s.height - ws.height);
		break;
	case 2:
		pos = ccp(5*s.width/4 + viewSize.width,s.height - (ws.height*3 + h));//mid
		break;
	case 3:
		pos = ccp(5*s.width/4 + viewSize.width,ws.height);
		break;
	case 4:
		pos = ccp(7*s.width/4 + viewSize.width,s.height - ws.height);
		break;
	case 5:
		pos = ccp(7*s.width/4 + viewSize.width,s.height - (ws.height*3 + h));//mid
		break;
	case 6:
		pos = ccp(7*s.width/4 + viewSize.width,ws.height);
		break;
	case 7:
		pos = ccp(9*s.width/4 + viewSize.width,s.height - ws.height);
		break;
	case 8:
		pos = ccp(9*s.width/4 + viewSize.width,s.height - (ws.height*3 + h));//mid
		break;
	case 9:
		pos = ccp(9*s.width/4 + viewSize.width,ws.height);
		break;
	};

	//random mid
	_randomMid = (CCRANDOM_0_1()>0.5?1:-1);
	if(_enemyID == 1 || _enemyID == 4 || _enemyID == 7)
	{
		_theWall->setPosition(ccp(pos.x,pos.y + ws.height/2));
		_enemyLabel->setPosition(ccp(pos.x,pos.y - ws.height/2));
		_theBalloon->setPosition(ccp(pos.x,pos.y - ws.height/2));
	}
	else if(_enemyID == 2 || _enemyID == 5 || _enemyID == 8)
	{
		//speed the mid
		if (_theGameView->getGameWorld()->getGameWorldLevelType() == WORLD_LEVEL_8192)
		{
			//set to 0-1 _enemyspeedx
			float rate = CCRANDOM_0_1();
			_enemySpeedY = rate * _enemySpeedX;
		}
		_theWall->setPosition(ccp(pos.x,pos.y + ws.height/2*_randomMid));
		_enemyLabel->setPosition(ccp(pos.x,pos.y - ws.height/2*_randomMid));
		_theBalloon->setPosition(ccp(pos.x,pos.y - ws.height/2*_randomMid));
	}
	else if(_enemyID == 3 || _enemyID == 6 || _enemyID == 9)
	{
		_theWall->setPosition(ccp(pos.x,pos.y - ws.height/2));
		_enemyLabel->setPosition(ccp(pos.x,pos.y + ws.height/2));
		_theBalloon->setPosition(ccp(pos.x,pos.y + ws.height/2));
	}
}

void Enemy::randomNumForProps()
{
    //
    string num;
	_numOfLabel = getLabelNum();
	num	= CCString::createWithFormat("%d",  _numOfLabel)->getCString();
    
    _enemyLabel->setString(num.c_str());
	this->setFontSize(num.length());
}

void Enemy::setFontSize(int charLen)
{
	if (charLen == 1)
	{
		_enemyLabel->setScale(0.9f);
	}
	else if (charLen == 2)
	{
		_enemyLabel->setScale(0.7f);
	}
	else if (charLen == 3)
	{
		_enemyLabel->setScale(0.5f);
	}
	else if(charLen == 4)
	{
		_enemyLabel->setScale(0.4f);
	}
}
void Enemy::enemyRefreshNum(int num)
{
	///数字标签
	string num_str;
	_numOfLabel =  num;
	num_str	= CCString::createWithFormat("%d", _numOfLabel)->getCString();
	_enemyLabel->setString(num_str.c_str());

	this->setFontSize(num_str.length());
}