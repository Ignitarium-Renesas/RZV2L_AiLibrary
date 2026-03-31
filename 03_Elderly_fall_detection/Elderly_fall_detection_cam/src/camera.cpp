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
* File Name    : camera.cpp
* Version      : 7.00
* Description  : RZ/V2L DRP-AI Sample Application for Darknet-PyTorch YOLOv3 Camera version
***********************************************************************************************************************/

/*****************************************
* Includes
******************************************/
#include "camera.h"

Camera::Camera()
{
    camera_width    = CAM_IMAGE_WIDTH;
    camera_height   = CAM_IMAGE_HEIGHT;
    camera_color    = CAM_IMAGE_CHANNEL_YUY2;
}

Camera::~Camera()
{
}


/*****************************************
* Function Name : start_camera
* Description   : Function to initialize USB/MIPI camera capture
* Arguments     : -
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t Camera::start_camera()
{
    int8_t ret = 0;
    int32_t i = 0;
    int32_t n = 0;
    int8_t wayland_buf_ret = -1;
    int8_t drpai_buf_ret = -1;
    int8_t dma_buf_ret[CAP_BUF_NUM];

#ifdef INPUT_CORAL
    const char* commands[7] =
    {
        "media-ctl -d /dev/media0 -r",
        "media-ctl -d /dev/media0 -l \"\'csi-10830400.csi2\':1 -> \'cru-ip-10830000.video\':0 [1]\"",
        "media-ctl -d /dev/media0 -l \"\'cru-ip-10830000.video\':1 -> \'CRU output\':0 [1]\"",
        "media-ctl -d /dev/media0 -V \"\'csi-10830400.csi2\':1 [fmt:UYVY8_2X8/640x480 field:none]\"",
        "media-ctl -d /dev/media0 -V \"\'ov5645 0-003c\':0 [fmt:UYVY8_2X8/640x480 field:none]\"",
        "media-ctl -d /dev/media0 -V \"\'cru-ip-10830000.video\':0 [fmt:UYVY8_2X8/640x480 field:none]\"",
        "media-ctl -d /dev/media0 -V \"\'cru-ip-10830000.video\':1 [fmt:UYVY8_2X8/640x480 field:none]\""  
    };

    /* media-ctl command */
    for (i=0; i<7; i++)
    {
        printf("%s\n", commands[i]);
        ret = system(commands[i]);
        printf("system ret = %d\n", ret);
        if (ret<0)
        {
            printf("%s: failed media-ctl commands. index = %d\n", __func__, i);
            ret = -1;
            goto end;
        }
    }
#endif /* INPUT_CORAL */

    ret = open_camera_device();
    if (0 != ret)
    {
        ret = -1;
        goto end;
    }

    ret = init_camera_fmt();
    if (0 != ret)
    {
        ret = -1;
        goto end;
    }

    ret = init_buffer();
    if (0 != ret)
    {
        ret = -1;
        goto end;
    }

    /* Initialize buffer */
    wayland_buf = NULL;
    drpai_buf = NULL;
    for (i = 0; i < CAP_BUF_NUM; i++)
    {
        dma_buf[i] = NULL;
        dma_buf_ret[i] = -1;
    }

    wayland_buf = (camera_dma_buffer*)malloc(sizeof(wayland_buf));
    if (NULL == wayland_buf)
    {
        fprintf(stderr, "[ERROR] Failed to malloc the wayland_buf\n");
        goto err_end;
    }
    
    wayland_buf_ret = video_buffer_alloc_dmabuf(wayland_buf,WAYLANDBUF);
    if (-1 == wayland_buf_ret)
        {
        fprintf(stderr, "[ERROR] Failed to Allocate DMA buffer for the wayland_buf\n");
        goto err_end;
    }

    drpai_buf = (camera_dma_buffer*)malloc(sizeof(camera_dma_buffer));
    if (NULL == drpai_buf)
    {
        fprintf(stderr, "[ERROR] Failed to malloc the drpai_buf\n");
        goto err_end;
    }
    
    drpai_buf_ret = video_buffer_alloc_dmabuf(drpai_buf,CAPTUREBUF);
    if (-1 == drpai_buf_ret)
    {
        fprintf(stderr, "[ERROR] Failed to Allocate DMA buffer for the drpai_buf\n");
        goto err_end;
    }

    for (n =0; n < CAP_BUF_NUM; n++)
    {
        dma_buf[n] = (camera_dma_buffer*)malloc(sizeof(camera_dma_buffer[n]));
        if (NULL == dma_buf[n])
        {
            fprintf(stderr, "[ERROR] Failed to malloc the dma_buf\n");
            goto err_end;
        }
        dma_buf_ret[n] = video_buffer_alloc_dmabuf(dma_buf[n],CAPTUREBUF);
        if (-1 == dma_buf_ret[n])
        {
            fprintf(stderr, "[ERROR] Failed to Allocate DMA buffer for the dma_buf\n");
            goto err_end;
        }
        memset(&buf_capture, 0, sizeof(buf_capture));
        buf_capture.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf_capture.memory = V4L2_MEMORY_DMABUF;
        buf_capture.index = n;
        buf_capture.m.fd = (unsigned long) dma_buf[n]->dbuf_fd;
        buf_capture.length = dma_buf[n]->size;
        ret = xioctl(m_fd, VIDIOC_QBUF, &buf_capture);
        if (-1 == ret)
        {
            goto err_end;
        }
    }    

    ret = start_capture();
    if (0 != ret)
    {
        goto err_end;
    }

    ret = 0;
    goto end;
    
err_end:
    /* free buffer */
    if (0 == wayland_buf_ret)
    {
        video_buffer_free_dmabuf(wayland_buf);
    }

    if (0 == drpai_buf_ret)
    {
        video_buffer_free_dmabuf(drpai_buf);
    }
    
    for (n = 0; n < CAP_BUF_NUM; n++)
    {
        if (0 == dma_buf_ret[n])
        {
            video_buffer_free_dmabuf(dma_buf[n]);
        }
    }
    
    free(wayland_buf);
    wayland_buf = NULL;

    free(drpai_buf);
    drpai_buf = NULL;

    for (n = 0; n < CAP_BUF_NUM; n++)
    {
        free(dma_buf[n]);
        dma_buf[n] = NULL;
    }
    
    close(m_fd);
    ret = -1;
    goto end;
end:
    return ret;
}



/*****************************************
* Function Name : close_capture
* Description   : Close camera and free buffer
* Arguments     : -
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t Camera::close_camera()
{
    int8_t ret = 0;
    int32_t i = 0;

    ret = stop_capture();
    if (0 != ret) return ret;

    video_buffer_free_dmabuf(wayland_buf);
    free(wayland_buf);
    wayland_buf = NULL;

    video_buffer_free_dmabuf(drpai_buf);
    free(drpai_buf);
    drpai_buf = NULL;

    for (i = 0;i<CAP_BUF_NUM;i++)
    {
        video_buffer_free_dmabuf(dma_buf[i]);
        free(dma_buf[i]);
        dma_buf[i] = NULL;
    }
    close(m_fd);
    return 0;
}

/*****************************************
* Function Name : xioctl
* Description   : ioctl calling
* Arguments     : fd = V4L2 file descriptor
*                 request = V4L2 control ID defined in videodev2.h
*                 arg = set value
* Return value  : int = output parameter
******************************************/
int8_t Camera::xioctl(int8_t fd, int32_t request, void * arg)
{
    int8_t r;
    do r = ioctl(fd, request, arg);
    while (-1 == r && EINTR == errno);
    return r;
}

/*****************************************
* Function Name : start_capture
* Description   : Set STREAMON
* Arguments     : -
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t Camera::start_capture()
{
    int8_t ret = 0;
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    ret = xioctl(m_fd, VIDIOC_STREAMON, &buf.type);
    if (-1 == ret)
    {
        return -1;
    }
    return 0;
}

/*****************************************
* Function Name : capture_qbuf
* Description   : Function to enqueue the buffer.
*                 (Call this function after capture_image() to restart filling image data into buffer)
* Arguments     : -
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t Camera::capture_qbuf()
{
    int8_t ret = 0;

    ret = xioctl(m_fd, VIDIOC_QBUF, &buf_capture);
    if (-1 == ret)
    {
        return -1;
    }
    return 0;
}


/*****************************************
* Function Name : capture_image
* Description   : Function to capture image and return the physical memory address where the captured image stored.
*                 Must call capture_qbuf after calling this function.
* Arguments     : -
* Return value  : the physical memory address where the captured image stored.
******************************************/
uint64_t Camera::capture_image()
{
    int8_t ret = 0;
    fd_set fds;
    /*Delete all file descriptor from fds*/
    FD_ZERO(&fds);
    /*Add m_fd to file descriptor set fds*/
    FD_SET(m_fd, &fds);

    /* Check when a new frame is available */
    while (1)
    {
        ret = select(m_fd + 1, &fds, NULL, NULL, NULL);
        if (0 > ret)
        {
            if (EINTR == errno) continue;
            return 0;
        }
        break;
    }

    /* Get buffer where camera stored data */
    ret = xioctl(m_fd, VIDIOC_DQBUF, &buf_capture);
    if (-1 == ret)
    {
        return 0;
    }

    return  dma_buf[buf_capture.index]->phy_addr;
}


/*****************************************
* Function Name : stop_capture
* Description   : Set STREAMOFF
* Arguments     : -
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t Camera::stop_capture()
{
    int8_t ret = 0;
    struct v4l2_buffer buf;
    memset(&buf, 0, sizeof(buf));

    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory = V4L2_MEMORY_DMABUF ;

    ret = xioctl(m_fd, VIDIOC_STREAMOFF, &buf.type);
    if (-1 == ret)
    {
        return -1;
    }
    return 0;
}

/*****************************************
* Function Name : open_camera_device
* Description   : Function to open camera *called by start_camera
* Arguments     : -
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t Camera::open_camera_device()
{
    char dev_name[4096] = {0};
    int32_t i = 0;
    int8_t ret = 0;
    struct v4l2_capability fmt;

    for (i = 0; i < 15; i++)
    {
        snprintf(dev_name, sizeof(dev_name), "/dev/video%d", i);
        m_fd = open(dev_name, O_RDWR);
        if (-1 == m_fd)
        {
            continue;
        }

        /* Check device is valid (Query Device information) */
        memset(&fmt, 0, sizeof(fmt));
        ret = xioctl(m_fd, VIDIOC_QUERYCAP, &fmt);
        if (-1 == ret)
        {
            return -1;
        }

#ifdef INPUT_CORAL
        ret = strcmp((const char*)fmt.driver, "rzg2l_cru");
        if (0 == ret)
        {
            printf("[INFO] CSI2 Camera: %s\n", dev_name);
            break;
        }
#else /* INPUT_CORAL */
        /* Search USB camera */
        ret = strcmp((const char*)fmt.driver, "uvcvideo");
        if (0 == ret)
        {
            printf("[INFO] USB Camera: %s\n", dev_name);
            break;
        }
#endif /* INPUT_CORAL */
        close(m_fd);
    }

    if (15 <= i)
    {
        return -1;
    }
    return 0;
}

/*****************************************
* Function Name : init_camera_fmt
* Description   : Function to request format *called by start_camera
* Arguments     : -
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t Camera::init_camera_fmt()
{
    int8_t ret = 0;
    struct v4l2_format fmt;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = camera_width;
    fmt.fmt.pix.height = camera_height;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
    fmt.fmt.pix.field = V4L2_FIELD_NONE;

    ret = xioctl(m_fd, VIDIOC_S_FMT, &fmt);
    if (-1 == ret)
    {
        return -1;
    }
    return 0;
}

/*****************************************
* Function Name : init_buffer
* Description   : Initialize camera buffer *called by start_camera
* Arguments     : -
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t Camera::init_buffer()
{
    int8_t ret = 0;
    int32_t i = 0;
    struct v4l2_requestbuffers req;
    memset(&req, 0, sizeof(req));
    req.count = CAP_BUF_NUM;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_DMABUF;

    /*Request a buffer that will be kept in the device*/
    ret = xioctl(m_fd, VIDIOC_REQBUFS, &req);
    if (-1 == ret)
    {
        return -1;
    }

    struct v4l2_buffer buf;
    for (i =0; i < CAP_BUF_NUM; i++)
    {
        memset(&buf, 0, sizeof(buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_DMABUF;
        buf.index = i;

        /* Extract buffer information */
        ret = xioctl(m_fd, VIDIOC_QUERYBUF, &buf);
        if (-1 == ret)
        {
            return -1;
        }

    }

    return 0;
}


/*****************************************
* Function Name : save_bin
* Description   : Get the capture image from buffer and save it into binary file
* Arguments     : filename = binary file name to be saved
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t Camera::save_bin(std::string filename)
{
    int8_t ret = 0;
    FILE * fp = fopen(filename.c_str(), "wb");
    if (!fp)
    {
        return -1;
    }

    /* Get data from buffer and write to binary file */
    ret = fwrite((uint8_t *)dma_buf[buf_capture.index]->mem, sizeof(uint8_t), dma_buf[buf_capture.index]->size, fp);
    if (!ret)
    {
        fclose(fp);
        return -1;
    }

    fclose(fp);
    return 0;
}


/*****************************************
* Function Name : video_buffer_alloc_dmabuf
* Description   : Allocate a DMA buffer for the camera
* Arguments     : buffer = pointer to the camera_dma_buffer struct
*                 buf_size = size of the allocation
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int8_t Camera::video_buffer_alloc_dmabuf(struct camera_dma_buffer *buffer,int buf_size)
{
    MMNGR_ID id;
    uint32_t phard_addr;
    void *puser_virt_addr;
    int m_dma_fd;

    buffer->size = buf_size;
    mmngr_alloc_in_user_ext(&id, buffer->size, &phard_addr, &puser_virt_addr, MMNGR_VA_SUPPORT_CACHED, NULL);
    memset((void*)puser_virt_addr, 0, buffer->size);
    buffer->idx = id;
    buffer->mem = (void *)puser_virt_addr;
    buffer->phy_addr = phard_addr;
    if (!buffer->mem)
    {
        return -1;
    }
    mmngr_export_start_in_user_ext(&id, buffer->size, phard_addr, &m_dma_fd, NULL);
    buffer->dbuf_fd = m_dma_fd;
    return 0;
}

/*****************************************
* Function Name : video_buffer_free_dmabuf
* Description   : free a DMA buffer for the camera
* Arguments     : buffer = pointer to the camera_dma_buffer struct
* Return value  : -
******************************************/
void Camera::video_buffer_free_dmabuf(struct camera_dma_buffer *buffer)
{
    mmngr_free_in_user_ext(buffer->idx);
    return;
}

/*****************************************
* Function Name : video_buffer_flush_dmabuf
* Description   : flush a DMA buffer in continuous memory area
*                 MUST be called when writing data to DMA buffer
* Arguments     : idx = id of the buffer to be flushed.
*                 size = size to be flushed.
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
int Camera::video_buffer_flush_dmabuf(uint32_t idx, uint32_t size)
{
    int mm_ret = 0;
    
    /* Flush capture image area cache */
    mm_ret = mmngr_flush(idx, 0, size);
    
    return mm_ret;
}

/*****************************************
* Function Name : get_img
* Description   : Function to return the camera buffer
* Arguments     : -
* Return value  : camera buffer
******************************************/
uint8_t * Camera::get_img()
{
    return (uint8_t *)dma_buf[buf_capture.index]->mem;
}


/*****************************************
* Function Name : get_size
* Description   : Function to return the camera buffer size (W x H x C)
* Arguments     : -
* Return value  : camera buffer size (W x H x C )
******************************************/
int32_t Camera::get_size()
{
    return dma_buf[buf_capture.index]->size;
}

/*****************************************
* Function Name : get_w
* Description   : Get camera_width. This function is currently NOT USED.
* Arguments     : -
* Return value  : camera_width = width of camera capture image.
******************************************/
int32_t Camera::get_w()
{
    return camera_width;
}

/*****************************************
* Function Name : set_w
* Description   : Set camera_width. This function is currently NOT USED.
* Arguments     : w = new camera capture image width
* Return value  : -
******************************************/
void Camera::set_w(int32_t w)
{
    camera_width= w;
    return;
}

/*****************************************
* Function Name : get_h
* Description   : Get camera_height. This function is currently NOT USED.
* Arguments     : -
* Return value  : camera_height = height of camera capture image.
******************************************/
int32_t Camera::get_h()
{
    return camera_height;
}

/*****************************************
* Function Name : set_h
* Description   : Set camera_height. This function is currently NOT USED.
* Arguments     : w = new camera capture image height
* Return value  : -
******************************************/
void Camera::set_h(int32_t h)
{
    camera_height = h;
    return;
}

/*****************************************
* Function Name : get_c
* Description   : Get camera_color. This function is currently NOT USED.
* Arguments     : -
* Return value  : camera_color = color channel of camera capture image.
******************************************/
int32_t Camera::get_c()
{
    return camera_color;
}

/*****************************************
* Function Name : set_c
* Description   : Set camera_color. This function is currently NOT USED.
* Arguments     : c = new camera capture image color channel
* Return value  : -
******************************************/
void Camera::set_c(int32_t c)
{
    camera_color= c;
    return;
}
