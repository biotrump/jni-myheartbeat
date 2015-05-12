/** @file cv_call_entry.h
 *  @brief Function prototypes for the cv call entry.
 *
 *  This contains the prototypes for the cv call entry.
 *
 *  @author Jerry Chen
 *  @bug No known bugs.
 */

#ifndef _CV_CALL_ENTRY_H
#define _CV_CALL_ENTRY_H

#define TRUE 1

#define FALSE 0

#define boolean int

/** @brief interface - Init cv native library that call by java layer method
 *
 *  Needs to call this interface before using any cv native library
 *
 *  @return success or failure result
 */
boolean Init() ;

/** @brief interface - check cv native library is init or ready that call by java layer method
 *
 *  Java layer can call this interface to know cv function is ready or not.
 *  This function should work even cv function is not init.
 *  @return The initial result. TRUE is cv native library ready. FALSE is cv native library not ready.
 */
boolean IsReady() ;

/** @brief interface - Stop using cv native library that call by java layer method
 *
 *  Java layer can call this interface to let cv function know java layer is stopped using.
 *
 *  @return void
 */
void Close() ;

/** @brief interface - set image resolution that call by java layer method
 *
 *  Java layer will call this interface to let native cv function know the resolution that will be send to
 *
 *  @param width The width of send image
 *  @param height The height of send image
 *
 *  @return void
 */
void SetImageResolution(int width, int height) ;

/** @brief interface - set camera info that call by java layer method
 *
 *  This interface still under planning. Expect java layer will send Camera resolution/aspect ratio/fps
 *
 *  @param bTBD TBD
 *
 *  @return void
 */
void SetCameraInfo(boolean bTBD) ;

/** @brief interface - send image to cv function t camera info that call by java layer method
 *
 *  This interface still under planning. Expect java layer will send rgb or yuv format image
 *
 *  @param bTBD TBD
 *
 *  @return void
 */
void SendImage(boolean bTBD) ;

/** @brief callback - Update Heart Rate info to java layer
 *
 *  In normally case, the value should between 0 to 200.
 *  So, the maximum value should be 200 and the current heart rate should in this range.
 *
 *  @param value Current heart rate value
 *
 *  @param max The maximum value of heart rate
 *
 *  @return success or failure result
 */
boolean cbUpdateHeartRate(int value, int max) ;

/** @brief callback - Update Heart Rate wave info to java layer
 *
 *  This call back work for let java layer know heart rate wave.
 *  Java layer can use these values to update heart rate wave on user view.
 *
 *  @param record How many record in this array.
 *
 *  @param count The count of this record.
 *
 *  @param value The value of this record.
 *
 *  @param max The maximum value of this record.
 *
 *  @return success or failure result
 */
boolean cbUpdateHeartRateWave(int value, int max) ;

/** @brief callback - Update Breathing info to java layer
 *
 *  In normally case, the value should between 0 to 60.
 *  So, the maximum value should be 60 and the current breathing value should in this range.
 *
 *  @param value Current breathing value
 *
 *  @param max The maximum value of breathing info
 *
 *  @return success or failure result
 */
boolean cbUpdateBreathing(int value, int max) ;

/** @brief callback - Update Face Detect info to java layer
 *
 *  Native layer can call this interface to let java layer know face is detected or not.
 *
 *  @param bDetect TRUE is face detected. FALSE is face not detected.
 *
 *  @return success or failure result
 */
boolean cbFaceDetected(boolean bDetect) ;

/** @brief callback - Set Camera by using java layer control method
 *
 *  Native layer can call this interface to set device camera.
 *
 *  @param bTBD TBD
 *
 *  @return success or failure result
 */
boolean cbSetCameraInfo(int bTBD) ;

#endif

