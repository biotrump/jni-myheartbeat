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
#include "dsp-lib.h"
#include "picornt.h"

#define	MAX_FACES	(2)	//up to 4 faces are detected!

#define LOG_TAG "CVJNI"

// common variable
static boolean bIsReady = FALSE ;
static boolean bExist = FALSE ;
static int 	win_width = DEFAULT_WIDTH;
static int 	win_height = DEFAULT_HEIGHT;

/*
 * callback functions
 */
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
    LOGI("++%s: %d",__func__ ,bDetect);
	boolean bResult = FALSE ;
	return bResult;
    pthread_mutex_lock(&cbMutex) ;
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

AND_CBLIST cblist={
	(PFNcbUpdateHeartRate)&cbUpdateHeartRate,
	(PFNcbUpdateHeartRateWave)&cbUpdateHeartRateWave,
	(PFNcbUpdateBreathing)&cbUpdateBreathing,
	(PFNcbFaceDetected)&cbFaceDetected,
	(PFNcbSetCameraInfo)&cbSetCameraInfo
	};

/*
 * jni functions
 */
boolean Init()
{
    LOGI("Init") ;

    // Init Update HeartRate Thread
#if 0
    if (pthread_create(&thUpdateHeartRate, NULL, thread_UpdateHeartRate,
		(void *)message_UpdateHeartRate)) {
        LOGI("pthread create failed.");
        return FALSE ;
    }

    // Init Update HeartRate Wave Thread
    if (pthread_create(&thUpdateHeartRateWave, NULL, thread_UpdateHeartRateWave,
		(void *)message_UpdateHeartRateWave)) {
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
#endif

    bIsReady = TRUE ;
    bExist = FALSE ;
    return bIsReady ;
}

void Close()
{
    LOGI("Close") ;
	dsp_deinit();
    bIsReady = FALSE ;
    bExist = TRUE ;
}

boolean IsReady()
{
    LOGI("IsReady") ;
    return bIsReady ;
}

/*
 * dsp init should be here
 */
void SetImageResolution(int width, int height)
{
	LOGI("%s: %d, get image resolution width:%d, height:%d", __func__,
		 bIsReady, width, height);
    if( !bIsReady )
        return ;
	win_width=height;
	win_height=width;
	if(dsp_init(DEFAULT_FPS, DEFAULT_WINSTEP, DEFAULT_MINWINTHR, DEFAULT_MAXWINTHR,
		MAX_CHANNELS, &cblist,1)){
		LOGE("DSP init failed!!!\n");
	}
}

void SetCameraInfo(boolean bTBD)
{
    if( !bIsReady )
        return ;
}

/*
 * p is a YUYV 422 format, so 640x480x16bits = 61440 bytes
 * ppg[MAX_CHANNELS*2]; LSB = B:(bpm, mag) G:(bpm, mag) R:(bpm,mag)
 * input : width, height of the frame buffer
 * 		band_min, band_max of the valid human heart beat
 * output: roi[], the detected faces' roi
 * 		nface : the detected faces which is no over than the MAX_FACES
 * return : the valid pulse rate
*/
float processFrame(unsigned sfmt, const void *p, int width, int height,
	float band_min, float band_max, RECT *roi, int *nface
)
{
	float mean_roi[MAX_CHANNELS]={0.0};
	float rs[MAX_FACES],cs[MAX_FACES],ss[MAX_FACES];
	unsigned char *imagedata_rgb=NULL, *imagedata_Y=NULL;
	RECT roi_face;
	int nd=0;
	LOGI("%s: p=%p, fmt=0x%x, %d, %d, fm=%.1f, fM=%.1f\n", __func__, p,sfmt,
		 width, height, band_min, band_max);

	if(V4L2_PIX_FMT_MJPEG == sfmt){

	}else if( (sfmt == V4L2_PIX_FMT_YUYV) /*|| (sfmt == V4L2_PIX_FMT_YVYU)*/ ||
		(sfmt == V4L2_PIX_FMT_YUV422P)){

		//LOGI("YUV422P YUYV");
		imagedata_Y=(unsigned char *)malloc(width * height);
		//yuv_to_rgb888(sfmt, width, height, (unsigned char *)p, imagedata_rgb);
		extractY(sfmt, width, height, (unsigned char *)p, imagedata_Y);
		//LOGI("---YUV422P YUYV");
	}else if( (sfmt == V4L2_PIX_FMT_UYVY) /*|| (sfmt == V4L2_PIX_FMT_VYUY)*/ ){
		//LOGI("YUV422P UYVY");
		imagedata_Y=(unsigned char *)malloc(width * height);
		//yuv_to_rgb888(sfmt, width, height, (unsigned char *)p, imagedata_rgb);
		extractY(sfmt, width, height, (unsigned char *)p, imagedata_Y);
		//LOGI("<<<YUV422P UYVY");
	}else if( (sfmt == V4L2_PIX_FMT_YUV420) || (sfmt == V4L2_PIX_FMT_YVU420)){
		//LOGI("YUV420 YVU420");
		imagedata_Y=(unsigned char *)malloc(width * height);
		//yuv_to_rgb888(sfmt, width, height, (unsigned char *)p, imagedata_rgb);
		extractY(sfmt, width, height, (unsigned char *)p, imagedata_Y);
		//LOGI("--YUV420 YVU420");
	}else if( (sfmt == V4L2_PIX_FMT_NV21) ){
		//LOGI("+++ NV21 YVU420sp");
		/* counter clockwise rotate 90deg */
		unsigned char *c90=(unsigned char *)malloc(width * height);
		unsigned char *pb=(unsigned char *)p;
		for(int i=0;i < height;i++)
			for(int j=0;j<width;j++)
				c90[width*(height - i - 1) + j]=pb[j*height + i];
		imagedata_Y=(unsigned char *)malloc(width * height);
		//LOGI("***NV21 YVU420sp ++");
		//yuv_to_rgb888(sfmt, width, height, (unsigned char *)p, imagedata_rgb);
		//LOGI("*****NV21 YVU420sp ++++");
		//extractY(sfmt, width, height, (unsigned char *)p, imagedata_Y);
		extractY(sfmt, width, height, c90, imagedata_Y);
		free(c90);
		//LOGI("*****NV21 YVU420sp ++++");
	}

	if(imagedata_Y){
		/*static int cnt=0;
		char szFilename[100];
		sprintf(szFilename, "/mnt/sdcard/DCIM/100ANDRO/yuv-%d.y", cnt++);
		xwrite( szFilename, 0, imagedata_Y, width*height) ;*/
		nd=pico_facedetection(imagedata_Y, width, height, MAX_FACES, rs, cs, ss);
		LOGI("*nd=%d\n",nd);
		//setup a rectangle ROI
		if(nd){//sqrt(2)=1.414
			int roi_Y_OFFSET =	ss[0]/10;
			int roi_WIDTH = ss[0]/1.414f;
			int roi_HEIGHT = ss[0]/1.414f + roi_Y_OFFSET;

			//the ROI in the image
			roi_face.x = cs[0] - roi_WIDTH/2;
			roi_face.y = rs[0] - roi_HEIGHT/2 + roi_Y_OFFSET;
			roi_face.width = roi_WIDTH;
			roi_face.height = roi_HEIGHT ;
			*roi=roi_face;
			*nface=1;	//only 1 face is returned
			//imagedata_rgb=(unsigned char *)malloc(roi_WIDTH * roi_HEIGHT * 3);//RGB888
			//yuv_roi_to_rgb888(sfmt, roi_WIDTH, roi_HEIGHT, (unsigned char *)p, imagedata_rgb);
			//callback to facedetection with RECT
			/*if(cblist.cbFaceDetected)
				cblist.cbFaceDetected(1);*/
		}else{//dummy ROI
			int roi_WIDTH = (width>>2);	//160
			int roi_HEIGHT = (height/3);	//160
			int roi_Y_OFFSET =	(70);//(DEFAULT_HEIGHT/height);
			roi_face.x = (width - roi_WIDTH)/2 - 1;
			roi_face.y = (height - roi_HEIGHT)/2 - 1+ roi_Y_OFFSET;
			roi_face.width = roi_WIDTH;
			roi_face.height = roi_HEIGHT;
			*nface=0;	//only 1 face is returned
			//imagedata_rgb=(unsigned char *)malloc(roi_WIDTH * roi_HEIGHT * 3);//RGB888
			/*if(cblist.cbFaceDetected)
				cblist.cbFaceDetected(0);*/
		}
		MATRIX img={height, width, imagedata_rgb };
		LOGI("roi=(x=%d,y=%d, w=%d,h=%d)", roi_face.x, roi_face.y, roi_face.width,
			 roi_face.height);
		//RGB mean value in the RECT roi
		//roiMean(MAX_CHANNELS, &img, roi_face, mean_roi);
		//LOGI("mean_roi(%.4f,%.4f,%.4f)\n", mean_roi[0],mean_roi[1],mean_roi[2]);
		if(imagedata_Y)
			free(imagedata_Y);
	}

	/*
	 * YUV to RGB test
	 */
	imagedata_rgb=(unsigned char *)malloc(width * height * 3);//RGB888
	yuv_to_rgb888(sfmt, height, width, (unsigned char *)p, imagedata_rgb);
	static int cnt=0;
	char szFilename[100];
	sprintf(szFilename, "/mnt/sdcard/DCIM/100ANDRO/yuv-%d.rgb", cnt++);
	xwrite( szFilename, 0, imagedata_rgb, width*height*3);

	float pr=0.0f;
	if(nd){
		//if(!dsp_process_sync(rgb, MIN_HR_BPM, MAX_HR_BPM)){
		if(!dsp_process(mean_roi, band_min, band_max)){
			float ppg[MAX_CHANNELS*2];
			getPPG(ppg, MAX_CHANNELS);//get the result of R,G,B channel
			// ppg[] LSB = B:(bpm, mag) G:(bpm, mag) R:(bpm,mag)
			pr = ppg[2];//return G's bpm
		}
	}
	if(imagedata_rgb)
		free(imagedata_rgb);

	return pr;
}

void SendImage(char *pSource, int length)
{
	LOGI("+%s: get image with length: %d", __func__, length);
    if( !bIsReady )
        return ;
	unsigned sfmt=V4L2_PIX_FMT_NV21;
	RECT roi[MAX_FACES];
	int nFace=0;
	float pr = processFrame(sfmt, (void *)pSource, win_width, win_height,
	MIN_HR_BPM, MAX_HR_BPM, roi, &nFace);

    //LOGI("SendImage: get image with length: %d", length);
}

#if 0
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
#endif

