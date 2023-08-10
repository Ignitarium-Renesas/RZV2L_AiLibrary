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
* File Name    : image.h
* Version      : 7.20
* Description  : RZ/V2L DRP-AI Sample Application for Darknet-PyTorch YOLO Image version
***********************************************************************************************************************/

#ifndef IMAGE_H
#define IMAGE_H

#include "define.h"
#include "ascii.h"

class Image
{
    public:
        Image();
        ~Image();

        uint8_t * img_buffer;
        int32_t get_H();
        int32_t get_W();
        int32_t get_C();
        void set_H(int32_t h);
        void set_W(int32_t w);
        uint8_t at(int32_t a);
        void set(int32_t a, uint8_t val);
        void set_image_buffer(uint8_t* in_img_addr);
        
        uint8_t init(uint32_t w, uint32_t h, uint32_t c);
        uint8_t read_bmp(std::string filename);
        uint8_t save_bmp(std::string filename);
        void draw_rect(int32_t x, int32_t y, int32_t w, int32_t h, const char * str);

    private:
        uint8_t header_size = FILEHEADERSIZE+INFOHEADERSIZE_W_V3;
        uint8_t bmp_header[FILEHEADERSIZE+INFOHEADERSIZE_W_V3];
        uint8_t udmabuf_fd;
        int32_t img_h;
        int32_t img_w;
        int32_t img_c;
        int32_t size;

        int32_t front_color = BLACK_DATA;
        int32_t back_color = WHITE_DATA;
        int32_t font_w = CPU_DRAW_FONT_WIDTH;
        int32_t font_h = CPU_DRAW_FONT_HEIGHT;
        void draw_point(int32_t x, int32_t y, int32_t color);
        void draw_line(int32_t x0, int32_t y0, int32_t x1, int32_t y1, int32_t color);
        void write_char(const uint8_t code, int32_t x, int32_t y, int32_t color, int32_t backcolor);
        void write_string(const char *pcode, int32_t x, int32_t y, int32_t color, int32_t backcolor);

};

#endif
