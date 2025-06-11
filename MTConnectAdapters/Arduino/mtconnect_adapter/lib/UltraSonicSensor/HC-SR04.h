#ifndef ULTRASONICSENSOR_H
#define ULTRASONICSENSOR_H

#include <Arduino.h>

typedef struct HCSR04
{
    String id;
    uint8_t trigPin;
    uint8_t echoPin;    

    struct InternalData
    {
        int distance;
        long duration;
    } data;

    int getDistance();
};

#endif