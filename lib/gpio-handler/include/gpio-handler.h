#pragma once
#include "config.h"
#include "driver/gpio.h"

class builtin_led{

public:
    builtin_led();
    void set_level(bool state);
};