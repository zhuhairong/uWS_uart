LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := uart_test

LOCAL_SRC_FILES := uart_access.cpp \
					uart_test.cpp
					
LOCAL_C_INCLUDES += /home/hopechart/work/android-ndk-r11c/platforms/android-21/arch-arm/usr/include

LOCAL_CPPFLAGS := -fexceptions -Wno-invalid-offsetof
LOCAL_LDFLAGS := -L$(LOCAL_PATH)/lib/ -llog 
LOCAL_LDFLAGS += -fPIC

LOCAL_CFLAGS +=

include $(BUILD_EXECUTABLE)
#########################################################
include $(CLEAR_VARS)

LOCAL_MODULE    := main_uws_uart

LOCAL_SRC_FILES := 	main_uws_uart.cpp \
					uart_access.cpp
					
LOCAL_C_INCLUDES += /home/hopechart/work/android-ndk-r11c/platforms/android-21/arch-arm/usr/include \
					$(LOCAL_PATH) \
					$(LOCAL_PATH)/../../uWebSockets-master/uWS/jni/src/ \
					$(LOCAL_PATH)/../../uWebSockets-master/uWS/OpenSSL1.0.1cForAndroid-master/include/

LOCAL_CPPFLAGS := 
LOCAL_LDFLAGS := $(LOCAL_PATH)/../../uWebSockets-master/uWS/obj/local/$(TARGET_ARCH_ABI)/libuWS.so\
				 -llog -fPIC -lz

LOCAL_CFLAGS += -std=c++11  -fPIC -fexceptions

include $(BUILD_EXECUTABLE)


#########################################################
include $(CLEAR_VARS)

LOCAL_MODULE    := main_uws

LOCAL_SRC_FILES := 	main_uws.cpp
					
LOCAL_C_INCLUDES += /home/hopechart/work/android-ndk-r11c/platforms/android-21/arch-arm/usr/include \
					$(LOCAL_PATH) \
					$(LOCAL_PATH)/../../uWebSockets-master/uWS/jni/src/ \
					$(LOCAL_PATH)/../../uWebSockets-master/uWS/OpenSSL1.0.1cForAndroid-master/include/

LOCAL_CPPFLAGS := 
LOCAL_LDFLAGS := $(LOCAL_PATH)/../../uWebSockets-master/uWS/obj/local/$(TARGET_ARCH_ABI)/libuWS.so\
				 -llog -fPIC -lz

LOCAL_CFLAGS += -std=c++11  -fPIC -fexceptions

include $(BUILD_EXECUTABLE)