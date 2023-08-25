# RZV2L AI Library - Hand Gesture Recognition

## Introduction

Hand gesture recognition application is used to recognize the hand signs i.e,(one, two/scissor, three, four, five/paper, thumbs_down, thumbs_up, rock, blank) shown by an user in front of the camera.

> Refer [08_Hand_Gesture_Recognition/Hand_Gesture_Recognition_usbcam/src/sample_gesture_prediction.cpp](08_hand_gesture_detection/src/sample_gesture_prediction.cpp) for an example usage

sample video on YouTube - [hand gesture detection video](https://youtu.be/HodMvkLqELc)

## Application details

```
|-- 08_Hand_Gesture_Recognition/
|-- | Hand_Gesture_Recognition_cam/
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration
|   |-- exe/
|   |   |-- hand_gesture_detection_cam_app # the executable
|   |   `-- hand_cam # DRP-AI files of hrnetv2
|   `-- src/ # source code directory
|-- | Hand_Gesture_Recognition_usbcam/
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration
|   |-- exe/
|   |   |-- hand_gesture_detection_usbcam_app # the executable
|   |   `-- hand_cam # DRP-AI files of hrnetv2
|   `-- src/ # source code directory
`-- README.md
```
### Sample Application Workflow

The application has 4 threads as described below:
- **Main thread** -> initializing all the parameters data, drawing bounding boxes around the detected object,running the tracker and sending data to display (wayland).
- **Inference thread** -> setting of paramters related to DRP-AI, inferencing from the deep learning model and assigning the results from the model to a variable.
- **Camera thread** -> reading the frame from the camera device, storing the frame into the buffer,later passed to the inference thread.
- **Termination thread** -> This thread waits for the user to press 'enter key' in the terminal, once pressed, it will join to the main thread and main thread will terminate the other thread to stop the program execution.

This pictorial representation describes these 4 threads in detail:

     |
     |  Main Thread
     |
     |---------------|----------------|---------------|--------------------|
     | Main Thread   |Inference thread| Camera thread | Termination Thread |
     |               |                |               |                    |
     |               |                |               |                    |
     |               |                |               |                    |
     |               |                |               |                    |
     |               |                |               |                    |
     |               |                |               |     Enter Key      |
     |---------------|----------------|---------------|--------------------|
     | Main Thread
     | application terminated


### Image buffer pipeline
The overall flow of the application can be described in following sequence :
1. Image is captured from the camera device and is stored into the buffer from the 'camera thread'.
2. The content in the buffer is processed by 'Inference thread'. This involves preprocessing, inference by the deep learning model (the DRP-AI model) and post processing specfic to the model.
3. Detections(keypoints) are then diplayed on the monitor. Existing detections(keypoints) are used by the random forest classifier to classify the gesture into predefined classes.

## Image buffer flow architecture

    |
    |-Image
    |-HRNetv2(21 keypoints + confidence)
    |-Random Forest(21 keypoints + confidence)
    |-wayland sink
    |

## Model details

### HRNETV2

High-Resolution Net, is a general purpose convolutional neural network for tasks like semantic segmentation, object detection and image classification. It is able to maintain high resolution representations through the whole process. official link [HRNetv2](hhttps://arxiv.org/pdf/1908.07919v2.pdf).
HRNet is trained on a open source dataset(Wholebody on Coco_wholebody_hand).
You can find the [dataset](https://github.com/jin-s13/COCO-WholeBody/).

### Random Forest

Random forest is a commonly-used machine learning algorithm, which combines the output of multiple decision trees to reach a single result. It was trained on custom dataset for selected classes i.e, [one, two/scissor, three, four, five/paper, thumbs_down, thumbs_up, rock, blank].


### Application with camera input
- Camera input is fed to the model with proper pre-processing.
- Model produces 21 keypoints for the hand.
- The 21 keypoints(x,y cords) and confidence are then given to the random forest to classify the hand gesture.

### How to build the sample application

Please follow the below steps once the desired changes are made in the source code:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/08_Hand_Gesture_Recognition/Hand_Gesture_Recognition_<cam/usbcam> 
make
```

### Known issues:
1. Model was trained on Wholebody on Coco_wholebody_hand dataset, so horizontal hands are detected in better way than vertical hands by the pretrained model.
2. Model was trained on palm facing hands. it underperforms with back of the hand is visible.
3. [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
4. permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.
5. [ERROR] Failed to open: <prefix>/<prefix>_weight.dat error=2. [ERROR] Failed to load data from memory: <prefix>/<prefix>_weight.dat Failed to load DRP-AI object files - This error suggests that the weight file is not availbale in the `exe` folder. Download the weight file from the release in github and place it properly in the `exe/subfolder`.
