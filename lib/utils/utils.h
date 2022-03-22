#ifndef UTILS_h
#define UTILS_h

/*
    PROGMEME UTILS
*/

#include <vector>
#include <WString.h>
#include "lwip/sockets.h"

#define GPIO_IS_VALID_ANALOG_GPIO(gpio_num)   ((gpio_num < 40 && gpio_num > 31)) ||   ((gpio_num < 16 && gpio_num > 11))  || ((gpio_num < 28 && gpio_num > 24)) ||   gpio_num == 0 ||  gpio_num == 2 || gpio_num == 4                                                                 

template <typename T>
T Get_CONFIG_P(const T *sce)
{
    static T temp;
    memcpy_P(&temp, sce, sizeof(T));
    return temp;
}

template <typename T, size_t N>
size_t Size_P(T (&)[N]) { return N; }

class oz_utils
{
public:
    static std::vector<String> splitString(const char *init, char c)
    {
        std::vector<String> ret;

        String src = init;

        int i2 = 0;
        int i = src.indexOf(c);

        while (i > 0)
        {
            String item = src.substring(i2, i);
            if (item.length() != 0 && item.indexOf(c) < 0)
            {
                ret.push_back(item);
            }
            i2 = i + 1;
            i = src.indexOf(c, i2);
        }

        String item = src.substring(i2, src.length());
        if (item.length() != 0 && item.indexOf(c) < 0)
        {
            ret.push_back(item);
        }

        return ret;
    }
};

#endif