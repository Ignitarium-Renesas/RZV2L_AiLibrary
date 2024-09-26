# RZV2L AI Library - Head detection from top application

## Introduction

This application is used to count the human heads present in an image or a video from camera input.

It can utilize the API, "*PRET_HC()*" provided in this library. Input to this API will be an image buffer address and it's height and width of the image. 
> Refer [Head_count_top_img/examples/headcount_top_img_example.cpp](Head_count_top_img/examples/headcount_top_img_example.cpp) for an example usage

sample video on YouTube -[Head counting from top demo](https://youtu.be/z2WW2yyLAV0)

## Application details
```
├── Head_count_topview_cam # Top view Head detection application on CORAL camera input
│   ├── etc # address map and pre/post process configuration of yolov3
│   ├── examples
│   │   └── headcount_topview_cam_example.cpp #example inference code on camera input
│   ├── exe
│   │   ├── headcount_topview_cam_app # the executable
│   │   ├── labels.txt # label map
│   │   └── yolov3_headcount_topview_cam # DRP-AI files of yolov3
│   ├── Makefile
│   └── src # source code directory
├── Head_count_topview_img
│   ├── etc # address map and pre/post process configuration of yolov3
│   ├── examples
│   │   └── headcount_topview_img_example.cpp # example inference code on image
│   ├── exe
│   │   ├── headcount_topview_img_app # the executable
│   │   ├── labels.txt # label map
│   │   └── yolov3_headcount_topview_img # DRP-AI files of yolov3
│   ├── Makefile
│   ├── src # source code directory
│   └── test_images # test images directory
├── Head_count_topview_usbcam # Top view Head detection application on USB camera input
│   ├── etc # address map and pre/post process configuration of yolov3
│   ├── examples
│   │   └── headcount_topview_usbcam_example.cpp #example inference code on camera input
│   ├── exe
│   │   ├── headcount_topview_usbcam_app # the executable
│   │   ├── labels.txt # label map
│   │   └── yolov3_headcount_topview_cam # DRP-AI files of yolov3
│   ├── Makefile
│   └── src # source code directory
└── README.md
```

### Model details

#### YOLOv3
In this application YOLOv3 is used for head detection.
- Official paper - [YOLOv3: An Incremental Improvement](https://arxiv.org/pdf/1804.02767.pdf)
- Official website - [YoloV3](https://pjreddie.com/darknet/yolo/)
- Datasets : 
*[HollywoodHeads](https://www.di.ens.fr/willow/research/headdetection/)
*[Head_data](https://www.kaggle.com/datasets/houssad/head-data)
*[RGBD_Indoor_Dataset](https://drive.google.com/file/d/1fOub9LcNqfDlr-mEcdnenAJWw-rqWGmG/view)

### Application with image input
- The user should input the relative path to the image,it's width and height.
- The number of heads in the image is detected and showed on the terminal.

### Application with camera input
- Camera input is fed to the model with proper pre-processing.
- The count is calculated & displayed based on the detected heads.

### Building the sample application:

This is an optional step since already compiled application is available in the `exe/` folder.
These steps must be performed in Laptop/PC with the support of RZV2L toolchain. Find the steps in the repository's Readme file available at home page.

After doing desired modifications in the source code, please follow the below steps to create he executable: 

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/11_Head_count_topview/<app dir>
make
```

### Running the sample application
Praparation on laptop:
1. Download the weights files ([yolov3_headcount_topview_cam_weight.dat](https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary/releases/tag/v5.00/yolov3_headcount_topview_cam_weight.dat) or [yolov3_headcount_topview_img_weight.dat](https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary/releases/tag/v5.00/yolov3_headcount_topview_img_weight.dat)) from the [release_v5.00](https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary/releases/tag/v5.00/).
2. Place the weight files at location proper location `11_Head_count_topview/Head_count_topview_img/exe/yolov3_headcount_topview_img` or `11_Head_count_topview/Head_count_topview_usbcam/exe/yolov3_headcount_topview_cam` or `11_Head_count_topview/Head_count_topview_cam/exe/yolov3_headcount_topview_cam` . 
3. Now, copy the `RZV2L_AiLibrary` directory to the RZV2L board (/home/root/).
4. Please follow these steps:

#### Application with image input
```
cd /home/root/RZV2L_AiLibrary 
cd 11_Head_count_topview/Head_count_topview_img/exe/
./headcount_topview_img_app
../test_images/03.bmp
```

#### Application with camera input
```
cd /home/root/RZV2L_AiLibrary 
cd 11_Head_count_topview/Head_count_topview_<cam/usbcam>/exe/
./headcount_topview_<cam/usbcam>_app
```
### Known issues:
1. [ERROR] Image buffer address is NULL : This error suggests that the input path to the image is improper. Verify the path, check whether an image available in the path.
2. Segmentation fault : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is larger than the actual image dimensions, then a segmentation fault will occur.
3. Improper output : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is smaller than the actual image dimensions, then improper or unexpected outputs will be observed.
4. [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
5. permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.
6. [ERROR] Failed to open: <prefix>/<prefix>_weight.dat error=2. [ERROR] Failed to load data from memory: <prefix>/<prefix>_weight.dat Failed to load DRP-AI object files - This error suggests that the weight file is not availbale in the `exe` folder. Download the weight file from the release in github and place it properly in the `exe/subfolder`.
