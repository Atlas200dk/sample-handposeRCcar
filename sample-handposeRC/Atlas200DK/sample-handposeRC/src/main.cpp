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

* File main.cpp
* Description:  main func
*/

#include <iostream>
#include <stdlib.h>
#include <dirent.h>

#include "hand_pose.h"
#include "utils.h"
#include "camera.h"
#include <sys/time.h>

using namespace std;

namespace {
uint32_t kModelWidth = 256;
uint32_t kModelHeight = 256;
const char* kModelPath = "../model/handpose_argmax_bgr.om";
uint32_t send_mode = SEND_TO_NONE;  // can choose between i2c, uart, none (defined in hand_pose.h)

shared_ptr<ImageData> g_imagedata = make_shared<ImageData>();
}

int main(int argc, char *argv[]) {
    bool bSetChannelId = true;
    int channelId = 0;

    // argument parsing
    if (argc > 1) {
        for (int i = 1; i < argc; ++i) {
            if (string(argv[i]) == "--send_mode") {
                if (i + 1 < argc) {
                    string send_mode_str = string(argv[++i]);
                    if (send_mode_str == "none"){send_mode = SEND_TO_NONE; INFO_LOG("Send Mode: None\n");}
                    else if (send_mode_str == "i2c"){send_mode = SEND_TO_I2C;INFO_LOG("Send Mode: I2C\n");}
                    else if (send_mode_str == "uart"){send_mode = SEND_TO_UART;INFO_LOG("Send Mode: UART\n");}
                    else {std::cerr << "wrong send_mode argument: " <<send_mode_str<< std::endl; return 1;}
                } else {
                    std::cerr << "--send_mode option requires one argument." << std::endl;
                    return 1;
                }
            }
            else
                if (std::string(argv[i]) == "--channel") {
                    if (i + 1 < argc) {
                        string channelName = string(argv[++i]);
                        Utils::GetChannelID(channelName, channelId);
                        if (0xFF == channelId) {
                            INFO_LOG("channelId = %d  ERROR \n", channelId);
                            return FAILED;
                        }
                    } else {
                        std::cerr << "--channel option requires one argument." << std::endl;
                        return 1;
                    }
                }
                else {
                    std::cerr << "wrong arguments" << std::endl;
                    return 1;
                }
        }
    }

    //create handpose processor object
    HandPose processor(kModelPath, kModelWidth, kModelHeight, send_mode);
    //initialize resource
    Result ret = processor.Init();
    if (ret != SUCCESS) {
        ERROR_LOG("Classification Init resource failed\n");
        return FAILED;
    }

    Camera  cameraDevice(channelId);
    if(false == cameraDevice.IsOpened(channelId))
    {
        if (cameraDevice.Open(channelId)) {
            ERROR_LOG("Failed to open channelId =%d.\n", channelId);
            return FAILED;
        }
    }

    void * buffer = nullptr;
    int size = cameraDevice.GetCameraDataSize(channelId);

    // alloc memory for camera input
    aclError aclRet = acldvppMalloc(&buffer, size);
    g_imagedata->data.reset((uint8_t*)buffer, [](uint8_t* p) { acldvppFree((void *)p); });

    aclmdlDataset* inferenceOutput = nullptr;
    cv::Mat frame(720, 1280, CV_8UC3, cv::Scalar(0, 0, 0));

    while(1)
    {
        //process frame by frame
        cameraDevice.Read(channelId, *(g_imagedata.get()));
        if (g_imagedata->data == nullptr) {
            ERROR_LOG("Read image %d failed\n", channelId);
            return FAILED;
        }
        //Convert YUV to BGR
        processor.ConvertYUVtoBGR(*(g_imagedata.get()), frame.data);
        
        // Resize img
        ret = processor.PreprocessOpenCV(frame);
        if (ret != SUCCESS) {
            ERROR_LOG("Preprocess image %d failed, continue to read next\n",  channelId);
            return FAILED;
        }

        // Inference
        ret = processor.Inference(inferenceOutput);
        if ((ret != SUCCESS) || (inferenceOutput == nullptr)) {
            ERROR_LOG("Inference model inference output data failed\n");
            return FAILED;
        }
        
        // Postprocess
        ret = processor.Postprocess(frame, inferenceOutput);
        if (ret != SUCCESS) {
            ERROR_LOG("Process model inference output data failed\n");
            return FAILED;
        }
    }

    INFO_LOG("Execute sample success");
    return SUCCESS;
}
