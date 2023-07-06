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
        int32_t PRET_HC(uint8_t* in_img_addr, uint32_t width, uint32_t height);

};

/* This method wraps the get_head_count function which is declared above. 
This function will be exposed to the user */
int32_t HeadCount::PRET_HC(uint8_t* in_img_addr, uint32_t width, uint32_t height)
{
    return get_head_count(in_img_addr, width, height);
}
