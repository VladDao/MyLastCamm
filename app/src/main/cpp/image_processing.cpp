//
// Created by vlad on 11/6/18.
//

#include "image_processing.h"
#include "camera_utils.h"

const int None = 0;
const int I_PSNR = 1;
const int I_WSNR = 2;
const int I_PSNR_HVS_M = 3;
const int I_MS_SSIM = 4;
const int I_GMSD = 5;
const int I_IW_SSIM = 6;
const int I_ADD_SSIM = 7;
const int I_GSM = 8;
const int I_Haar_PSI = 9;
/*

Image_processing::Image_processing(android_app *app) {

}
*/
std::string stateTest;

void Image_processing::Metrics(int metrics) {

    ChengaMetrics(metrics);

}


void Image_processing::ChengaMetrics(int metrics) {

    switch (metrics) {
        case None: {
            stateTest = "None";
            break;
        }case I_PSNR: {
            stateTest = "I_PSNR";
            break;
        }case I_WSNR: {
            stateTest = "I_WSNR";
            break;
        }case I_PSNR_HVS_M: {
            stateTest = "I_PSNR_HVS_M";
            break;
        }case I_MS_SSIM: {
            stateTest = "I_MS_SSIM";
            break;
        }case I_GMSD: {
            stateTest = "I_GMSD";
            break;
        }case I_IW_SSIM: {
            stateTest = "I_IW_SSIM";
            break;
        }case I_ADD_SSIM: {
            stateTest = "I_ADD_SSIM";
            break;
        }case I_GSM: {
            stateTest = "I_GSM";
            break;
        }case I_Haar_PSI: {
            stateTest = "I_Haar_PSI";
            break;
        }
        default:{
            stateTest = "None";
            break;
        }


    }


}

Image_processing::Image_processing(android_app *app) {

}

std::string Image_processing::State() {
    return stateTest;
}


