/***********************************************************************************************************************
* DISCLAIMER
* This software is sufaceied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* afaceicable laws, including copyright laws.
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
* File Name    : sample_app_deeppose_pre-tinyyolov2_cam.cpp
* Version      : 7.20
* Description  : RZ/V2L DRP-AI Driver Monitoring Sample application using DeepPose with TinyYOLOv2 USB Camera version
***********************************************************************************************************************/

/*****************************************
* Includes
******************************************/
/*DRPAI Driver Header*/
#include <linux/drpai.h>
/*Definition of Macros & other variables*/
#include "define.h"
/*MIPI camera control*/
#include "camera.h"
/*Image control*/
#include "image.h"
/*Wayland control*/
#include "wayland.h"
/*TinyYOLOv2 Post-Processing*/
#include "box.h"

/*****************************************
* Global Variables
******************************************/
/*Multithreading*/
static sem_t terminate_req_sem;
static pthread_t ai_inf_thread;
static pthread_t kbhit_thread;
static pthread_t capture_thread;
static pthread_t hdmi_thread;

/*Flags*/
static std::atomic<uint8_t> inference_start (0);
static std::atomic<uint8_t> img_obj_ready   (0);

/*Global Variables*/
static float drpai_output_buf[INF_OUT_SIZE];
static float drpai_output_buf0[INF_OUT_SIZE_TINYYOLOV2];
static uint32_t capture_address;
static uint64_t udmabuf_address = 0;
static Image img;
uint8_t yawn_flag=0;
uint8_t blink_flag=0;

/*AI Inference for DRPAI*/
static std::string prefix;
static st_addr_t drpai_address;
static st_addr_t drpai_address0;
static std::string dir;
static std::string address_file;
static int8_t drpai_fd = -1;
static int8_t drpai_fd0 = -1;
static uint32_t ai_time = 0;
static uint32_t total_time = 0;
static uint32_t yolo_time = 0;
static uint32_t deeppose_time[NUM_MAX_FACE];
static uint32_t deeppose_preds[NUM_OUTPUT_KEYPOINT][2];
static uint16_t id_x[NUM_OUTPUT_KEYPOINT][NUM_MAX_FACE];
static uint16_t id_y[NUM_OUTPUT_KEYPOINT][NUM_MAX_FACE];
static uint16_t id_x_local[NUM_OUTPUT_KEYPOINT][NUM_MAX_FACE]; /*To be used only in Inference Threads*/
static uint16_t id_y_local[NUM_OUTPUT_KEYPOINT][NUM_MAX_FACE]; /*To be used only in Inference Threads*/

static int16_t cropx[NUM_MAX_FACE];
static int16_t cropy[NUM_MAX_FACE];
static int16_t croph[NUM_MAX_FACE];
static int16_t cropw[NUM_MAX_FACE];

/*TinyYOLOv2*/
static uint32_t bcount = 0;
static uint32_t face_count_local = 0; /*To be used only in Inference Threads*/
static uint32_t face_count = 0;
static std::vector<detection> det_res;
static std::vector<detection> det_face;

static Wayland wayland;

/*ML model inferencing*/
static cv::Ptr<cv::ml::RTrees> tree = cv::ml::RTrees::create();
static cv::Ptr<cv::ml::RTrees> dtree = tree->load(ML_DESC_NAME);
static std::string random_forest_preds[NUM_MAX_FACE];

/*****************************************
* Function Name : timedifference_msec
* Description   : compute the time differences in ms between two moments
* Arguments     : t0 = start time
*                 t1 = stop time
* Return value  : the time difference in ms
******************************************/
static double timedifference_msec(struct timespec t0, struct timespec t1)
{
    return (t1.tv_sec - t0.tv_sec) * 1000.0 + (t1.tv_nsec - t0.tv_nsec) / 1000000.0;
}

/*****************************************
* Function Name : wait_join
* Description   : waits for a fixed amount of time for the thread to exit
* Arguments     : p_join_thread = thread that the function waits for to Exit
*                 join_time = the timeout time for the thread for exiting
* Return value  : 0 if successful
*                 not 0 otherwise
******************************************/
static int8_t wait_join(pthread_t *p_join_thread, uint32_t join_time)
{
    int8_t ret_err;
    struct timespec join_timeout;
    ret_err = clock_gettime(CLOCK_REALTIME, &join_timeout);
    if ( 0 == ret_err )
    {
        join_timeout.tv_sec += join_time;
        ret_err = pthread_timedjoin_np(*p_join_thread, NULL, &join_timeout);
    }
    return ret_err;
}

/*****************************************
* Function Name : sigmoid
* Description   : helper function for YOLO Post Processing
* Arguments     : x = input argument for the calculation
* Return value  : sigmoid result of input x
******************************************/
static double sigmoid(double x)
{
    return 1.0/(1.0+exp(-x));
}

/*****************************************
* Function Name : get_result
* Description   : Get DRP-AI Output from memory via DRP-AI Driver
* Arguments     : drpai_fd = file descriptor of DRP-AI Driver
*                 output_addr = memory start address of DRP-AI output
*                 output_size = output data size
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
static int8_t get_result(int8_t drpai_fd, uint32_t output_addr, uint32_t output_size)
{
    drpai_data_t drpai_data;
    float drpai_buf[BUF_SIZE];
    drpai_data.address = output_addr;
    drpai_data.size = output_size;
    int32_t i = 0;
    int8_t ret = 0;

    errno = 0;
    /* Assign the memory address and size to be read */
    ret = ioctl(drpai_fd, DRPAI_ASSIGN, &drpai_data);
    if (-1 == ret)
    {
        fprintf(stderr, "[ERROR] Failed to run DRPAI_ASSIGN: errno=%d\n", errno);
        return -1;
    }

    /* Read the memory via DRP-AI Driver and store the output to buffer */
    for (i = 0; i < (drpai_data.size/BUF_SIZE); i++)
    {
        errno = 0;
        ret = read(drpai_fd, drpai_buf, BUF_SIZE);
        if ( -1 == ret )
        {
            fprintf(stderr, "[ERROR] Failed to read via DRP-AI Driver: errno=%d\n", errno);
            return -1;
        }

        memcpy(&drpai_output_buf0[BUF_SIZE/sizeof(float)*i], drpai_buf, BUF_SIZE);
    }

    if ( 0 != (drpai_data.size % BUF_SIZE))
    {
        errno = 0;
        ret = read(drpai_fd, drpai_buf, (drpai_data.size % BUF_SIZE));
        if ( -1 == ret)
        {
            fprintf(stderr, "[ERROR] Failed to read via DRP-AI Driver: errno=%d\n", errno);
            return -1;
        }

        memcpy(&drpai_output_buf0[(drpai_data.size - (drpai_data.size%BUF_SIZE))/sizeof(float)], drpai_buf, (drpai_data.size % BUF_SIZE));
    }
    return 0;
}
/*****************************************
* Function Name : softmax
* Description   : Helper function for YOLO Post Processing
* Arguments     : val[] = array to be computed softmax
* Return value  : -
******************************************/
static void softmax(float val[NUM_CLASS])
{
    float max_num = -FLT_MAX;
    float sum = 0;
    int32_t i;
    for ( i = 0 ; i<NUM_CLASS ; i++ )
    {
        max_num = std::max(max_num, val[i]);
    }

    for ( i = 0 ; i<NUM_CLASS ; i++ )
    {
        val[i]= (float) exp(val[i] - max_num);
        sum+= val[i];
    }

    for ( i = 0 ; i<NUM_CLASS ; i++ )
    {
        val[i]= val[i]/sum;
    }
    return;
}

/*****************************************
* Function Name : read_addrmap_txt
* Description   : Loads address and size of DRP-AI Object files into struct addr.
* Arguments     : addr_file = filename of addressmap file (from DRP-AI Object files)
*                 address_drp = drp address map to fill
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
static int8_t read_addrmap_txt(std::string addr_file, st_addr_t* address_drp)
{
    std::string str;
    uint32_t l_addr;
    uint32_t l_size;
    std::string element, a, s;

    std::ifstream ifs(addr_file);
    if (ifs.fail())
    {
        fprintf(stderr, "[ERROR] Failed to open address map list : %s\n", addr_file.c_str());
        return -1;
    }

    while (getline(ifs, str))
    {
        std::istringstream iss(str);
        iss >> element >> a >> s;
        l_addr = strtol(a.c_str(), NULL, 16);
        l_size = strtol(s.c_str(), NULL, 16);

        if ("drp_config" == element)
        {
            address_drp->drp_config_addr = l_addr;
            address_drp->drp_config_size = l_size;
        }
        else if ("desc_aimac" == element)
        {
            address_drp->desc_aimac_addr = l_addr;
            address_drp->desc_aimac_size = l_size;
        }
        else if ("desc_drp" == element)
        {
            address_drp->desc_drp_addr = l_addr;
            address_drp->desc_drp_size = l_size;
        }
        else if ("drp_param" == element)
        {
            address_drp->drp_param_addr = l_addr;
            address_drp->drp_param_size = l_size;
        }
        else if ("weight" == element)
        {
            address_drp->weight_addr = l_addr;
            address_drp->weight_size = l_size;
        }
        else if ("data_in" == element)
        {
            address_drp->data_in_addr = l_addr;
            address_drp->data_in_size = l_size;
        }
        else if ("data" == element)
        {
            address_drp->data_addr = l_addr;
            address_drp->data_size = l_size;
        }
        else if ("data_out" == element)
        {
            address_drp->data_out_addr = l_addr;
            address_drp->data_out_size = l_size;
        }
        else if ("work" == element)
        {
            address_drp->work_addr = l_addr;
            address_drp->work_size = l_size;
        }
        else
        {
            /*Ignore other space*/
        }
    }

    return 0;
}

/*****************************************
* Function Name : load_data_to_mem
* Description   : Loads a file to memory via DRP-AI Driver
* Arguments     : data = filename to be written to memory
*                 drpai_fd = file descriptor of DRP-AI Driver
*                 from = memory start address where the data is written
*                 size = data size to be written
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
static int8_t load_data_to_mem(std::string data, int8_t drpai_fd, uint32_t from, uint32_t size)
{
    int8_t ret_load_data = 0;
    int8_t obj_fd;
    drpai_data_t drpai_data;
    size_t ret = 0;
    int32_t i = 0;
    uint8_t drpai_buf[BUF_SIZE];

    printf("Loading : %s\n", data.c_str());
    errno = 0;
    obj_fd = open(data.c_str(), O_RDONLY);
    if (0 > obj_fd)
    {
        fprintf(stderr, "[ERROR] Failed to open: %s errno=%d\n", data.c_str(), errno);
        ret_load_data = -1;
        goto end;
    }

    drpai_data.address = from;
    drpai_data.size = size;

    errno = 0;
    ret = ioctl(drpai_fd, DRPAI_ASSIGN, &drpai_data);
    if ( -1 == ret )
    {
        fprintf(stderr, "[ERROR] Failed to run DRPAI_ASSIGN: errno=%d\n", errno);
        ret_load_data = -1;
        goto end;
    }

    for (i = 0 ; i<(drpai_data.size/BUF_SIZE) ; i++)
    {
        errno = 0;
        ret = read(obj_fd, drpai_buf, BUF_SIZE);
        if (0 > ret)
        {
            fprintf(stderr, "[ERROR] Failed to read: %s errno=%d\n", data.c_str(), errno);
            ret_load_data = -1;
            goto end;
        }
        ret = write(drpai_fd, drpai_buf, BUF_SIZE);
        if ( -1 == ret )
        {
            fprintf(stderr, "[ERROR] Failed to write via DRP-AI Driver: errno=%d\n", errno);
            ret_load_data = -1;
            goto end;
        }
    }
    if ( 0 != (drpai_data.size % BUF_SIZE))
    {
        errno = 0;
        ret = read(obj_fd, drpai_buf, (drpai_data.size % BUF_SIZE));
        if ( 0 > ret )
        {
            fprintf(stderr, "[ERROR] Failed to read: %s errno=%d\n", data.c_str(), errno);
            ret_load_data = -1;
            goto end;
        }
        ret = write(drpai_fd, drpai_buf, (drpai_data.size % BUF_SIZE));
        if (-1 == ret)
        {
            fprintf(stderr, "[ERROR] Failed to write via DRP-AI Driver: errno=%d\n", errno);
            ret_load_data = -1;
            goto end;
        }
    }
    goto end;

end:
    if (0 < obj_fd)
    {
        close(obj_fd);
    }
    return ret_load_data;
}

/*****************************************
* Function Name : load_drpai_data
* Description   : Loads DRP-AI Object files to memory via DRP-AI Driver.
* Arguments     : drpai_fd = file descriptor of DRP-AI Driver
*                 address_drp = drp address map
* Return value  : 0 if succeeded
*               : not 0 otherwise
******************************************/
static int8_t load_drpai_data(int8_t drpai_fd, st_addr_t address_drp)
{
    uint32_t addr = 0;
    uint32_t size = 0;
    uint8_t i = 0;
    uint8_t ret = 0;
    std::string drpai_file_path[5] =
    {
        prefix+"/drp_desc.bin",
        prefix+"/"+prefix+"_drpcfg.mem",
        prefix+"/drp_param.bin",
        prefix+"/aimac_desc.bin",
        prefix+"/"+prefix+"_weight.dat",
    };

    for (i = 0; i < 5; i++)
    {
        switch (i)
        {
            case (INDEX_W):
                addr = address_drp.weight_addr;
                size = address_drp.weight_size;
                break;
            case (INDEX_C):
                addr = address_drp.drp_config_addr;
                size = address_drp.drp_config_size;
                break;
            case (INDEX_P):
                addr = address_drp.drp_param_addr;
                size = address_drp.drp_param_size;
                break;
            case (INDEX_A):
                addr = address_drp.desc_aimac_addr;
                size = address_drp.desc_aimac_size;
                break;
            case (INDEX_D):
                addr = address_drp.desc_drp_addr;
                size = address_drp.desc_drp_size;
                break;
            default:
                break;
        }

        ret = load_data_to_mem(drpai_file_path[i], drpai_fd, addr, size);
        if (0 != ret)
        {
            fprintf(stderr,"[ERROR] Failed to load data from memory: %s\n",drpai_file_path[i].c_str());
            return -1;
        }
    }
    return 0;
}

/*****************************************
* Function Name : index
* Description   : Get the index of the bounding box attributes based on the input offset
* Arguments     : offs = offset to access the bounding box attributes
*                 channel = channel to access each bounding box attribute.
* Return value  : index to access the bounding box attribute.
******************************************/
static int32_t index(int32_t offs, int32_t channel)
{
    return offs + channel * NUM_GRID_X * NUM_GRID_Y;
}

/*****************************************
* Function Name : offset_yolo
* Description   : Get the offset number to access the bounding box attributes
*                 To get the actual value of bounding box attributes, use index() after this function.
* Arguments     : b = Number to indicate which bounding box in the region [0~4]
*                 y = Number to indicate which region [0~13]
*                 x = Number to indicate which region [0~13]
* Return value  : offset to access the bounding box attributes.
*******************************************/
static int offset_yolo(int b, int y, int x)
{
    return b *(NUM_CLASS + 5)* NUM_GRID_X * NUM_GRID_Y + y * NUM_GRID_X + x;
}

/*****************************************
* Function Name : R_Post_Proc
* Description   : Process CPU post-processing for TinyYOLOv2
* Arguments     : address = drpai output address
*                 det = detected boxes details
*                 box_count = total number of boxes
* Return value  : -
******************************************/
static void R_Post_Proc(float* floatarr, std::vector<detection>& det, uint32_t* box_count)
{
    uint32_t count = 0;
    /*Memory Access*/
    /* Following variables are required for correct_yolo_boxes in Darknet implementation*/
    /* Note: This implementation refers to the "darknet detector test" */
    float new_w, new_h;
    float correct_w = 1.;
    float correct_h = 1.;
    if ((float) (MODEL_IN_W / correct_w) < (float) (MODEL_IN_H/correct_h) )
    {
        new_w = (float) MODEL_IN_W;
        new_h = correct_h * MODEL_IN_W / correct_w;
    }
    else
    {
        new_w = correct_w * MODEL_IN_H / correct_h;
        new_h = MODEL_IN_H;
    }

    int32_t b = 0;
    int32_t y = 0;
    int32_t x = 0;
    int32_t offs = 0;
    int32_t i = 0;
    float tx = 0;
    float ty = 0;
    float tw = 0;
    float th = 0;
    float tc = 0;
    float center_x = 0;
    float center_y = 0;
    float box_w = 0;
    float box_h = 0;
    float objectness = 0;
    Box bb;
    float classes[NUM_CLASS];
    float max_pred = 0;
    int32_t pred_class = -1;
    float max_bbox_area = 0;

    float probability = 0;
    detection d;
    /* Clear the detected result list */
    det.clear();

    /*Post Processing Start*/
    for(b = 0; b < NUM_BB; b++)
    {
        for(y = 0; y < NUM_GRID_Y; y++)
        {
            for(x = 0; x < NUM_GRID_X; x++)
            {
                offs = offset_yolo(b, y, x);
                tx = floatarr[offs];
                ty = floatarr[index(offs, 1)];
                tw = floatarr[index(offs, 2)];
                th = floatarr[index(offs, 3)];
                tc = floatarr[index(offs, 4)];

                /* Compute the bounding box */
                /*get_region_box*/
                center_x = ((float) x + sigmoid(tx)) / (float) NUM_GRID_X;
                center_y = ((float) y + sigmoid(ty)) / (float) NUM_GRID_Y;
                box_w = (float) exp(tw) * anchors[2*b+0] / (float) NUM_GRID_X;
                box_h = (float) exp(th) * anchors[2*b+1] / (float) NUM_GRID_Y;

                /* Adjustment for VGA size */
                /* correct_region_boxes */
                center_x = (center_x - (MODEL_IN_W - new_w) / 2. / MODEL_IN_W) / ((float) new_w / MODEL_IN_W);
                center_y = (center_y - (MODEL_IN_H - new_h) / 2. / MODEL_IN_H) / ((float) new_h / MODEL_IN_H);
                box_w *= (float) (MODEL_IN_W / new_w);
                box_h *= (float) (MODEL_IN_H / new_h);

                center_x = round(center_x * DRPAI_IN_WIDTH);
                center_y = round(center_y * DRPAI_IN_HEIGHT);
                box_w = round(box_w * DRPAI_IN_WIDTH);
                box_h = round(box_h * DRPAI_IN_HEIGHT);

                objectness = sigmoid(tc);

                bb = {center_x, center_y, box_w, box_h};
                float bbox_area = 0;
                bbox_area = box_w * box_h;
                /* Get the class prediction */
                for (i = 0; i < NUM_CLASS; i++)
                {
                    classes[i] = floatarr[index(offs, 5+i)];
                }
                softmax(classes);
                max_pred = 0;
                pred_class = -1;
                for (i = 0; i < NUM_CLASS; i++)
                {
                    if (classes[i] > max_pred)
                    {
                        pred_class = i;
                        max_pred = classes[i];
                    }
                }

                /* Store the result into the list if the probability is more than the threshold */
                probability = max_pred * objectness;
                if ((probability > TH_PROB))    //face = 14
                {
                    d = {bb, pred_class, probability};
                    det.push_back(d);
                    count++;
                }
            }
        }
    }
    /* Non-Maximum Supression filter */
    filter_boxes_nms(det, det.size(), TH_NMS);
    *box_count = count;

    return ;
}


/*****************************************
* Function Name : face_counter
* Description   : Function to count the real number of face detected and does not exceeds the maximum number
* Arguments     : det = detected boxes details
*                 face = detected face details
*                 box_count = total number of boxes
*                 face_count = actual number of face
* Return value  : -
******************************************/
static void face_counter(std::vector<detection>& det, std::vector<detection>& face, uint32_t box_count, uint32_t* face_count)
{
    uint32_t count = 0;
    face.clear();
    for(uint32_t i = 0; i<box_count; i++)
    {
        if(0 == det[i].prob)
        {
            continue;
        }
        else
        {
            face.push_back(det[i]);
            count++;
            if(count > NUM_MAX_FACE-1)
            {
                break;
            }
        }
    }
    *face_count = count;
}

/*****************************************
* Function Name : R_Inf_RandomForest
* Description   : CPU inference for Random Forest
* Arguments     : floatarr = drpai output address
*                 n_pers = number of the face detected
* Return value  : -
******************************************/
static void R_Inf_RandomForest(float* floatarr, uint8_t n_pers)
{
    cv::Mat X = cv::Mat(1, INF_OUT_SIZE, CV_32F, floatarr);
    cv::Mat preds = {};
    dtree->predict(X, preds);
    random_forest_preds[n_pers] = label_list[((int)(preds.at<float>(0)) - 1)];
    return;
}


/*****************************************
* Function Name : R_Post_Proc_DeepPose
* Description   : CPU post-processing for DeepPose
* Arguments     : floatarr = drpai output address
*                 n_pers = number of the face detected
* Return value  : -
******************************************/
static void R_Post_Proc_DeepPose(float* floatarr, uint8_t n_pers)
{
    float scale[] = {(float)(cropw[n_pers]), (float)(croph[n_pers])};
    for (int i = 0; i < NUM_OUTPUT_KEYPOINT; i++)
    {
        deeppose_preds[i][0] = (int32_t)(floatarr[2*i] * scale[0]);
        deeppose_preds[i][1] = (int32_t)(floatarr[2*i+1] * scale[1]);
    }
    return;
}

/*****************************************
* Function Name :  load_drpai_param_file
* Description   : Loads DRP-AI Parameter File to memory via DRP-AI Driver.
* Arguments     : drpai_fd = file descriptor of DRP-AI Driver
*                 proc = drpai data
*                 param_file = drpai parameter file to load
*                 file_size = drpai parameter file size
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
static uint8_t load_drpai_param_file(int drpai_fd, drpai_data_t proc, std::string param_file, uint32_t file_size)
{
    uint8_t ret_load_para = 0;
    int obj_fd = -1;
    uint8_t drpai_buf[BUF_SIZE];
    
    drpai_assign_param_t crop_assign;
    crop_assign.info_size = file_size;
    crop_assign.obj.address = proc.address;
    crop_assign.obj.size = proc.size;
    if (ioctl(drpai_fd, DRPAI_ASSIGN_PARAM, &crop_assign)!=0)
    {
        printf("[ERROR] DRPAI Assign Parameter Failed:%d\n", errno);
        return -1;
    }
    obj_fd = open(param_file.c_str(), O_RDONLY);
    if (obj_fd < 0)
    {
        ret_load_para = ERR_OPEN;
        goto end;
    }
    for(uint32_t i = 0 ; i<(file_size/BUF_SIZE) ; i++)
    {
        if( 0 > read(obj_fd, drpai_buf, BUF_SIZE))
        {
            ret_load_para = ERR_READ;
            goto end;
        }
        if ( 0 > write(drpai_fd, drpai_buf, BUF_SIZE))
        {
            printf("[ERROR] DRPAI Write Failed:%d\n", errno);
            return -1;
        }
    }
    if ( 0 != (file_size%BUF_SIZE))
    {
        if( 0 > read(obj_fd, drpai_buf, (file_size % BUF_SIZE)))
        {
            ret_load_para = ERR_READ;
            goto end;
        }
        if( 0 > write(drpai_fd, drpai_buf, (file_size % BUF_SIZE)))
        {
            printf("[ERROR] DRPAI Write Failed:%d\n", errno);
            return -1;
        }
    }
end:
    if(obj_fd >= 0)
    {
        close(obj_fd);
    }
    return ret_load_para;
}

/*****************************************
* Function Name : R_DeepPose_Coord_Convert
* Description   : Convert the post processing result into drawable coordinates
* Arguments     : n_pers = number of the detected face
* Return value  : -
******************************************/
static void R_DeepPose_Coord_Convert(uint8_t n_pers)
{
    /* Render skeleton on image and print their details */
    int32_t posx;
    int32_t posy;
    int8_t i;

    for (i = 0; i < NUM_OUTPUT_KEYPOINT; i++)
    {
        /* Conversion from input image coordinates to display image coordinates. */
        /* +0.5 is for rounding.*/
        posx = (int32_t)(deeppose_preds[i][0] + 0.5) + cropx[n_pers] + OUTPUT_ADJ_X;
        posy = (int32_t)(deeppose_preds[i][1] + 0.5) + cropy[n_pers] + OUTPUT_ADJ_Y;
        /* Make sure the coordinates are not off the screen. */
        posx = (posx < 0) ? 0 : posx;
        posx = (posx > IMREAD_IMAGE_WIDTH - KEY_POINT_SIZE -1 ) ? IMREAD_IMAGE_WIDTH -KEY_POINT_SIZE -1 : posx;
        posy = (posy < 0) ? 0 : posy;
        posy = (posy > IMREAD_IMAGE_HEIGHT -KEY_POINT_SIZE -1) ? IMREAD_IMAGE_HEIGHT -KEY_POINT_SIZE -1 : posy;
        id_x_local[i][n_pers] = posx;
        id_y_local[i][n_pers] = posy;
    }
    return;
}

static double calculate_euclidean_distance(int x1, int x2, int y1, int y2)
{
    double x = x1 - x2; // calculating number to square in next step
    double y = y1 - y2;
    double euclidean_distance;

    euclidean_distance = pow(x, 2) + pow(y, 2); // calculating Euclidean distance
    euclidean_distance = sqrt(euclidean_distance);
    return euclidean_distance;
}



/*****************************************
* Function Name : draw_skeleton
* Description   : Draw Complete Skeleton on image.
* Arguments     : -
* Return value  : -
******************************************/
static void draw_skeleton(void)
{
    int32_t sk_id;
    uint8_t v;
    uint8_t i;

    double lips_width;
    double lips_height;
    
    double left_eye_height;
    double right_eye_height;
    
    if(face_count > 0)
    {
        for(i=0; i < face_count; i++)
        {    
                /*Draw Rectangle As Key Points*/
                for(v = 0; v < NUM_OUTPUT_KEYPOINT; v++)
                // {
                //     /*Draw Rectangles On Each Skeleton Key Points*/
                //     img.draw_rect(id_x[v][i], id_y[v][i], KEY_POINT_SIZE, KEY_POINT_SIZE, RED_DATA);
                // }
                
                // img.draw_line2(id_x[76][i], id_y[76][i],id_x[82][i], id_y[82][i], YELLOW_DATA); // lips horizontal
                // img.draw_line2(id_x[79][i], id_y[79][i],id_x[85][i], id_y[85][i], YELLOW_DATA); //lips vertical
                
                lips_width = calculate_euclidean_distance(id_x[76][i], id_x[82][i], id_y[76][i], id_y[82][i]);
                lips_height = calculate_euclidean_distance(id_x[79][i], id_x[85][i], id_y[79][i], id_y[85][i]);
                                
                if(lips_width/lips_height < 1.1)
                {
                   yawn_flag=2;
                }
                else 
                {
                  yawn_flag=1;
                }
                left_eye_height = calculate_euclidean_distance(id_x[62][i], id_x[66][i], id_y[62][i], id_y[66][i]);
                right_eye_height = calculate_euclidean_distance(id_x[70][i], id_x[74][i], id_y[70][i], id_y[74][i]);
                if((left_eye_height < 5.0) && (right_eye_height <5.0))
                {
                   blink_flag=2;
                }
                else
                {
                   blink_flag=1;
                }
                
        }
    }
    return;
}



/*****************************************
* Function Name : print_result
* Description   : print the result on display.
* Arguments     : -
* Return value  : 0 if succeeded
*               not 0 otherwise
******************************************/
static int8_t print_result(Image* img)
{
    int32_t i = 0;
    int32_t result_cnt = 0;
    std::stringstream stream;
    std::string str0 = "";
    std::string str1 = "";
    std::string str2 = "";
    std::string str3 = "";
    std::string str4 = "";
    uint32_t ai_inf_prev = 0;
    uint32_t yolotime_prev = 0;
    /* Draw Total Inference Time Result on RGB image.*/
    if (ai_inf_prev != (uint32_t) total_time)
    {
        ai_inf_prev = (uint32_t) total_time;
        stream.str("");
        stream << "Total Inference Time: " <<std::setw(3) << ai_inf_prev << " msec";
        str0 = stream.str();
    }
    img->write_string_rgb(str0, TEXT_WIDTH_OFFSET, LINE_HEIGHT, CHAR_SCALE_SMALL, WHITE_DATA);
    // img->write_string_rgb(str1, TEXT_WIDTH_OFFSET, LINE_HEIGHT*2, CHAR_SCALE_SMALL, WHITE_DATA);

    if( face_count > 0 )
    {

        if(yawn_flag==2){
                 stream.str("");
                 stream << "Yawn Detected";
                 str3 = stream.str();
                 img->write_string_rgb(str3, TEXT_WIDTH_OFFSET, LINE_HEIGHT*4, CHAR_SCALE_SMALL, WHITE_DATA);
            }      
        if(blink_flag==2){
                 stream.str("");
                 stream << "Blink Detected";
                 str4 = stream.str();
                 img->write_string_rgb(str4, TEXT_WIDTH_OFFSET, LINE_HEIGHT*5, CHAR_SCALE_SMALL, WHITE_DATA);
            }
        for(i=0; i<face_count; i++) 
        {
            stream.str("");
            stream << "Head Pose " << i+1 << ": " << std::setw(3) << random_forest_preds[i];
            str0 = stream.str();
            img->write_string_rgb(str0, TEXT_WIDTH_OFFSET, LINE_HEIGHT*(2+i), CHAR_SCALE_SMALL, WHITE_DATA);
        }
    }

    return 0;
}
/*****************************************
* Function Name : R_Inf_Thread
* Description   : Executes the DRP-AI inference thread
* Arguments     : threadid = thread identification
* Return value  : -
******************************************/
void *R_Inf_Thread(void *threadid)
{
    /*Semaphore Variable*/
    int32_t inf_sem_check = 0;
    /*Variable for getting Inference output data*/
    drpai_data_t drpai_data;
    drpai_data_t drpai_data0;
    /*Inference Variables*/
    fd_set rfds;
    fd_set rfds0;
    struct timespec tv;
    int8_t inf_status = 0;
    drpai_data_t proc[DRPAI_INDEX_NUM];
    drpai_data_t proc0[DRPAI_INDEX_NUM];
    drpai_status_t drpai_status;
    /*Variable for checking return value*/
    int8_t ret = 0;
    /*Variable for Performance Measurement*/
    static struct timespec yolo_start_time;
    static struct timespec start_time;
    static struct timespec inf_end_time;
    /*DeepPose Modify Parameters*/
    drpai_crop_t crop_param;
    static std::string drpai_param_file;
    uint32_t drp_param_info_size;
    uint8_t i;
    uint32_t yolo_time_tmp = 0;

    printf("Inference Thread Starting\n");
    /*DeepPose*/
    proc[DRPAI_INDEX_INPUT].address = drpai_address.data_in_addr;
    proc[DRPAI_INDEX_INPUT].size = drpai_address.data_in_size;
    proc[DRPAI_INDEX_DRP_CFG].address = drpai_address.drp_config_addr;
    proc[DRPAI_INDEX_DRP_CFG].size = drpai_address.drp_config_size;
    proc[DRPAI_INDEX_DRP_PARAM].address = drpai_address.drp_param_addr;
    proc[DRPAI_INDEX_DRP_PARAM].size = drpai_address.drp_param_size;
    proc[DRPAI_INDEX_AIMAC_DESC].address = drpai_address.desc_aimac_addr;
    proc[DRPAI_INDEX_AIMAC_DESC].size = drpai_address.desc_aimac_size;
    proc[DRPAI_INDEX_DRP_DESC].address = drpai_address.desc_drp_addr;
    proc[DRPAI_INDEX_DRP_DESC].size = drpai_address.desc_drp_size;
    proc[DRPAI_INDEX_WEIGHT].address = drpai_address.weight_addr;
    proc[DRPAI_INDEX_WEIGHT].size = drpai_address.weight_size;
    proc[DRPAI_INDEX_OUTPUT].address = drpai_address.data_out_addr;
    proc[DRPAI_INDEX_OUTPUT].size = drpai_address.data_out_size;
    /*TinyYOLOv2*/
    proc0[DRPAI_INDEX_INPUT].address = drpai_address0.data_in_addr;
    proc0[DRPAI_INDEX_INPUT].size = drpai_address0.data_in_size;
    proc0[DRPAI_INDEX_DRP_CFG].address = drpai_address0.drp_config_addr;
    proc0[DRPAI_INDEX_DRP_CFG].size = drpai_address0.drp_config_size;
    proc0[DRPAI_INDEX_DRP_PARAM].address = drpai_address0.drp_param_addr;
    proc0[DRPAI_INDEX_DRP_PARAM].size = drpai_address0.drp_param_size;
    proc0[DRPAI_INDEX_AIMAC_DESC].address = drpai_address0.desc_aimac_addr;
    proc0[DRPAI_INDEX_AIMAC_DESC].size = drpai_address0.desc_aimac_size;
    proc0[DRPAI_INDEX_DRP_DESC].address = drpai_address0.desc_drp_addr;
    proc0[DRPAI_INDEX_DRP_DESC].size = drpai_address0.desc_drp_size;
    proc0[DRPAI_INDEX_WEIGHT].address = drpai_address0.weight_addr;
    proc0[DRPAI_INDEX_WEIGHT].size = drpai_address0.weight_size;
    proc0[DRPAI_INDEX_OUTPUT].address = drpai_address0.data_out_addr;
    proc0[DRPAI_INDEX_OUTPUT].size = drpai_address0.data_out_size;

    /*DRP-AI Output Memory Preparation*/
    /*DeepPose*/
    drpai_data.address = drpai_address.data_out_addr;
    drpai_data.size = drpai_address.data_out_size;
    /*TinyYOLOv2*/
    drpai_data0.address = drpai_address0.data_out_addr;
    drpai_data0.size = drpai_address0.data_out_size;

    /*DRP Param Info Preparation*/
    prefix = AI_DESC_NAME;
    dir = prefix + "/";
    drpai_param_file = dir + "drp_param_info.txt";
    std::ifstream param_file(drpai_param_file, std::ifstream::ate);
    drp_param_info_size = static_cast<uint32_t>(param_file.tellg());
    /*Load DRPAI Parameter for Cropping later*/
    if(load_drpai_param_file(drpai_fd, proc[DRPAI_INDEX_DRP_PARAM], drpai_param_file, drp_param_info_size) != 0)
    {
        fprintf(stderr,"[ERROR] Failed to load DRPAI Parameter\n");
        goto err;
    }

    printf("Inference Loop Starting\n");
    /*Inference Loop Start*/
    while(1)
    {
        while(1)
        {
            /*Gets the Termination request semaphore value. If different then 1 Termination was requested*/
            /*Checks if sem_getvalue is executed wihtout issue*/
            errno = 0;
            ret = sem_getvalue(&terminate_req_sem, &inf_sem_check);
            if (0 != ret)
            {
                fprintf(stderr, "[ERROR] Failed to get Semaphore Value: errno=%d\n", errno);
                goto err;
            }
            /*Checks the semaphore value*/
            if (1 != inf_sem_check)
            {
                goto ai_inf_end;
            }
            /*Checks if image frame from Capture Thread is ready.*/
            if (inference_start.load())
            {
                break;
            }
            usleep(WAIT_TIME);
        }

        /*Registers physical address & size of input data to DRP-AI*/
        proc[DRPAI_INDEX_INPUT].address = (uintptr_t) capture_address;
        proc0[DRPAI_INDEX_INPUT].address = proc[DRPAI_INDEX_INPUT].address;
        /*Gets inference starting time*/
        ret = timespec_get(&yolo_start_time, TIME_UTC);
        if (0 == ret)
        {
            fprintf(stderr, "[ERROR] Failed to get Inference Start Time\n");
            goto err;
        }
        /*Start DRP-AI Driver*/
        errno = 0;
        ret = ioctl(drpai_fd0, DRPAI_START, &proc0[0]);
        if (0 != ret)
        {
            fprintf(stderr, "[ERROR] Failed to run DRPAI_START: errno=%d\n", errno);
            goto err;
        }

        while(1)
        {
            
            errno = 0;
            ret = sem_getvalue(&terminate_req_sem, &inf_sem_check);
            if (0 != ret)
            {
                fprintf(stderr, "[ERROR] Failed to get Semaphore Value: errno=%d\n", errno);
                goto err;
            }
            /*Checks the semaphore value*/
            if (1 != inf_sem_check)
            {
                goto ai_inf_end;
            }

            /*Setup pselect settings*/
            FD_ZERO(&rfds0);
            FD_SET(drpai_fd0, &rfds0);
            tv.tv_sec = DRPAI_TIMEOUT;
            tv.tv_nsec = 0;

            /*Wait Till The DRP-AI Ends*/
            ret = pselect(drpai_fd0+1, &rfds0, NULL, NULL, &tv, NULL);
            if (0 == ret)
            {
                fprintf(stderr, "[ERROR] DRP-AI Inference pselect() Timeout: errno=%d\n", errno);
                goto err;
            }
            else if (0 > ret)
            {
                fprintf(stderr, "[ERROR] DRP-AI Inference pselect() Error: errno=%d\n", errno);
                ret = ioctl(drpai_fd, DRPAI_GET_STATUS, &drpai_status);
                if (-1 == ret)
                {
                    fprintf(stderr, "[ERROR] Failed to run DRPAI_GET_STATUS : errno=%d\n", errno);
                }
                goto err;
            }
            else
            {
                /*Gets AI Inference End Time*/
                ret = timespec_get(&inf_end_time, TIME_UTC);
                if ( 0 == ret)
                {
                    fprintf(stderr, "[ERROR] Failed to Get Inference End Time\n");
                    goto err;
                }
                yolo_time_tmp = (uint32_t)((timedifference_msec(yolo_start_time, inf_end_time)*TIME_COEF));

                /*Checks if DRPAI Inference ended without issue*/
                inf_status = ioctl(drpai_fd0, DRPAI_GET_STATUS, &drpai_status);
                if (0 == inf_status)
                {
                    /*Process to read the DRPAI output data.*/
                    ret = get_result(drpai_fd0, drpai_data0.address, drpai_data0.size);
                    if (0 != ret)
                    {
                        fprintf(stderr, "[ERROR] Failed to get result from memory.\n");
                        goto err;
                    }
                    /*Preparation For Post-Processing*/
                    bcount = 0;
                    det_res.clear();
                    /*CPU Post-Processing For YOLOv2*/
                    R_Post_Proc(&drpai_output_buf0[0], det_res, &bcount);
                    /*Count the Number of People Detected*/
                    face_count_local = 0;
                    face_counter(det_res, det_face, bcount, &face_count_local);
                    /*If Person is detected run DeepPose for Pose Estimation three times*/
                    if(face_count_local > 0)
                    {
                        for(i = 0; i < face_count_local; i++)
                        {
                            croph[i] = det_face[i].bbox.h + CROP_ADJ_X;
                            cropw[i] = det_face[i].bbox.w + CROP_ADJ_Y;
                            /*Checks that cropping height and width does not exceeds image dimension*/
                            if(croph[i] < 1)
                            {
                                croph[i] = 1;
                            }
                            else if(croph[i] > IMREAD_IMAGE_HEIGHT)
                            {
                                croph[i] = IMREAD_IMAGE_HEIGHT;
                            }
                            else
                            {
                                /*Do Nothing*/
                            }
                            if(cropw[i] < 1)
                            {
                                cropw[i] = 1;
                            }
                            else if(cropw[i] > IMREAD_IMAGE_WIDTH)
                            {
                                cropw[i] = IMREAD_IMAGE_WIDTH;
                            }
                            else
                            {
                                /*Do Nothing*/
                            }
                            /*Compute Cropping Y Position based on Detection Result*/
                            /*If Negative Cropping Position*/
                            if(det_face[i].bbox.y < (croph[i]/2))
                            {
                                cropy[i] = 0;
                            }
                            else if(det_face[i].bbox.y > (IMREAD_IMAGE_HEIGHT-croph[i]/2)) /*If Exceeds Image Area*/
                            {
                                cropy[i] = IMREAD_IMAGE_HEIGHT-croph[i];
                            }
                            else
                            {
                                cropy[i] = (int16_t)det_face[i].bbox.y - croph[i]/2;
                            }
                            /*Compute Cropping X Position based on Detection Result*/
                            /*If Negative Cropping Position*/
                            if(det_face[i].bbox.x < (cropw[i]/2))
                            {
                                cropx[i] = 0;
                            }
                            else if(det_face[i].bbox.x > (IMREAD_IMAGE_WIDTH-cropw[i]/2)) /*If Exceeds Image Area*/
                            {
                                cropx[i] = IMREAD_IMAGE_WIDTH-cropw[i];
                            }
                            else
                            {
                                cropx[i] = (int16_t)det_face[i].bbox.x - cropw[i]/2;
                            }
                            /*Checks that combined cropping position with width and height does not exceed the image dimension*/
                            if(cropx[i] + cropw[i] > IMREAD_IMAGE_WIDTH)
                            {
                                cropw[i] = IMREAD_IMAGE_WIDTH - cropx[i];
                            }
                            if(cropy[i] + croph[i] > IMREAD_IMAGE_HEIGHT)
                            {
                                croph[i] = IMREAD_IMAGE_HEIGHT - cropy[i];
                            }
                            /*Change DeepPose Crop Parameters*/
                            crop_param.img_owidth = (uint16_t)cropw[i];
                            crop_param.img_oheight = (uint16_t)croph[i];
                            crop_param.pos_x = (uint16_t)cropx[i];
                            crop_param.pos_y = (uint16_t)cropy[i];
                            crop_param.obj.address = proc[DRPAI_INDEX_DRP_PARAM].address;
                            crop_param.obj.size = proc[DRPAI_INDEX_DRP_PARAM].size;
                            ret = ioctl(drpai_fd, DRPAI_PREPOST_CROP, &crop_param);
                            if (0 != ret)
                            {
                                fprintf(stderr, "[ERROR] Failed to DRPAI prepost crop: errno=%d\n", errno);
                                goto err;
                            }
                            /*Get Inference Start Time*/
                            ret = timespec_get(&start_time, TIME_UTC);
                            if ( 0 == ret)
                            {
                                fprintf(stderr, "[ERROR] Failed to Get Inference Start Time\n");
                                goto err;
                            }
                            /*Start DRP-AI Driver*/
                            ret = ioctl(drpai_fd, DRPAI_START, &proc[0]);
                            if (0 != ret)
                            {
                                fprintf(stderr, "[ERROR] Failed to run DRPAI_START: errno=%d\n", errno);
                                goto err;
                            }

                            while(1)
                            {
                                /*Gets The Termination Request Semaphore Value, If Different Then 1 Termination Was Requested*/
                                /*Checks If sem_getvalue Is Executed Without Issue*/
                                ret = sem_getvalue(&terminate_req_sem, &inf_sem_check);
                                if (0 != ret)
                                {
                                    fprintf(stderr, "[ERROR] Failed to get Semaphore Value: errno=%d\n", errno);
                                    goto err;
                                }
                                /*Checks The Semaphore Value*/
                                if (1 != inf_sem_check)
                                {
                                    goto ai_inf_end;
                                }
                                /*Wait Till The DRP-AI Ends*/
                                FD_ZERO(&rfds);
                                FD_SET(drpai_fd, &rfds);
                                tv.tv_sec = DRPAI_TIMEOUT;
                                tv.tv_nsec = 0;
                                ret = pselect(drpai_fd+1, &rfds, NULL, NULL, &tv, NULL);
                                if(ret == 0)
                                {
                                    /*Do Nothing*/
                                }
                                else if(ret < 0)
                                {
                                    fprintf(stderr, "[ERROR] DRPAI Inference pselect() Error: %d\n", errno);
                                    goto err;
                                }
                                else
                                {
                                    /*Gets AI Inference End Time*/
                                    ret = timespec_get(&inf_end_time, TIME_UTC);
                                    if ( 0 == ret)
                                    {
                                        fprintf(stderr, "[ERROR] Failed to Get Inference End Time\n");
                                        goto err;
                                    }
                                    deeppose_time[i] = deeppose_time[i] = (uint32_t)((timedifference_msec(start_time, inf_end_time)*TIME_COEF));

                                    /*Checks If DRPAI Inference Ended Without Issue*/
                                    inf_status = ioctl(drpai_fd, DRPAI_GET_STATUS, &drpai_status);
                                    if(inf_status == 0)
                                    {
                                        /*Get DRPAI Output Data*/
                                        inf_status = ioctl(drpai_fd, DRPAI_ASSIGN, &drpai_data);
                                        if (inf_status != 0)
                                        {
                                            fprintf(stderr, "[ERROR] Failed to run DRPAI_ASSIGN: errno=%d\n", errno);
                                                goto err;
                                        }
                                        inf_status = read(drpai_fd, &drpai_output_buf[0], drpai_data.size);
                                        if(inf_status < 0)
                                        {
                                            fprintf(stderr, "[ERROR] Failed to read via DRP-AI Driver: errno=%d\n", errno);
                                            goto err;
                                        }
                                        /*CPU Post Processing For DeepPose, Random Forest & Display the Results*/
                                        R_Post_Proc_DeepPose(&drpai_output_buf[0],i);
                                        R_DeepPose_Coord_Convert(i);
                                        R_Inf_RandomForest(&drpai_output_buf[0],i);
                                        break;
                                    }
                                    else //inf_status != 0
                                    {
                                            fprintf(stderr, "[ERROR] DRPAI Internal Error: errno=%d\n", errno);
                                            goto err;
                                    }
                                }
                            }
                        }
                    }

                    /*Copy data for Display Thread*/
                    face_count = 0;
                    memcpy(id_x, id_x_local, sizeof(id_x_local));
                    memcpy(id_y, id_y_local,sizeof(id_y_local));
                    face_count = face_count_local;
                    /*Display Processing Time On Console*/
                    ai_time = 0;
                    for(uint8_t i = 0; i < face_count_local; i++)
                    {
                        ai_time += deeppose_time[i];
                    }
                    total_time = ai_time + yolo_time_tmp;
                    yolo_time = yolo_time_tmp;
                    break;

                }
                else
                {
                    /* inf_status != 0 */
                    fprintf(stderr, "[ERROR] DRPAI Internal Error: errno=%d\n", errno);
                    goto err;
                }
            }
        }
        inference_start.store(0);
    }
    /*End of Inference Loop*/

/*Error Processing*/
err:
    /*Set Termination Request Semaphore to 0*/
    sem_trywait(&terminate_req_sem);
    goto ai_inf_end;
/*AI Thread Termination*/
ai_inf_end:
    /*To terminate the loop in Capture Thread.*/
    printf("AI Inference Thread Terminated\n");
    pthread_exit(NULL);
}

/*****************************************
* Function Name : R_Capture_Thread
* Description   : Executes the V4L2 capture with Capture thread.
* Arguments     : threadid = thread identification
* Return value  : -
******************************************/
void *R_Capture_Thread(void *threadid)
{
    Camera* capture = (Camera*) threadid;
    /*Semaphore Variable*/
    int32_t capture_sem_check = 0;
    /*First Loop Flag*/
    uint32_t capture_addr = 0;
    int8_t ret = 0;
    int32_t counter = 0;
    uint8_t * img_buffer;
    uint8_t * img_buffer0;
    const int32_t th_cnt = INF_FRAME_NUM;
    uint8_t udmabuf_fd0;
    uint8_t capture_stabe_cnt = 8;  // Counter to wait for the camera to stabilize

    printf("Capture Thread Starting\n");

    udmabuf_fd0 = open("/dev/udmabuf0", O_RDWR );
    img_buffer0 = (unsigned char*) mmap(NULL, CAM_IMAGE_WIDTH * CAM_IMAGE_HEIGHT * CAM_IMAGE_CHANNEL_YUY2 ,PROT_READ|PROT_WRITE, MAP_SHARED,  udmabuf_fd0, UDMABUF_INFIMAGE_OFFSET);
    capture_address = (uint32_t)udmabuf_address + UDMABUF_INFIMAGE_OFFSET;

    while(1)
    {
        /*Gets the Termination request semaphore value. If different then 1 Termination was requested*/
        /*Checks if sem_getvalue is executed wihtout issue*/
        errno = 0;
        ret = sem_getvalue(&terminate_req_sem, &capture_sem_check);
        if (0 != ret)
        {
            fprintf(stderr, "[ERROR] Failed to get Semaphore Value: errno=%d\n", errno);
            goto err;
        }
        /*Checks the semaphore value*/
        if (1 != capture_sem_check)
        {
            goto capture_end;
        }

        /* Capture MIPI camera image and stop updating the capture buffer */
        capture_addr = (uint32_t)capture->capture_image(udmabuf_address);
        if (capture_addr == 0)
        {
            fprintf(stderr, "[ERROR] Failed to capture image from camera.\n");
            goto err;
        }
        else
        {
            /* Do not process until the camera stabilizes, because the image is unreliable until the camera stabilizes. */
            if( capture_stabe_cnt > 0 )
            {
                capture_stabe_cnt--;
            }
            else
            {
                img_buffer = capture->get_img();
                if (!inference_start.load())
                {
                    /* Copy captured image to Image object. This will be used in Display Thread. */
                    memcpy(img_buffer0, img_buffer, CAM_IMAGE_WIDTH * CAM_IMAGE_HEIGHT * CAM_IMAGE_CHANNEL_YUY2);
                    inference_start.store(1); /* Flag for AI Inference Thread. */
                }

                if (!img_obj_ready.load())
                {
                    img.camera_to_image(img_buffer, capture->get_size());
                    img_obj_ready.store(1); /* Flag for Display Thread. */
                }
            }
        }

        /* IMPORTANT: Place back the image buffer to the capture queue */
        ret = capture->capture_qbuf();
        if (0 != ret)
        {
            fprintf(stderr, "[ERROR] Failed to enqueue capture buffer.\n");
            goto capture_end;
        }
    } /*End of Loop*/

/*Error Processing*/
err:
    sem_trywait(&terminate_req_sem);
    goto capture_end;

capture_end:
    /*To terminate the loop in AI Inference Thread.*/
    inference_start.store(1);

    printf("Capture Thread Terminated\n");
    pthread_exit(NULL);
}

/*****************************************
* Function Name : R_Display_Thread
* Description   : Executes the HDMI Display with Display thread
* Arguments     : threadid = thread identification
* Return value  : -
******************************************/
void *R_Display_Thread(void *threadid)
{
    /*Semaphore Variable*/
    int32_t hdmi_sem_check = 0;
    /*image buffer id*/
    uint8_t img_buf_id;
    /*Variable for checking return value*/
    int8_t ret = 0;

    printf("Display Thread Starting\n");

    while(1)
    {
        /*Gets The Termination Request Semaphore Value, If Different Then 1 Termination Is Requested*/
        /*Checks If sem_getvalue Is Executed Without Issue*/
        errno = 0;
        ret = sem_getvalue(&terminate_req_sem, &hdmi_sem_check);
        if (0 != ret)
        {
            fprintf(stderr, "[ERROR] Failed to get Semaphore Value: errno=%d\n", errno);
            goto err;
        }
        /*Checks the semaphore value*/
        if (1 != hdmi_sem_check)
        {
            goto hdmi_end;
        }
        /* Check img_obj_ready flag which is set in Capture Thread. */
        if (img_obj_ready.load())
        {
            /* Draw Complete Skeleton. */
            draw_skeleton();

            /* Convert YUYV image to BGRA format. */
            img.convert_format();
            img.flip();
            /*displays AI Inference Results on display.*/
            print_result(&img);

            /*Update Wayland*/
            img_buf_id = img.get_buf_id();
            wayland.commit(img_buf_id);
            img_obj_ready.store(0);
        }
        usleep(WAIT_TIME); //wait 1 tick time
    } /*End Of Loop*/

/*Error Processing*/
err:
    /*Set Termination Request Semaphore To 0*/
    sem_trywait(&terminate_req_sem);
    goto hdmi_end;

hdmi_end:
    /*To terminate the loop in Capture Thread.*/
    img_obj_ready.store(0);
    printf("Display Thread Terminated\n");
    pthread_exit(NULL);
}

/*****************************************
* Function Name : R_Kbhit_Thread
* Description   : Executes the Keyboard hit thread (checks if enter key is hit)
* Arguments     : threadid = thread identification
* Return value  : -
******************************************/
void *R_Kbhit_Thread(void *threadid)
{
    /*Semaphore Variable*/
    int32_t kh_sem_check = 0;
    /*Variable to store the getchar() value*/
    int32_t c = 0;
    /*Variable for checking return value*/
    int8_t ret = 0;

    printf("Key Hit Thread Starting\n");

    printf("************************************************\n");
    printf("* Press ENTER key to quit. *\n");
    printf("************************************************\n");

    /*Set Standard Input to Non Blocking*/
    errno = 0;
    ret = fcntl(0, F_SETFL, O_NONBLOCK);
    if (-1 == ret)
    {
        fprintf(stderr, "[ERROR] Failed to run fctnl(): errno=%d\n", errno);
        goto err;
    }

    while(1)
    {
        /*Gets the Termination request semaphore value. If different then 1 Termination was requested*/
        /*Checks if sem_getvalue is executed wihtout issue*/
        errno = 0;
        ret = sem_getvalue(&terminate_req_sem, &kh_sem_check);
        if (0 != ret)
        {
            fprintf(stderr, "[ERROR] Failed to get Semaphore Value: errno=%d\n", errno);
            goto err;
        }
        /*Checks the semaphore value*/
        if (1 != kh_sem_check)
        {
            goto key_hit_end;
        }

        c = getchar();
        if (EOF != c)
        {
            /* When key is pressed. */
            printf("key Detected.\n");
            goto err;
        }
        else
        {
            /* When nothing is pressed. */
            usleep(WAIT_TIME);
        }
    }

/*Error Processing*/
err:
    /*Set Termination Request Semaphore to 0*/
    sem_trywait(&terminate_req_sem);
    goto key_hit_end;

key_hit_end:
    printf("Key Hit Thread Terminated\n");
    pthread_exit(NULL);
}

/*****************************************
* Function Name : R_Main_Process
* Description   : Runs the main process loop
* Arguments     : -
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t R_Main_Process()
{
    /*Main Process Variables*/
    int8_t main_ret = 0;
    /*Semaphore Related*/
    int32_t sem_check = 0;
    /*Variable for checking return value*/
    int8_t ret = 0;

    printf("Main Loop Starts\n");
    while(1)
    {
        /*Gets the Termination request semaphore value. If different then 1 Termination was requested*/
        errno = 0;
        ret = sem_getvalue(&terminate_req_sem, &sem_check);
        if (0 != ret)
        {
            fprintf(stderr, "[ERROR] Failed to get Semaphore Value: errno=%d\n", errno);
            goto err;
        }
        /*Checks the semaphore value*/
        if (1 != sem_check)
        {
            goto main_proc_end;
        }
        usleep(WAIT_TIME); //wait 1 tick time
    } //end main loop

/*Error Processing*/
err:
    sem_trywait(&terminate_req_sem);
    main_ret = 1;
    goto main_proc_end;
/*Main Processing Termination*/
main_proc_end:
    printf("Main Process Terminated\n");
    return main_ret;
}


int32_t main(int32_t argc, char * argv[])
{
    int8_t main_proc = 0;
    int8_t ret = 0;
    int8_t ret_main = 0;
    /*Multithreading Variables*/
    int32_t create_thread_ai = -1;
    int32_t create_thread_key = -1;
    int32_t create_thread_capture = -1;
    int32_t create_thread_hdmi = -1;
    int32_t sem_create = -1;
    Camera* capture = NULL;

    /* Obtain udmabuf memory area starting address */
    int8_t fd = 0;
    char addr[1024];
    int32_t read_ret = 0;
    errno = 0;
    fd = open("/sys/class/u-dma-buf/udmabuf0/phys_addr", O_RDONLY);
    if (0 > fd)
    {
        fprintf(stderr, "[ERROR] Failed to open udmabuf0/phys_addr : errno=%d\n", errno);
        return -1;
    }
    read_ret = read(fd, addr, 1024);
    if (0 > read_ret)
    {
        fprintf(stderr, "[ERROR] Failed to read udmabuf0/phys_addr : errno=%d\n", errno);
        close(fd);
        return -1;
    }
    sscanf(addr, "%lx", &udmabuf_address);
    close(fd);
    /* Filter the bit higher than 32 bit */
    udmabuf_address &=0xFFFFFFFF;

    printf("RZ/V2L DRP-AI Sample Application\n");
    printf("Model : Driver Monitoring System with Deeppose and Tinyyolov2 | deeppose_cam, tinyyolov2_cam\n");
    printf("Input : USB Camera\n");

    /*DRP-AI Driver Open*/
    /*For TinyYOLOv2*/
    errno = 0;
    drpai_fd0 = open("/dev/drpai0", O_RDWR);
    if (0 > drpai_fd0)
    {
        fprintf(stderr, "[ERROR] Failed to open DRP-AI Driver: errno=%d\n", errno);
        return -1;
    }
    /*For DeepPose*/
    errno = 0;
    drpai_fd = open("/dev/drpai0", O_RDWR);
    if (0 > drpai_fd)
    {
        fprintf(stderr, "[ERROR] Failed to open DRP-AI Driver: errno=%d\n", errno);
        return -1;
    }

    /* Load DRP-AI Data from Filesystem to Memory via DRP-AI Driver */
    /* TinyYOLOv2 */
    prefix = AI0_DESC_NAME;
    dir = prefix + "/";
    address_file = dir + prefix + "_addrmap_intm.txt";
    ret = read_addrmap_txt(address_file, &drpai_address0);
    if (0 != ret)
    {
        fprintf(stderr, "[ERROR] Failed to read addressmap text file: %s\n", address_file.c_str());
        ret_main = ret;
        goto end_close_drpai;
    }
    ret = load_drpai_data(drpai_fd0, drpai_address0);
    if (0 != ret)
    {
        fprintf(stderr, "[ERROR] Failed to load DRP-AI Data\n");
        ret_main = ret;
        goto end_close_drpai;
    }
    /* DeepPose */
    prefix = AI_DESC_NAME;
    dir = prefix + "/";
    address_file = dir + prefix + "_addrmap_intm.txt";
    ret = read_addrmap_txt(address_file, &drpai_address);
    if (0 != ret)
    {
        fprintf(stderr, "[ERROR] Failed to read addressmap text file: %s\n", address_file.c_str());
        goto end_close_drpai;
    }
    ret = load_drpai_data(drpai_fd, drpai_address);
    if (0 != ret)
    {
        fprintf(stderr, "[ERROR] Failed to load DRP-AI Data\n");
        ret_main = ret;
        goto end_close_drpai;
    }

    /* Create Camera Instance */
    capture = new Camera();

    /* Init and Start Camera */
    ret = capture->start_camera();
    if (0 != ret)
    {
        fprintf(stderr, "[ERROR] Failed to initialize Camera.\n");
        delete capture;
        ret_main = ret;
        goto end_close_drpai;
    }

    /*Initialize Image object.*/
    ret = img.init(CAM_IMAGE_WIDTH, CAM_IMAGE_HEIGHT, CAM_IMAGE_CHANNEL_YUY2, IMAGE_OUTPUT_WIDTH, IMAGE_OUTPUT_HEIGHT, IMAGE_CHANNEL_BGRA);
    if (0 != ret)
    {
        fprintf(stderr, "[ERROR] Failed to initialize Image object.\n");
        ret_main = ret;
        goto end_close_camera;
    }

    /* Initialize waylad */
    ret = wayland.init(img.udmabuf_fd, IMAGE_OUTPUT_WIDTH, IMAGE_OUTPUT_HEIGHT, IMAGE_CHANNEL_BGRA);
    if(0 != ret)
    {
        fprintf(stderr, "[ERROR] Failed to initialize Image for Wayland\n");
        ret_main = -1;
        goto end_close_camera;
    }

    /*Termination Request Semaphore Initialization*/
    /*Initialized value at 1.*/
    sem_create = sem_init(&terminate_req_sem, 0, 1);
    if (0 != sem_create)
    {
        fprintf(stderr, "[ERROR] Failed to Initialize Termination Request Semaphore.\n");
        ret_main = -1;
        goto end_threads;
    }

    /*Create Key Hit Thread*/
    create_thread_key = pthread_create(&kbhit_thread, NULL, R_Kbhit_Thread, NULL);
    if (0 != create_thread_key)
    {
        fprintf(stderr, "[ERROR] Failed to create Key Hit Thread.\n");
        ret_main = -1;
        goto end_threads;
    }

    /*Create Inference Thread*/
    create_thread_ai = pthread_create(&ai_inf_thread, NULL, R_Inf_Thread, NULL);
    if (0 != create_thread_ai)
    {
        sem_trywait(&terminate_req_sem);
        fprintf(stderr, "[ERROR] Failed to create AI Inference Thread.\n");
        ret_main = -1;
        goto end_threads;
    }

    /*Create Capture Thread*/
    create_thread_capture = pthread_create(&capture_thread, NULL, R_Capture_Thread, (void *) capture);
    if (0 != create_thread_capture)
    {
        sem_trywait(&terminate_req_sem);
        fprintf(stderr, "[ERROR] Failed to create Capture Thread.\n");
        ret_main = -1;
        goto end_threads;
    }

    /*Create Display Thread*/
    create_thread_hdmi = pthread_create(&hdmi_thread, NULL, R_Display_Thread, NULL);
    if(0 != create_thread_hdmi)
    {
        sem_trywait(&terminate_req_sem);
        fprintf(stderr, "[ERROR] Failed to create Display Thread.\n");
        ret_main = -1;
        goto end_threads;
    }

    /*Main Processing*/
    main_proc = R_Main_Process();
    if (0 != main_proc)
    {
        fprintf(stderr, "[ERROR] Error during Main Process\n");
        ret_main = -1;
    }
    goto end_threads;


end_threads:
    if(0 == create_thread_hdmi)
    {
        ret = wait_join(&hdmi_thread, DISPLAY_THREAD_TIMEOUT);
        if (0 != ret)
        {
            fprintf(stderr, "[ERROR] Failed to exit Display Thread on time.\n");
            ret_main = -1;
        }
    }
    if (0 == create_thread_capture)
    {
        ret = wait_join(&capture_thread, CAPTURE_TIMEOUT);
        if (0 != ret)
        {
            fprintf(stderr, "[ERROR] Failed to exit Capture Thread on time.\n");
            ret_main = -1;
        }
    }
    if (0 == create_thread_ai)
    {
        ret = wait_join(&ai_inf_thread, AI_THREAD_TIMEOUT);
        if (0 != ret)
        {
            fprintf(stderr, "[ERROR] Failed to exit AI Inference Thread on time.\n");
            ret_main = -1;
        }
    }
    if (0 == create_thread_key)
    {
        ret = wait_join(&kbhit_thread, KEY_THREAD_TIMEOUT);
        if (0 != ret)
        {
            fprintf(stderr, "[ERROR] Failed to exit Key Hit Thread on time.\n");
            ret_main = -1;
        }
    }

    /*Delete Terminate Request Semaphore.*/
    if (0 == sem_create)
    {
        sem_destroy(&terminate_req_sem);
    }

    /* Exit waylad */
    wayland.exit();
    goto end_close_camera;

end_close_camera:
    /*Close MIPI Camera.*/
    ret = capture->close_camera();
    if (0 != ret)
    {
        fprintf(stderr, "[ERROR] Failed to close Camera.\n");
        ret_main = -1;
    }
    delete capture;
    goto end_close_drpai;

end_close_drpai:
    /*Close DRP-AI Driver.*/
    if (0 < drpai_fd0)
    {
        errno = 0;
        ret = close(drpai_fd0);
        if (0 != ret)
        {
            fprintf(stderr, "[ERROR] Failed to close DRP-AI Driver: errno=%d\n", errno);
            ret_main = -1;
        }
    }
    if (0 < drpai_fd)
    {
        errno = 0;
        ret = close(drpai_fd);
        if (0 != ret)
        {
            fprintf(stderr, "[ERROR] Failed to close DRP-AI Driver: errno=%d\n", errno);
            ret_main = -1;
        }
    }
    goto end_main;

end_main:
    printf("Application End\n");
    return ret_main;
}
