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
* File Name    : image.cpp
* Version      : 7.20
* Description  : RZ/V2L DRP-AI Sample Application for PyTorch ResNet50 Image version
***********************************************************************************************************************/

/*****************************************
* Includes
******************************************/
#include "image.h"

Image::Image()
{
}


Image::~Image()
{
    munmap(img_buffer, size);
    close(udmabuf_fd);
}

/*****************************************
* Function Name : get_H
* Description   : Function to get the image height
*                 This function is NOT used currently.
* Arguments     : -
* Return value  : img_h = current image height
******************************************/
int32_t Image::get_H()
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
int32_t Image::get_W()
{
    return img_w;
}
/*****************************************
* Function Name : get_C
* Description   : Function to get the number of image channel
*                 This function is NOT used currently.
* Arguments     : -
* Return value  : img_c = current number of image channel
******************************************/
int32_t Image::get_C()
{
    return img_c;
}
/*****************************************
* Function Name : set_H
* Description   : Function to set the image height
*                 This function is NOT used currently.
* Arguments     : h = new image height to be set
* Return value  : -
******************************************/
void Image::set_H(int32_t h)
{
    img_h = h;
    return;
}
/*****************************************
* Function Name : set_W
* Description   : Function to set the image width
*                 This function is NOT used currently.
* Arguments     : w = new image width to be set
* Return value  : -
******************************************/
void Image::set_W(int32_t w)
{
    img_w = w;
    return;
}

/*****************************************
* Function Name : init
* Description   : Function to initialize img_buffer in Image class
*                 This application uses udmabuf in order to
*                 continuous memory area for DRP-AI input data
* Arguments     : w = image width
*                 h = image height
*                 c = image channel
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
uint8_t Image::init(uint32_t w, uint32_t h, uint32_t c)
{
    int32_t i = 0;
    img_w = w;
    img_h = h;
    img_c = c;
    size = w * h * c;
    udmabuf_fd = open("/dev/udmabuf0", O_RDWR );
    if (udmabuf_fd < 0)
    {
        return -1;
    }
    img_buffer =(uint8_t*) mmap(NULL, size ,PROT_READ|PROT_WRITE, MAP_SHARED,  udmabuf_fd, 0);

    if (img_buffer == MAP_FAILED)
    {
        return -1;
    }
    /* Write once to allocate physical memory to u-dma-buf virtual space.
    * Note: Do not use memset() for this.
    *       Because it does not work as expected. */
    {
        for (i = 0 ; i < size; i++)
        {
            img_buffer[i] = 0;
        }
    }
    return 0;
}

/*****************************************
* Function Name : read_bmp
* Description   : Function to load BMP file into img_buffer
* NOTE          : This is just the simplest example to read Windows Bitmap v3 file.
*                 This function does not have header check.
* Arguments     : filename = name of BMP file to be read
* Return value  : 0 if succeeded
*                 not 0 otherwise
******************************************/
uint8_t Image::read_bmp(std::string filename)
{
    uint32_t width = img_w;
    uint32_t height = img_h;
    uint32_t channel = img_c;
    int32_t i = 0;
    FILE *fp = NULL;
    size_t ret = 0;
    uint8_t * bmp_line_data;
    /* Number of byte in single row */
    /* NOTE: Number of byte in single row of Windows Bitmap image must be aligned to 4 bytes. */
    int32_t line_width = width * channel + width % 4;

    /*  Read header for Windows Bitmap v3 file. */
    fp = fopen(filename.c_str(), "rb");
    if (NULL == fp)
    {
        return -1;
    }

    /* Read all header */
    ret = fread(bmp_header, sizeof(uint8_t), header_size, fp);
    if (!ret)
    {
        fclose(fp);
        return -1;
    }
    /* Single row image data */
    bmp_line_data = (uint8_t *) malloc(sizeof(uint8_t) * line_width);
    if (NULL == bmp_line_data)
    {
        free(bmp_line_data);
        fclose(fp);
        return -1;
    }

    for (i = height-1; i >= 0; i--)
    {
        ret = fread(bmp_line_data, sizeof(uint8_t), line_width, fp);
        if (!ret)
        {
            free(bmp_line_data);
            fclose(fp);
            return -1;
        }
        memcpy(img_buffer+i*width*channel, bmp_line_data, sizeof(uint8_t)*width*channel);
    }

    free(bmp_line_data);
    fclose(fp);
    return 0;
}


/*****************************************
* Function Name : save_bmp
* Description   : Save the image in img_buffer into Windows Bitmap v3 file.
*                 This function uses the bmp_header,
*                  which read_bmp() stored the input image header information
* Arguments     : filename = name of output image file
* Return value  : 0 if suceeded
*                 not 0 otherwise
******************************************/
uint8_t Image::save_bmp(std::string filename)
{
    int32_t i = 0;
    FILE * fp = NULL;
    uint8_t * bmp_line_data;
    uint32_t width = img_w;
    uint32_t height = img_h;
    uint32_t channel = img_c;
    size_t ret = 0;

    /* Number of byte in single row */
    int32_t line_width = width * channel + width % 4;

    fp = fopen(filename.c_str(), "wb");
    if (NULL == fp)
    {
        return -1;
    }

    /* Write header for Windows Bitmap v3 file. */
    fwrite(bmp_header, sizeof(uint8_t), header_size, fp);

    bmp_line_data = (uint8_t *) malloc(sizeof(uint8_t) * line_width);
    if (NULL == bmp_line_data)
    {
        free(bmp_line_data);
        fclose(fp);
        return -1;
    }

    for (i = height - 1; i >= 0; i--)
    {
        memcpy(bmp_line_data, img_buffer + i*width*channel, sizeof(uint8_t)*width*channel);
        ret = fwrite(bmp_line_data, sizeof(uint8_t), line_width, fp);
        if (!ret)
        {
            free(bmp_line_data);
            fclose(fp);
            return -1;
        }
    }
    free(bmp_line_data);
    fclose(fp);
    return 0;
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
    return img_buffer[a];
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
    img_buffer[a] = val;
    return;
}

/*****************************************
* Function Name : set_image_buffer
* Description   : Copy the values from the input image buffer to the of Image img_buffer
* Arguments     : in_img_addr = address of the user input image buffer
* Return Value  : -
******************************************/
void Image::set_image_buffer(uint8_t* in_img_addr)
{
    memcpy(img_buffer, in_img_addr, sizeof(uint8_t)*img_h*img_w*img_c);
    return;
}
