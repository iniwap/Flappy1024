#include "PlatformInterface.h"

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)

#include "PlatformInterfaceIOS.h"

#else 
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
//#include ""
#endif

#endif


void PlatformInterface::showAdView()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    PlatformInterfaceIOS::showAdView();
#else
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    //#include ""
#endif
    
#endif
}
void PlatformInterface::hideADView()
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    PlatformInterfaceIOS::hideADView();
#else
#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    //#include ""
#endif
    
#endif
}
void PlatformInterface::share(int sns)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    PlatformInterfaceIOS::share(sns);
#else

#if(CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    //
#endif
    
#endif
}