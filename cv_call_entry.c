/** @file cv_call_entry.c
 *  @brief cv call entry
 *
 *  This contains the function for the cv call entry.
 *
 *  @author Jerry Chen
 *  @bug No known bugs.
 */

#include <string.h>
#include <jni.h>
#include <android/log.h>
#include <sys/types.h>
#include "cv_common.h"
#include "cv_pthread.h"
#include "cv_c_jni.h"
#include "cv_call_entry.h"

#define LOG_TAG "CVJNI"

// common variable
static boolean bIsReady = FALSE ;
static boolean bExist = FALSE ;

boolean Init()
{
    LOGI("Init") ;
    // Init Update HeartRate Thread
    if (pthread_create(&thUpdateHeartRate, NULL, thread_UpdateHeartRate, (void *)message_UpdateHeartRate)) {
        LOGI("pthread create failed.");
        return FALSE ;
    }

    // Init Update HeartRate Wave Thread
    if (pthread_create(&thUpdateHeartRateWave, NULL, thread_UpdateHeartRateWave, (void *)message_UpdateHeartRateWave)) {
        LOGI("pthread create failed.");
        return FALSE ;
    }

    // Init Update Breathing Thread
    if (pthread_create(&thUpdateBreathing, NULL, thread_UpdateBreathing, (void *)message_UpdateBreathing)) {
        LOGI("pthread create failed.");
        return FALSE ;
    }

    // Init Face Detect Thread
    if (pthread_create(&thFaceDetected, NULL, thread_FaceDetected, (void *)message_FaceDetected)) {
        LOGI("pthread create failed.");
        return FALSE ;
    }

    bIsReady = TRUE ;
    bExist = FALSE ;
    return bIsReady ;
}

void Close()
{
    LOGI("Close") ;
    bIsReady = FALSE ;
    bExist = TRUE ;
}

boolean IsReady()
{
    LOGI("IsReady") ;
    return bIsReady ;
}

void SetImageResolution(int width, int height)
{
    if( !bIsReady )
        return ;
    LOGI("SetImageResolution: get image resolution width:%d, height:%d", width, height);
}

void SetCameraInfo(boolean bTBD)
{
    if( !bIsReady )
        return ;
}

void SendImage(char *pSource, int length)
{
    if( !bIsReady )
        return ;
    LOGI("SendImage: get image with length: %d", length);
}

boolean cbUpdateHeartRate(int value, int max)
{
    boolean bResult = FALSE ;
    pthread_mutex_lock(&cbMutex) ;
    LOGI("UpdateHeartRate: value: %d, max: %d", value, max);
    bResult = Native_com_biotrump_myheartbeat_cv_NativeJNI_UpdateHeartRate(value, max) ;
    pthread_mutex_unlock(&cbMutex);
    return bResult ;
}

boolean cbUpdateHeartRateWave(int value, int max)
{
    boolean bResult = FALSE ;
    pthread_mutex_lock(&cbMutex) ;
    LOGI("UpdateHeartRateWave: value: %d, max: %d", value, max);
    bResult = Native_com_biotrump_myheartbeat_cv_NativeJNI_UpdateHeartRateWave(value, max) ;
    pthread_mutex_unlock(&cbMutex);
    return bResult ;
}

boolean cbUpdateBreathing(int value, int max)
{
    boolean bResult = FALSE ;
    pthread_mutex_lock(&cbMutex) ;
    LOGI("UpdateBreathing: value: %d, max: %d", value, max);
    bResult = Native_com_biotrump_myheartbeat_cv_NativeJNI_UpdateBreathing(value,max) ;
    pthread_mutex_unlock(&cbMutex);
    return bResult ;
}

boolean cbFaceDetected(boolean bDetect)
{
    boolean bResult = FALSE ;
    pthread_mutex_lock(&cbMutex) ;
    LOGI("FaceDetected: %d", bDetect);
    bResult = Native_com_biotrump_myheartbeat_cv_NativeJNI_FaceDetected(bDetect) ;
    pthread_mutex_unlock(&cbMutex);
    return bResult ;
}

boolean cbSetCameraInfo(int bTBD)
{
    boolean bResult = FALSE ;
    pthread_mutex_lock(&cbMutex) ;
    LOGI("SetCameraInfo: %d", bTBD);
    bResult = Native_com_biotrump_myheartbeat_cv_NativeJNI_SetCameraInfo(bTBD) ;
    pthread_mutex_unlock(&cbMutex);
    return bResult ;
}

void *thread_UpdateHeartRate(void *arg)
{
    int nSleep = 5 ;
    int nExist = 0 ;
    int nValue = 0 ;
    int nMax = 200 ;

    while( nExist == 0 ) {
        sleep(5) ;
        if( bExist == TRUE ) {
            nExist = 1 ;
        } else {
            // Update HeartRate to Java
            if( nValue < nMax )
                nValue++ ;
            else
                nValue = 0 ;
            cbUpdateHeartRate( nValue, nMax) ;
        }
    }

    pthread_exit(message_ThreadExit);
    return ;
}

void *thread_UpdateHeartRateWave(void *arg)
{
    int nSleep = 5 ;
    int nExist = 0 ;
    int nValue = 0 ;
    int nMax = 200 ;

    while( nExist == 0 ) {
        sleep(5) ;
        if( bExist == TRUE ) {
            nExist = 1 ;
        } else {
            // Update HeartRate to Java
            if( nValue < nMax )
                nValue++ ;
            else
                nValue = 0 ;
            cbUpdateHeartRateWave( nValue, nMax) ;
        }
    }

    pthread_exit(message_ThreadExit);
    return ;
}

void *thread_UpdateBreathing(void *arg)
{
    int nSleep = 5 ;
    int nExist = 0 ;
    int nValue = 0 ;
    int nMax = 60 ;

    while( nExist == 0 ) {
        sleep(5) ;
        if( bExist == TRUE ) {
            nExist = 1 ;
        } else {
            // Update HeartRate to Java
            if( nValue < nMax )
                nValue++ ;
            else
                nValue = 0 ;
            cbUpdateBreathing( nValue, nMax) ;
        }
    }

    pthread_exit(message_ThreadExit);
    return ;
}

void *thread_FaceDetected(void *arg)
{
    int nSleep = 10 ;
    int nExist = 0 ;
    boolean bDetected = 0 ;

    while( nExist == 0 ) {
        sleep(5) ;
        if( bExist == TRUE ) {
            nExist = 1 ;
        } else {
            // Update Face Detect info to Java
            if( bDetected == TRUE )
                bDetected = FALSE ;
            else
                bDetected = TRUE ;
            cbFaceDetected( bDetected) ;
        }
    }

    pthread_exit(message_ThreadExit);
    return ;
}

