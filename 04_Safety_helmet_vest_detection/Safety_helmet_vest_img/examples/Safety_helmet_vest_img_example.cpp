#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "sample_app_yolov3_img.h"

using namespace std;
using namespace cv;

//path to safety helmet vest exe
string shv_path = ".";

/* We initialize the demo class here */
class SHVDemo
{
    public:
        void start(string img_path, int width, int height);
};

/* This method invokes the API */
void SHVDemo::start(string img_path, int width, int height)
{
    /* Initialize the SafetyHelmetVest class */
    SafetyHelmetVest SHV = SafetyHelmetVest();

    SHV.img_path = img_path;
    SHV.width = width;
    SHV.height = height;

    /* read image */
    Mat image = imread(SHV.img_path.c_str());

    if(image.data == NULL)
    {
        printf("Invalid image buffer\n");
        return;
    }

    if(SHV.width != image.size().width ||
       SHV.height != image.size().height)
    {
        SHV.width = image.size().width;
        SHV.height = image.size().height;
    }

    /* Get inference */
    SHV.PRED_SHV(image.data, SHV.width, SHV.height);
    
    return;
}

int32_t main(int32_t argc, char * argv[])
{
    if(argc != 2 && argc != 4)
    {
        printf("Usage :\n");
        printf("\t%s [image_path]\n", argv[0]);
        printf("\t%s [image_path] [width] [height]\n\n", argv[0]);
        printf("Note : width and height are optional\n");
    }

    /* Initialize the demo object*/
    SHVDemo demo = SHVDemo();

    /* Start the demo */
    if(argc == 2)
    {
        demo.start(argv[1], 0, 0);
    }
    else if(argc == 4)
    {
        demo.start(argv[1], atoi(argv[2]), atoi(argv[3]));
    }
    
    return 0;
}
