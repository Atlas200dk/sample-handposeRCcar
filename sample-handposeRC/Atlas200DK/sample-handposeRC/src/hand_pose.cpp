/**
* Copyright 2020 Huawei Technologies Co., Ltd
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at

* http://www.apache.org/licenses/LICENSE-2.0

* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.

* File hand_pose.cpp
* Description: hand pose proccesor
*/
#include <iostream>

#include "acl/acl.h"
#include "model_process.h"
#include "utils.h"
#include "hand_pose.h"


using namespace std;

uint32_t gSendNum = 0;

HandPose::HandPose(const char* modelPath,
                           uint32_t modelWidth,
                           uint32_t modelHeight,
                           uint32_t send_mode)
:deviceId_(0), context_(nullptr), stream_(nullptr), modelWidth_(modelWidth),
 modelHeight_(modelHeight), isInited_(false), send_mode(send_mode){
    imageInfoSize_ = RGBU8_IMAGE_SIZE(modelWidth_, modelHeight_);
    inputDataSize_ = RGBU8_IMAGE_SIZE(modelWidth_, modelHeight_);
    imageInfoBuf_ = nullptr;
    modelPath_ = modelPath;
    Channel* chan = nullptr;
    PresenterErrorCode openChannelret = OpenChannelByConfig(chan, "../data/param.conf");
    if (openChannelret != PresenterErrorCode::kNone) {
        ERROR_LOG("Open channel failed, error %d\n", (int)openChannelret);
    }
    chan_.reset(chan);
    
    if(send_mode == SEND_TO_I2C){
        if(i2c_ctrl.atlas_setI2CSlave(8) < 0) // set target to slave device at address 0x08 
        {
        ERROR_LOG("SET SLAVE failed...\r\n");
        }
    }
    else if(send_mode == SEND_TO_UART){
        // open UART with correct baudrate (38400)
        if(uart_ctrl.uart_open() < 0)
        {
        ERROR_LOG("uart_open error");
        }
        if(uart_ctrl.uart_set_option(38400,8,'N',1) < 0)
        {
        ERROR_LOG("uart_set_option error");
        }
    }
}

HandPose::~HandPose() {
    DestroyResource();
}

Result HandPose::InitResource() {
    // ACL init
    const char *aclConfigPath = "../src/acl.json";
    aclError ret = aclInit(aclConfigPath);
    if (ret != ACL_ERROR_NONE) {
        ERROR_LOG("acl init failed\n");
        return FAILED;
    }
    INFO_LOG("acl init success\n");

    ret = aclrtSetDevice(deviceId_);
    if (ret != ACL_ERROR_NONE) {
        ERROR_LOG("acl open device %d failed\n", deviceId_);
        return FAILED;
    }
    INFO_LOG("open device %d success\n", deviceId_);

    ret = aclrtGetRunMode(&runMode_);
    if (ret != ACL_ERROR_NONE) {
        ERROR_LOG("acl get run mode failed\n");
        return FAILED;
    }

    return SUCCESS;
}

Result HandPose::InitModel(const char* omModelPath) {
    Result ret = model_.LoadModelFromFileWithMem(omModelPath);
    if (ret != SUCCESS) {
        ERROR_LOG("execute LoadModelFromFileWithMem failed\n");
        return FAILED;
    }

    ret = model_.CreateDesc();
    if (ret != SUCCESS) {
        ERROR_LOG("execute CreateDesc failed\n");
        return FAILED;
    }

    ret = model_.CreateOutput();
    if (ret != SUCCESS) {
        ERROR_LOG("execute CreateOutput failed\n");
        return FAILED;
    }

    aclrtMalloc(&imageInfoBuf_, (size_t)(inputDataSize_),
                ACL_MEM_MALLOC_HUGE_FIRST);
    if (imageInfoBuf_ == nullptr) {
        ERROR_LOG("Acl malloc image buffer failed.");
        return FAILED;
    }

    ret = model_.CreateInput(imageInfoBuf_, inputDataSize_);
    if (ret != SUCCESS) {
        ERROR_LOG("Create mode input dataset failed");
        return FAILED;
    }
    return SUCCESS;
}

Result HandPose::Init() {
    if (isInited_) {
        INFO_LOG("Classify instance is initied already!\n");
        return SUCCESS;
    }

    Result ret = InitResource();
    if (ret != SUCCESS) {
        ERROR_LOG("Init acl resource failed\n");
        return FAILED;
    }

    ret = InitModel(modelPath_);
    if (ret != SUCCESS) {
        ERROR_LOG("Init model failed\n");
        return FAILED;
    }

    isInited_ = true;
    return SUCCESS;
}

void HandPose::ConvertYUVtoBGR(ImageData& img, unsigned char *dest)
{   
    unsigned char* src = img.data.get();
    int width = 1280;
    int height = 720;
    cv::Mat myuv(height + height/2, width, CV_8UC1, src); // pass buffer pointer, not its address
    cv::Mat mrgb(height, width, CV_8UC3, dest);
    cv::cvtColor(myuv, mrgb, cv::COLOR_YUV420sp2BGR);

    return;
}

Result HandPose::PreprocessOpenCV(cv::Mat& frame) {
    //resize
    cv::Mat reiszeMat;
    cv::resize(frame, reiszeMat, cv::Size(modelWidth_, modelHeight_));
    if (reiszeMat.empty()) {
        ERROR_LOG("Resize image failed");
        return FAILED;
    }
    memcpy(imageInfoBuf_, reiszeMat.ptr<void>(), inputDataSize_);

    return SUCCESS;
}

Result HandPose::Inference(aclmdlDataset*& inferenceOutput) {
    Result ret = model_.Execute();
    if (ret != SUCCESS) {
        model_.DestroyInput();
        ERROR_LOG("Execute model inference failed\n");
        return FAILED;
    }

    inferenceOutput = model_.GetModelOutputData();
    return SUCCESS;
}

Result HandPose::Postprocess(cv::Mat& origImage, aclmdlDataset* modelOutput) {
    uint32_t dataSize = 0;
    
    // model output is argmax of heatmaps(shape: 1,64*64,21) on dim 1.
    // output in memory is 21 float32 values.
    float* detectData = (float *)GetInferenceOutputItem(dataSize, modelOutput,
                                                        0);

    if (detectData == nullptr)
        return FAILED;

    // get keypoint coordinates from inference output
    vector<DetectionResult> detectResults;
    DetectionResult one_result;
    Point point_lt, point_rb;

    int widthScale = (int)origImage.cols / 64;  
    int heightScale = (int)origImage.rows / 64; 

    for (int32_t k = 0; k < dataSize / 4; k++) {
      float argmax_index = detectData[k];
      // to get x,y coordinates from each value: cast to int then x = val % 64, y = val / 64
      int32_t x_coordinate = (int)argmax_index % 64;
      int32_t y_coordinate = (int)argmax_index / 64;
      x_coordinate = x_coordinate * widthScale;
      y_coordinate = y_coordinate * heightScale;
      handposeDecode_.x_arr[k] = x_coordinate;
      handposeDecode_.y_arr[k] = y_coordinate;
    }

    one_result.result_text.append("Command: ");

    // get rc_command index from keypoints and add to presenter server string
    int rc_command = int(handposeDecode_.get_rc_command());
    switch(rc_command){
        case 0: one_result.result_text.append("STOP"); break;
        case 1: one_result.result_text.append("FORWARD"); break;
        case 2: one_result.result_text.append("BACKWARD"); break;
        case 3: one_result.result_text.append("RIGHT"); break;
        case 4: one_result.result_text.append("LEFT"); break;
    }

    // delimiter to seperate text and coordinates
    one_result.result_text.append("--");

    // add keypoint coordinates to presenter server string
    // if command is not "stop"
    if(rc_command != 0){
        for (int32_t k = 0; k < dataSize / 4; k++) {
          one_result.result_text.append(to_string((int)origImage.cols-handposeDecode_.x_arr[k]));
          one_result.result_text.append(",");
          one_result.result_text.append(to_string(handposeDecode_.y_arr[k]));
          one_result.result_text.append(" ");
        }
      }

    // left top
    point_lt.x = 0;
    point_lt.y = 0;
    // right bottom
    point_rb.x = 0;
    point_rb.y = 0;
    one_result.lt = point_lt;
    one_result.rb = point_rb;

//    INFO_LOG("send mode %d ",send_mode);
//    INFO_LOG(" cmd: %d", rc_command);

    if(send_mode == SEND_TO_I2C){
        // send command through i2c
        unsigned char cmd = 0x01;  // unused, can be any byte
        i2c_ctrl.atlas_i2c_smbus_write_byte_data(cmd, (unsigned char)rc_command);
    }
    else if(send_mode == SEND_TO_UART){
        // send command character through UART
        char send_cmd;
        switch(rc_command){
            case 0: send_cmd = 's'; break;
            case 1: send_cmd = 'f'; break;
            case 2: send_cmd = 'b'; break;
            case 3: send_cmd = 'r'; break;
            case 4: send_cmd = 'l'; break;
        }
        char* send_buf = &send_cmd;
        int len = uart_ctrl.uart_send(send_buf, 1);
        if (len < 0)
        {
            printf("UART write data error");
        }
    }

    detectResults.emplace_back(one_result);

    cv::Mat mirrored_Image;
    cv::flip(origImage, mirrored_Image, 1);

    Result ret = SendImage(detectResults, mirrored_Image);
    return ret;
}


void HandPose::EncodeImage(vector<uint8_t>& encodeImg,
                                  cv::Mat& origImg) {
    vector<int> param = vector<int>(2);
    param[0] = CV_IMWRITE_JPEG_QUALITY;
    param[1] = 95;//default(95) 0-100
    cv::Mat RGBimg;
    cv::cvtColor(origImg, RGBimg, cv::COLOR_BGR2RGB);
    cv::imencode(".jpg", RGBimg, encodeImg, param);
}

void* HandPose::GetInferenceOutputItem(uint32_t& itemDataSize,
                                           aclmdlDataset* inferenceOutput,
                                           uint32_t idx) {
    aclDataBuffer* dataBuffer = aclmdlGetDatasetBuffer(inferenceOutput, idx);
    if (dataBuffer == nullptr) {
        ERROR_LOG("Get the %dth dataset buffer from model "
                  "inference output failed\n", idx);
        return nullptr;
    }

    void* dataBufferDev = aclGetDataBufferAddr(dataBuffer);
    if (dataBufferDev == nullptr) {
        ERROR_LOG("Get the %dth dataset buffer address "
                  "from model inference output failed\n", idx);
        return nullptr;
    }

    size_t bufferSize = aclGetDataBufferSize(dataBuffer);
    if (bufferSize == 0) {
        ERROR_LOG("The %d   th dataset buffer size of "
                  "model inference output is 0\n", idx);
        return nullptr;
    }

    void* data = nullptr;
    data = dataBufferDev;

    itemDataSize = bufferSize;
    return data;
}

void HandPose::DestroyResource()
{
    aclError ret = aclrtResetDevice(deviceId_);
    if (ret != ACL_ERROR_NONE) {
        ERROR_LOG("reset device failed\n");
    }
    INFO_LOG("end to reset device is %d\n", deviceId_);

    ret = aclFinalize();
    if (ret != ACL_ERROR_NONE) {
        ERROR_LOG("finalize acl failed\n");
    }
    INFO_LOG("end to finalize acl");
    aclrtFree(imageInfoBuf_);
}
Result HandPose::SendImage(vector<DetectionResult>& detectionResults,
                                  cv::Mat& origImg) {

    vector<uint8_t> encodeImg;
    EncodeImage(encodeImg, origImg);

    ImageFrame imageParam;
    imageParam.format = ImageFormat::kJpeg;
    imageParam.width = origImg.cols;
    imageParam.height = origImg.rows;
    imageParam.size = encodeImg.size();
    imageParam.data = reinterpret_cast<uint8_t*>(encodeImg.data());
    imageParam.detection_results = detectionResults;

    PresenterErrorCode ret = PresentImage(chan_.get(), imageParam);
    // send to presenter failed
    if (ret != PresenterErrorCode::kNone) {
        ERROR_LOG("Send JPEG image to presenter failed, error %d\n", (int)ret);
        return FAILED;
    }

    //INFO_LOG("Send JPEG image to presenter success, ret %d,num =%d \n", (int)ret,gSendNum++);
    return SUCCESS;
}


