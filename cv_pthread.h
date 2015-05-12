/** @file cv_pthread.h
 *  @brief Function prototypes for the cv pthread.
 *
 *  This contains the prototypes for the cv pthread.
 *
 *  @author Jerry Chen
 *  @bug No known bugs.
 */

#ifndef _CV_PTHREAD_H
#define _CV_PTHREAD_H

#include <pthread.h>

pthread_t thUpdateHeartRate ;
void *thread_UpdateHeartRate(void *arg) ;
char message_UpdateHeartRate[]="Update Heart Rate thread is running";

pthread_t thUpdateHeartRateWave ;
void *thread_UpdateHeartRateWave(void *arg) ;
char message_UpdateHeartRateWave[]="Update Heart Rate wave thread is running";

pthread_t thUpdateBreathing ;
void *thread_UpdateBreathing(void *arg) ;
char message_UpdateBreathing[]="Update Breathing thread is running";

pthread_t thFaceDetected ;
void *thread_FaceDetected(void *arg) ;
char message_FaceDetected[]="Face Detect thread is running";

char message_ThreadExit[]="Thread Exit";

static pthread_mutex_t cbMutex = PTHREAD_MUTEX_INITIALIZER;

#endif

