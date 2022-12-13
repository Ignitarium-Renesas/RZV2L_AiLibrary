**03_Line_crossing_object_counting**

### Installing the application

~~~ 
make
~~~

### Command to run the application

~~~ 
cd exe/
./03_Line_crossing_object_counting person 150 0 350 450 1
~~~

### Image for the sample above command

<img src="./sample_office.png" alt="Sample application output"
     margin-bottom=10px; 
     width=600px;
     height=334px />


### What's the command

**<application_name> <class_name> <line_pointx1> <line_pointy1> <line_pointx2> <line_pointy2> <flow_direction>**

1. Applicaiton name :- 03_Line_crossing_object_counting
2. Class_name :- person,car,cat,dog...(any class from coco dataset)(string dtype)
3. line_pointx1 :- xmin for the line (int dtype)
4. line_pointx1 :- ymin for the line (int dtype)
5. line_pointx1 :- xmax for the line (int dtype)
6. line_pointx1 :- ymax for the line (int dtype)
7. flow_direction :- 0/1 (0 for top-bottom/ 1 for bottom-top) (int dtype)


### Sample Application Workflow

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


- **Main thread** -> initializing all the parameters data, drawing bbox,running tracker and sending data to wayland.
- **Inference thread** -> setting of paramters related to drpai, inferencing from the model and storing the results from the model.
- **Camera thread** -> reading the image from the camera device,storing the image into the buffer,later passed to the model(inference thread).
- **Termination thread** -> or Keyboard hit thread, waits for the user to press enter in the terminal, once pressed it will join to the main thread and main thread will terminate the other thread to stop the program.

### Image pipeline

1. Image captured from the device and stored into the buffer from camera thread.
2. Buffer is passed through the model, post processing specfic to the model is done and detections are stored.
3. Detections are then diplayed, stored detections are used by the tracker to track the object and gives the id.
4. Unidirectional flow of the images are checked using the tracker and id to increase the counter and mapped into the image.
5. Output, detection mapped image then passed to the wayland to display out.

### Models(TinyYolov2,Centroid based Tracker)

**<ins>TinyYoloV2</ins>** :- Tiny yolov2 , SOTA model, related to yolo family with lesser number of parameters to train.

**<ins>Centroid Based Tracker</ins>** :- Generating a unique ID for each detected object based on centroid detection, tracking the objects as they move around in a video while maintaining the ID assignment.

#### <ins>Working of centroid tracker</ins>

1. Centroids are calculated based on detected bounding boxes.
2. Calculate the Euclidean distance between the centroid/s of all the objects detected in current frame and previous frames.
3. If the distance between the centroid of current frame and previous frame is less than the threshold, it is the same object in motion. Hence, use the existing object Id and update the bounding box coordinates of the object to the new bounding box value.
4. If the distance between the centroid of current frame and previous frame exceeds the threshold, add a new object id.
5. When objects detected in the previous frame cannot be matched to any existing objects, remove the object id from tracking.
6. If the object wasn't detected in previous n_frames, remove the object id from tracking.

#### <ins>How line crossing/counting works</ins>

- User gives the co-ordinates to draw the line (x1,y1),(x2,y2) and flow of direction(0/1).
- Based on line we define our two roi(top_roi,bottom_roi).
- To check if an object/person moved from one roi to another, we store their first occurance(object_id) in a hashmap.
- When an object from one roi moves to the other roi, depending upon the direction we increment the counter.

<img src="./tracker_ss.png" alt="Sample application output"
     margin-bottom=10px; 
     width=600px;
     height=334px />

#### <ins>Pros</ins>

**TinyYolov2** :- 
- SOTA model
- Easy to implement(sequntial)
- Few conv operations
- Less complexity
- Fast inference speed.

**Centroid Based Tracker** :- 
- Easy to implement
- Faster.

#### <ins>Cons</ins>

**TinyYolov2** :- 
- Light-weight model(less learning)
- Few labelled classes
- mAP is low
- Comparatively poor performance.

**Centroid Based Tracker** :- 
- Works poor on occlusion
- ID switching
- Missed detection
- No concept of re-identification
- No learning(compared to other DNN trackers.)


