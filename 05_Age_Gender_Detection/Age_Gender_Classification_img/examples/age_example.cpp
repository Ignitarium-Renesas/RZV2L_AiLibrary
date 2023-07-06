#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include "sample_app_age_gender_img.h"

using namespace std;
using namespace cv;

string age_path = ".";

/* We initialize the demo class here */
class ageDemo
{
    public:
        void start(string img_path, int width, int height);
};

/* This method invokes the API */
void ageDemo::start(string img_path, int width, int height)
{
    /* Initialize the age class */
    age G = age();

    G.img_path = img_path;
    G.width = width;
    G.height = height;

    /* read image */
    Mat image = imread(G.img_path.c_str());

    if(image.data == NULL)
    {
        printf("Invalid image buffer\n");
        return;
    }

    if(G.width != image.size().width ||
       G.height != image.size().height)
    {
        G.width = image.size().width;
        G.height = image.size().height;
    }

    /* Get inference */
    G.PRET_G(image.data, G.width, G.height);
    
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
    ageDemo demo = ageDemo();

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
