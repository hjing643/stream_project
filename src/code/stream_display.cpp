#include "stream_display.h"

int CStreamDisplay::init()
{
    if (is_init_)
    {
        return 1;
    }   
    is_init_ = true;
    return 1;
}
int CStreamDisplay::display()
{
    return 1;
}

