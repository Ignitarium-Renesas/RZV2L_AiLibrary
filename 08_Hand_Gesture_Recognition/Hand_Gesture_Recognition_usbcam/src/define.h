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
* Description  : RZ/V2L DRP-AI Sample Application for MMPose HRNet_v2 MIPI Camera version
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
#include <signal.h>
#include <vector>
#include <map>
#include <fstream>
#include <errno.h>
#include <math.h>
#include <iomanip>
#include <atomic>
#include <semaphore.h>
#include <cstring>

/*****************************************
* Static Variables for MMPose HRNet_v2
* Indices for skeleton to tell which keypoint is connected to which
******************************************/
const static uint8_t skeleton[][2] =
{
    {0,1},
    {1,2},
    {2,3},
    {3,4},
    {0,5},
    {5,6},
    {6,7},
    {7,8},
    {0,9},
    {9,10},
    {10,11},
    {11,12},
    {0,13},
    {13,14},
    {14,15},
    {15,16},
    {0,17},
    {17,18},
    {18,19},
    {19,20}
};

/*****************************************
* Macro for MMPose HRNet_v2
******************************************/
/*Inference Related Parameters*/
#define AI_DESC_NAME                "hand_cam"
#define ML_DESC_NAME                "hand_model.xml"
static std::vector<std::string> label_file_map = {"one","two/scissor","three","four","five/paper","thumbs_down","thumbs_up","rock","blank"};
/*Cropping Image Related*/
#define CROPPED_IMAGE_WIDTH         (270)
#define CROPPED_IMAGE_HEIGHT        (CAM_IMAGE_HEIGHT)
/*HRNet_v2 Related*/
#define NUM_OUTPUT_W                (64)
#define NUM_OUTPUT_H                (64)
#define NUM_OUTPUT_C                (21)
#define INF_OUT_SIZE                (NUM_OUTPUT_W*NUM_OUTPUT_H*NUM_OUTPUT_C)
/*HRNet_v2 Post Processing & Drawing Related*/
#define TH_KPT                      (0.3f)
#define OUTPUT_LEFT                 (185)
#define OUTPUT_TOP                  (0)
#define OUTPUT_WIDTH                (CROPPED_IMAGE_WIDTH)
#define OUTPUT_HEIGHT               (CROPPED_IMAGE_HEIGHT)
#define OUTPUT_ADJ_X                (2)
#define OUTPUT_ADJ_Y                (0)
/*Graphic Drawing Settings Related*/
#define KEY_POINT_SIZE              (2)
#define NUM_LIMB                    (20)

/*****************************************
* Macro for Application
******************************************/
/* Coral Camera support */
// #define INPUT_CORAL

/*Maximum DRP-AI Timeout threshold*/
#define DRPAI_TIMEOUT               (5)

/*Frame threshold to execute inference in every loop
 *This value must be determined by DRP-AI processing time and capture processing time.
 *For your information hrnet_v2 takes around 270 msec and capture takes around 35 msec. */
#define INF_FRAME_NUM               (3)

/*Camera:: Capture Image Information*/
#define CAM_IMAGE_WIDTH             (640)
#define CAM_IMAGE_HEIGHT            (480)
#define CAM_IMAGE_CHANNEL_YUY2      (2)

/*Camera:: Capture Information */
#ifdef INPUT_CORAL
#define CAP_BUF_NUM                 (6)
#else /* INPUT_CORAL */
#define CAP_BUF_NUM                 (3)
#endif /* INPUT_CORAL */

/*DRP-AI Input image information*/
#define DRPAI_IN_WIDTH              (CAM_IMAGE_WIDTH)
#define DRPAI_IN_HEIGHT             (CAM_IMAGE_HEIGHT)
#define DRPAI_IN_CHANNEL_YUY2       (CAM_IMAGE_CHANNEL_YUY2)

/*Wayland:: Wayland Information */
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
#define YELLOW_DATA                 (0xFF00FFu) /* in RGB */
#define BLUE_DATA                   (0x00FF00u) /* in RGB */
#define RED_DATA                    (0x0000FFu) /* in RGB */
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
#define DISPLAY_THREAD_TIMEOUT      (20)  /* seconds */
#define KEY_THREAD_TIMEOUT          (5)   /* seconds */
#define TIME_COEF                   (1)

/*Buffer size for writing data to memory via DRP-AI Driver.*/
#define BUF_SIZE                    (1024)

/*Index to access drpai_file_path[]*/
#define INDEX_D                     (0)
#define INDEX_C                     (1)
#define INDEX_P                     (2)
#define INDEX_A                     (3)
#define INDEX_W                     (4)

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
