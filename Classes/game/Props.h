#ifndef __PROPS_H__
#define __PROPS_H__
#include "cocos2d.h"


using namespace cocos2d;


typedef enum
{
    PROPS_PROPS = 0,
    RANDOM_PROPS = 1,
    SPEEDX_PLUS = 2,
    SPEEDX_REDUCE = 3,
    SPEEDY_PLUS = 4,
    SPEEDY_REDUCE = 5,
    GRAVITY_PLUS = 6,
    GRAVITY_REDUCE = 7,
    TIME_BACK = 8,//not need time forword
    
}PropsType;

class GameView;

class Props : public CCNode
{
public:
	Props(void);
	virtual ~Props(void);

public:
    static Props* initPropsWithType(GameView* gameView,PropsType type);
	bool initProps(GameView* gameView,PropsType type);
public:
    CC_SYNTHESIZE(GameView*, _theGameView, TheGameView);
    
	CC_SYNTHESIZE(CCSprite*, _theProps, TheProps);
    CC_SYNTHESIZE(PropsType, _propsType, PropsType);
};
#endif

