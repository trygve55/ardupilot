/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <AP_Param/AP_Param.h>

class AP_SheepRTT_Backend;

class AP_SheepRTT {
public:
    AP_SheepRTT();

    AP_SheepRTT(const AP_SheepRTT &other) = delete;
    AP_SheepRTT &operator=(const AP_SheepRTT&) = delete;

    static AP_SheepRTT *get_singleton();
    static AP_SheepRTT *_singleton;

    // indicate whether this module is enabled or not
    bool enabled() const { return _enabled; }

    // initialise the sheepRTT
    void init();

    // grab - move the servo to the grab position
    void grab();

    // release - move the servo output to the release position
    void release();

    // released - returns true if currently in released position
    bool released() const;

    // grabbed - returns true if currently in grabbed position
    bool grabbed() const;

    // update - should be called at at least 10hz
    void update();

    // valid - returns true if we have a sheepRTT and it should work
    bool valid() const;

    static const struct AP_Param::GroupInfo        var_info[];

    // parameters
    AP_Int8     _enabled;               // grabber enable/disable

    typedef enum {
        STATE_GRABBING,
        STATE_RELEASING,
        STATE_GRABBED,
        STATE_RELEASED,
    }  sheepRTT_state;

    struct Backend_Config {
        AP_Int8     type;                  // grabber type (e.g. EPM or servo)
        AP_Int16    grab_pwm;              // PWM value sent to SheepRTT to initiate grabbing the cargo
        AP_Int16    release_pwm;           // PWM value sent to SheepRTT to release the cargo
        AP_Int16    neutral_pwm;           // PWM value sent to sheepRTT when not grabbing or releasing
        AP_Int8     regrab_interval;       // Time in seconds that sheepRTT will regrab the cargo to ensure grip has not weakend
        AP_Int16    uavcan_hardpoint_id;

        sheepRTT_state state = STATE_RELEASED;
    } config;

private:

    AP_SheepRTT_Backend *backend;
};

namespace AP {
    AP_SheepRTT *sheepRTT();
};
