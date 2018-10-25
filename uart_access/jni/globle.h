#ifndef _GLOBLE_H__
#define _GLOBLE_H__

#define LOG_TAG "Uart"
#include <android/log.h>

#define ALOGD(...) __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#define ALOGI(...) __android_log_print(ANDROID_LOG_INFO,LOG_TAG,__VA_ARGS__)
#define ALOGW(...) __android_log_print(ANDROID_LOG_WARN,LOG_TAG,__VA_ARGS__)
#define ALOGE(...) __android_log_print(ANDROID_LOG_ERROR,LOG_TAG,__VA_ARGS__)
#define ALOGF(...) __android_log_print(ANDROID_LOG_FATAL,LOG_TAG,__VA_ARGS__)

//#define ZZZD 
#ifdef ZZZD
#define DPRINTF(fmt, argv...)\
{\
	ALOGD(fmt, ##argv);\
	printf("----Debug-----[%s %d]"fmt,__FUNCTION__, __LINE__, ##argv);}
#else
#define DPRINTF(fmt, argv...)
#endif

#define ZZZE
#ifdef ZZZE
#define EPRINTF(fmt, argv...)\
{\
	ALOGE(fmt, ##argv);\
	printf("----Error-----[%s %d]" fmt,__FUNCTION__, __LINE__, ##argv);}
#else
#define EPRINTF(fmt, argv...)
#endif

#define ZZZI
#ifdef ZZZI
#define IPRINTF(fmt, argv...)\
{\
	ALOGI(fmt, ##argv);\
	printf("----Info-----[%s %d]" fmt,__FUNCTION__, __LINE__, ##argv);}
#else
#define EPRINTF(fmt, argv...)
#endif


#define CMD(n) IPRINTF(""#n"")


#endif
