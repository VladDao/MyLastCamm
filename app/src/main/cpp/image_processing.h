//
// Created by vlad on 11/6/18.
//

#ifndef MYLASTCAMM_IMAGE_PROCESSING_H
#define MYLASTCAMM_IMAGE_PROCESSING_H


#include <jni.h>
#include <android_native_app_glue.h>
#include "image_reader.h"
#include "camera_manager.h"

class Image_processing {
public:
    explicit Image_processing(android_app* app);

    static void Metrics(int);
    std::string static State();

private:

    struct android_app* app_;
    ImageFormat savedNativeWinRes_;
    bool cameraGranted_;
    int rotation_;
    volatile bool cameraReady_;
    NDKCamera* camera_;
    ImageReader* yuvReader_;
    ImageReader* jpgReader_;




    static void ChengaMetrics(int metrics);
};

/**
 * retrieve global singleton CameraEngine instance
 * @return the only instance of CameraEngine in the app
 */
Image_processing* GetAppParams(void);


#endif //MYLASTCAMM_IMAGE_PROCESSING_H
