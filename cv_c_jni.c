/**
 * @file cv_c_jni.c
 * cv java native interface in c language
 *
 * @brief cv java native interface in c language
 *
 * @author Jerry Chen.
 * @version 0.1
 */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <jni.h>
#include <android/log.h>
#include "cv_common.h"
#include "cv_call_entry.h"
#include "fileutil.h"

// Local function definition start
typedef struct {
    // Cached JNI references.
    JavaVM* mJavaVM;
    jobject mNativeJNI ;
    jobject mNativeEvent ;
    // Classes.
    jclass ClassNativeJNI ;
    jclass ClassNativeEvent ;

    // NativeEvent Methods.
    jmethodID MethodConstructor ;
    jmethodID MethodUpdateHeartRate ;
    jmethodID MethodUpdateHeartRateWave ;
    jmethodID MethodUpdateBreathing ;
    jmethodID MethodFaceDetected ;
    jmethodID MethodSetCameraInfo ;

    int mInit;

} cv_jni_ctl ;

cv_jni_ctl* GetJNICtl() ;

int jni_init(JNIEnv* pEnv, cv_jni_ctl* pJNICtl) ;

void jni_stop(JNIEnv* pEnv, cv_jni_ctl* pJNICtl) ;

JNIEnv* AttachJNIEnv(JavaVM* pJavaVM) ;

void DetachJNIEnv(JavaVM* pJavaVM) ;

void makeGlobalRef(JNIEnv* pEnv, jobject* pRef) ;

void deleteGlobalRef(JNIEnv* pEnv, jobject* pRef) ;

void SaveTimeStamp();

void GetFilename(char *szFilename);

jboolean Java_com_biotrump_myheartbeat_cv_NativeJNI_ctInit( JNIEnv* env, jobject thiz ) ;

jboolean Java_com_biotrump_myheartbeat_cv_NativeJNI_ctIsReady( JNIEnv* env, jobject thiz ) ;

jboolean Java_com_biotrump_myheartbeat_cv_NativeJNI_ctClose( JNIEnv* env, jobject thiz ) ;

jboolean Java_com_biotrump_myheartbeat_cv_NativeJNI_ctSetImageResolution( JNIEnv* env, jobject thiz, jint width, jint height );

jboolean Java_com_biotrump_myheartbeat_cv_NativeJNI_ctSetCameraInfo( JNIEnv* env, jobject thiz ) ;

jboolean Java_com_biotrump_myheartbeat_cv_NativeJNI_ctSendImage( JNIEnv* env, jobject thiz, jbyteArray pJavaSource) ;

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_UpdateHeartRate(int value, int max) ;

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_UpdateHeartRateWave(int value, int max) ;

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_UpdateBreathing(int value, int max) ;

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_FaceDetected(boolean bDetect) ;

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_SetCameraInfo(int bTBD) ;

static cv_jni_ctl mJNICtl ;
static boolean mInit = FALSE ;

// Local function start

cv_jni_ctl* GetJNICtl()
{
    return &mJNICtl ;
}

int jni_start(JNIEnv* pEnv, cv_jni_ctl* pJNICtl, jobject thiz)
{
    int result ;

    LOGI("jni_start");

    result = 0 ;
    memset( pJNICtl, 0, sizeof(cv_jni_ctl));

    // Caches the VM.
    if ((*pEnv)->GetJavaVM(pEnv, &pJNICtl->mJavaVM) != JNI_OK)
        return result ;

    // Caches NativeJNI classes.
    pJNICtl->ClassNativeJNI = (*pEnv)->FindClass(pEnv, "com/biotrump/myheartbeat/cv/NativeJNI");
    makeGlobalRef(pEnv, &pJNICtl->ClassNativeJNI);
    if( pJNICtl->ClassNativeJNI == NULL ) {
        LOGI("com/biotrump/myheartbeat/cv/NativeJNI not found") ;
        return result ;
    }

    // Caches NativeEvent classes.
    pJNICtl->ClassNativeEvent = (*pEnv)->FindClass(pEnv, "com/biotrump/myheartbeat/cv/NativeEvent");
    makeGlobalRef(pEnv, &pJNICtl->ClassNativeEvent);
    if( pJNICtl->ClassNativeEvent == NULL ) {
        LOGI("NativeEvent not found") ;
        return result ;
    }

    // Caches NativeEvent constructor
    pJNICtl->MethodConstructor = (*pEnv)->GetMethodID(pEnv, pJNICtl->ClassNativeEvent, "<init>", "()V");
    if (pJNICtl->MethodConstructor == NULL) {
        LOGI("NativeEvent constructor not found") ;
        return result ;
    }

    // Caches UpdateHeartRate methods.
    pJNICtl->MethodUpdateHeartRate = (*pEnv)->GetMethodID(pEnv, pJNICtl->ClassNativeEvent, "UpdateHeartRate", "(II)Z");
    if (pJNICtl->MethodUpdateHeartRate == NULL) {
        LOGI("UpdateHeartRate not found") ;
        return result ;
    }

    // Caches UpdateHeartRateWave methods.
    pJNICtl->MethodUpdateHeartRateWave = (*pEnv)->GetMethodID(pEnv, pJNICtl->ClassNativeEvent, "UpdateHeartRateWave", "(II)Z");
    if (pJNICtl->MethodUpdateHeartRateWave == NULL) {
        LOGI("UpdateHeartRateWave not found") ;
        return result ;
    }

    // Caches UpdateBreathing methods.
    pJNICtl->MethodUpdateBreathing = (*pEnv)->GetMethodID(pEnv, pJNICtl->ClassNativeEvent, "UpdateBreathing", "(II)Z");
    if (pJNICtl->MethodUpdateBreathing == NULL) {
        LOGI("UpdateBreathing not found") ;
        return result ;
    }

    // Caches FaceDetected methods.
    pJNICtl->MethodFaceDetected = (*pEnv)->GetMethodID(pEnv, pJNICtl->ClassNativeEvent, "FaceDetected", "(Z)Z");
    if (pJNICtl->MethodFaceDetected == NULL) {
        LOGI("FaceDetected not found") ;
        return result ;
    }

    // Caches SetCameraInfo methods.
    pJNICtl->MethodSetCameraInfo = (*pEnv)->GetMethodID(pEnv, pJNICtl->ClassNativeEvent, "SetCameraInfo", "(I)Z");
    if (pJNICtl->MethodSetCameraInfo == NULL) {
        LOGI("SetCameraInfo not found") ;
        return result ;
    }

    // Cache NativeJNI object
    pJNICtl->mNativeJNI = (*pEnv)->NewGlobalRef(pEnv, thiz) ;
    if (pJNICtl->mNativeJNI == NULL) {
        LOGI("mNativeJNI new global reference failed") ;
        return result ;
    }

    pJNICtl->mNativeEvent = (*pEnv)->NewObject(pEnv, pJNICtl->ClassNativeEvent, pJNICtl->MethodConstructor);
    makeGlobalRef(pEnv, &pJNICtl->mNativeEvent);
    if (pJNICtl->mNativeEvent == NULL) {
        LOGI("mNativeEvent new global reference failed") ;
        return result ;
    }

    SaveTimeStamp();
    pJNICtl->mInit = 1 ;
    result = 1 ;

    return result ;
}

void jni_stop(JNIEnv* pEnv, cv_jni_ctl* pJNICtl)
{
    // Close CV Native Library process
    Close() ;
    // Deletes global references to avoid memory leaks.
    deleteGlobalRef(pEnv, &pJNICtl->mNativeJNI);
    deleteGlobalRef(pEnv, &pJNICtl->mNativeEvent);
    deleteGlobalRef(pEnv, &pJNICtl->ClassNativeJNI);
    deleteGlobalRef(pEnv, &pJNICtl->ClassNativeEvent);
}

JNIEnv* AttachJNIEnv(JavaVM* pJavaVM)
{
    JNIEnv* lEnv;
    if ((*pJavaVM)->AttachCurrentThread(pJavaVM, &lEnv, NULL) != JNI_OK) {
        lEnv = NULL;
    }
    return lEnv;
}

void DetachJNIEnv(JavaVM* pJavaVM)
{
    (*pJavaVM)->DetachCurrentThread(pJavaVM);
}

void makeGlobalRef(JNIEnv* pEnv, jobject* pRef)
{
    if (*pRef != NULL) {
        jobject lGlobalRef = (*pEnv)->NewGlobalRef(pEnv, *pRef);
        // No need for a local reference any more.
        (*pEnv)->DeleteLocalRef(pEnv, *pRef);
        // Here, lGlobalRef may be null.
        *pRef = lGlobalRef;
    }
}
static time_t InitTime ;
static int    InitCount ;

void SaveTimeStamp()
{
    time(&InitTime);
    InitCount = 0 ;
}

void GetFilename(char *szFilename)
{
    InitCount++;
    sprintf(szFilename, "/mnt/sdcard/DCIM/100MEDIA/%ld-%d.yuv", InitTime, InitCount);
    LOGI("%s", szFilename) ;
}

void deleteGlobalRef(JNIEnv* pEnv, jobject* pRef)
{
    if (*pRef != NULL) {
        (*pEnv)->DeleteGlobalRef(pEnv, *pRef);
        *pRef = NULL;
    }
}

jboolean Java_com_biotrump_myheartbeat_cv_NativeJNI_ctInit( JNIEnv* env, jobject thiz )
{
    LOGI("NativeJNI: Init");
    if( mInit == TRUE )
        return TRUE ;
    if( jni_start( env, &mJNICtl, thiz) != 1 )
        return 0 ;
    if( Init() )
        mInit = TRUE ;
    else
        mInit = FALSE ;
    return mInit ;
}

jboolean Java_com_biotrump_myheartbeat_cv_NativeJNI_ctClose( JNIEnv* env, jobject thiz )
{
    LOGI("NativeJNI: Close");
    if( mInit == FALSE )
        return FALSE ;
    Close() ;
    mInit = FALSE ;
    return TRUE ;
}

jboolean Java_com_biotrump_myheartbeat_cv_NativeJNI_ctIsReady( JNIEnv* env, jobject thiz )
{
    LOGI("NativeJNI: IsReady");
    if( mInit == FALSE )
        return FALSE ;
    return IsReady() ;
}

jboolean Java_com_biotrump_myheartbeat_cv_NativeJNI_ctSetDebugMode( JNIEnv* env, jobject thiz, jboolean bON)
{
    LOGI("NativeJNI: SetDebugMode");
    if( mInit == FALSE )
        return FALSE ;
    LOGI("NativeJNI: SetDebugMode: %s", bON?"ON":"OFF");
    return TRUE ;
}

jboolean Java_com_biotrump_myheartbeat_cv_NativeJNI_ctSetImageResolution( JNIEnv* env, jobject thiz, jint width, jint height )
{
    LOGI("NativeJNI: SetImageResolution");
    if( mInit == FALSE )
        return FALSE ;
    SetImageResolution( width, height);
    return TRUE ;
}

jboolean Java_com_biotrump_myheartbeat_cv_NativeJNI_ctSetCameraInfo( JNIEnv* env, jobject thiz )
{
    LOGI("NativeJNI: SetCameraInfo");
    SetCameraInfo(TRUE);
    return 0 ;
}

jboolean Java_com_biotrump_myheartbeat_cv_NativeJNI_ctSendImage( JNIEnv* pEnv, jobject thiz,  jbyteArray pJavaSource)
{
    char szFilename[128] ;
    jsize nlength ;
    LOGI("NativeJNI: SendImage");
    // Accesses source array data.
    jbyte* pNativeSource = (*pEnv)->GetPrimitiveArrayCritical(pEnv, pJavaSource, 0);
    if (pNativeSource == NULL) {
        return 0;
    }
    nlength = (*pEnv)->GetArrayLength(pEnv, pJavaSource);
    LOGI("NativeJNI: Image length:%d", nlength);
    GetFilename(szFilename);

    // xwrite( szFilename, 0, pNativeSource, nlength) ;

    SendImage( pNativeSource, nlength);

    // Unlocks the bitmap and releases the Java array when finished.
    (*pEnv)-> ReleasePrimitiveArrayCritical(pEnv,pJavaSource,pNativeSource, 0);
    return 0 ;
}

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_UpdateHeartRate(int value, int max)
{
    if( mInit == FALSE )
        return FALSE ;
    boolean bResult ;
    cv_jni_ctl* pJNICtl = GetJNICtl();
    JNIEnv* pEnv = AttachJNIEnv(pJNICtl->mJavaVM);
    if (pEnv == NULL) {
        LOGI("JNI attach failed");
        return FALSE ;
    }

    bResult = FALSE ;

    jboolean lResult = (*pEnv)->CallBooleanMethod(pEnv, pJNICtl->mNativeEvent, pJNICtl->MethodUpdateHeartRate, value, max);

    LOGI("UpdateHeartRate lResult: %d", lResult);

    if( lResult )
        bResult = TRUE ;
    else
        bResult = FALSE ;
    LOGI("DetachJNIEnv");
    DetachJNIEnv(pJNICtl->mJavaVM);
    return bResult ;
}

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_UpdateHeartRateWave(int value, int max)
{
    if( mInit == FALSE )
        return FALSE ;
    boolean bResult ;
    cv_jni_ctl* pJNICtl = GetJNICtl();
    JNIEnv* pEnv = AttachJNIEnv(pJNICtl->mJavaVM);
    if (pEnv == NULL) {
        LOGI("JNI attach failed");
        return FALSE ;
    }

    bResult = FALSE ;

    jboolean lResult = (*pEnv)->CallBooleanMethod(pEnv, pJNICtl->mNativeEvent, pJNICtl->MethodUpdateHeartRateWave, value, max);

    LOGI("UpdateHeartRateWave lResult: %d", lResult);

    if( lResult )
        bResult = TRUE ;
    else
        bResult = FALSE ;
    LOGI("DetachJNIEnv");
    DetachJNIEnv(pJNICtl->mJavaVM);
    return bResult ;
}

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_UpdateBreathing(int value, int max)
{
    if( mInit == FALSE )
        return FALSE ;
    boolean bResult ;
    cv_jni_ctl* pJNICtl = GetJNICtl();
    JNIEnv* pEnv = AttachJNIEnv(pJNICtl->mJavaVM);
    if (pEnv == NULL) {
        LOGI("JNI attach failed");
        return FALSE ;
    }

    bResult = FALSE ;

    jboolean lResult = (*pEnv)->CallBooleanMethod(pEnv, pJNICtl->mNativeEvent, pJNICtl->MethodUpdateBreathing, value, max);

    LOGI("UpdateBreathing lResult: %d", lResult);

    if( lResult )
        bResult = TRUE ;
    else
        bResult = FALSE ;
    LOGI("DetachJNIEnv");
    DetachJNIEnv(pJNICtl->mJavaVM);
    return bResult ;
}

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_FaceDetected(boolean bDetect)
{
    if( mInit == FALSE )
        return FALSE ;
    boolean bResult ;
    cv_jni_ctl* pJNICtl = GetJNICtl();
    JNIEnv* pEnv = AttachJNIEnv(pJNICtl->mJavaVM);
    if (pEnv == NULL) {
        LOGI("JNI attach failed");
        return FALSE ;
    }

    bResult = FALSE ;

    jboolean lResult = (*pEnv)->CallBooleanMethod(pEnv, pJNICtl->mNativeEvent, pJNICtl->MethodFaceDetected, bDetect);

    LOGI("FaceDetected lResult: %d", lResult);

    if( lResult )
        bResult = TRUE ;
    else
        bResult = FALSE ;
    LOGI("DetachJNIEnv");
    DetachJNIEnv(pJNICtl->mJavaVM);
    return bResult ;
}

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_SetCameraInfo(int bTBD)
{
    if( mInit == FALSE )
        return FALSE ;
    boolean bResult ;
    cv_jni_ctl* pJNICtl = GetJNICtl();
    JNIEnv* pEnv = AttachJNIEnv(pJNICtl->mJavaVM);
    if (pEnv == NULL) {
        LOGI("JNI attach failed");
        return FALSE ;
    }

    bResult = FALSE ;

    jboolean lResult = (*pEnv)->CallBooleanMethod(pEnv, pJNICtl->mNativeEvent, pJNICtl->MethodSetCameraInfo, bTBD);

    LOGI("SetCameraInfo lResult: %d", lResult);

    if( lResult )
        bResult = TRUE ;
    else
        bResult = FALSE ;
    LOGI("DetachJNIEnv");
    DetachJNIEnv(pJNICtl->mJavaVM);
    return bResult ;
}
