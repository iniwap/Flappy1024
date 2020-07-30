#include "Props.h"
#include "GameView.h"

using namespace std;


Props::Props(void)
{

}


Props::~Props(void)
{

}

Props* Props::initPropsWithType(GameView* gameView,PropsType type)
{
	Props *pRet = new Props();
	if (pRet && pRet->initProps(gameView,type))
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
bool Props::initProps(GameView* gameView,PropsType type)
{
	bool bRet = false;
	do
	{
        string propsImagePath = "PropsImage_";
        _theGameView = gameView;
        _propsType = type;
        
        string num;
        num	= CCString::createWithFormat("%d",  _propsType)->getCString();
        propsImagePath = propsImagePath+num+".png";
        
        
        _theProps = CCSprite::create(propsImagePath.c_str(),CCRectMake((type - 1)*90, 0, 90, 90));
        
        _theGameView->addChild(_theProps);
        
		bRet = true;
        
	} while (0);
    
	return bRet;
}