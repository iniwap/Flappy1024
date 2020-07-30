/**
*
*继承自单例类，用于保存全局对象的访问
*
*/

#ifndef __GLOBAL_SINGLETON_H__
#define __GLOBAL_SINGLETON_H__

#include "SingletonTemplate.h"


class GlobalSingleton : public SingletonTemplate<GlobalSingleton>
{
public:
	GlobalSingleton(void);
	~GlobalSingleton(void);

};

#define s_GlobalSingleton GlobalSingleton::instance()

#endif  // __GLOBAL_SINGLETON_H__