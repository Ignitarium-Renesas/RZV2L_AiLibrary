#include <string>
#include <iostream>

using namespace std;

/* The function declared below will be called internally by the API */
int32_t predict_gender(uint8_t* in_img_addr, uint32_t width, uint32_t height);

/* The Gender detection application class*/
class Gender
{
    public:
        string img_path;
        uint32_t width, height;
        void get_image_path(void);
        int32_t PRET_G(uint8_t* in_img_addr, uint32_t width, uint32_t height);

};

/* This method is used to get the input image relative path with 
respect to the executable directory */
void Gender::get_image_path(void)
{
    cout << "----------------------------------------" << endl;
    cout << "Running Gender detection demo" << endl;
    cout << "----------------------------------------" << endl << endl;
    cout << "Enter the relative path for the input image:" << endl;
    cin >> Gender::img_path;
    cout << "Enter the width:" << endl;
    cin >> Gender::width;
    cout << "Enter the height:" << endl;
    cin >> Gender::height;
}

/* This method wraps the predict_gender function which is declared above. 
This function will be exposed to the user */
int32_t Gender::PRET_G(uint8_t* in_img_addr, uint32_t width, uint32_t height)
{
    return predict_gender(in_img_addr, width, height);
}