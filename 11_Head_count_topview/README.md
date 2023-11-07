# RZV2L AI Library - Head counting from top application

## Introduction

This application is used to count the human heads from topview present in an image or a video from camera input.

It can utilize the API, "*PRET_HC()*" provided in this library. Input to this API will be an image buffer address and it's height and width of the image. 

<!-- sample video on YouTube -[Head counting demo](https://youtu.be/YNcCCiSx9YM) -->

## Application details
```
|-- 01_Head_Count_topview/ # Head count topview application
|   |-- test images/ # test images directory
|   |-- etc/ # address map and pre/post process configuration of yolov3
|   |-- exe/
|   |   |-- head_count_app # the executable
|   |   `-- yolov3_onnx # DRP-AI files of yolov3
|   `-- src/ # source code directory
`-- README.md
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
*[SCUT Dataset](https://github.com/HCIILAB/SCUT-HEAD-Dataset-Release)

### Application with image input
- The user should pass the path to the image as an argument, optionally it's width and height.
- The number of heads in the image is detected and showed on the terminal.

### Application with camera input
- Camera input (from Coral Camera or USB Camera) is fed to the model with proper pre-processing.
- The count is calculated & displayed based on the detected heads.

### Building the sample application:

These steps must be performed in Laptop/PC with the support of RZV2L toolchain. Find the steps in the repository's Readme file available at home page.

After doing desired modifications in the source code, please follow the below steps: 

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
```
To create the executable
```
cd RZV2L_AiLibrary/11_Head_count_topview/src
mkdir -p build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=./toolchain/runtime.cmake ..
make -j$(nproc) head_count_topview_app
```
### Running the sample application
Before running the application, download the weights from [deploy.so](https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary/releases/download/tvm_v1.0.0/11_head_count_topview.zip). extract deploy.so file and place it in to `exe/topview_headcount_yolov3/`

#### Application with image input
```
cd /home/root/RZV2L_AiLibrary 
cd 01_Head_count/exe/
./head_count_topview_app IMAGES IMAGE ../test_images/01.bmp
```
Optionally the user can pass width and height as arguments.

#### Application with Coral camera input
```
cd /home/root/RZV2L_AiLibrary 
cd 01_Head_count/exe/
./head_count_topview_app MIPI
```

#### Application with USB camera input
```
cd /home/root/RZV2L_AiLibrary 
cd 01_Head_count/exe/
./head_count_topview_app USB
```

### Known issues:
1. [ERROR] Image buffer address is NULL : This error suggests that the input path to the image is improper. Verify the path, check whether an image available in the path.
2. Segmentation fault : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is larger than the actual image dimensions, then a segmentation fault will occur.
3. Improper output : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is smaller than the actual image dimensions, then improper or unexpected outputs will be observed.
4. [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
5. permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.
6. [ERROR] Failed to open: <prefix>/<prefix>_weight.dat error=2. [ERROR] Failed to load data from memory: <prefix>/<prefix>_weight.dat Failed to load DRP-AI object files - This error suggests that the weight file is not availbale in the `exe` folder. Download the weight file from the release in github and place it properly in the `exe/subfolder`.
