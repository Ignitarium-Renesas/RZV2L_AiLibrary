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
* Description  : RZ/V2L DRP-AI Sample Application for Resnet18 with TinyYOLOv3 USB Camera version
***********************************************************************************************************************/

#ifndef DEFINE_MACRO_H
#define DEFINE_MACRO_H

/*****************************************
* includes
******************************************/
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <vector>
#include <map>
#include <climits>
#include <fstream>
#include <errno.h>
#include <math.h>
#include <iomanip>
#include <atomic>
#include <semaphore.h>
#include <cstring>
#include <float.h>

/*****************************************
* Macro for ResNet-18
******************************************/
/*Inference Related Parameters*/
const static std::string label_list     = "hagrid_labels.txt";
#define AI0_DESC_NAME               "tinyyolov3_cam"
#define AI_DESC_NAME                "resnet18_cam"

/*Hand Gesture (Resnet18) Related*/
#define INF_OUT_SIZE                (18)
#define MODEL_IN_WIDTH_RESNET         (224)
#define MODEL_IN_HEIGHT_RESNET        (224)
#define NUM_CLASS_RESNET            (18)
/*Index to access drpai_file_path[]*/
#define INDEX_D                   (0)
#define INDEX_C                   (1)
#define INDEX_P                   (2)
#define INDEX_A                   (3)
#define INDEX_W                   (4)

/*CAMERA & ISP Settings Related*/
#define MIPI_WIDTH                (960)
#define MIPI_HEIGHT               (540)
#define MIPI_BUFFER               (8)
#define IMAGE_NUM                 (1)
#define IMREAD_IMAGE_WIDTH        (640)
#define IMREAD_IMAGE_HEIGHT       (480)
#define IMREAD_IMAGE_CHANNEL      (2)
#define IMREAD_IMAGE_SIZE         (IMREAD_IMAGE_WIDTH*IMREAD_IMAGE_HEIGHT*IMREAD_IMAGE_CHANNEL)

/* Size of input image to the model */
#define DISPLAY_WIDTH             (MIPI_WIDTH)
#define DISPLAY_HEIGHT            (MIPI_HEIGHT)
#define DISPLAY_TEXT_SIZE         (256)
#define INF_OUT_SIZE_YOLO         (0x373c8c)

/*Hand Gesture Post Processing & Drawing Related*/
#define OUTPUT_ADJ_X              (2)
#define OUTPUT_ADJ_Y              (0)
#define NUM_MAX_HAND              (7)
#define CROP_ADJ_X                (20)
#define CROP_ADJ_Y                (20)

/*For memory mapping*/
#define MEM_PAGE_SIZE             (4096)

/*****************************************
* Macro for Tiny YOLOv3
******************************************/
/* Thresholds */
#define TH_PROB                 (0.5f)
#define TH_NMS                  (0.5f)
/* Size of input image to the model */
#define MODEL_IN_W              (416)
#define MODEL_IN_H              (416)
/* Number of class to be detected */
#define NUM_CLASS           (1)
/* Number for [yolo] layer num parameter */
#define NUM_BB              (3)
/* Number of output layers. This value MUST match with the length of num_grids[] below */
#define NUM_INF_OUT_LAYER   (2)
/* Number of grids in the image. The length of this array MUST match with the NUM_INF_OUT_LAYER */
const static uint8_t num_grids[] = { 13, 26 };
/* Number of DRP-AI output */
const static uint32_t INF_OUT_SIZE_TINYYOLOV3 =  (NUM_CLASS + 5)* NUM_BB * num_grids[0] * num_grids[0]
                                + (NUM_CLASS + 5)* NUM_BB * num_grids[1] * num_grids[1];
/* Anchor box information */
const static double anchors[] =
{
    10, 14,
    23, 27,
    37, 58,
    81, 82,
    135, 169,
    344, 319
};

/*****************************************
* Macro for Application
******************************************/
/* Coral Camera support */
//#define INPUT_CORAL

/*Maximum DRP-AI Timeout threshold*/
#define DRPAI_TIMEOUT               (5)

/*Frame threshold to execute inference in every loop
 *This value must be determined by DRP-AI processing time and capture processing time.
 *For your information Resnet34 takes around 60 msec and capture takes around 35 msec. */
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
#define IMAGE_OUTPUT_WIDTH          (640)
#define IMAGE_OUTPUT_HEIGHT         (480)
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

/*****************************************
* ERROR CODE
******************************************/
#define SUCCESS                   (0x00)
#define ERR_ADDRESS_CMA           (0x01)
#define ERR_OPEN                  (0x10)
#define ERR_FORMAT                (0x11)
#define ERR_READ                  (0x12)
#define ERR_MMAP                  (0x13)
#define ERR_MLOCK                 (0x14)
#define ERR_MALLOC                (0x15)
#define ERR_GET_TIME              (0x16)
#define ERR_DRPAI_TIMEOUT         (0x20)
#define ERR_DRPAI_START           (0x21)
#define ERR_DRPAI_ASSIGN          (0x22)
#define ERR_DRPAI_WRITE           (0x23)
#define ERR_DRPAI_ASSIGN_PARAM    (0x24)
#define ERR_SYSTEM_ID             (-203)

#endif
