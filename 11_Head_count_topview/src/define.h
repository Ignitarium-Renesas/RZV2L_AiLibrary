/*
 * Original Code (C) Copyright Renesas Electronics Corporation 2023
 *　
 *  *1 DRP-AI TVM is powered by EdgeCortix MERA(TM) Compiler Framework.
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

/***********************************************************************************************************************
* File Name    : define.h
* Version      : 1.1.0
* Description  : DRP-AI TVM[*1] Application for Top view Head Count

***********************************************************************************************************************/
#ifndef DEFINE_MACRO_H
#define DEFINE_MACRO_H

/*****************************************
* includes
******************************************/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <vector>
#include <map>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <float.h>
#include <atomic>
#include <semaphore.h>
#include <math.h>


/*****************************************
* Static Variables for YOLOv3
* Following variables need to be changed in order to custormize the AI model
******************************************/
/*****************************************
* YOLOv3
******************************************/
/* Model Binary */
const static std::string model_dir = "topview_headcount_yolov3";
/* Pre-processing Runtime Object */
const static std::string pre_dir = model_dir + "/preprocess";

/* Label list file */
const static std::string label_list = "labels.txt";

/* Empty List to store label list */
static std::vector<std::string> label_file_map = {};

/* DRP-AI memory offset for model object file*/
#define DRPAI_MEM_OFFSET            (0X38E0000)

/*****************************************
 * Macro for YOLOv3
 ******************************************/
/* Number of class to be detected */
#define NUM_CLASS                   (1)
/* Number for [region] layer num parameter */
#define NUM_BB                      (3)
#define NUM_INF_OUT_LAYER           (3)

/* Number of grids in the image. The length of this array MUST match with the NUM_INF_OUT_LAYER */
const static uint8_t num_grids[] = {13, 26, 52};
/* Number of DRP-AI output */
const static uint32_t INF_OUT_SIZE = (NUM_CLASS + 5) * NUM_BB * num_grids[0] * num_grids[0]
                                + (NUM_CLASS + 5) * NUM_BB * num_grids[1] * num_grids[1]
                                + (NUM_CLASS + 5) * NUM_BB * num_grids[2] * num_grids[2];
const static double anchors[] =
    {
     10, 13,
    16, 30,
    33, 23,
    30, 61,
    62, 45,
    59, 119,
    116, 90,
    156, 198,
    373, 326
    };


/* Thresholds */
#define TH_PROB                     (0.5f)
#define TH_NMS                      (0.5f)
/* Size of input image to the model */
#define MODEL_IN_W                  (416)
#define MODEL_IN_H                  (416)

/*DRP-AI Input image information*/
#define IMAGE_WIDTH                 (640)
#define IMAGE_HEIGHT                (480)
#define DRPAI_IN_WIDTH              (IMAGE_WIDTH)
#define DRPAI_IN_HEIGHT             (IMAGE_HEIGHT)

/*Image:: Text information to be drawn on image*/
#define CHAR_SCALE_LARGE            (0.8)
#define CHAR_SCALE_SMALL            (0.7)
#define CHAR_SCALE_XS               (0.3)
#define CHAR_THICKNESS              (1)
#define LINE_HEIGHT                 (30) 
#define LINE_HEIGHT_OFFSET          (20) 

#endif