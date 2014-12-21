#include "stdafx.h"
#include "external/Configuration.h"
#import <UIKit/UIKit.h>
#import <sys/socket.h>
#import <netinet/in.h>
#import <netinet6/in6.h>
#import <arpa/inet.h>
#import <ifaddrs.h>
#include <netdb.h>
#import <SystemConfiguration/SCNetworkReachability.h>

void CConfiguration::GatherDeviceInfo()
{
	NSString * strModel = [UIDevice currentDevice].model;
	NSString * strSys = [UIDevice currentDevice].systemVersion;
    m_osInfo = [strSys UTF8String];
    m_modelInfo = [strModel UTF8String];
	
	m_eDeviceType = [[UIDevice currentDevice].model rangeOfString:@"iPad"].location != NSNotFound ? EDeviceType::eDT_Pad : EDeviceType::eDT_Phone;
}

ENetState CConfiguration::GetNetworkState() const
{
    ENetState ret = ENetState::eNS_UNAVAILABLE;
    struct sockaddr_in zeroAddress;
    bzero(&zeroAddress, sizeof(zeroAddress));
    zeroAddress.sin_len = sizeof(zeroAddress);
    zeroAddress.sin_family = AF_INET;
    SCNetworkReachabilityRef defaultRouteReachability = SCNetworkReachabilityCreateWithAddress(NULL, (struct sockaddr *)&zeroAddress);
    SCNetworkReachabilityFlags flags;
    SCNetworkReachabilityGetFlags(defaultRouteReachability, &flags);
    CFRelease(defaultRouteReachability);
	if ((flags & kSCNetworkReachabilityFlagsReachable) != 0)  
    {
		if ((flags & kSCNetworkReachabilityFlagsConnectionRequired) == 0)
		{
			ret = eNS_WIFI;
		}
		if ((((flags & kSCNetworkReachabilityFlagsConnectionOnDemand ) != 0) ||    
			 (flags & kSCNetworkReachabilityFlagsConnectionOnTraffic) != 0))
		{
			if ((flags & kSCNetworkReachabilityFlagsInterventionRequired) == 0)  
			{
				ret = eNS_WIFI;    
			}
		}
		if ((flags & kSCNetworkReachabilityFlagsIsWWAN) == kSCNetworkReachabilityFlagsIsWWAN)
		{
			if((flags & kSCNetworkReachabilityFlagsReachable) == kSCNetworkReachabilityFlagsReachable) {
				if ((flags & kSCNetworkReachabilityFlagsTransientConnection) == kSCNetworkReachabilityFlagsTransientConnection) {
					ret = eNS_MOBILE;//2G
					if((flags & kSCNetworkReachabilityFlagsConnectionRequired) == kSCNetworkReachabilityFlagsConnectionRequired) {
						ret = eNS_MOBILE;//3G
					}
				}
			}
		}
    }
    return ret;
}
