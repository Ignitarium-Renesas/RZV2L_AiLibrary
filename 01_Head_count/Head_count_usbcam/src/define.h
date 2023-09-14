/***********************************************************************************************************************
* DISCLAIMER
* This software is supplied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* applicable laws, including copyright laws.
* THIS SOFTWARE IS PROVIDED "AS IS" AND RENESAS MAKES NO WARRANTIES REGARDING
* THIS SOFTWARE, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. ALL SUCH WARRANTIES ARE EXPRESSLY DISCLAIMED. TO THE MAXIMUM
* EXTENT PERMITTED NOT PROHIBITED BY LAW, NEITHER RENESAS ELECTRONICS CORPORATION NOR ANY OF ITS AFFILIATED COMPANIES
* SHALL BE LIABLE FOR ANY DIRECT, INDIRECT, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES FOR ANY REASON RELATED TO THIS
* SOFTWARE, EVEN IF RENESAS OR ITS AFFILIATES HAVE BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES.
* Renesas reserves the right, without notice, to make changes to this software and to discontinue the availability of
* this software. By using this software, you agree to the additional terms and conditions found by accessing the
* following link:
* http://www.renesas.com/disclaimer
*
* Copyright (C) 2022 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : define.h
* Version      : 7.20
* Description  : RZ/V2L DRP-AI Sample Application for Darknet-PyTorch YOLOv3 MIPI Camera version
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
*  - drpai_prefix = directory name of DRP-AI Object files (DRP-AI Translator output)
******************************************/
/*****************************************
* YOLOv3
******************************************/
/* Directory name of DRP-AI Object files (DRP-AI Translator output) */
const static std::string drpai_prefix   = "yolov3_cam";
/* Empty since labels will be loaded from label_list file */
static std::vector<std::string> label_file_map = {"head"};

/* Number of class to be detected */
#define NUM_CLASS           (1)
/* Number for [yolo] layer num parameter */
#define NUM_BB              (3)
/* Number of output layers. This value MUST match with the length of num_grids[] below */
#define NUM_INF_OUT_LAYER   (3)
/* Number of grids in the image. The length of this array MUST match with the NUM_INF_OUT_LAYER */
const static uint8_t num_grids[] = { 13, 26, 52 };
/* Number of DRP-AI output */
const static uint32_t num_inf_out =  (NUM_CLASS + 5) * NUM_BB * num_grids[0] * num_grids[0]
                                + (NUM_CLASS + 5) * NUM_BB * num_grids[1] * num_grids[1]
                                + (NUM_CLASS + 5) * NUM_BB * num_grids[2] * num_grids[2];
/* Anchor box information */
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
/*****************************************
* Static Variables (No need to change)
* Following variables are the file name of each DRP-AI Object file
* drpai_file_path order must be same as the INDEX_* defined later.
******************************************/
const static std::string drpai_address_file = drpai_prefix+"/"+drpai_prefix+"_addrmap_intm.txt";
const static std::string drpai_file_path[5] =
{
    drpai_prefix+"/drp_desc.bin",
    drpai_prefix+"/"+drpai_prefix+"_drpcfg.mem",
    drpai_prefix+"/drp_param.bin",
    drpai_prefix+"/aimac_desc.bin",
    drpai_prefix+"/yolov3_Head_counting_weights.dat",
};
#define TH_PROB                     (0.5f)
#define TH_NMS                      (0.5f)
/* Size of input image to the model */
#define MODEL_IN_W                  (416)
#define MODEL_IN_H                  (416)

/*****************************************
* Macro for Application
******************************************/

/*Maximum DRP-AI Timeout threshold*/
#define DRPAI_TIMEOUT               (5)

/*Frame threshold to execute inference in every loop
 *This value must be determined by DRP-AI processing time and capture processing time.
 *For your information TinyYOLOv2 takes around 50 msec and capture takes around 50 msec. */
#define INF_FRAME_NUM               (2)

/*Camera Capture Image Information*/
#define CAM_IMAGE_WIDTH             (640)
#define CAM_IMAGE_HEIGHT            (480)
#define CAM_IMAGE_CHANNEL_YUY2      (2)

/*Camera Capture Information */
#define CAP_BUF_NUM                 (3)

/*DRP-AI Input image information*/
#define DRPAI_IN_WIDTH              (CAM_IMAGE_WIDTH)
#define DRPAI_IN_HEIGHT             (CAM_IMAGE_HEIGHT)
#define DRPAI_IN_CHANNEL_YUY2       (CAM_IMAGE_CHANNEL_YUY2)

/*Wayland Display Image Information*/
#define IMAGE_OUTPUT_WIDTH          (1280)
#define IMAGE_OUTPUT_HEIGHT         (720)
#define IMAGE_CHANNEL_BGRA          (4)
#define WL_BUF_NUM                  (2)

/*udmabuf memory area Information*/
#define UDMABUF_OFFSET              (CAM_IMAGE_WIDTH * CAM_IMAGE_HEIGHT * CAM_IMAGE_CHANNEL_YUY2 * CAP_BUF_NUM)
#define UDMABUF_INFIMAGE_OFFSET     (IMAGE_OUTPUT_WIDTH * IMAGE_OUTPUT_HEIGHT * IMAGE_CHANNEL_BGRA * WL_BUF_NUM + UDMABUF_OFFSET)

/*Image:: Text information to be drawn on image*/
#define CHAR_SCALE_LARGE            (0.8)
#define CHAR_SCALE_SMALL            (0.7)
#define CHAR_THICKNESS              (2)
#define LINE_HEIGHT                 (30) /*in pixel*/
#define LINE_HEIGHT_OFFSET          (20) /*in pixel*/
#define TEXT_WIDTH_OFFSET           (10) /*in pixel*/
#define WHITE_DATA                  (0xFFFFFFu) /* in RGB */
#define BLACK_DATA                  (0x000000u)
/*RESIZE_SCALE=((OUTPUT_WIDTH/IMAGE_WIDTH > OUTPUT_HEIGHT/IMAGE_HEIGHT) ?
        OUTPUT_HEIGHT/IMAGE_HEIGHT : OUTPUT_WIDTH/IMAGE_WIDTH)*/
#define RESIZE_SCALE                (1.5)

/*Waiting Time*/
#define WAIT_TIME                   (1000) /* microseconds */

/*Timer Related*/
#define CAPTURE_TIMEOUT             (20)  /* seconds */
#define AI_THREAD_TIMEOUT           (20)  /* seconds */
#define KEY_THREAD_TIMEOUT          (5)   /* seconds */

/*Buffer size for writing data to memory via DRP-AI Driver.*/
#define BUF_SIZE                    (1024)

/*Index to access drpai_file_path[]*/
#define INDEX_D                     (0)
#define INDEX_C                     (1)
#define INDEX_P                     (2)
#define INDEX_A                     (3)
#define INDEX_W                     (4)

/*****************************************
* For image.cpp
******************************************/
/*For drawing the bounding box label on image*/
#define FONTDATA_WIDTH              (6)
#define FONTDATA_HEIGHT             (8)

/*****************************************
* Typedef
******************************************/
/* For DRP-AI Address List */
typedef struct
{
    unsigned long desc_aimac_addr;
    unsigned long desc_aimac_size;
    unsigned long desc_drp_addr;
    unsigned long desc_drp_size;
    unsigned long drp_param_addr;
    unsigned long drp_param_size;
    unsigned long data_in_addr;
    unsigned long data_in_size;
    unsigned long data_addr;
    unsigned long data_size;
    unsigned long work_addr;
    unsigned long work_size;
    unsigned long data_out_addr;
    unsigned long data_out_size;
    unsigned long drp_config_addr;
    unsigned long drp_config_size;
    unsigned long weight_addr;
    unsigned long weight_size;
} st_addr_t;
#endif
