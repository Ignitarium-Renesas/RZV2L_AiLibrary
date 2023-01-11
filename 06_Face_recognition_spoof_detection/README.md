# RZV2L AI Library - Face Recognition & Spoof Detection
# WORK IN PROGRESS - ONLY FACE RECOGNITION IS UP AND RUNNING AS OF NOW

## Introduction

The facial biometric system is widely being used for multiple applications. Its popularity is increasing because of its ease of use. One of the problem in this system is, its vulnerability to the spoof attacks. Facial spoof detection is the task of identifying false facial verification by using a photo, video, mask or a different substitute for an authorized person's face.

sample video on YouTube -[Face recognition demo](https://youtu.be/Yoo-W1mMLu4)

## Application details

This whole application is divided into multiple single applications as described below.

### Face Registration

A tool to create face register database integrated with Coral camera to capture and save faces.

#### Face Recognition Image

Identifies a person using facial features in an image.

#### Face Recognition Camera

Identifies a person using facial features from camera frames.

```
|-- 06_Face_Recognition.mp4		# Demo video
|-- database				
|   |-- face_features.csv		# Sample face feature database 
|-- Face_registration   		# Face registration app
|   |-- Makefile
|   |-- etc
|   |-- exe
|   |   |-- Face_registration	# The executable 
|   |   `-- resnet50_bmp		# DRP-AI files of resnet50
|   `-- src
|-- Face_recognition_img		# Face recognition on image app
|   |-- Makefile
|   |-- etc
|   |-- exe
|   |   |-- Face_recognition_img	# The executable
|   |   `-- resnet50_bmp		# DRP-AI files of resnet50
|   `-- src
|-- Face_recognition_cam		# Face recognition on cam app
|   |-- Makefile
|   |-- etc
|   |-- exe
|   |   |-- Face_recognition_cam	# The executable
|   |   `-- resnet50_bmp		# DRP-AI files of resnet50
|   `-- src
`-- README.md
```

### Model details

#### ResNet-50

- Official paper - [Deep Residual Learning for Image Recognition](https://arxiv.org/pdf/1512.03385.pdf)
- Dataset - [VGGFace2](https://www.robots.ox.ac.uk/~vgg/data/vgg_face2/)
- In this application ResNet-50 is used for extracting facial features.
- Finetuned the model with 2190 images of 73 classes to get 512 feature embeddings.

### Application with image input

- The user has to create multiple embeddings using `Face_registration` app, and will be saved in `database/face_features.csv`.
- The facial features of test image (ie, `sample.bmp`) obtained from ResNet-50 are compared with the embeddings of known people by taking euclidean distance. The identity is considered to be the person whose name corresponds to the shortest distance.
- Outputs the recognized person if the distance is less than a threshold value.

### Application with camera input

- The user has to create multiple embeddings using `Face_registration` app, and will be saved in `database/face_features.csv`.
- The facial features of obtained from camera input are compared with the embeddings of known people by taking euclidean distance. The identity is considered to be the person whose name corresponds to the shortest distance.
- Outputs the recognized person if the distance is less than a threshold value.

## Running the application

### Building the sample application:

This is an optional step since already compiled application is available in `exe/`.

These steps must be performed in Laptop/PC with the support of RZV2L toolchain.

Please follow the below steps for image application:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/06_Face_recognition_spoof_detection/<application>
make
```

### Running the sample applications

#### Face Registration

1. Copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Follow below steps;

```
cd /home/root/RZV2L_AiLibrary 
cd 06_Face_recognition_spoof_detection/Face_registration/exe/
./Face_registration
```
3. Choose whether to capture image, append or write database.
    1. for capturing new face
        1. Enter the name of the image to be saved.
        2. The face must be inside the square box.
        3. Press "Enter" key to capture.
        ```
        root@smarc-rzv2l:~/RZV2L_AiLibrary/06_Face_recognition_spoof_detection/Face_registration/exe# ./Face_registration 
        Choose any one option from below
        [0] for capturing new face
        [1] for adding new face to register [Add new values to existing register]
        [2] for creating new face register [Erasing previous register]
        0
        ---------------------------
        Capturing from Coral Camera
        ---------------------------

        Enter the name of the picture
        test 
        ---------------------------
        ..Press Enter to capture..
        ---------------------------
        ```
        ![](res/wayland-screenshot-2023-01-03_07-09-10.png)
    2. for creating new face register or for adding new face to register.
        1. Enter the number of samples to be embdded.
        2. Enter person name and path to his/her image.
        ```
        root@smarc-rzv2l:~/RZV2L_AiLibrary/06_Face_recognition_spoof_detection/Face_registration/exe# ./Face_registration 
        Choose any one option from below
        [0] for capturing new face
        [1] for adding new face to register [Add new values to existing register]
        [2] for creating new face register [Erasing previous register]
        1
        Enter the number of samples to be embedded 
        5
        Enter the person name 
        Christo
        Enter the image path 
        /home/root/06_Face_recognition_spoof_detection_backup/Capture_face/captures/Chris.bmp           
        Loading : resnet50_bmp/drp_desc.bin
        Loading : resnet50_bmp/resnet50_bmp_drpcfg.mem
        Loading : resnet50_bmp/drp_param.bin
        Loading : resnet50_bmp/aimac_desc.bin
        Loading : resnet50_bmp/resnet50_bmp_weight.dat
        Inference -----------------------------------------------
        [START] DRP-AI
        [END] DRP-AI
        Input: /home/root/06_Face_recognition_spoof_detection_backup/Capture_face/captures/Chris.bmp
        ```
6. The face embedding database - `database/face_features.csv` would get updated.

#### Face Recognition Image

1. Copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Follow below steps;

```
cd /home/root/RZV2L_AiLibrary 
cd 06_Face_recognition_spoof_detection/Face_recognition_img/exe/
```
3. Copy test image (`sample.bmp`) to `06_Face_recognition_spoof_detection/Face_recognition_img/exe/`.
4. Run the executable.
```
root@smarc-rzv2l:~/RZV2L_AiLibrary/06_Face_recognition_spoof_detection/Face_recognition_img/exe# ./Face_recognition_img 
RZ/V2L DRP-AI Sample Application
Model : PyTorch ResNet    | resnet50_bmp
Input : Windows Bitmap v3 | sample.bmp
Loading : resnet50_bmp/drp_desc.bin
Loading : resnet50_bmp/resnet50_bmp_drpcfg.mem
Loading : resnet50_bmp/drp_param.bin
Loading : resnet50_bmp/aimac_desc.bin
Loading : resnet50_bmp/resnet50_bmp_weight.dat
Inference -----------------------------------------------
[START] DRP-AI
[END] DRP-AI
Input: sample.bmp

Recognized person is Abin
```

#### Face Recognition Camera

1. Copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Follow below steps;

```
cd /home/root/RZV2L_AiLibrary 
cd 06_Face_recognition_spoof_detection/Face_recognition_cam/exe/
```
4. Run the executable.
```
root@smarc-rzv2l:~/RZV2L_AiLibrary/06_Face_recognition_spoof_detection/Face_recognition_cam/exe# ./Face_recognition_cam 
RZ/V2L DRP-AI Sample Application
Model : PyTorch ResNet    | resnet50_cam
Input : Coral Camera
Loading : resnet50_cam/drp_desc.bin
Loading : resnet50_cam/resnet50_cam_drpcfg.mem
Loading : resnet50_cam/drp_param.bin
Loading : resnet50_cam/aimac_desc.bin
Loading : resnet50_cam/resnet50_cam_weight.dat
media-ctl -d /dev/media0 -r
system ret = 0
media-ctl -d /dev/media0 -V "'ov5645 0-003c':0 [fmt:UYVY8_2X8/640x480 field:none]"
system ret = 0
media-ctl -d /dev/media0 -l "'rzg2l_csi2 10830400.csi2':1 -> 'CRU output':0 [1]"
system ret = 0
media-ctl -d /dev/media0 -V "'rzg2l_csi2 10830400.csi2':1 [fmt:UYVY8_2X8/640x480 field:none]"
system ret = 0
[INFO] CSI2 Camera: /dev/video0
Key Hit Thread Starting
************************************************
* Press ENTER key to quit. *
************************************************
Main Loop Starts
Capture Thread Starting
Inference Thread Starting
Inference Loop Starting
Background calibration is done

key Detected.
Key Hit Thread Terminated
Main Process Terminated
AI Inference Thread Terminated
Capture Thread Terminated
Application End
```

## Limitations
1. Image input size (224x224) is fixed.
2. Background and light conditions affect the face recognition performance. It is recommeneded to fix device's place and then register a new face.
3. Multiple face embeddings of a person are required for better face recognition.
4. Spoof detection is not implemented.(To Do)

### Known issues:
1. [ERROR] Image buffer address is NULL : This error suggests that the input path to the image is improper. Verify the path, check whether an image available in the path.
2. Segmentation fault : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is larger than the actual image dimensions, then a segmentation fault will occur.
3. Improper output : If you are running the application in image mode, beware of the image dimensions entered. If entered image width or height is smaller than the actual image dimensions, then improper or unexpected outputs will be observed.
4. [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
5. permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.
6. [ERROR] Failed to open: <prefix>/<prefix>_weight.dat error=2. [ERROR] Failed to load data from memory: <prefix>/<prefix>_weight.dat Failed to load DRP-AI object files - This error suggests that the weight file is not availbale in the `exe` folder. Download the weight file from the release in github and place it properly in the `exe/subfolder`.

