# RZV2L AI Library - Car ahead departure detection

## Introduction
Car ahead departure detection is a sample application that detects the movement of a car in the front away from a reference point.
Application uses a deep learning based object detector tinyYoloV3 to detect the vehicles and to track the objects. 

sample video on YouTube -https://youtu.be/t-owFwsRWTI

## Application details
```
|-- Car_ahead_departure_detection_cam/ # Car ahead departure detection application on Coral camera input
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration of tiny yolovc
|   |-- examples/
|   |-- exe/
|   |   |-- car_ahead_departure_detection_cam_app # the executable
|   |   |-- tinyyolov3_cam # DRP-AI files of yolov3
|   |   |-- alert.wav #audio alert 
|   |   `-- labels.txt  # label file
|   `-- src/ # source code directory
|-- Car_ahead_departure_detection_usbcam/ # Car ahead departure detection application on USB camera input
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration of tiny yolov3
|   |-- examples/
|   |-- exe/
|   |   |-- car_ahead_departure_detection_usbcam_app # the executable
|   |   |-- tinyyolov3_cam # DRP-AI files of yolov3
|   |   |-- alert.wav #audio alert 
|   |   `-- labels.txt  # label file
|   `-- src/ # source code directory
`-- README.md
```
###  Deep learning model details

**<ins>TinyYoloV3</ins>** :- TinyYoloV3 is a state of the art neural network model. It belongs to well known Yolo family of object detectors. It requires less parameters to get  trained properly. 
- official link : [ Official Yolo website](https://pjreddie.com/darknet/yolo/)

### Building the application 
After doing desired modifications in the source code, Perform this steps on a laptop/PC with SDK installed.
- MIPI Cam
~~~ 
cd RZV2L_AiLibrary/13_Car_ahead_departure_detection/Car_ahead_departure_detection_cam
make
~~~
- USB Cam
~~~ 
cd RZV2L_AiLibrary/13_Car_ahead_departure_detection/Car_ahead_departure_detection_usbcam
make
~~~

## Running the application
The generated executable file from laptop needs to be transfered on the RZv2L board along with other files.

### Syntax
**./<application_name> <audio_interface>**
1. Applicaiton name :- car_ahead_departure_detection_<cam/usbcam>_app
2. Audio Interface:- 0/1 (0 for audio jack/ 1 for HDMI) (int dtype)

### Sample commands

- MIPI Cam
~~~ 
cd /home/root/RZV2L_AiLibrary 
cd 13_Car_ahead_departure_detection/Car_ahead_departure_detection_cam/exe/
./car_ahead_departure_detection_cam_app 0
~~~
- USB Cam
~~~ 
cd /home/root/RZV2L_AiLibrary 
cd 13_Car_ahead_departure_detection/Car_ahead_departure_detection_usbcam/exe/
./car_ahead_departure_detection_usbcam_app 0
~~~

 
 ## Limitations
- This is a simple sample tutorial application. It is provided for an user to experiment with an object detection model with a very basic tracker algorithm. 
- The tracking only happens within a central virtual space.
- Since we are using pretrained TinyYolov3 the experiment shows lesser accuracy at poor light condition.

**TinyYolov3** :- 
- Light-weight model :- Total number of learnable parameters are less as compared to other yolo models.
- Comparatively lower accuracy performance:- Some detections are missed in the challenging environment like fast moving objects, noisy background etc.

## Known issues:
- [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
- permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.  

## Note
This application less than v5.10 includes source code with GPLv3.0.
