#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "sample_app_yolov3_img.h"

using namespace std;
using namespace cv;

/* We initialize the demo class here */
class HCDemo
{
    public:
        void start(void);
};

/* This method invokes the API */
void HCDemo::start(void)
{
    /* Initialize the HeadCount class */
    HeadCount HC = HeadCount();

    /* Obtain the relative image path of the input image */
    HC.get_image_path();

    /* read image */
    Mat image = imread(HC.img_path.c_str());

    if(image.data == NULL)
    {
        printf("Invalid image buffer\n");
        return;
    }

    /* Get inference */
    HC.PRET_HC(image.data, HC.width, HC.height);
    
    return;
}

int32_t main(int32_t argc, char * argv[])
{
    /* Initialize the demo object*/
    HCDemo demo = HCDemo();

    /* Start the demo */
    demo.start();
    
    return 0;
}
