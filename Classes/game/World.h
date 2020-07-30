#ifndef __WORLD_H__
#define __WORLD_H__

#include "cocos2d.h"

//////////////////////////////////////////////////////////////////////////
typedef enum
{
	GAME_G_PLUS1 = 1,
	GAME_G_PLUS2 = 2,//for gravity
	GAME_G_PLUS4 = 3,//
}GameWorldGCharacterType;	
typedef enum
{
	GAME_A_PLUS1 = 1,
	GAME_A_PLUS2 = 2,//
	GAME_A_PLUS4 = 3,//
}GameWorldACharacterType;
typedef enum
{
	GAME_VX_PLUS1 = 1,
	GAME_VX_PLUS2 = 2,//this use for vx
	GAME_VX_PLUS4 = 3,
}GameWorldVXCharacterType;
typedef enum
{
	GAME_VY_PLUS1 = 1,
	GAME_VY_PLUS2 = 2,//this use for touch +v
	GAME_VY_PLUS4 = 3,
}GameWorldVYCharacterType;//this can use for hero to do right thing,otherwise dead

typedef enum
{
	GAME_COLLIDE_BALL_BOTH = 1,
	GAME_COLLIDE_BALL_UP = 2,
	GAME_COLLIDE_BALL_DOWN = 3,
}GameCollideBallDirectionType;

//////////////////////////////////////////////////////////////////////////
typedef enum
{
	GAME_NO_REQUIRE = 0,
	GAME_REQUIRE_2 = 1,//if world is this,only cat num 2 can alive
	GAME_REQUIRE_4 = 2,
	GAME_REQUIRE_8 = 3,
	GAME_REQUIRE_16 = 4,
	GAME_REQUIRE_32 = 5,
	GAME_REQUIRE_64 = 6,
	GAME_REQUIRE_128 = 7,
	GAME_REQUIRE_256 = 8,
}GameRequireNumType;//this can use for hero to do right thing,otherwise dead

typedef enum 
{
	GAME_WORLD_SILENT_NORMAL = 0,
	GAME_WORLD_SILENT_DISORDER = 1,
	GAME_WORLD_ATHLETIC_NORMAL = 2,
	GAME_WORLD_ATHLETIC_DISORDER = 3,
}GameWorldType;

//////////////////////////////////////////////////////////////////////////
typedef enum
{
	WORLD_LEVEL_1024 = 0,
	WORLD_LEVEL_2048 = 1,
	WORLD_LEVEL_4096 = 2,
	WORLD_LEVEL_8192 = 3,//all over
}WorldLevelType;


typedef enum
{
	WORLD_CHANGE_COLLIDE_TYPE =1,
	WORLD_CHANGE_VY,
	WORLD_CHANGE_G,
	WORLD_CHANGE_VX,
	WORLD_CHANGE_A,
	WORLD_CHANGE_WORLD,
}WorldChangeType;

class GameView;

class World : cocos2d::CCNode
{
public:
	World(GameView *gameView);
	virtual ~World(void);
public:
	CC_SYNTHESIZE(GameRequireNumType,_gameRequireNum,GameRequireNum);//
	CC_SYNTHESIZE(GameWorldType,_gameWorld,GameWorld);
private:
	GameWorldGCharacterType _gameWorldGCharacter;//
	GameWorldACharacterType _gameWorldACharacter;//
	GameWorldVXCharacterType _gameWorldVXCharacter;//
	GameWorldVYCharacterType _gameWorldVYCharacter;//
	float _gameGravity;//
	float _gameAcceleration;//
	float _gameVelocityX;//
	float _gameVelocityY;//

	GameCollideBallDirectionType _gameCollideBallDirection;

	WorldLevelType  _gameWorldLevel;
public:
	GameWorldGCharacterType getGameWorldGCharacter();
	GameWorldACharacterType getGameWorldACharacter();
	GameWorldVXCharacterType getGameWorldVXCharacter();
	GameWorldVYCharacterType getGameWorldVYCharacter();
	GameCollideBallDirectionType getGameCollideBallDirection();
	//basic change
	void setGameWorldGCharacter(GameWorldGCharacterType type,float dt);
	void setGameWorldACharacter(GameWorldACharacterType type,float dt);
	void setGameWorldVXCharacter(GameWorldVXCharacterType type,float dt);
	void setGameWorldVYCharacter(GameWorldVYCharacterType type,float dt);
	void setGameCollideBallDirection(GameCollideBallDirectionType type,float dt);

	float getGameGravity();//
	float getGameAcceleration();
	float getGameVelocityX();
	float getGameVelocityY();

	WorldLevelType getGameWorldLevelType();
	void setGameWorldLevelType(WorldLevelType type);
	void restG(float dt);
	void restA(float dt);
	void restVx(float dt);
	void restVy(float dt);
	void restCollideType(float dt);
	void unscheduleAll();
	void updateTimer(float dt);
	int _timerCount;
public:
	CC_SYNTHESIZE(GameView*,_gameView,GameView);//
	
	void showWorldChangeInfo(WorldChangeType type);
	void hideInfo();
};
#endif //__WORLD_H__

