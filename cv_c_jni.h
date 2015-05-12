/** @file cv_c_jni.h
 *  @brief Function prototypes for the cv c jni.
 *
 *  This contains the prototypes for the cv c jni.
 *
 *  @author Jerry Chen
 *  @bug No known bugs.
 */

#ifndef _CV_C_JNI_H
#define _CV_C_JNI_H

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_cbUpdateHeartRate(int value, int max) ;

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_cbUpdateHeartRateWave(int value, int max) ;

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_cbUpdateBreathing(int value, int max) ;

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_cbFaceDetected(boolean bDetect) ;

boolean Native_com_biotrump_myheartbeat_cv_NativeJNI_cbSetCameraInfo(int bTBD) ;

#endif

