LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := libuWS

LOCAL_SRC_FILES := src/Extensions.cpp \
					src/Group.cpp \
					src/Networking.cpp \
					src/Hub.cpp \
					src/Node.cpp \
					src/WebSocket.cpp \
					src/HTTPSocket.cpp \
					src/Socket.cpp \
					src/Epoll.cpp \
					src/Room.cpp
					
LOCAL_C_INCLUDES += /home/hopechart/work/android-ndk-r11c/platforms/android-21/arch-arm/usr/include \
					$(LOCAL_PATH)/src/ \
					$(LOCAL_PATH)/../OpenSSL1.0.1cForAndroid-master/include

LOCAL_CPPFLAGS := 
LOCAL_LDFLAGS := $(LOCAL_PATH)/../OpenSSL1.0.1cForAndroid-master/libs/$(TARGET_ARCH_ABI)/libssl_zhr.so \
				$(LOCAL_PATH)/../OpenSSL1.0.1cForAndroid-master/libs/$(TARGET_ARCH_ABI)/libcrypto_zhr.so \
				-llog  -lz 
				
LOCAL_CFLAGS += -std=c++11  -fPIC -fexceptions

include $(BUILD_SHARED_LIBRARY)
#########################################################
include $(CLEAR_VARS)

LOCAL_MODULE    := uws_test

LOCAL_SRC_FILES := multithreaded_echo.cpp 
					
LOCAL_C_INCLUDES += /home/hopechart/work/android-ndk-r11c/platforms/android-21/arch-arm/usr/include \
					$(LOCAL_PATH) \
					$(LOCAL_PATH)/src/ \
					$(LOCAL_PATH)/../OpenSSL1.0.1cForAndroid-master/include

LOCAL_CPPFLAGS := 
LOCAL_LDFLAGS := $(LOCAL_PATH)/../obj/local/$(TARGET_ARCH_ABI)/libuWS.so  -llog -fPIC -lz 
				
LOCAL_CFLAGS += -std=c++11  -fPIC -fexceptions

include $(BUILD_EXECUTABLE)


#########################################################
include $(CLEAR_VARS)

LOCAL_MODULE    := main_ws

LOCAL_SRC_FILES := 	main_ws.cpp
					
LOCAL_C_INCLUDES += /home/hopechart/work/android-ndk-r11c/platforms/android-21/arch-arm/usr/include \
					$(LOCAL_PATH) \
					$(LOCAL_PATH)/../../websocket-master/jni/include \
					$(LOCAL_PATH)/../../websocket-master/jni

LOCAL_CPPFLAGS := 
LOCAL_LDFLAGS := $(LOCAL_PATH)/../../libevent-master/libs/$(TARGET_ARCH_ABI)/libevent.so \
			-L$(LOCAL_PATH)/../../websocket-master/libs/armeabi-v7a/  -llog -lwebsocket -fPIC


#include $(BUILD_EXECUTABLE)