#include <string>
#include <iostream>

using namespace std;

/* The function declared below will be called internally by the API */
int32_t predict_age(uint8_t* in_img_addr, uint32_t width, uint32_t height);

/* The age detection application class*/
class age
{
    public:
        string img_path;
        uint32_t width, height;
        void get_image_path(void);
        int32_t PRET_G(uint8_t* in_img_addr, uint32_t width, uint32_t height);

};

/* This method is used to get the input image relative path with 
respect to the executable directory */
void age::get_image_path(void)
{
    cout << "----------------------------------------" << endl;
    cout << "Running age detection demo" << endl;
    cout << "----------------------------------------" << endl << endl;
    cout << "Enter the relative path for the input image:" << endl;
    cin >> age::img_path;
    cout << "Enter the width:" << endl;
    cin >> age::width;
    cout << "Enter the height:" << endl;
    cin >> age::height;
}

/* This method wraps the predict_age function which is declared above. 
This function will be exposed to the user */
int32_t age::PRET_G(uint8_t* in_img_addr, uint32_t width, uint32_t height)
{
    return predict_age(in_img_addr, width, height);
}