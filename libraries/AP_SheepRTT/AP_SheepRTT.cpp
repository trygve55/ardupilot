#include "AP_SheepRTT.h"

#include "AP_SheepRTT_Servo.h"

extern const AP_HAL::HAL& hal;

#define SHEEPRTT_GRAB_PWM_DEFAULT        1900
#define SHEEPRTT_RELEASE_PWM_DEFAULT     1100
// EPM PWM definitions
#define SHEEPRTT_NEUTRAL_PWM_DEFAULT     1500
#define SHEEPRTT_REGRAB_DEFAULT          0           // default re-grab interval (in seconds) to ensure cargo is securely held

const AP_Param::GroupInfo AP_SheepRTT::var_info[] = {
    // @Param: ENABLE
    // @DisplayName: SheepRTT Enable/Disable
    // @Description: SheepRTT enable/disable
    // @User: Standard
    // @Values: 0:Disabled, 1:Enabled
    AP_GROUPINFO_FLAGS("ENABLE", 0, AP_SheepRTT, _enabled, 0, AP_PARAM_FLAG_ENABLE),

    // @Param: TYPE
    // @DisplayName: SheepRTT Type
    // @Description: SheepRTT enable/disable
    // @User: Standard
    // @Values: 0:None,1:Servo,2:EPM
    AP_GROUPINFO("TYPE", 1, AP_SheepRTT, config.type, 0),

    // @Param: GRAB
    // @DisplayName: SheepRTT Grab PWM
    // @Description: PWM value in microseconds sent to SheepRTT to initiate grabbing the cargo
    // @User: Advanced
    // @Range: 1000 2000
    // @Units: PWM
    AP_GROUPINFO("GRAB",    2, AP_SheepRTT, config.grab_pwm, SHEEPRTT_GRAB_PWM_DEFAULT),

    // @Param: RELEASE
    // @DisplayName: SheepRTT Release PWM
    // @Description: PWM value in microseconds sent to SheepRTT to release the cargo
    // @User: Advanced
    // @Range: 1000 2000
    // @Units: PWM
    AP_GROUPINFO("RELEASE", 3, AP_SheepRTT, config.release_pwm, SHEEPRTT_RELEASE_PWM_DEFAULT),

    // @Param: NEUTRAL
    // @DisplayName: Neutral PWM
    // @Description: PWM value in microseconds sent to grabber when not grabbing or releasing
    // @User: Advanced
    // @Range: 1000 2000
    // @Units: PWM
    AP_GROUPINFO("NEUTRAL", 4, AP_SheepRTT, config.neutral_pwm, SHEEPRTT_NEUTRAL_PWM_DEFAULT),

    // @Param: REGRAB
    // @DisplayName: SheepRTT Regrab interval
    // @Description: Time in seconds that sheepRTT will regrab the cargo to ensure grip has not weakened; 0 to disable
    // @User: Advanced
    // @Range: 0 255
    // @Units: s
    AP_GROUPINFO("REGRAB",  5, AP_SheepRTT, config.regrab_interval, SHEEPRTT_REGRAB_DEFAULT),

    // @Param: UAVCAN_ID
    // @DisplayName: EPM UAVCAN Hardpoint ID
    // @Description: Refer to https://docs.zubax.com/opengrab_epm_v3#UAVCAN_interface
    // @User: Standard
    // @Range: 0 255
    AP_GROUPINFO("UAVCAN_ID", 6, AP_SheepRTT, config.uavcan_hardpoint_id, 0),

    AP_GROUPEND
};

AP_SheepRTT::AP_SheepRTT()
{
    if (_singleton) {
#if CONFIG_HAL_BOARD == HAL_BOARD_SITL
        AP_HAL::panic("Too many sheepRTTs");
#endif
        return;
    }
    _singleton = this;

    AP_Param::setup_object_defaults(this, var_info);
}

/*
 * Get the AP_SheepRTT singleton
 */
AP_SheepRTT *AP_SheepRTT::_singleton = nullptr;
AP_SheepRTT *AP_SheepRTT::get_singleton()
{
    return _singleton;
}

void AP_SheepRTT::init()
{
    // return immediately if not enabled
    if (!_enabled.get()) {
        return;
    }

    switch(config.type.get()) {
    case 0:
        break;
    case 1:
        backend = new AP_SheepRTT_Servo(config);
        break;
    default:
        break;
    }
    if (backend != nullptr) {
        backend->init();
    }
}

// update - should be called at at least 10hz
#define PASS_TO_BACKEND(function_name) \
    void AP_SheepRTT::function_name()   \
    {                                  \
        if (!enabled()) {              \
            return;                    \
        }                              \
        if (backend != nullptr) {      \
            backend->function_name();  \
        }                              \
    }

PASS_TO_BACKEND(grab)
PASS_TO_BACKEND(release)
PASS_TO_BACKEND(update)

#undef PASS_TO_BACKEND


#define PASS_TO_BACKEND(function_name)        \
    bool AP_SheepRTT::function_name() const    \
    {                                         \
        if (!enabled()) {                     \
            return false;                     \
        }                                     \
        if (backend != nullptr) {             \
            return backend->function_name();  \
        }                                     \
        return false;                         \
    }

PASS_TO_BACKEND(valid)
PASS_TO_BACKEND(released)
PASS_TO_BACKEND(grabbed)

#undef PASS_TO_BACKEND

namespace AP {

AP_SheepRTT *sheepRTT()
{
    return AP_SheepRTT::get_singleton();
}

};
