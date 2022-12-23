#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "sample_app_gender_img.h"

using namespace std;
using namespace cv;

string gender_path = ".";

/* We initialize the demo class here */
class GenderDemo
{
    public:
        void start(void);
};

/* This method invokes the API */
void GenderDemo::start(void)
{
    /* Initialize the Gender class */
    Gender G = Gender();

    /* Obtain the relative image path of the input image */
    G.get_image_path();

    /* read image */
    Mat image = imread(G.img_path.c_str());

    if(image.data == NULL)
    {
        printf("Invalid image buffer\n");
        return;
    }

    /* Get inference */
    G.PRET_G(image.data, G.width, G.height);
    
    return;
}

int32_t main(int32_t argc, char * argv[])
{
    /* Initialize the demo object*/
    GenderDemo demo = GenderDemo();

    /* Start the demo */
    demo.start();
    
    return 0;
}