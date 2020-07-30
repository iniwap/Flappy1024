#ifndef __ENEMY_H__
#define __ENEMY_H__
#include "cocos2d.h"


using namespace cocos2d;


typedef enum
{
	ENEMY_ENEMY = 0,
	ENEMY_WALL_DEAD = 1,//静态墙
	ENEMY_WALL_LIVE = 2,//动态墙
	ENEMY_MOVED = 3,//自由移动怪物
	ENEMY_ATTACK = 4,//攻击性怪物
}EnemyType;

class GameView;

class Enemy : public CCNode
{
public:
	Enemy(void);
	virtual ~Enemy(void);

public:
	static Enemy* initEnemyWithGameView(GameView* gameView);
	bool initEnemy(GameView* gameView);
public:
	void update(float dt);
	void draw(void);
    void start();
	void initEnemyParam();
	void setEnemyID(int enemyID);
	void stop();
	int getLabelNum();
	int getNumLabelColorIndex();
	void setFontSize(int charLen);
    void randomNumForProps();

	void enemyRefreshNum(int num);
public:
	CC_SYNTHESIZE(GameView*, _theGameView, TheGameView);
	CC_SYNTHESIZE(CCSprite*, _theEnemy, TheEnemy);

	CC_SYNTHESIZE(float,_enemySpeedX,EnemySpeedX);
	CC_SYNTHESIZE(float,_enemySpeedY,EnemySpeedY);
	CC_SYNTHESIZE(float,_enemyA,EnemyA);
	CC_SYNTHESIZE(CCPoint,_enemyPos,EnmyPos);
	CC_SYNTHESIZE(EnemyType,_enemyType,EnemyType);

	CC_SYNTHESIZE(bool,_balloonHasCollide,BalloonHasCollide);

	CC_SYNTHESIZE(CCSprite*, _theWall, TheWall);//wall
	CC_SYNTHESIZE(CCSprite*, _theBalloon, TheBalloon);//wall
	CC_SYNTHESIZE(CCLabelBMFont*,_enemyLabel,EnemyLabel);//Label
	CC_SYNTHESIZE(int,_numOfLabel,NumOfLabel);//Label
	CC_SYNTHESIZE(bool ,_hasJudgeLoseCount,HasJudgeLoseCount);


	CC_SYNTHESIZE(float,_Vx,Vx);
	CC_SYNTHESIZE(float,_Vy,Vy);
	CC_SYNTHESIZE(float,_Ax,Ax);
	CC_SYNTHESIZE(float,_Ay,Ay);

private:
	int _enemyID;
	float _S;
	int _randomMid;

};
#endif

