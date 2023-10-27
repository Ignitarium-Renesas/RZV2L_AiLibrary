# RZV2L AI Library - Animal detection application

## Introduction

This application is used to detect specific set of animals in a given image or camera input.

| Classes | Animal |
| :---: | :---: |
| 1 | Boar |
| 2 | Deer |
| 3 | Crow |
| 4 | Monkey |
| 5 | Bear |
| 6 | Racoon |
| 7 | Fox |
| 8 | Weasal |
| 9 | Skunk |
| 10 | Dog |
| 11 | Cat |

sample video on YouTube -[Animal Detection demo](https://youtu.be/o-3oeOCRHws)

## Application details
```
|-- 07_Animal_detection/ # Animal detection application
|   |-- exe/
|   |   |-- animal_detection_app # the executable
|   |   `-- animal_yolov3_onnx # DRP-AI files of yolov3
|   `-- src/ # source code directory
|   `-- test_images # test images directory
`-- README.md
```

### Model details

#### YOLOv3
In this application YOLOv3 is used for detecting animals.
- Official paper - [YOLOv3: An Incremental Improvement](https://arxiv.org/pdf/1804.02767.pdf)
- Official website - [YoloV3](https://github.com/AlexeyAB/darknet)
- Datasets :  

| classes | Animals | Dataset |
| --- | --- | :---: |
|<p> 1 <p> 2 <p> 3 <p> 4 <p> 5 <p> 6 |<p> Fox <p> Deer <p> Crow <p> Monkey <p> Bear <p> Raccoon | [Animals Detection Images Dataset](https://www.kaggle.com/datasets/antoreepjana/animals-detection-images-dataset) |
|<p> 7 <p> 8 <p> 9 |<p> Boar <p> Weasal <p> Skunk | [Images.cv](https://images.cv) |
|<p> 10 <p> 11 |<p> Dog <p> Cat | [Coco Dataset](https://cocodataset.org/#download) |

### Application with image input
- The user should pass the path to the image as an argument.
- The specified set of animal(s) in the image is(are) detected 

### Application with camera input
- Camera input is fed to the model with proper pre-processing.
- The specified set of animal(s) in the image is(are) detected 

### Building the sample application:

This is an optional step since already compiled application is available in the `exe/` folder.
These steps must be performed in Laptop/PC with the support of RZV2L toolchain. Find the steps in [DRP-AI TVM](https://github.com/renesas-rz/rzv_drp-ai_tvm/blob/main/setup/README.md#installing-drp-ai-tvm1-with-docker).

After doing desired modifications in the source code, please follow the below steps to create the executable: 

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
```
To create the executable
```
cd RZV2L_AiLibrary/01_Head_count/src
mkdir -p build && cd build
cmake -DCMAKE_TOOLCHAIN_FILE=./toolchain/runtime.cmake ..
make -j$(nproc)
```

### Running the sample application
Praparation on laptop:
1. Download the weights file `deploy.so` from the release [release v1.2.3](https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary/releases/tag/v1.2.3) depending on which application is being used.Precise file links: [animal_yolov3_weight.dat](https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary/releases/download/v1.2.3/animal_yolov3_weight.dat) 
2. Place the weight files at location proper location `07_Animal_detection/exe/animal_yolov3_onnx/`.
3. Now, copy the `RZV2L_AiLibrary` directory to the RZV2L board (/home/root/).
4. Please follow these steps:
For image input continue with steps 5 to 7.
5. The user should pass the path to the image as an argument.

#### Application with image input
```
cd /home/root/RZV2L_AiLibrary 
cd 07_Animal_detection/exe/
./07_animal_detection_app IMAGE ../test_images/raccoon.bmp 
```

#### Application with coral camera input
```
cd /home/root/RZV2L_AiLibrary 
cd 07_Animal_detection/exe/
./07_animal_detection_app MIPI
```

#### Application with usb camera input
```
cd /home/root/RZV2L_AiLibrary 
cd 07_Animal_detection/Animal_detection_usbcam/exe/
./07_animal_detection_app USB
```


### Known issues:
1. [ERROR] Image buffer address is NULL : This error suggests that the input path to the image is improper. Verify the path, check whether an image available in the path.
2. Segmentation fault : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is larger than the actual image dimensions, then a segmentation fault will occur.
3. Improper output : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is smaller than the actual image dimensions, then improper or unexpected outputs will be observed.
4. [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
5. permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.
6. [ERROR] Failed to open: <prefix>/<prefix>_weight.dat error=2. [ERROR] Failed to load data from memory: <prefix>/<prefix>_weight.dat Failed to load DRP-AI object files - This error suggests that the weight file is not availbale in the `exe` folder. Download the weight file from the release in github and place it properly in the `exe/subfolder`.

