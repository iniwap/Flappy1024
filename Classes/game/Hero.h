#ifndef HERO_H
#define  HERO_H
#include "cocos2d.h"


using namespace cocos2d;

class GameView;

typedef enum
{
	UP_LEVEL_SCORE = 1,
	UP_NUM_SCORE = 2,
	DOWN_NUM_SCORE = 3,
	WIN_SCORE = 4,
}ScoreType;

class Hero : public CCNode
{
public:
	Hero(void);
	~Hero(void);
public:
	//float _v0;//init v
	CC_SYNTHESIZE(float,_vx0,Vx0);
	CC_SYNTHESIZE(float,_vy0,Vy0);
	//float _v;//current v
	CC_SYNTHESIZE(float,_vx,Vx);
	CC_SYNTHESIZE(float,_vy,Vy);
	//float _g;//9.8 gravity
	CC_SYNTHESIZE(float,_g,G);
	//
	CC_SYNTHESIZE(float,_a,A);

public:
	void update(float dt);
	void draw(void);
	void start();
	void fallDown(float dt);
	void heroDead();
	void gameOver();
	void stop();
//
public:
	static Hero* initHeroWithGameView(GameView* game);
	bool initHero(GameView* gameView);
	void initHeroAnimation();
	void initHeroNum();
	//getter//setter
	CC_SYNTHESIZE(GameView*, _theGameView, TheGameView);
	CC_SYNTHESIZE(CCSprite*, _heroSprite, HeroSprite);
	CC_SYNTHESIZE(CCSprite*, _heroNPCSprite, HeroNPCSprite);
	CC_SYNTHESIZE(CCLabelBMFont*, _heroLabel, HeroLabel);
	CC_SYNTHESIZE(int,_heroNum,HeroNum);
    int getHeroSNum();
    //CC_SYNTHESIZE(int,_heroSNum,HeroSNum);//
	void setHeroLoseNumCount(int count,int enemyID);
	int getHeroLoseNumCount();
    void setLoseID();
    int getLoseID();
private:
	int _heroLoseNumCount;
    int _prevLoseID;
    bool getIfAddLoseNum(int enemyID);
	void setScore(ScoreType type);
	void setFontSize(int charLen,CCLabelBMFont *numLabel);

public:
	void runNumUpAction(bool isWin);
	void GameNumUp(CCNode *pTarget,void *data);
	void GameIfWin(CCNode *pTarget,void *data);
public:
	bool checkIfCollideDirectionIsRight();
	void removeDoubleScoreInfo(CCNode *pTarget,void *data);
	void removeLoseNumInfo(CCNode *pTarget,void *data);
};

#endif //
