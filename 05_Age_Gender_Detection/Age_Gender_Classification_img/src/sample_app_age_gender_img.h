#include <string>
#include <iostream>

using namespace std;

/* The function declared below will be called internally by the API */
int32_t predict_age_gender(uint8_t* in_img_addr, uint32_t width, uint32_t height);

/* The age detection application class*/
class age
{
    public:
        string img_path;
        uint32_t width, height;
        int32_t PRET_G(uint8_t* in_img_addr, uint32_t width, uint32_t height);

};

/* This method wraps the predict_age_gender function which is declared above. 
This function will be exposed to the user */
int32_t age::PRET_G(uint8_t* in_img_addr, uint32_t width, uint32_t height)
{
    return predict_age_gender(in_img_addr, width, height);
}
