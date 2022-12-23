# RZV2L AI Library - Face Recognition & Spoof Detection
# WORK IN PROGRESS - ONLY FACE RECOGNITION IS UP AND RUNNING AS OF NOW

## Introduction

The facial biometric system is widely being used for multiple applications. Its popularity is increasing because of its ease of use. One of the problem in this system is, its vulnerability to the spoof attacks. Facial spoof detection is the task of identifying false facial verification by using a photo, video, mask or a different substitute for an authorized person's face.

## Application details

```
|-- Makefile
|-- README.md
|-- etc/ # address map and pre/post process configuration of resnet-50
|-- exe/
|   |-- 06_Face_recognition_spoof_detection # The executable
|   |-- face_features.csv # Feature vectors of known people
|   |-- resnet50_bmp # DRP-AI files of resnet-50
|   `-- sample.bmp # sample test image
`-- src/ # source code directory
```

### Model details

#### ResNet-50

- Official paper - [Deep Residual Learning for Image Recognition](https://arxiv.org/pdf/1512.03385.pdf)
- Dataset - [VGGFace2](https://www.robots.ox.ac.uk/~vgg/data/vgg_face2/)
- In this application ResNet-50 is used for extracting facial features.

> Note: No transfer learning on custom data was performed for this application.

### Application with image input

- The user has to create embeddings of people and save them in `exe/face_features.csv`.
- The facial features of test image (ie, `sample.bmp`) obtained from ResNet-50 are compared with the embeddings of known people by taking euclidean distance. The identity is considered to be the person whose name corresponds to the shortest distance.
- Outputs the recognized person if the distance is less than a threshold value.

## Running the application

### Building the sample application:

This is an optional step since already compiled application is available in `exe/`.

These steps must be performed in Laptop/PC with the support of RZV2L toolchain.

Please follow the below steps for image application:

```
cd $work_dir
git clone https://github.com/Ignitarium-Renesas/RZV2L_AiLibrary 
cd RZV2L_AiLibrary/06_Face_recognition_spoof_detection/
make
```

### Running the sample application

1. Copy the `RZV2L_AiLibrary` directory to the board (/home/root/).
2. Follow below steps;

```
cd /home/root/RZV2L_AiLibrary 
cd 06_Face_recognition_spoof_detection/exe/
./06_Face_recognition_spoof_detection
```

### Example:

```
cd /home/root/RZV2L_AiLibrary 
cd 06_Face_recognition_spoof_detection/exe/
./06_Face_recognition_spoof_detection
```


## Limitations

1. Image input size (640x480) is fixed.
2. No provision to create face_features.csv from user end.(To Do)
3. Spoof detection is not implemented.(To Do)
