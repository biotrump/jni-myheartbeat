
/** @file cv_common.h
 *  @brief Function prototypes for the cv common define and variable.
 *
 *  This contains the prototypes for the cv pthread.
 *
 *  @author Jerry Chen
 *  @bug No known bugs.
 */

#ifndef _CV_CALL_ENTRY_H
#define _CV_CALL_ENTRY_H

#include <android/log.h>

#define LOG_TAG "CVJNI"
#define LOGI(...) ((void)__android_log_print( ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print( ANDROID_LOG_WARN, LOG_TAG, __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__))

typedef enum {
    TRUE = (1 == 1),
    FALSE = (1 == 0)
} boolean ;

#endif
