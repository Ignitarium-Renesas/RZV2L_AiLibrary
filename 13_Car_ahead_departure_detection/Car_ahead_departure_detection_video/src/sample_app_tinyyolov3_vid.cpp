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
* Copyright (C) 2023 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : sample_app_tinyyolov3_vid.cpp
* Version      : 7.30
* Description  : RZ/V2L DRP-AI Sample Application for PyTorch Tiny YOLOv3 Video version
***********************************************************************************************************************/

/*****************************************
* Includes
******************************************/
/*DRPAI Driver Header*/
#include <linux/drpai.h>
/*Definition of Macros & other variables*/
#include "define.h"

#include "box.h"

/*tracker */
#include "tracker.h"
#include "utils.h"
#include <set>

using namespace std;
using namespace cv;
/*****************************************
* Global Variables
******************************************/
st_addr_t drpai_address;
float drpai_output_buf[num_inf_out];
unsigned char* img_buffer;
uint64_t udmabuf_address = 0;
vector<detection> det;
int flag =0;

/*Tracker Variables*/
TRACKER tracker;
std::vector<cv::Rect> boxes;
static std::map<int,vector<float>> Area_MAP;
static std::map<int,float> Avg_Area_MAP;
static std::set<int>DONE_IDS;   

/*Audio selection*/
int sel_aud;

/*****************************************
* DRP-AI Inference
******************************************/
int8_t drpai_fd;
fd_set rfds;
int8_t ret_drpai;
drpai_data_t proc[DRPAI_INDEX_NUM];

VideoCapture cap(input_vid);
double fps = cap.get(CAP_PROP_FPS);
VideoWriter video("video_inf_out.avi", cv::VideoWriter::fourcc('M','J','P','G'), std::round(fps), Size(DRPAI_IN_WIDTH,DRPAI_IN_HEIGHT));
string label;

/*****************************************
* Function Name : read_addrmap_txt
* Description   : Loads address and size of DRP-AI Object files into struct addr.
* Arguments     : addr_file = filename of addressmap file (from DRP-AI Object files)
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t read_addrmap_txt(string addr_file)
{
    string str;
    uint32_t l_addr;
    uint32_t l_size;
    string element, a, s;

    ifstream ifs(addr_file);
    if (ifs.fail())
    {
        fprintf(stderr, "[ERROR] Failed to open address map list : %s\n", addr_file.c_str());
        return -1;
    }

    while (getline(ifs, str))
    {
        istringstream iss(str);
        iss >> element >> a >> s;
        l_addr = strtol(a.c_str(), NULL, 16);
        l_size = strtol(s.c_str(), NULL, 16);

        if ("drp_config" == element)
        {
            drpai_address.drp_config_addr = l_addr;
            drpai_address.drp_config_size = l_size;
        }
        else if ("desc_aimac" == element)
        {
            drpai_address.desc_aimac_addr = l_addr;
            drpai_address.desc_aimac_size = l_size;
        }
        else if ("desc_drp" == element)
        {
            drpai_address.desc_drp_addr = l_addr;
            drpai_address.desc_drp_size = l_size;
        }
        else if ("drp_param" == element)
        {
            drpai_address.drp_param_addr = l_addr;
            drpai_address.drp_param_size = l_size;
        }
        else if ("weight" == element)
        {
            drpai_address.weight_addr = l_addr;
            drpai_address.weight_size = l_size;
        }
        else if ("data_in" == element)
        {
            drpai_address.data_in_addr = l_addr;
            drpai_address.data_in_size = l_size;
        }
        else if ("data" == element)
        {
            drpai_address.data_addr = l_addr;
            drpai_address.data_size = l_size;
        }
        else if ("data_out" == element)
        {
            drpai_address.data_out_addr = l_addr;
            drpai_address.data_out_size = l_size;
        }
        else if ("work" == element)
        {
            drpai_address.work_addr = l_addr;
            drpai_address.work_size = l_size;
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
int8_t load_data_to_mem(string data, int8_t drpai_fd, uint32_t from, uint32_t size)
{
    int8_t ret_load_data = 0;
    int8_t obj_fd;
    uint8_t drpai_buf[BUF_SIZE];
    drpai_data_t drpai_data;
    size_t ret = 0;
    int32_t i = 0;

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

    for (i = 0; i < (drpai_data.size / BUF_SIZE); i++)
    {
        errno = 0;
        ret = read(obj_fd, drpai_buf, BUF_SIZE);
        if ( 0 > ret )
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
        if ( -1 == ret )
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
* Return value  : 0 if succeeded
*               : not 0 otherwise
******************************************/
int8_t load_drpai_data(int8_t drpai_fd)
{
    uint32_t addr = 0;
    uint32_t size = 0;
    int32_t i = 0;
    size_t ret = 0;
    for ( i = 0; i < 5; i++ )
    {
        switch (i)
        {
            case (INDEX_W):
                addr = drpai_address.weight_addr;
                size = drpai_address.weight_size;
                break;
            case (INDEX_C):
                addr = drpai_address.drp_config_addr;
                size = drpai_address.drp_config_size;
                break;
            case (INDEX_P):
                addr = drpai_address.drp_param_addr;
                size = drpai_address.drp_param_size;
                break;
            case (INDEX_A):
                addr = drpai_address.desc_aimac_addr;
                size = drpai_address.desc_aimac_size;
                break;
            case (INDEX_D):
                addr = drpai_address.desc_drp_addr;
                size = drpai_address.desc_drp_size;
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
* Function Name     : load_label_file
* Description       : Load label list text file and return the label list that contains the label.
* Arguments         : label_file_name = filename of label list. must be in txt format
* Return value      : vector<string> list = list contains labels
*                     empty if error occured
******************************************/
vector<string> load_label_file(string label_file_name)
{
    vector<string> list = {};
    vector<string> empty = {};
    ifstream infile(label_file_name);

    if (!infile.is_open())
    {
        return list;
    }

    string line = "";
    while (getline(infile,line))
    {
        list.push_back(line);
        if (infile.fail())
        {
            return empty;
        }
    }

    return list;
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
int8_t get_result(int8_t drpai_fd, uint32_t output_addr, uint32_t output_size)
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
    for (i = 0; i < (drpai_data.size / BUF_SIZE); i++)
    {
        errno = 0;
        ret = read(drpai_fd, drpai_buf, BUF_SIZE);
        if ( -1 == ret )
        {
            fprintf(stderr, "[ERROR] Failed to read via DRP-AI Driver: errno=%d\n", errno);
            return -1;
        }

        memcpy(&drpai_output_buf[BUF_SIZE/sizeof(float)*i], drpai_buf, BUF_SIZE);
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

        memcpy(&drpai_output_buf[(drpai_data.size - (drpai_data.size%BUF_SIZE))/sizeof(float)], drpai_buf, (drpai_data.size % BUF_SIZE));
    }
    return 0;
}

/*****************************************
* Function Name : sigmoid
* Description   : Helper function for YOLO Post Processing
* Arguments     : x = input argument for the calculation
* Return value  : sigmoid result of input x
******************************************/
double sigmoid(double x)
{
    return 1.0/(1.0 + exp(-x));
}

/*****************************************
* Function Name : softmax
* Description   : Helper function for YOLO Post Processing
* Arguments     : val[] = array to be computed softmax
* Return value  : -
******************************************/
void softmax(float val[NUM_CLASS])
{
    float max_num = -FLT_MAX;
    float sum = 0;
    int32_t i;
    for ( i = 0 ; i<NUM_CLASS ; i++ )
    {
        max_num = max(max_num, val[i]);
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
* Function Name : yolo_index
* Description   : Get the index of the bounding box attributes based on the input offset.
* Arguments     : n = output layer number.
*                 offs = offset to access the bounding box attributesd.
*                 channel = channel to access each bounding box attribute.
* Return value  : index to access the bounding box attribute.
******************************************/
int32_t yolo_index(uint8_t n, int32_t offs, int32_t channel)
{
    uint8_t num_grid = num_grids[n];
    return offs + channel * num_grid * num_grid;
}

/*****************************************
* Function Name : yolo_offset
* Description   : Get the offset nuber to access the bounding box attributes
*                 To get the actual value of bounding box attributes, use yolo_index() after this function.
* Arguments     : n = output layer number [0~2].
*                 b = Number to indicate which bounding box in the region [0~2]
*                 y = Number to indicate which region [0~13]
*                 x = Number to indicate which region [0~13]
* Return value  : offset to access the bounding box attributes.
******************************************/
int32_t yolo_offset(uint8_t n, int32_t b, int32_t y, int32_t x)
{
    uint8_t num = num_grids[n];
    uint32_t prev_layer_num = 0;
    int32_t i = 0;

    for (i = 0 ; i < n; i++)
    {
        prev_layer_num += NUM_BB *(NUM_CLASS + 5)* num_grids[i] * num_grids[i];
    }
    return prev_layer_num + b *(NUM_CLASS + 5)* num * num + y * num + x;
}

/*****************************************
* Function Name : print_result_yolo
* Description   : Process CPU post-processing for YOLO (drawing bounding boxes) and print the result on console.
* Arguments     : floatarr = float DRP-AI output data
*                 img = image to draw the detection result
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t print_result_yolo(float* floatarr, Mat * img)
{
    /* Following variables are required for correct_yolo/region_boxes in Darknet implementation*/
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

    int32_t n = 0;
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
    uint8_t num_grid = 0;
    uint8_t anchor_offset = 0;
    float classes[NUM_CLASS];
    float max_pred = 0;
    int32_t pred_class = -1;
    float probability = 0;
    detection d;
    std::string result_str;
    /* Clear the detected result list */
    det.clear();

    for (n = 0; n<NUM_INF_OUT_LAYER; n++)
    {
        num_grid = num_grids[n];
        anchor_offset = 2 * NUM_BB * (NUM_INF_OUT_LAYER - (n + 1));

        for (b = 0;b<NUM_BB;b++)
        {
            for (y = 0;y<num_grid;y++)
            {
                for (x = 0;x<num_grid;x++)
                {
                    offs = yolo_offset(n, b, y, x);
                    tx = floatarr[offs];
                    ty = floatarr[yolo_index(n, offs, 1)];
                    tw = floatarr[yolo_index(n, offs, 2)];
                    th = floatarr[yolo_index(n, offs, 3)];
                    tc = floatarr[yolo_index(n, offs, 4)];

                    /* Compute the bounding box */
                    /*get_yolo_box/get_region_box in paper implementation*/
                    center_x = ((float) x + sigmoid(tx)) / (float) num_grid;
                    center_y = ((float) y + sigmoid(ty)) / (float) num_grid;
                    box_w = (float) exp(tw) * anchors[anchor_offset+2*b+0] / (float) MODEL_IN_W;
                    box_h = (float) exp(th) * anchors[anchor_offset+2*b+1] / (float) MODEL_IN_W;
                    /* Adjustment for VGA size */
                    /* correct_yolo/region_boxes */
                    center_x = (center_x - (MODEL_IN_W - new_w) / 2. / MODEL_IN_W) / ((float) new_w / MODEL_IN_W);
                    center_y = (center_y - (MODEL_IN_H - new_h) / 2. / MODEL_IN_H) / ((float) new_h / MODEL_IN_H);
                    box_w *= (float) (MODEL_IN_W / new_w);
                    box_h *= (float) (MODEL_IN_H / new_h);

                    center_x = round(center_x * DRPAI_IN_WIDTH);
                    center_y = round(center_y * DRPAI_IN_HEIGHT);
                    box_w = round(box_w * DRPAI_IN_WIDTH);
                    box_h = round(box_h * DRPAI_IN_HEIGHT);

                    objectness = sigmoid(tc);

                    Box bb = {center_x, center_y, box_w, box_h};
                    /* Get the class prediction */
                    for (i = 0;i < NUM_CLASS;i++)
                    {
                        classes[i] = sigmoid(floatarr[yolo_index(n, offs, 5+i)]);
                    }

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
                    if((probability > TH_PROB) && ((pred_class == 2)||(pred_class == 5)||(pred_class == 6)))
                    {
                        d = {bb, pred_class, probability};
                        det.push_back(d);
                    }
                }
            }
        }
    }
    /* Non-Maximum Supression filter */
    filter_boxes_nms(det, det.size(), TH_NMS);
    boxes.clear();
    float close_point = -1;
    int selected = -1;
    /* Render boxes on image and print their details */
    n = 1;
    for (i = 0;i < det.size(); i++)
    {
        /* Skip the overlapped bounding boxes */
        if (det[i].prob == 0) continue;

        /* Virtual lines inside which detection takes place*/
        if ((det[i].bbox.x >= 220) && ( det[i].bbox.x <= 440)){
            if ((det[i].bbox.y+det[i].bbox.h/2) > close_point){
                close_point = (det[i].bbox.y+det[i].bbox.h/2);
                selected = i;
            }   
        }
    }
    if (selected != -1){
        /* Clear string stream for bounding box labels */
        result_str = label_file_map[det[selected].c];
        boxes.push_back({(int)det[selected].bbox.x, (int)det[selected].bbox.y, (int)det[selected].bbox.w, (int)det[selected].bbox.h});
    }
   

    tracker.Run(boxes);
    const auto tracks = tracker.GetTracks();
    for (auto &trk : tracks) {
        const auto &box_trk = trk.second.GetStateAsBbox();
        std::string ID = std::to_string(trk.first);
        float x = box_trk.tl().x;
        float y = box_trk.tl().y;
        int I_ID = std::atoi(ID.c_str());
        std::ostringstream oss;
        
        if (trk.second.coast_cycles_ < kMaxCoastCycles&& trk.second.hit_streak_ >= kMinHits)
        {
            int32_t x_min = box_trk.tl().x - round(box_trk.width / 2.);
            int32_t y_min = box_trk.tl().y - round( box_trk.height / 2.);
            int32_t x_max = box_trk.tl().x + round(box_trk.width / 2.) - 1;
            int32_t y_max = box_trk.tl().y + round( box_trk.height / 2.) -1;
            /* Check the bounding box is in the image range */
            x_min = x_min < 1 ? 1 : x_min;
            x_max = ((img->size().width - 2) < x_max) ? (img->size().width - 2) : x_max;
            y_min = y_min < 1 ? 1 : y_min;
            y_max = ((img->size().height - 2) < y_max) ? (img->size().height - 2) : y_max;
            float X_SCALE = img->size().width / MODEL_IN_W;
            float Y_SCALE = img->size().height / MODEL_IN_H;
            x_min *= X_SCALE;
            x_max *= X_SCALE;
            y_min *= Y_SCALE;
            y_max *= Y_SCALE;
            float area = box_trk.height*box_trk.width;

            Point p1(x_min, y_min);
            Point p2(x_max,y_max);
            rectangle(*img, p1, p2, Scalar(0, 0, 255, 255), LINE_8); 

            std::vector<float> Areas;
            /*Takes the area of bounding box for three frames*/
            while(Area_MAP[I_ID].size() <= 3){
                Area_MAP[I_ID].push_back(area);
            }

             /*Calculates the area of bounding box for three frames*/
            if(Avg_Area_MAP.find(I_ID)==Avg_Area_MAP.end()){
                float avg_area = std::accumulate( Area_MAP[I_ID].begin(), Area_MAP[I_ID].end(), 0.0)/Area_MAP[I_ID].size();   
                Avg_Area_MAP.insert({I_ID, avg_area});
            }
            while(Areas.size() <= 3){
                Areas.push_back(area);
            }
            /*Current average area*/
            float avg_areas = std::accumulate(Areas.begin(), Areas.end(), 0.0)/Areas.size(); 

            if ((avg_areas <(0.90*Avg_Area_MAP[I_ID])) && (flag == 0))
                flag=1;
            if(flag==1)
            {
            
                oss << result_str <<" in front moved ";
                if (DONE_IDS.find(I_ID) == DONE_IDS.end()){
                    int count = 0;
                    DONE_IDS.insert(I_ID);
                    while (count < 10)
                    {
                        if (sel_aud == 0){
                            if(system("/usr/bin/aplay -D default:CARD=rzssidaiwm8978h alert.wav &>/dev/null &") == -1)
                                std::cout << "Unable to play alert.wav" << std::endl;
                               count++;
                        }
                        if (sel_aud == 1){
                            if(system("/usr/bin/aplay -D default:CARD=soundcard alert.wav &>/dev/null &") == -1)
                                std::cout << "Unable to play alert.wav" << std::endl;
                             count++;   
                        }
                    }
                }
                putText(*img, oss.str(), Point(x_min, y_min), FONT_HERSHEY_SIMPLEX, 0.5, Scalar(0, 255, 0, 255), 1.5);
            }
            Areas.erase(Areas.begin());
            Areas.push_back(area);
        }
                
    }
    return 0;
}


int32_t load_drpai(void)
{
    int32_t ret = 0;

    /* Read DRP-AI Object files address and size */
    ret = read_addrmap_txt(drpai_address_file);
    if (0 != ret)
    {
        fprintf(stderr, "[ERROR] Failed to read addressmap text file: %s\n", drpai_address_file.c_str());
        ret = -1;
    }

    /* Open DRP-AI Driver */
    errno = 0;
    drpai_fd = open("/dev/drpai0", O_RDWR);
    if (0 > drpai_fd)
    {
        fprintf(stderr, "[ERROR] Failed to open DRP-AI Driver: errno=%d\n", errno);
        ret = -1;
    }

    /* Load DRP-AI Data from Filesystem to Memory via DRP-AI Driver */
    ret = load_drpai_data(drpai_fd);
    if (0 > ret)
    {
        fprintf(stderr, "[ERROR] Failed to load DRP-AI Object files.\n");
        errno = 0;
        ret = close(drpai_fd);
        if (0 != ret)
        {
            fprintf(stderr, "[ERROR] Failed to close DRP-AI Driver: errno=%d\n", errno);
            ret = -1;
        }
    }

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

    /* Set DRP-AI Driver Input (DRP-AI Object files address and size)*/
    proc[DRPAI_INDEX_INPUT].address       = drpai_address.data_in_addr;
    proc[DRPAI_INDEX_INPUT].size          = drpai_address.data_in_size;
    proc[DRPAI_INDEX_DRP_CFG].address     = drpai_address.drp_config_addr;
    proc[DRPAI_INDEX_DRP_CFG].size        = drpai_address.drp_config_size;
    proc[DRPAI_INDEX_DRP_PARAM].address   = drpai_address.drp_param_addr;
    proc[DRPAI_INDEX_DRP_PARAM].size      = drpai_address.drp_param_size;
    proc[DRPAI_INDEX_AIMAC_DESC].address  = drpai_address.desc_aimac_addr;
    proc[DRPAI_INDEX_AIMAC_DESC].size     = drpai_address.desc_aimac_size;
    proc[DRPAI_INDEX_DRP_DESC].address    = drpai_address.desc_drp_addr;
    proc[DRPAI_INDEX_DRP_DESC].size       = drpai_address.desc_drp_size;
    proc[DRPAI_INDEX_WEIGHT].address      = drpai_address.weight_addr;
    proc[DRPAI_INDEX_WEIGHT].size         = drpai_address.weight_size;
    proc[DRPAI_INDEX_OUTPUT].address      = drpai_address.data_out_addr;
    proc[DRPAI_INDEX_OUTPUT].size         = drpai_address.data_out_size;

    return ret;
}

int32_t get_video_inf(Mat &frame)
{
    int32_t ret = 0;
    int8_t udmabuf_fd;
    int32_t i;
    
    /**********************************************************************/
    /* Inference preparation                                              */
    /**********************************************************************/
    struct timeval tv;
    drpai_status_t drpai_status;

    printf("Inference -----------------------------------------------\n");

    /* Allocate the img_buffer in udmabuf memory area */
    errno = 0;
    udmabuf_fd = open("/dev/udmabuf0", O_RDWR );
    if (0 > udmabuf_fd)
    {
        fprintf(stderr, "[ERROR] Failed to open udmabuf: errno=%d\n", errno);
        ret = -1;
        goto end_close_drpai;
    }
    img_buffer =(uint8_t *) mmap(NULL, drpai_address.data_in_size ,PROT_READ|PROT_WRITE, MAP_SHARED,  udmabuf_fd, 0);

    if (MAP_FAILED == img_buffer)
    {
        fprintf(stderr, "[ERROR] Failed to mmap udmabuf memory area: errno=%d\n", errno);
        ret = -1;
        goto end_close_udmabuf;
    }
    /* Write once to allocate physical memory to u-dma-buf virtual space.
    * Note: Do not use memset() for this.
    *       Because it does not work as expected. */
    {
        for (i = 0 ; i < drpai_address.data_in_size; i++)
        {
            img_buffer[i] = 0;
        }
    }

    memcpy(img_buffer, frame.data, sizeof(uint8_t)*DRPAI_IN_WIDTH*DRPAI_IN_HEIGHT*DRPAI_IN_CHANNEL_BGR);

    proc[DRPAI_INDEX_INPUT].address       = udmabuf_address;
    /**********************************************************************
    * START Inference
    **********************************************************************/
    printf("[START] DRP-AI\n");
    errno = 0;
    ret = ioctl(drpai_fd, DRPAI_START, &proc[0]);
    if (0 != ret)
    {
        fprintf(stderr, "[ERROR] Failed to run DRPAI_START: errno=%d\n", errno);
        ret = -1;
        goto end_munmap_udmabuf;
    }

    /**********************************************************************
    * Wait until the DRP-AI finish (Thread will sleep)
    **********************************************************************/
    FD_ZERO(&rfds);
    FD_SET(drpai_fd, &rfds);
    tv.tv_sec = DRPAI_TIMEOUT;
    tv.tv_usec = 0;

    ret_drpai = select(drpai_fd+1, &rfds, NULL, NULL, &tv);
    if (0 == ret_drpai)
    {
        fprintf(stderr, "[ERROR] DRP-AI select() Timeout : errno=%d\n", errno);
        ret = -1;
        goto end_munmap_udmabuf;
    }
    else if (-1 == ret_drpai)
    {
        fprintf(stderr, "[ERROR] DRP-AI select() Error : errno=%d\n", errno);
        ret_drpai = ioctl(drpai_fd, DRPAI_GET_STATUS, &drpai_status);
        if (-1 == ret_drpai)
        {
            fprintf(stderr, "[ERROR] Failed to run DRPAI_GET_STATUS : errno=%d\n", errno);
        }
        ret = -1;
        goto end_munmap_udmabuf;
    }
    else
    {
        /*Do nothing*/
    }

    if (FD_ISSET(drpai_fd, &rfds))
    {
        errno = 0;
        ret_drpai = ioctl(drpai_fd, DRPAI_GET_STATUS, &drpai_status);
        if (-1 == ret_drpai)
        {
            fprintf(stderr, "[ERROR] Failed to run DRPAI_GET_STATUS : errno=%d\n", errno);
            ret = -1;
            goto end_munmap_udmabuf;
        }
        printf("[END] DRP-AI\n");
    }


    /**********************************************************************
    * CPU Post-processing
    **********************************************************************/
    /* Get the output data from memory */
    ret = get_result(drpai_fd, drpai_address.data_out_addr, drpai_address.data_out_size);
    if (0 != ret)
    {
        fprintf(stderr, "[ERROR] Failed to get result from memory.\n");
        ret = -1;
        goto end_munmap_udmabuf;
    }

    /* Compute the classification result and display it on console */
    ret = print_result_yolo(drpai_output_buf, &frame);
    if (0 != ret)
    {
        fprintf(stderr, "[ERROR] Failed to run CPU Post Processing.\n");
        ret = -1;
        goto end_munmap_udmabuf;
    }

    /* Terminating process */
end_munmap_udmabuf:
    munmap(img_buffer, drpai_address.data_in_size);
    goto end_close_udmabuf;

end_close_udmabuf:
    close(udmabuf_fd);
    goto end_close_drpai;

end_close_drpai:
    errno = 0;
    ret = close(drpai_fd);
    if (0 != ret)
    {
        fprintf(stderr, "[ERROR] Failed to close DRP-AI Driver: errno=%d\n", errno);
        ret = -1;
    }
    goto end_main;

end_main:
    return ret;
}

int32_t main(int32_t argc, char * argv[])
{
    int val =0;
    
    if(!cap.isOpened()){
        cout << "Error opening video file" << endl;
        return -1;
    }

    cout << "Opened video file successfully." << endl;

    load_drpai();

    /* Checking the of arguments passed*/
    if(argc>=1)
    {
       sel_aud  = std::atoi(argv[1]);
      }
    else{
        std::cout<<"Give all command line args\n";
        return EXIT_SUCCESS;
    }

    /*Load Label from label_list file*/
    label_file_map = load_label_file(label_list);
    if (label_file_map.empty())
    {
        fprintf(stderr,"[ERROR] Failed to load label file: %s\n", label_list.c_str());
        return -1;
    }

    while(1){

        Mat frame;
        /*Capture frame-by-frame*/
        cap >> frame;
        val++;
    
        /* If the frame is empty, break immediately*/
        if (frame.empty())
        break;
    
        /* Display the resulting frame */
        load_drpai();
    	get_video_inf(frame);
    	cv::putText(frame, label.c_str(), cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 0, 255), 2);
    	label = " ";
    	video.write(frame);
        imshow( "Frame", frame );
        /* Press  ESC on keyboard to exit*/
        char c=(char)waitKey(25);
        if(c==27)
        break;
    }
    cap.release();
    video.release();
 
    /* Closes all the frames*/
    destroyAllWindows();

    return 0;
}
