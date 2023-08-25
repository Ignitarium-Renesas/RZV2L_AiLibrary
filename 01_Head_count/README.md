# RZV2L AI Library - Head counting application

## Introduction

This application is used to count the human heads present in an image or a video from camera input.

It can utilize the API, "*PRET_HC()*" provided in this library. Input to this API will be an image buffer address and it's height and width of the image. 
> Refer [Head_count_img/examples/head_count_example.cpp](Head_count_img/examples/head_count_example.cpp) for an example usage

sample video on YouTube -[Head counting demo](https://youtu.be/YNcCCiSx9YM)

## Application details
```
|-- 01_Head_count.mp4 # sample demo
|-- Head_count_cam/ # Head count application on Coral camera input
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration of yolov3
|   |-- examples/
|   |   `-- head_count_cam_example.cpp # example inference code
|   |-- exe/
|   |   |-- 01_head_count_cam_app # the executable
|   |   `-- yolov3_cam # DRP-AI files of yolov3
|   `-- src/ # source code directory
|-- Head_count_img # Head count application on image input
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration of yolov3
|   |-- examples/
|   |   `-- head_count_example.cpp # example inference code
|   |-- exe/
|   |   |-- 01_head_count_img_app # the executable
|   |   |-- labels.txt # label map
|   |   `-- yolov3_bmp # DRP-AI files of yolov3
|   |-- src/ # source code directory
|   `-- test_images # test images directory
|-- Head_count_usbcam # Head count application on USB camera input
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration of yolov3
|   |-- examples/
|   |   `-- head_count_cam_example.cpp # example inference code
|   |-- exe/
|   |   |-- 01_head_count_usbcam_app # the executable
|   |   `-- yolov3_cam # DRP-AI files of yolov3
|   `-- src/ # source code directory
`-- README.md
```

### Model details

#### YOLOv3
In this application YOLOv3 is used for head detection.
- Official paper - [YOLOv3: An Incremental Improvement](https://arxiv.org/pdf/1804.02767.pdf)
- Official website - [YoloV3](https://pjreddie.com/darknet/yolo/)
- Dataset - [HollywoodHeads](https://www.di.ens.fr/willow/research/headdetection/)

### Application with image input
- The user should pass the path to the image as an argument, optionally it's width and height.
- The number of heads in the image is detected and showed on the terminal.

### Application with camera input
- Camera input (from Coral Camera or USB Camera) is fed to the model with proper pre-processing.
- The count is calculated & displayed based on the detected heads.

### Building the sample application:

This is an optional step since already compiled application is available in the `exe/` folder.
These steps must be performed in Laptop/PC with the support of RZV2L toolchain. Find the steps in the repository's Readme file available at home page.

After doing desired modifications in the source code, please follow the below steps to create he executable: 

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/01_Head_count/<app dir>
make
```

### Running the sample application
Praparation on laptop:
1. Download the weight file `yolov3_Head_counting_weights.dat` from the [release v1.2.2](https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary/releases/tag/v1.2.2). Precise file link: [yolov3_Head_counting_weights.dat](https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary/releases/download/v1.2.2/yolov3_Head_counting_weights.dat)
2. Place the weight files at location proper location `01_Head_count/Head_count_img/exe/yolov3_bmp/`, `01_Head_count/Head_count_cam/exe/yolov3_cam/` or `01_Head_count/Head_count_usbcam/exe/yolov3_cam/`
3. Now, copy the `RZV2L_AiLibrary` directory to the RZV2L board (/home/root/).
4. Please follow these steps:

#### Application with image input
```
cd /home/root/RZV2L_AiLibrary 
cd 01_Head_count/Head_count_img/exe/
./01_head_count_img_app ../test_images/elevator_google_0007_640_480.bmp
```
Optionally the user can pass width and height as arguments.

#### Application with Coral camera input
```
cd /home/root/RZV2L_AiLibrary 
cd 01_Head_count/Head_count_cam/exe/
./01_head_count_cam_app
```

#### Application with USB camera input
```
cd /home/root/RZV2L_AiLibrary 
cd 01_Head_count/Head_count_usbcam/exe/
./01_head_count_usbcam_app
```

### Known issues:
1. [ERROR] Image buffer address is NULL : This error suggests that the input path to the image is improper. Verify the path, check whether an image available in the path.
2. Segmentation fault : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is larger than the actual image dimensions, then a segmentation fault will occur.
3. Improper output : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is smaller than the actual image dimensions, then improper or unexpected outputs will be observed.
4. [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
5. permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.
6. [ERROR] Failed to open: <prefix>/<prefix>_weight.dat error=2. [ERROR] Failed to load data from memory: <prefix>/<prefix>_weight.dat Failed to load DRP-AI object files - This error suggests that the weight file is not availbale in the `exe` folder. Download the weight file from the release in github and place it properly in the `exe/subfolder`.
