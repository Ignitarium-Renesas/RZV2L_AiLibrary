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
* Description  : RZ/V2L DRP-AI Sample Application for ArcFace USB Camera version
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
#include <atomic>
#include <semaphore.h>
#include <limits>                                                                                                
#include <cmath>

/*****************************************
* For database 
******************************************/
#define DATA_DIR "../../database/"

/*****************************************
* Static Variables for ArcFace
* Following variables need to be changed in order to custormize the AI model
*  - feature_list = embeddings of known people
*  - drpai_prefix = directory name of DRP-AI Object files (DRP-AI Translator output)
*  - input_img = input image to DRP-AI (size and format are determined in DRP-AI Translator)
******************************************/
const static std::string feature_list     = (std::string)DATA_DIR+"face_features.csv";
const static std::string drpai_prefix   = "arcface_cam";

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
    drpai_prefix+"/"+drpai_prefix+"_weight.dat",
};
/*****************************************
* Macro for ArcFace
******************************************/
/*Number of features in the embedding*/                  
#define NUM_FEATURES            (512)              

/*****************************************
* Macro for Application
******************************************/
//#define INPUT_CORAL
/*Camera:: Capture Information */
#ifdef INPUT_CORAL
#define CAP_BUF_NUM                 (6)
#else /* INPUT_CORAL */
#define CAP_BUF_NUM                 (3)
#endif /* INPUT_CORAL */

/*Maximum DRP-AI Timeout threshold*/
#define DRPAI_TIMEOUT               (5)

/*Minimum Distance for feature comparision*/
#define MIN_DIST_TH		(30.0)

/*Camera Capture Image Information*/
#define CAM_IMAGE_WIDTH         (640)
#define CAM_IMAGE_HEIGHT        (480)
#define CAM_IMAGE_CHANNEL_YUY2  (2)

/*DRP-AI Input image information*/
#define DRPAI_IN_WIDTH              (CAM_IMAGE_WIDTH)
#define DRPAI_IN_HEIGHT             (CAM_IMAGE_HEIGHT)
#define DRPAI_IN_CHANNEL_YUY2       (CAM_IMAGE_CHANNEL_YUY2)

/*Cropping image related*/
#define CROPPED_IMAGE_WIDTH         (112)
#define CROPPED_IMAGE_HEIGHT        (112)
#define OUTPUT_LEFT                 (208)
#define OUTPUT_TOP                  (128)

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
#define WHITE_DATA                  (0xFFFFFF) /* in RGB */
#define YELLOW_DATA                 (0xFF00FFu) /* in RGB */
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
