# RZV2L AI Library - Sample Applications

## Overview

This Library has API functions for leveraging AI applications that will run on Renesas RZ/V2L Board. Currently this library has following sample applications: 

1. Human Head Counter. 
2. Line crossing object Counter.
3. Elderly people fall detection (Work in progress)
4. Safety helmet and vest detection
5. Human age and gender detection
6. Face recognition and spoof detection

## Prerequisites
1. Board bring up sequence. (TODO).
2. SDK installation steps(TODO).

## Hardware details 
 
These sample applications run on a Renesas RZ/V2L-SMARC board.

<img src="./Renesas_RZV2L_image.jpg?raw=true" alt="Markdown Monster icon"
     margin-right=10px; 
     width=600px;
     height=334px />


## Software requirements

1. Install RZ/V2L Linux Standard Development Kit (SDK). For more details refer [Renesas website](https://www.renesas.com/us/en/products/microcontrollers-microprocessors/rz-mpus/rzv2l-general-purpose-microprocessor-equipped-renesas-original-ai-accelerator-drp-ai-12ghz-dual#overview). For SDK build, please refer [here](https://github.com/renesas-rz/meta-rzv)
2. The installation and compilation of the sample application was tested on a laptop with Ubuntu 20.04 operating system.

## Editing and building sample applications

Please follow the below steps:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/01_Head_count/Head_count_img
make
```

## Running sample application

1. Download the weights file `01_head_count_weights.zip` file from the release.
2. Extract it. Place `yolov3_bmp_weight.dat` file at location `01_Head_count/Head_count_img/exe/yolov3_bmp/`
3. Now, copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
4. Please follow these steps:

```
cd /home/root/RZV2L_AiLibrary 
cd 01_Head_count/Head_count_img/exe/<path to sample application>
./sample_application
```
3. Enter the relative path for the test_image with respect to the directory of the executable when prompted

### Example:
```
cd /home/root/RZV2L_AiLibrary 
cd 01_Head_count/Head_count_img/exe/
./01_head_count_app
# When prompted for the image path
./test_images/Ben_001_640_480.bmp
```
