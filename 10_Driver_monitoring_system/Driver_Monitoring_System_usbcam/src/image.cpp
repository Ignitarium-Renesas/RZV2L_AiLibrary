/***********************************************************************************************************************
* DISCLAIMER
* This software is suheadied by Renesas Electronics Corporation and is only intended for use with Renesas products. No
* other uses are authorized. This software is owned by Renesas Electronics Corporation and is protected under all
* aheadicable laws, including copyright laws.
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
* File Name    : image.cpp
* Version      : 7.20
* Description  : RZ/V2L DRP-AI Driver Monitoring Sample application using DeepPose with TinyYOLOv2 USB Camera version
***********************************************************************************************************************/

/*****************************************
* Includes
******************************************/
#include "image.h"
#include <opencv2/opencv.hpp>

Image::Image()
{

}


Image::~Image()
{
    int32_t i;
    
    for (i = 0; i < WL_BUF_NUM; i++)
    {
        munmap(img_buffer[i], img_w * img_h * img_c);
    }
    close(udmabuf_fd);
}

void Image::flip()
{
    /*OpenCV image data is in BGRA */
    cv::Mat bgra_image(out_h, out_w, CV_8UC4, img_buffer[buf_id]);
    cv::flip(bgra_image, bgra_image, 1);
}

/*****************************************
* Function Name : get_H
* Description   : Function to get the image height
*                 This function is NOT used currently.
* Arguments     : -
* Return value  : img_h = current image height
******************************************/
uint32_t Image::get_H()
{
    return img_h;
}


/*****************************************
* Function Name : get_W
* Description   : Function to get the image width
*                 This function is NOT used currently.
* Arguments     : -
* Return value  : img_w = current image width
******************************************/
uint32_t Image::get_W()
{
    return img_w;
}


/*****************************************
* Function Name : get_C
* Description   : Function to set the number of image channel
*                 This function is NOT used currently.
* Arguments     : c = new number of image channel to be set
* Return value  : -
******************************************/
uint32_t Image::get_C()
{
    return img_c;
}


/*****************************************
* Function Name : init
* Description   : Function to initialize img_buffer in Image class
*                 This aheadication uses udmabuf in order to
*                 continuous memory area for DRP-AI input data
* Arguments     : w = input image width in YUYV
*                 h = input image height in YUYV
*                 c = input image channel in YUYV
*                 ow = output image width in BGRA to be displayed via Wayland
*                 oh = output image height in BGRA to be displayed via Wayland
*                 oc = output image channel in BGRA to be displayed via Wayland
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
uint8_t Image::init(uint32_t w, uint32_t h, uint32_t c,
                    uint32_t ow, uint32_t oh, uint32_t oc)
{
    int32_t i;
    int32_t j;
    
    /*Initialize input image information */
    img_w = w;
    img_h = h;
    img_c = c;
    /*Initialize output image information*/
    out_w = ow;
    out_h = oh;
    out_c = oc;

    uint32_t out_size = out_w * out_h * out_c;
    udmabuf_fd = open("/dev/udmabuf0", O_RDWR );
    if (0 > udmabuf_fd)
    {
        return -1;
    }
    for (i = 0; i < WL_BUF_NUM; i++)
    {
        img_buffer[i] =(unsigned char*) mmap(NULL, out_size ,PROT_READ|PROT_WRITE, MAP_SHARED,  udmabuf_fd, UDMABUF_OFFSET + i*out_size);
        if (MAP_FAILED == img_buffer[i])
        {
            return -1;
        }
        /* Write once to allocate physical memory to u-dma-buf virtual space.
        * Note: Do not use memset() for this.
        *       Because it does not work as expected. */
        {
            for(j = 0 ; j < out_size; j++)
            {
                img_buffer[i][j] = 0;
            }
        }
    }
    return 0;
}


/*****************************************
* Function Name : write_string_rgb
* Description   : OpenCV putText() in RGB
* Arguments     : str = string to be drawn
*                 x = bottom left coordinate X of string to be drawn
*                 y = bottom left coordinate Y of string to be drawn
*                 scale = scale for letter size
*                 color = letter color must be in RGB, and border color is complementary
*                         e.g. white = 0xFFFFFF, and border color is black = 0x000000
*                         caution: if all R,G,B value is close to 0x80, the border is unclear
* Return Value  : -
******************************************/
void Image::write_string_rgb(std::string str, uint32_t x, uint32_t y, float scale, uint32_t color)
{
    uint8_t thickness = CHAR_THICKNESS;
    /*Extract RGB information*/
    uint8_t r = (color >> 16) & 0x0000FF;
    uint8_t g = (color >>  8) & 0x0000FF;
    uint8_t b = color & 0x0000FF;
    /*OpenCV image data is in BGRA */
    cv::Mat bgra_image(out_h, out_w, CV_8UC4, img_buffer[buf_id]);
    /*Color must be in BGR order*/
    cv::putText(bgra_image, str.c_str(), cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar(b^0xFF, g^0xFF, r^0xFF), thickness+4);
    cv::putText(bgra_image, str.c_str(), cv::Point(x, y), cv::FONT_HERSHEY_SIMPLEX, scale, cv::Scalar(b, g, r), thickness);
}

/*****************************************
* Function Name : draw_point_yuyv
* Description   : Draw a single point on YUYV image
* Arguments     : x = X coordinate to draw a point
*                 y = Y coordinate to draw a point
*                 color = point color
* Return Value  : -
******************************************/
void Image::draw_point_yuyv(int32_t x, int32_t y, uint32_t color)
{
    uint32_t x0 = (uint32_t)x & ~0x1;

    uint32_t draw_u = (color >> 8) & 0xFF;
    uint32_t draw_v = (color >> 0) & 0xFF;

    uint32_t target_u = img_buffer[buf_id][(y * img_w + x0) * img_c + 1];
    uint32_t target_v = img_buffer[buf_id][(y * img_w + x0) * img_c + 3];

    img_buffer[buf_id][(y * img_w + x0) * img_c + 1] = (draw_u + target_u) / 2;
    img_buffer[buf_id][(y * img_w + x0) * img_c + 3] = (draw_v + target_v) / 2;

    if ((x & 1) == 0)
    {
        img_buffer[buf_id][(y * img_w + x0) * img_c]     = (color >> 16) & 0xFF;
    }
    else
    {
        img_buffer[buf_id][(y * img_w + x0) * img_c + 2] = (color >> 16) & 0xFF;
    }
    return;
}

/*****************************************
* Function Name : draw_line
* Description   : Draw a single line
* Arguments     : x0 = X coordinate of a starting point
*                 y0 = Y coordinate of a starting point
*                 x1 = X coordinate of a end point
*                 y1 = Y coordinate of a end point
*                 color = line color
* Return Value  : -
******************************************/
void Image::draw_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
{
    int32_t dx = x1 - x0;
    int32_t dy = y1 - y0;
    int32_t sx = 1;
    int32_t sy = 1;
    float de;
    int32_t i;

    /* Change direction */
    if (0 > dx)
    {
        dx *= -1;
        sx *= -1;
    }

    if (0 > dy)
    {
        dy *= -1;
        sy *= -1;
    }

    draw_point_yuyv(x0, y0, color);

    if (dx > dy)
    {
        /* Horizontal Line */
        for (i = dx, de = i / 2; i; i--)
        {
            x0 += sx;
            de += dy;
            if(de > dx)
            {
                de -= dx;
                y0 += sy;
            }
            draw_point_yuyv(x0, y0, color);
        }
    }
    else
    {
        /* Vertical Line */
        for (i = dy, de = i / 2; i; i--)
        {
            y0 += sy;
            de += dx;
            if(de > dy)
            {
                de -= dy;
                x0 += sx;
            }
            draw_point_yuyv(x0, y0, color);
        }
    }
    return;
}


/*****************************************
* Function Name : draw_line2
* Description   : Draw a dowble line
* Arguments     : x0 = X coordinate of a starting point
*                 y0 = Y coordinate of a starting point
*                 x1 = X coordinate of a end point
*                 y1 = Y coordinate of a end point
*                 color = line color
* Return Value  : -
******************************************/
void Image::draw_line2(int32_t x0, int32_t y0, int32_t x1, int32_t y1, uint32_t color)
{
    int32_t x_min = x0;
    int32_t y_min = y0;
    int32_t x_max = x1;
    int32_t y_max = y1;
    
    draw_line(x_min, y_min, x_max, y_max, color);
    draw_line(x_min-1, y_min-1, x_max+1, y_max-1, color);
    return;
}


/*****************************************
* Function Name : draw_rect
* Description   : Draw a rectangle
* Arguments     : x = X coordinate at the top left of the rectangle
*                 y = Y coordinate at the top left of the rectangle
*                 w = width of the rectangle
*                 h = height of the rectangle
* Return Value  : -
******************************************/
void Image::draw_rect(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
    int32_t x_min = x;
    int32_t y_min = y;
    int32_t x_max = x + w;
    int32_t y_max = y + h;
    /* Check the bounding box is in the image range */
    x_min = x_min < 1 ? 1 : x_min;
    x_max = ((img_w - 2) < x_max) ? (img_w - 2) : x_max;
    y_min = y_min < 1 ? 1 : y_min;
    y_max = ((img_h - 2) < y_max) ? (img_h - 2) : y_max;

    /* Draw the bounding box */
    draw_line(x_min, y_min, x_max, y_min, color);
    draw_line(x_max, y_min, x_max, y_max, color);
    draw_line(x_max, y_max, x_min, y_max, color);
    draw_line(x_min, y_max, x_min, y_min, color);
    draw_line(x_min-1, y_min-1, x_max+1, y_min-1, color);
    draw_line(x_max+1, y_min-1, x_max+1, y_max+1, color);
    draw_line(x_max+1, y_max+1, x_min-1, y_max+1, color);
    draw_line(x_min-1, y_max+1, x_min-1, y_min-1, color);

    return;
}


/*****************************************
* Function Name : convert_format
* Description   : Convert YUYV image to BGRA format
* Arguments     : -
* Return value  : -
******************************************/
void Image::convert_format()
{
    cv::Mat yuyv_image(img_h, img_w, CV_8UC2, img_buffer[buf_id]);
    cv::Mat bgra_image;
    cv::Mat out_image(img_h, img_w, CV_8UC4, img_buffer[buf_id]);
    cv::cvtColor(yuyv_image, bgra_image, cv::COLOR_YUV2BGRA_YUYV);
    memcpy(out_image.data, bgra_image.data, img_w * img_h * out_c);
}


/*****************************************
* Function Name : convert_size
* Description   : Scale up the input data (640x480) to the intermediate data (960x720) using OpenCV.
*                 To convert to the final output size (1280x720), fill the right margin of the
*                 intermediate data (960x720) with black.
* Arguments     : -
* Return value  : -
******************************************/
void Image::convert_size()
{
    cv::Mat org_image(img_h, img_w, CV_8UC4, img_buffer[buf_id]);
    cv::Mat resize_image;
    uint32_t resized_width = img_w * RESIZE_SCALE;
    /* Resize: 640x480 to 960x720 */
    cv::resize(org_image, resize_image, cv::Size(), RESIZE_SCALE, RESIZE_SCALE);

    /* Padding: 960x720 to 1280x720 */
    uint8_t *dst = img_buffer[buf_id];
    uint8_t *src = resize_image.data;
    int32_t i;
    int32_t j;
    
    for(i = 0; i < out_h; i++)
    {
        memcpy(dst, src, resized_width * out_c);
        dst += (resized_width * out_c);
        src += (resized_width * out_c);
        for(j = 0; j < (out_w - resized_width) * out_c; j+=out_c) {
            dst[j]   = 0; //B
            dst[j+1] = 0; //G
            dst[j+2] = 0; //R
            dst[j+3] = 0xff; //A
        }
        dst += ((out_w - resized_width) * out_c);
    }
}

/*****************************************
* Function Name : camera_to_image
* Description   : Function to copy the external image buffer data to img_buffer
*                 This is only place where the buf_id is updated.
* Arguments     : buffer = buffer to copy the image data
*                 size = size of buffer
* Return value  : none
******************************************/
void Image::camera_to_image(const uint8_t* buffer, int32_t size)
{
    /* Update buffer id */
    buf_id = ++buf_id % WL_BUF_NUM;
    memcpy(img_buffer[buf_id], buffer, sizeof(uint8_t)*size);
}


/*****************************************
* Function Name : at
* Description   : Get the value of img_buffer at index a.
*                 This function is NOT used currently.
* Arguments     : a = index of img_buffer
* Return Value  : value of img_buffer at index a
******************************************/
uint8_t Image::at(int32_t a)
{
    return img_buffer[buf_id][a];
}

/*****************************************
* Function Name : set
* Description   : Set the value of img_buffer at index a.
*                 This function is NOT used currently.
* Arguments     : a = index of img_buffer
*                 val = new value to be set
* Return Value  : -
******************************************/
void Image::set(int32_t a, uint8_t val)
{
    img_buffer[buf_id][a] = val;
    return;
}
/*****************************************
* Function Name : get_buf_id
* Description   : Get the value of the buf_id.
* Arguments     : -
* Return Value  : value of buf_id-
******************************************/
uint8_t Image::get_buf_id(void)
{
    return buf_id;
}
