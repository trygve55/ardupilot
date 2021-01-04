#include "AP_SheepRTT_Backend.h"

extern const AP_HAL::HAL& hal;

void AP_SheepRTT_Backend::init()
{
    init_sheepRTT();
}

// update - should be called at at least 10hz
void AP_SheepRTT_Backend::update()
{
    update_sheepRTT();
}
