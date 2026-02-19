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
* Copyright (C) 2026 Renesas Electronics Corporation. All rights reserved.
***********************************************************************************************************************/
/***********************************************************************************************************************
* File Name    : camera.h
* Version      : 7.00
* Description  : RZ/V2L DRP-AI Sample Application for  ResNet-18 with TinyYOLOv2 Camera version
***********************************************************************************************************************/

#ifndef CAMERA_H
#define CAMERA_H

#include <linux/videodev2.h>
#include "define.h"
/* This block of code is only accessible from C code. */
#ifdef __cplusplus
extern "C" {
#endif
#include "mmngr_user_public.h"
#include "mmngr_buf_user_public.h"
#ifdef __cplusplus
}
#endif

class Camera
{
    public:
        Camera();
        ~Camera();

        struct camera_dma_buffer
        {
         /* The index of the buffer. */
         uint32_t idx;
         /* The file descriptor for the DMA buffer. */
         uint32_t dbuf_fd;
         /* The size of the buffer in bytes. */
         uint32_t size;
         /* The physical address of DMA buffer. */
         uint32_t phy_addr;
         /* The pointer to the memory for the buffer. */
         void *mem;           
        };
        struct camera_dma_buffer *wayland_buf;
        struct camera_dma_buffer *drpai_buf;
        int8_t start_camera();
        int8_t capture_qbuf();
        uint64_t capture_image();
        int8_t close_camera();
        int8_t save_bin(std::string filename);
        int video_buffer_flush_dmabuf(uint32_t idx, uint32_t size);
        uint8_t * get_img();
        int32_t get_size();
        int32_t get_w();
        void set_w(int32_t w);
        int32_t get_h();
        void set_h(int32_t h);
        int32_t get_c();
        void set_c(int32_t c);

    private:
        std::string device;
        int32_t camera_width;
        int32_t camera_height;
        int32_t camera_color;
        int m_fd;
        uint8_t *buffer[CAP_BUF_NUM];

        #define WAYLANDBUF      (IMAGE_OUTPUT_WIDTH * IMAGE_OUTPUT_HEIGHT * IMAGE_CHANNEL_BGRA * WL_BUF_NUM)
        #define CAPTUREBUF      (CAM_IMAGE_WIDTH * CAM_IMAGE_HEIGHT * CAM_IMAGE_CHANNEL_YUY2)


        struct v4l2_buffer buf_capture;
        struct camera_dma_buffer *dma_buf[CAP_BUF_NUM];
        int8_t xioctl(int8_t fd, int32_t request, void *arg);
        int8_t start_capture();
        int8_t stop_capture();
        int8_t open_camera_device();
        int8_t init_camera_fmt();
        int8_t init_buffer();
        int8_t video_buffer_alloc_dmabuf(struct camera_dma_buffer *buffer,int buf_size);
        void video_buffer_free_dmabuf(struct camera_dma_buffer *buffer);
};

#endif
