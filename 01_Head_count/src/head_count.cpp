/*
 * Original Code (C) Copyright Edgecortix, Inc. 2022
 * Modified Code (C) Copyright Renesas Electronics Corporation 2023
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
* File Name    : head_count.cpp
* Version      : 1.1.0
* Description  : DRP-AI TVM[*1] Application Example
***********************************************************************************************************************/

/*****************************************
* includes
******************************************/
#include "define.h"
#include "box.h"
#include "MeraDrpRuntimeWrapper.h"
#include "PreRuntime.h"
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


/* DRP-AI TVM[*1] Runtime object */
MeraDrpRuntimeWrapper runtime;

/*Global Variables*/
static float drpai_output_buf[INF_OUT_SIZE];

std::vector<float> floatarr(1);

uint64_t drpaimem_addr_start = 0;
bool runtime_status = false; 
static vector<detection> det;

float fps = 0;

/*Global frame */
Mat g_frame;
VideoCapture cap;

/* Map to store input source list */
std::map<std::string, int> input_source_map ={    
    {"IMAGE", 1},
    {"MIPI", 2},
    {"USB", 3}
    } ;


/*****************************************
 * Function Name     : float16_to_float32
 * Description       : Function by Edgecortex. Cast uint16_t a into float value.
 * Arguments         : a = uint16_t number
 * Return value      : float = float32 number
 ******************************************/
float float16_to_float32(uint16_t a)
{
    return __extendXfYf2__<uint16_t, uint16_t, 10, float, uint32_t, 23>(a);
}

/*****************************************
 * Function Name     : load_label_file
 * Description       : Load label list text file and return the label list that contains the label.
 * Arguments         : label_file_name = filename of label list. must be in txt format
 * Return value      : vector<string> list = list contains labels
 *                     empty if error occurred
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
    while (getline(infile, line))
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
 * Function Name : sigmoid
 * Description   : Helper function for YOLO Post Processing
 * Arguments     : x = input argument for the calculation
 * Return value  : sigmoid result of input x
 ******************************************/
double sigmoid(double x)
{
    return 1.0 / (1.0 + exp(-x));
}

/*****************************************
* Function Name : yolo_index
* Description   : Get the index of the bounding box attributes based on the input offset
* Arguments     : n = output layer number
                  offs = offset to access the bounding box attributes
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
* Description   : Get the offset number to access the bounding box attributes
*                 To get the actual value of bounding box attributes, use yolo_index() after this function.
* Arguments     : n = output layer number [0~2].
                  b = Number to indicate which bounding box in the region [0~4]
*                 y = Number to indicate which region [0~13]
*                 x = Number to indicate which region [0~13]
* Return value  : offset to access the bounding box attributes.
******************************************/
int32_t yolo_offset(uint8_t n, int32_t b, int32_t y, int32_t x)
{

    uint8_t num = num_grids[n];
    uint32_t prev_layer_num = 0;
    int32_t i = 0;

    for (i = 0; i < n; i++)
    {
        prev_layer_num += NUM_BB * (NUM_CLASS + 5) * num_grids[i] * num_grids[i];
    }
    return prev_layer_num + b * (NUM_CLASS + 5) * num * num + y * num + x;
}


 /*****************************************
 * Function Name : tvm_inference
 * Description   : Function to run tvm inference
 * Arguments     : frame
 * Return value  : 0 if succeeded
 *               not 0 otherwise
 ******************************************/
int tvm_inference(Mat& frame)
{
    /*start inference using drp runtime*/
    runtime.SetInput(0, frame.ptr<float>());
    runtime.Run();

    /*load inference out on drpai_out_buffer*/
    int8_t ret = 0;
    int32_t i = 0;
    int32_t output_num = 0;
    std::tuple<InOutDataType, void *, int64_t> output_buffer;
    int64_t output_size;
    uint32_t size_count = 0;

    /* Get the number of output of the target model. */
    output_num = runtime.GetNumOutput();
    size_count = 0;
    /*GetOutput loop*/
    for (i = 0; i < output_num; i++)
    {
        /* output_buffer below is tuple, which is { data type, address of output data, number of elements } */
        output_buffer = runtime.GetOutput(i);
        /*Output Data Size = std::get<2>(output_buffer). */
        output_size = std::get<2>(output_buffer);

        /*Output Data Type = std::get<0>(output_buffer)*/
        if (InOutDataType::FLOAT16 == std::get<0>(output_buffer))
        {
            /*Output Data = std::get<1>(output_buffer)*/
            uint16_t *data_ptr = reinterpret_cast<uint16_t *>(std::get<1>(output_buffer));
            for (int j = 0; j < output_size; j++)
            {
                /*FP16 to FP32 conversion*/
                drpai_output_buf[j + size_count] = float16_to_float32(data_ptr[j]);
            }
        }
        else if (InOutDataType::FLOAT32 == std::get<0>(output_buffer))
        {
            /*Output Data = std::get<1>(output_buffer)*/
            float *data_ptr = reinterpret_cast<float *>(std::get<1>(output_buffer));
            for (int j = 0; j < output_size; j++)
            {
                drpai_output_buf[j + size_count] = data_ptr[j];
            }
        }
        else
        {
            fprintf(stderr, "[ERROR] Output data type : not floating point.\n", errno);
            ret = -1;
            break;
        }
        size_count += output_size;
    }return ret;
}

/*****************************************
 * Function Name : R_Post_Proc
 * Description   : Process CPU post-processing for YOLOv3
 * Arguments     : floatarr = drpai output address
 * Return value  : -
 ******************************************/
void R_Post_Proc(float *floatarr)
{
    /* Following variables are required for correct_region_boxes in Darknet implementation*/
    /* Note: This implementation refers to the "darknet detector test" */
    
    float new_w, new_h;
    float correct_w = 1.;
    float correct_h = 1.;
    if ((float)(MODEL_IN_W / correct_w) < (float)(MODEL_IN_H / correct_h))
    {
        new_w = (float)MODEL_IN_W;
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
    /* Clear the detected result list */
    det.clear();

    for (n = 0; n < NUM_INF_OUT_LAYER; n++)
    {
        num_grid = num_grids[n];
        anchor_offset = 2 * NUM_BB * (NUM_INF_OUT_LAYER - (n + 1));

        for (b = 0; b < NUM_BB; b++)
        {
            for (y = 0; y < num_grid; y++)
            {
                for (x = 0; x < num_grid; x++)
                {
                    offs = yolo_offset(n, b, y, x);
                    tx = floatarr[offs];
                    ty = floatarr[yolo_index(n, offs, 1)];
                    tw = floatarr[yolo_index(n, offs, 2)];
                    th = floatarr[yolo_index(n, offs, 3)];
                    tc = floatarr[yolo_index(n, offs, 4)];

                    /* Compute the bounding box */
                    /*get_region_box*/
                    center_x = ((float)x + sigmoid(tx)) / (float)num_grid;
                    center_y = ((float)y + sigmoid(ty)) / (float)num_grid;

                    box_w = (float)exp(tw) * anchors[anchor_offset + 2 * b + 0] / (float)MODEL_IN_W;
                    box_h = (float)exp(th) * anchors[anchor_offset + 2 * b + 1] / (float)MODEL_IN_W;
                    /* Adjustment for VGA size */
                    /* correct_region_boxes */
                    center_x = (center_x - (MODEL_IN_W - new_w) / 2. / MODEL_IN_W) / ((float)new_w / MODEL_IN_W);
                    center_y = (center_y - (MODEL_IN_H - new_h) / 2. / MODEL_IN_H) / ((float)new_h / MODEL_IN_H);
                    box_w *= (float)(MODEL_IN_W / new_w);
                    box_h *= (float)(MODEL_IN_H / new_h);

                    center_x = round(center_x * DRPAI_IN_WIDTH);
                    center_y = round(center_y * DRPAI_IN_HEIGHT);
                    box_w = round(box_w * DRPAI_IN_WIDTH);
                    box_h = round(box_h * DRPAI_IN_HEIGHT);

                    objectness = sigmoid(tc);

                    Box bb = {center_x, center_y, box_w, box_h};

                    /* Get the class prediction associated with each BB  [5: ] */
                    for (i = 0; i < NUM_CLASS; i++)
                    {
                        classes[i] = sigmoid(floatarr[yolo_index(n, offs, 5 + i)]);
                    }

                    max_pred = 0;
                    pred_class = -1;
                    /*Get the predicted class */
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
                    if (probability > TH_PROB)
                    {
                        d = {bb, pred_class, probability};
                        det.push_back(d);
                    }
                }
            }
        }
    }

    /* Non-Maximum Suppression filter */
    filter_boxes_nms(det, det.size(), TH_NMS);
    
    return;
}

/*****************************************
 * Function Name : draw_bounding_box
 * Description   : Draw bounding box on image.
 * Arguments     : -
 * Return value  : 0 if succeeded
 *               not 0 otherwise
 ******************************************/
void draw_bounding_box(void)
{
    stringstream stream;
    string str = "";
    string result_str;
    int32_t result_cnt =0;
    uint32_t x = 60;
    uint32_t y = 30;
    /* Draw bounding box on RGB image. */
    int32_t i = 0;
    for (i = 0; i < det.size(); i++)
    {
        /* Skip the overlapped bounding boxes */
        if (det[i].prob == 0)
        {
            continue;
        }
        result_cnt++;
        /* Clear string stream for bounding box labels */
        stream.str("");
        /* Draw the bounding box on the image */
        stream << fixed << setprecision(2) << det[i].prob;
        result_str = label_file_map[det[i].c] + " " + stream.str();    
           
        int32_t x_min = (int)det[i].bbox.x - round((int)det[i].bbox.w / 2.);
        int32_t y_min = (int)det[i].bbox.y - round((int)det[i].bbox.h / 2.);
        int32_t x_max = (int)det[i].bbox.x + round((int)det[i].bbox.w / 2.) - 1;
        int32_t y_max = (int)det[i].bbox.y + round((int)det[i].bbox.h / 2.) - 1;

        /* Check the bounding box is in the image range */
        x_min = x_min < 1 ? 1 : x_min;
        x_max = ((DRPAI_IN_WIDTH - 2) < x_max) ? (DRPAI_IN_WIDTH - 2) : x_max;
        y_min = y_min < 1 ? 1 : y_min;
        y_max = ((DRPAI_IN_HEIGHT - 2) < y_max) ? (DRPAI_IN_HEIGHT - 2) : y_max;

        Point topLeft(x_min, y_min);
        Point bottomRight(x_max, y_max);

        /* Creating bounding box and class labels */
        rectangle(g_frame, topLeft, bottomRight, Scalar(0, 0, 0), CHAR_SCALE_SMALL);
        putText(g_frame, result_str, topLeft, FONT_HERSHEY_SIMPLEX, CHAR_SCALE_XS, Scalar(0, 255, 0), CHAR_THICKNESS);
    }
    stream.str("");
    stream << "Head Count: " << result_cnt;
    str = stream.str();
    putText(g_frame, str,Point(x, y), FONT_HERSHEY_SIMPLEX, CHAR_SCALE_SMALL, Scalar(255, 255, 255), CHAR_THICKNESS);

    return;
}


/*****************************************
 * Function Name : Head Detection
 * Description   : Function to perform over all detection
 * Arguments     : -
 * Return value  : 0 if succeeded
 *               not 0 otherwise
 ******************************************/
int Head_Detection()
{   
     /* Temp frame */
    Mat frame1;

    Size size(MODEL_IN_H, MODEL_IN_W);

    /*resize the image to the model input size*/
    resize(g_frame, frame1, size);

    /* changing channel from hwc to chw */
    vector<Mat> rgb_images;
    split(frame1, rgb_images);
    Mat m_flat_r = rgb_images[0].reshape(1, 1);
    Mat m_flat_g = rgb_images[1].reshape(1, 1);
    Mat m_flat_b = rgb_images[2].reshape(1, 1);
    Mat matArray[] = {m_flat_r, m_flat_g, m_flat_b};
    Mat frameCHW;
    hconcat(matArray, 3, frameCHW);
    /*convert to FP32*/
    frameCHW.convertTo(frameCHW, CV_32FC3);

    /* normailising  pixels */
    divide(frameCHW, 255.0, frameCHW);

    /* DRP AI input image should be continuous buffer */
    if (!frameCHW.isContinuous())
        frameCHW = frameCHW.clone();

    Mat frame = frameCHW;
    int ret = 0;

    /* Inference start time */
    auto t1 = std::chrono::high_resolution_clock::now();

    ret = tvm_inference(frame);

    if (ret != 0)
    {
        fprintf(stderr, "[Error] DRP Inference Not working !!! ");
        return -1;
    }

    auto t2 = std::chrono::high_resolution_clock::now();
    auto inf_duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
   
   /* Do post process to get bounding boxes */
    R_Post_Proc(drpai_output_buf);
    auto t3 = std::chrono::high_resolution_clock::now();
    auto post_duration = std::chrono::duration_cast<std::chrono::milliseconds>(t3 - t2).count();

    float total_time = float(inf_duration) + float(post_duration);
    /*Calculating the fps*/
    fps = (1000.0/float(total_time));
    return 0;
}

/*****************************************
 * Function Name : capture_frame
 * Description   : function to open camera gstreamer pipeline.
 * Arguments     : string cap_pipeline input pipeline
 ******************************************/
void capture_frame(std::string gstreamer_pipeline )
{
    stringstream stream;
    string str = "";

    int wait_key;
    /* Capture stream of frames from camera using Gstreamer pipeline */
    cap.open(gstreamer_pipeline, CAP_GSTREAMER);
    if (!cap.isOpened())
    {
        std::cout << "[ERROR] Error opening video stream or camera !\n" << std::endl;
        return;
    }
    while (true)
    {
        cap >> g_frame;
        if (g_frame.empty())
        {
            std::cout << "[INFO] Video ended or corrupted frame !\n";
            return;
        }
        else
        {
            resize(g_frame, g_frame, Size(IMAGE_WIDTH, IMAGE_HEIGHT));
            int ret = Head_Detection();
            if (ret != 0)
            {
                fprintf(stderr, "[Error] Inference Not working !!! ");
            }

            /* Draw bounding box on the frame */
            draw_bounding_box();
            /*Display frame */
            stream.str("");
            stream << "FPS: " << fixed << setprecision(2)<< fps;
            str = stream.str();
            putText(g_frame, str,Point(500, 50), FONT_HERSHEY_SIMPLEX, 
                        CHAR_SCALE_SMALL, Scalar(0, 255, 0), CHAR_THICKNESS);
            imshow("Output Image", g_frame);
            wait_key = waitKey(30); 
            if (wait_key == 27) /* If 'Esc' is entered break the loop */
                break;
        }
    }
    cap.release(); 
    destroyAllWindows();
    return;
}

/*****************************************
* Function Name : get_drpai_start_addr
* Description   : Function to get the start address of DRPAImem.
* Arguments     : -
* Return value  : uint32_t = DRPAImem start address in 32-bit.
******************************************/
uint32_t get_drpai_start_addr()
{
    int fd  = 0;
    int ret = 0;
    drpai_data_t drpai_data;

    errno = 0;

    fd = open("/dev/drpai0", O_RDWR);
    if (0 > fd )
    {
        LOG(FATAL) << "[ERROR] Failed to open DRP-AI Driver : errno=" << errno;
        return NULL;
    }

    /* Get DRP-AI Memory Area Address via DRP-AI Driver */
    ret = ioctl(fd , DRPAI_GET_DRPAI_AREA, &drpai_data);
    if (-1 == ret)
    {
        LOG(FATAL) << "[ERROR] Failed to get DRP-AI Memory Area : errno=" << errno ;
        return (uint32_t)NULL;
    }

    return drpai_data.address;
}


int main(int argc, char *argv[])
{
    std::cout << "Starting Head Count Application" << std::endl;
    
    if (argc>3)
    {
        std::cerr << "Wrong number Arguments are passed \n";
        return -1;
    }

  
    /*Load Label from label_list file*/
    label_file_map = load_label_file(label_list);
    /*Load model_dir structure and its weight to runtime object */
    drpaimem_addr_start = get_drpai_start_addr();
    if (drpaimem_addr_start == (uint64_t)NULL)
    {
        /* Error notifications are output from function get_drpai_start_addr(). */
	    fprintf(stderr, "[ERROR] Failed to get DRP-AI memory area start address. \n");
        return -1;
    }
    runtime_status = runtime.LoadModel(model_dir, drpaimem_addr_start + DRPAI_MEM_OFFSET);
    
    if(!runtime_status)
    {
        fprintf(stderr, "[ERROR] Failed to load model. \n");
        return -1;
    }    

    std::cout << "[INFO] loaded runtime model :" << model_dir << "\n\n";
    /* Get input Source IMAGE/VIDEO/CAMERA */
    std::string input_source = argv[1];
    switch (input_source_map[input_source])
    {
        /* Input Source : IMAGE*/
        case 1:
        {
            std::cout << "Image_path: " << argv[2] << std::endl;
            // read frame
            g_frame = imread(argv[2]);

            /* If empty frame */
            if (g_frame.empty())
            {
                std::cout << "Failed to load image!" << std::endl;
                return -1;
            }
            resize(g_frame, g_frame, Size(IMAGE_WIDTH, IMAGE_HEIGHT));
            int ret = Head_Detection();

            if (ret != 0)
            {
                fprintf(stderr, "[Error] Inference Not working !!! ");
                return -1;
            }
            draw_bounding_box();
            imshow("Output Image", g_frame);
            waitKey(0);
            destroyAllWindows();
        }
        break;
        /* Input Source : MIPI*/
        case 2:{
            std::cout << "[INFO] MIPI CAMERA \n";
            std::string gstreamer_pipeline = "v4l2src device=/dev/video0 ! videoconvert ! appsink";
            int ret = 0;
            std::cout<<"[INFO] MIPI CAM Init \n";
            const char* commands[4] =
            {
                "media-ctl -d /dev/media0 -r",
                "media-ctl -d /dev/media0 -V \"\'ov5645 0-003c\':0 [fmt:UYVY8_2X8/640x480 field:none]\"",
                "media-ctl -d /dev/media0 -l \"\'rzg2l_csi2 10830400.csi2\':1 -> \'CRU output\':0 [1]\"",
                "media-ctl -d /dev/media0 -V \"\'rzg2l_csi2 10830400.csi2\':1 [fmt:UYVY8_2X8/640x480 field:none]\""
            };

            /* media-ctl command */
            for (int i=0; i<4; i++)
            {
                std::cout<<commands[i]<<"\n";
                ret = system(commands[i]);
                std::cout<<"system ret = "<<ret<<"\n";
                if (ret<0)
                {
                    std::cout<<"[ERROR]"<<__func__<<": failed media-ctl commands. index = "<<i<<"\n";
                    return -1;
                }
            }
            capture_frame(gstreamer_pipeline);
        }
        break;
        /* Input Source : USB*/
        case 3:{
            std::cout << "[INFO] USB CAMERA \n";
            std::string gstreamer_pipeline = "v4l2src device=/dev/video1 ! videoconvert ! appsink";
            capture_frame(gstreamer_pipeline);
        }
        break;
        default:
            std::cout<<"Choose the correct option."<<std::endl;
    }
    /* Exit the program */
    return 0;

}
