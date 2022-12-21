#include <string>
#include <iostream>

using namespace std;

/* The function declared below will be called internally by the API */
int32_t get_safety_helmet_vest(uint8_t* in_img_addr, uint32_t width, uint32_t height);

/* The Safety Helmet Vest application class*/
class SafetyHelmetVest
{
    public:
        string img_path;
        uint32_t width, height;
        void get_image_path(void);
        int32_t PRED_SHV(uint8_t* in_img_addr, uint32_t width, uint32_t height);

};

/* This method is used to get the input image relative path with 
respect to the executable directory */
void SafetyHelmetVest::get_image_path(void)
{
    cout << "----------------------------------------" << endl;
    cout << "Running Safety Helmet Vest demo" << endl;
    cout << "----------------------------------------" << endl << endl;
    cout << "Enter the relative path for the input image:" << endl;
    cin >> SafetyHelmetVest::img_path;
    cout << "Enter the width:" << endl;
    cin >> SafetyHelmetVest::width;
    cout << "Enter the height:" << endl;
    cin >> SafetyHelmetVest::height;
}

/* This method wraps the get_safety_helmet_vest function which is declared above. 
This function will be exposed to the user */
int32_t SafetyHelmetVest::PRED_SHV(uint8_t* in_img_addr, uint32_t width, uint32_t height)
{
    return get_safety_helmet_vest(in_img_addr, width, height);
}
