//
// Created by vlad on 11/14/18.
//

#include <camera/NdkCameraCaptureSession.h>

#include <camera/NdkCameraError.h>

#include <camera/NdkCameraMetadata.h>
#include <camera/NdkCameraMetadataTags.h>
#include <camera/NdkCaptureRequest.h>

#include <assert.h>
#include <jni.h>
#include <pthread.h>

#include <android/native_window_jni.h>

#include <camera/NdkCameraDevice.h>
#include <camera/NdkCameraManager.h>

#include "messages-internal.h"

static ANativeWindow *theNativeWindow;
static ACameraDevice *cameraDevice;
static ACaptureRequest *captureRequest;
static ACameraOutputTarget *cameraOutputTarget;
static ACaptureSessionOutput *sessionOutput;
static ACaptureSessionOutputContainer *captureSessionOutputContainer;
static ACameraCaptureSession *captureSession;

static ACameraDevice_StateCallbacks deviceStateCallbacks;
static ACameraCaptureSession_stateCallbacks captureSessionStateCallbacks;

static void camera_device_on_disconnected(void *context, ACameraDevice *device) {
    LOGI("Camera(id: %s) is diconnected.\n", ACameraDevice_getId(device));
}

static void camera_device_on_error(void *context, ACameraDevice *device, int error) {
    LOGE("Error(code: %d) on Camera(id: %s).\n", error, ACameraDevice_getId(device));
}

static void capture_session_on_ready(void *context, ACameraCaptureSession *session) {
    LOGI("Session is ready. %p\n", session);
}

static void capture_session_on_active(void *context, ACameraCaptureSession *session) {
    LOGI("Session is activated. %p\n", session);
}

static void capture_session_on_closed(void *context, ACameraCaptureSession *session) {
    LOGI("Session is closed. %p\n", session);
}

extern "C" { //com.example.vlad.mylastcamm.MainActivity.startPreview
JNIEXPORT void JNICALL Java_com_example_vlad_mylastcamm_MainActivity_stopPreview(JNIEnv *env,
                                                                                 jobject instance);
JNIEXPORT void JNICALL Java_com_example_vlad_mylastcamm_MainActivity_startPreview(JNIEnv *env,
                                                                                  jobject instance,
                                                                                  jobject surface);
JNIEXPORT void JNICALL Java_com_example_vlad_mylastcamm_MainActivity_stopExtraView(JNIEnv *env,
                                                                                   jobject instance);
JNIEXPORT void JNICALL Java_com_example_vlad_mylastcamm_MainActivity_startExtraView(JNIEnv *env,
                                                                                    jobject instance,
                                                                                    jobject surface);
}

static void openCamera(ACameraDevice_request_template templateId)
{
    ACameraIdList *cameraIdList;
    ACameraMetadata *cameraMetadata ;

    const char *selectedCameraId ;
    camera_status_t camera_status = ACAMERA_OK;
    ACameraManager *cameraManager = ACameraManager_create();

    camera_status = ACameraManager_getCameraIdList(cameraManager, &cameraIdList);
    if (camera_status != ACAMERA_OK) {
        LOGE("Failed to get camera id list (reason: %d)\n", camera_status);
        return;
    }

    if (cameraIdList->numCameras < 1) {
        LOGE("No camera device detected.\n");
        return;
    }

    selectedCameraId = cameraIdList->cameraIds[0];

    LOGI("Trying to open Camera2 (id: %s, num of camera : %d)\n", selectedCameraId,
         cameraIdList->numCameras);

    camera_status = ACameraManager_getCameraCharacteristics(cameraManager, selectedCameraId,
                                                            &cameraMetadata);

    if (camera_status != ACAMERA_OK) {
        LOGE("Failed to get camera meta data of ID:%s\n", selectedCameraId);
    }

    deviceStateCallbacks.onDisconnected = camera_device_on_disconnected;
    deviceStateCallbacks.onError = camera_device_on_error;

    camera_status = ACameraManager_openCamera(cameraManager, selectedCameraId,
                                              &deviceStateCallbacks, &cameraDevice);

    if (camera_status != ACAMERA_OK) {
        LOGE("Failed to open camera device (id: %s)\n", selectedCameraId);
    }

    camera_status = ACameraDevice_createCaptureRequest(cameraDevice, templateId,
                                                       &captureRequest);

    if (camera_status != ACAMERA_OK) {
        LOGE("Failed to create preview capture request (id: %s)\n", selectedCameraId);
    }

    ACaptureSessionOutputContainer_create(&captureSessionOutputContainer);

    captureSessionStateCallbacks.onReady = capture_session_on_ready;
    captureSessionStateCallbacks.onActive = capture_session_on_active;
    captureSessionStateCallbacks.onClosed = capture_session_on_closed;

    ACameraMetadata_free(cameraMetadata);
    ACameraManager_deleteCameraIdList(cameraIdList);
    ACameraManager_delete(cameraManager);
}

static void closeCamera(void)
{
    camera_status_t camera_status = ACAMERA_OK;

    if (captureRequest != NULL) {
        ACaptureRequest_free(captureRequest);
        captureRequest = NULL;
    }

    if (cameraOutputTarget != NULL) {
        ACameraOutputTarget_free(cameraOutputTarget);
        cameraOutputTarget = NULL;
    }

    if (cameraDevice != NULL) {
        camera_status = ACameraDevice_close(cameraDevice);

        if (camera_status != ACAMERA_OK) {
            LOGE("Failed to close CameraDevice.\n");
        }
        cameraDevice = NULL;
    }

    if (sessionOutput != NULL) {
        ACaptureSessionOutput_free(sessionOutput);
        sessionOutput = NULL;
    }

    if (captureSessionOutputContainer != NULL) {
        ACaptureSessionOutputContainer_free(captureSessionOutputContainer);
        captureSessionOutputContainer = NULL;
    }

    LOGI("Close Camera\n");
}

JNIEXPORT void JNICALL Java_com_example_vlad_mylastcamm_MainActivity_startPreview(JNIEnv *env,
                                                                                  jobject instance,
                                                                                  jobject surface) {
    theNativeWindow = ANativeWindow_fromSurface(env, surface);

    openCamera(TEMPLATE_PREVIEW);

    LOGI("Surface is prepared in %p.\n", surface);

    ACameraOutputTarget_create(theNativeWindow, &cameraOutputTarget);
    ACaptureRequest_addTarget(captureRequest, cameraOutputTarget);

    ACaptureSessionOutput_create(theNativeWindow, &sessionOutput);
    ACaptureSessionOutputContainer_add(captureSessionOutputContainer, sessionOutput);

    ACameraDevice_createCaptureSession(cameraDevice, captureSessionOutputContainer,
                                       &captureSessionStateCallbacks, &captureSession);

    ACameraCaptureSession_setRepeatingRequest(captureSession, NULL, 1, &captureRequest, NULL);

}

JNIEXPORT void JNICALL Java_com_example_vlad_mylastcamm_MainActivity_stopPreview(JNIEnv *env,
                                                                                 jobject instance) {
    closeCamera();
    if (theNativeWindow != NULL) {
        ANativeWindow_release(theNativeWindow);
        theNativeWindow = NULL;
    }
}

static ANativeWindow *extraViewWindow;
static ACaptureRequest *extraViewCaptureRequest;
static ACameraOutputTarget *extraViewOutputTarget;
static ACaptureSessionOutput *extraViewSessionOutput;

JNIEXPORT void JNICALL Java_com_example_vlad_mylastcamm_MainActivity_startExtraView(JNIEnv *env,
                                                                                    jobject instance,
                                                                                    jobject surface) {

    /* Assuming that camera preview has already been started */
    extraViewWindow = ANativeWindow_fromSurface(env, surface);

    LOGI("Extra view surface is prepared in %p.\n", surface);
    ACameraCaptureSession_stopRepeating(captureSession);

    ACameraDevice_createCaptureRequest(cameraDevice, TEMPLATE_STILL_CAPTURE,
                                       &extraViewCaptureRequest);

    ACameraOutputTarget_create(extraViewWindow, &extraViewOutputTarget);
    ACaptureRequest_addTarget(extraViewCaptureRequest, extraViewOutputTarget);

    ACaptureSessionOutput_create(extraViewWindow, &extraViewSessionOutput);
    ACaptureSessionOutputContainer_add(captureSessionOutputContainer,
                                       extraViewSessionOutput);

    /* Not sure why the session should be recreated.
     * Otherwise, the session state goes to ready */
    ACameraCaptureSession_close(captureSession);
    ACameraDevice_createCaptureSession(cameraDevice, captureSessionOutputContainer,
                                       &captureSessionStateCallbacks, &captureSession);

    ACaptureRequest *requests[2];
    requests[0] = captureRequest;
    requests[1] = extraViewCaptureRequest;

    ACameraCaptureSession_setRepeatingRequest(captureSession, NULL, 2, requests, NULL);
}

JNIEXPORT void JNICALL Java_com_example_vlad_mylastcamm_MainActivity_stopExtraView(JNIEnv *env,
                                                                                   jobject instance) {

    ACameraCaptureSession_stopRepeating(captureSession);

    ACaptureSessionOutputContainer_remove(captureSessionOutputContainer, extraViewSessionOutput);

    ACaptureRequest_removeTarget(extraViewCaptureRequest, extraViewOutputTarget);

    ACameraOutputTarget_free(extraViewOutputTarget);
    ACaptureSessionOutput_free(extraViewSessionOutput);
    ACaptureRequest_free(extraViewCaptureRequest);

    ACameraCaptureSession_setRepeatingRequest(captureSession, NULL, 1, &captureRequest, NULL);

    if (extraViewWindow != NULL) {
        ANativeWindow_release(extraViewWindow);
        extraViewWindow = NULL;
        LOGI("Extra view surface is released\n");

    }
}