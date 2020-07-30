#ifndef __GAME_VIEW_H__
#define __GAME_VIEW_H__

#include "cocos2d.h"
#include "Hero.h"
#include "World.h"

typedef enum
{
	BG_ZORDER = 0,
	ENEMY_ZORDER = 101,
	HERO_ZORDER = 201,
}LayerZoder;

class Hero;
class Props;
class World;


class GameView : public cocos2d::CCLayer
{
public:
	GameView();
	~GameView();
public:
	virtual bool init();  
	static cocos2d::CCScene* scene();
	void menuClickCallback(CCObject* pSender);
	CREATE_FUNC(GameView);

	virtual void ccTouchesBegan(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesMoved(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);
	virtual void ccTouchesEnded(cocos2d::CCSet *pTouches, cocos2d::CCEvent *pEvent);

	//
private:
	void initEnemys();
    void initProps();
	void backToMainView(float dt);


private:
	int _gameScore;
	int _gameNum;
	CCLabelBMFont* _gameNumLabel;

    CC_SYNTHESIZE(bool, _isGameStart, IsGameStart);
public:
	CC_SYNTHESIZE(Hero*, _theHero,TheHero);
	CC_SYNTHESIZE(CCArray *, _enemies,TheEnemies);

	void setGameScore(int score);
	int getGameScore();
	void setGameNum(int num);
	int getGameNum();
	void setFontSize(int charLen);
	void setGameNumLabel();

public:
	bool gameOver();
	void showGameOverOrWinView(bool isOver);
	void initBgTileMap();
	void initScoreBoard();
	void initTapGoInfo();
	void initGameWinAnimation();
	void initGameWorld();
	void update(float dt);
    void start();
    void updateProps(float dt);

	void stopGame();
	void resumeGame();

private://game over view//game win view
	CCSprite*      _gameOverView;
	CCLabelBMFont*  _scoreLabel;
	CCLabelBMFont*  _bestScoreLabel;
	CCMenu*        _rePlayAndShareMenu;
    
    bool _isPlayingCollideWithScoreBoard;
public:
	void gameWin();
	void showGameScoreBord(bool isOver);
	void hideWinAnimation(float dt);

private:
	//far
	CCSprite *m_pBgSprite;
	CCSprite *m_pBgSpriteRe;
	float m_fBgVelocity;
	float m_fBgWidth;
	//near
	CCTMXTiledMap *m_pTileMap;
	CCTMXTiledMap *m_pTileMapRe;
	float m_fTileMapVelocity;
	float m_fTileMapWidth;
    
    //////Props///////
public:
    CC_SYNTHESIZE(CCArray*, _props, Props);//one screen one props
    void addProps(float dt);
    bool checkIfCollosionPropsAndHero(CCSprite *props);

	void refreshEnemyNum();

public:
	 CC_SYNTHESIZE(World*, _gameWorld, GameWorld);
	 void updateWorld();

	 void changeWorld(int index);
	 void changeGameWorldVx(GameWorldVXCharacterType type);
	 void changeGameWorldVy(GameWorldVYCharacterType type);
	 void changeGameWorldGravity(GameWorldGCharacterType type);
	 void changeGameWorldCollideType(GameCollideBallDirectionType type);

	 bool getIfChange(int level);
	 int  getBasicChange();
	 float getBasicChangeTime();
	 void shuffleNum();
private:
	std::vector<int>_changeAtIndex;
	bool _isChangingWorld;
	void delayChangeWorld(float dt);
	int _delayChangeIndex;
public:
	void setIsChangingWorld(bool isChanging);
	bool getIsChangingWorld();
};

#endif // __GAME_VIEW_H__
