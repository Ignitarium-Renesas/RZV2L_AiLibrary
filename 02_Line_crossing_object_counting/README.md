# RZV2L AI Library - Line crossing object counting

## Introduction
Line crossing object counting is a sample application that demonstrates counting of the objects when they cross a virtual line drawn by the user.
Application uses a deep learning based object detector tinyYoloV3 to detect the objects and a SORT based tracker to track the objects. 

sample video on YouTube -[Line crossing object counting demo](https://youtu.be/-fZypjgsBYo)

## Application details
```
|-- 02_Line_crossing_object_counting_MIPI_cam/ # Line crossing application on Coral camera input
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration of tiny yolov3
|   |-- examples/
|   |-- exe/
|   |   |-- 02_Line_crossing_object_counting # the executable
|   |   |-- tinyyolov3_crossing # DRP-AI files of yolov3
|   |   `-- labels.txt  # label file
|   `-- src/ # source code directory
|-- 02_Line_crossing_object_counting_USB_cam/ # Line crossing application on USB camera input
|   |-- Makefile
|   |-- etc/ # address map and pre/post process configuration of tiny yolov3
|   |-- examples/
|   |-- exe/
|   |   |-- 02_Line_crossing_object_counting # the executable
|   |   |-- tinyyolov3_crossing # DRP-AI files of yolov3
|   |   `-- labels.txt  # label file
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


### Image pipeline
The overall flow of the application can be described in following sequence :
1. Image is captured from the camera device and is stored into the buffer from the 'camera thread'.
2. The content in the buffer is processed by 'Inference thread'. This involves preprocessing, inference by the deep learning model (the DRP-AI model) and post processing specfic to the model.
3. Detections are then diplayed on the monitor. Existing detections are used by the object tracking algorithm to track the object. Tracker algorithm assigns a unique identification number (ID) to an object.
4. Movement of the object in one direction is ensured by the tracker. Unique IDs are mapped to the actual count of the object. 
5. This object count, along with the detection bounding boxes is displayed on the monitor.

###  Deep learning model details

**<ins>TinyYoloV3</ins>** :- TinyYoloV3 is a state of the art neural network model. It belongs to well known Yolo family of object detectors. It requires less parameters to get  trained properly. 
- official link : [ Official Yolo website](https://pjreddie.com/darknet/yolo/)
- Dataset: [HollywoodHeads](https://www.di.ens.fr/willow/research/headdetection/)


#### <ins>Working of SORT tracker</ins>
**<ins>SORT Tracker</ins>** :- SORT Tracker is a visual multiple object tracking framework based on rudimentary data association and state estimation techniques. It is designed for online tracking applications where only past and current frames are available and the method produces object identities on the fly.
- Tracker:[SORT](https://github.com/yasenh/sort-cpp)

#### <ins>How line crossing/counting works</ins>

- User gives the co-ordinates to draw the line (x1,y1),(x2,y2) and flow of direction(0/1). ( 0 means left to right, 1 means right to left).
- This line creates two virtual region of interest (top_roi,bottom_roi).
- To check if an person has moved from one roi to another, their first occurance(object_id) is stored in a hashmap.
- When a person from one roi moves to the other roi, depending upon the direction the counter is incremented.

<img src="./tracker_ss.png" alt="Sample application output"
     margin-right=10px; 
     width=600px;
     height=334px />

### Building the application 
After doing desired modifications in the source code, Perform this steps on a laptop/PC with SDK installed.
- MIPI Cam
~~~ 
cd RZV2L_AiLibrary/02_Line_crossing_object_counting/02_Line_crossing_object_counting_MIPI_cam
make
~~~
- USB Cam
~~~ 
cd RZV2L_AiLibrary/02_Line_crossing_object_counting/02_Line_crossing_object_counting_USB_cam
make
~~~

## Running the application
The generated executable file from laptop needs to be transfered on the RZv2L board along with other files.
~~~ 
cd exe/
./02_Line_crossing_object_counting person 150 0 350 450 1
~~~

### Sample image for the command entered above:

<img src="./sample_office.png" alt="Sample application output"
     margin-right=10px; 
     width=600px;
     height=334px />
     
### detailed explaination of the syntax

**./<application_name> <class_name> <line_pointx1> <line_pointy1> <line_pointx2> <line_pointy2> <flow_direction>**

1. Applicaiton name :- 02_Line_crossing_object_counting
2. Class_name :- person,car,cat,dog...(any class from coco dataset but limited to person for now)(string dtype)
3. line_pointx1 :- xmin(leftmost x co-ordinate) for the line (int dtype)
4. line_pointx1 :- ymin(leftmost y co-ordinate) for the line (int dtype)
5. line_pointx1 :- xmax(rightmost x co-ordinate) for the line (int dtype)
6. line_pointx1 :- ymax(rightmost y co-ordinate) for the line (int dtype)
7. flow_direction :- 0/1 (0 for left to right/ 1 for right to left) (int dtype)

## Limitations
- This is a simple sample tutorial application. It is provided for an user to experiment with an object detection model with a very basic tracker algorithm.
- It shows limited performance if people are moving very fast. this limitation can be improved with custom trained, better deep learning model. 
 
**TinyYolov3** :- 
- Light-weight model :- Total number of learnable parameters are less as compared to other yolo models.
- Comparatively lower accuracy performance:- Some detections are missed in the challenging environment like fast moving objects, noisy background etc.

**SORT Tracker** :- 
- Performance is strictly average in case of occlusions.
- Missed detection :- In case of missed detection, tracker may not be able to predict the precise location of bounding boxes.

- [ERROR] Failed to initialize Coral Camera - This error is observed if camera is not connected to the board. Check camera connection properly. Connect and restart the board.
- permission denied - This error may occur if executable file does not have execution permission. Use this command - `chmod 777 executable_filename` to assign proper permissions.  

