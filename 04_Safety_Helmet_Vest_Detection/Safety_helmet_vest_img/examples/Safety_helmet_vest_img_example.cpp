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
        void start(void);
};

/* This method invokes the API */
void SHVDemo::start(void)
{
    /* Initialize the SafetyHelmetVest class */
    SafetyHelmetVest SHV = SafetyHelmetVest();

    /* Obtain the relative image path of the input image */
    SHV.get_image_path();

    /* read image */
    Mat image = imread(SHV.img_path.c_str());

    if(image.data == NULL)
    {
        printf("Invalid image buffer\n");
        return;
    }

    /* Get inference */
    SHV.PRED_SHV(image.data, SHV.width, SHV.height);
    
    return;
}

int32_t main(int32_t argc, char * argv[])
{
    /* Initialize the demo object*/
    SHVDemo demo = SHVDemo();

    /* Start the demo */
    demo.start();
    
    return 0;
}
