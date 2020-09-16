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

* File hand_pose.h
* Description: hand pose processor
*/
#pragma once
#include "utils.h"
#include <iostream>
#include <mutex>
#include <unistd.h>
#include "presenter/agent/presenter_channel.h"
#include "presenter/agent/presenter_types.h"

#include "acl/acl.h"
#include "model_process.h"
#include "handpose_decode.h"
#include "i2c.h"
#include "uart.h"

#define SEND_TO_NONE 0
#define SEND_TO_I2C 1
#define SEND_TO_UART 2

using namespace std;
using namespace ascend::presenter;


class HandPose {
public:
    HandPose(const char* modelPath, 
                 uint32_t modelWidth, uint32_t modelHeight, uint32_t send_mode);
    ~HandPose();

    Result Init();
    Result PreprocessOpenCV(cv::Mat& frame);
    Result Inference(aclmdlDataset*& inferenceOutput);
    Result Postprocess(cv::Mat& origImage, aclmdlDataset* modelOutput);
    Result SendImage(vector<DetectionResult>& detRes, cv::Mat& origImage);
    void ConvertYUVtoBGR(ImageData& img, unsigned char *dest);

private:
    Result InitResource();
    Result InitModel(const char* omModelPath);
    void* GetInferenceOutputItem(uint32_t& itemDataSize,
                                 aclmdlDataset* inferenceOutput,
                                 uint32_t idx);
    void EncodeImage(vector<uint8_t>& encodeImg, cv::Mat& origImg);
    void DestroyResource();
private:
    int32_t deviceId_;
    aclrtContext context_;
    aclrtStream stream_;
    uint32_t imageInfoSize_;
    void* imageInfoBuf_;
    ModelProcess model_;

    const char* modelPath_;
    uint32_t modelWidth_;
    uint32_t modelHeight_;
    uint32_t inputDataSize_;
    aclrtRunMode runMode_;
    bool isInited_;
    std::shared_ptr<Channel> chan_;

    // objects for handpose RC car
    HandposeDecode handposeDecode_;  // get command from keypoint coordinates
    i2c i2c_ctrl;
    uart uart_ctrl;
    uint32_t send_mode;  // i2c, uart, or none

};


