# file:///path/to/android-ndk-r10d/docs/Start_Here.html : Android.mk
# file:///path/to/android-ndk-r10d/docs/Programmers_Guide/html/md_3__key__topics__building__chapter_1-section_8__android_8mk.html

LOCAL_PATH:= $(call my-dir)
#copied from dsp/lib/Android.mk to out/dsp/ffts/lib/Android.mk
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE:= libpicort-NDK-arm
LOCAL_SRC_FILES:= $(PICO_OUT)/lib/libpicort-NDK-arm.a
include $(PREBUILT_STATIC_LIBRARY)

include $(CLEAR_VARS)

#http://192.168.9.54/git/apps1/android-myheartbeat/tree/master/app/src/main/jni
LOCAL_MODULE := cvjni

#This is useful to pass the name of specific "system libraries" with the '-l' prefix
LOCAL_LDLIBS := \
	-llog \
	-lm \
	-lz
#	-landroid \

LOCAL_STATIC_LIBRARIES := libpicort-NDK-arm

#this is for 3rd party .so
#The list of shared libraries this module, cvjni, depends on at runtime.
#This is necessary at link time and to embed the corresponding information in the generated file.
LOCAL_SHARED_LIBRARIES := libbcv-dsp-NDK-arm

#LAPACKE to skip default complex.h whichis not found in NDK
LOCAL_CFLAGS := \
	-DHAVE_LAPACK_CONFIG_H  \
	-DLAPACK_COMPLEX_STRUCTURE

#An optional list of paths, relative to the NDK root directory,
#which will be appended to the include search path when compiling all sources (C, C++ and Assembly).
LOCAL_C_INCLUDES := $(LOCAL_PATH) \
		../../lib \
		../../ffts/include \
		../../LAPACK/CBLAS/include \
		../../LAPACK/LAPACKE/include \
		$(PICO_DIR)/rnt

LOCAL_SRC_FILES := \
	cv_call_entry.c \
	cv_c_jni.c \
	fileutil.c

include $(BUILD_SHARED_LIBRARY)

#import 3rd party .so from PREBUILT_SHARED_LIBRARY
#NDK_MODULE_PATH := /home2/thomas/bcv/out/dsp
$(call import-add-path, $(DSPLIB_OUT))
$(call import-module, lib)

