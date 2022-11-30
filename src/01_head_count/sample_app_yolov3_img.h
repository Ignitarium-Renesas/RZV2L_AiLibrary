#include <string>
#include <iostream>

using namespace std;

/* The function declared below will be called internally by the API */
int32_t get_head_count(uint8_t* in_img_addr, uint32_t width, uint32_t height);

/* The head count application class*/
class HeadCount
{
    public:
        string img_path;
        uint32_t width, height;
        void get_image_path(void);
        int32_t PRET_HC(uint8_t* in_img_addr, uint32_t width, uint32_t height);

};

/* This method is used to get the input image relative path with 
respect to the executable directory */
void HeadCount::get_image_path(void)
{
    cout << "----------------------------------------" << endl;
    cout << "Running Head Count demo" << endl;
    cout << "----------------------------------------" << endl << endl;
    cout << "Enter the relative path for the input image:" << endl;
    cin >> HeadCount::img_path;
    cout << "Enter the width:" << endl;
    cin >> HeadCount::width;
    cout << "Enter the height:" << endl;
    cin >> HeadCount::height;
}

/* This method wraps the get_head_count function which is declared above. 
This function will be exposed to the user */
int32_t HeadCount::PRET_HC(uint8_t* in_img_addr, uint32_t width, uint32_t height)
{
    return get_head_count(in_img_addr, width, height);
}
