#ifndef SWGL
#include "../../SWGL.hpp"
#endif

bool SWGL::xinputConnected(int port/*=0*/) {  
    if (SWGL::win.enabled_flags & 2) {
        XINPUT_STATE state;
        ZeroMemory( &state, sizeof(XINPUT_STATE) );
        DWORD dwResult = XInputGetState(port, &state );
        if( dwResult == ERROR_SUCCESS ) { return true;}
    }
    else {
        SWGL::messageBox("Can't call `xinputConnected()` - XInput isn't init in the program", "Make sure to include the `XINPUT_ENABLED` flag  in your `SWGL::Window` variable, otherwise no Xinput functions will be executed.", 0, MB_OK);
    }
    return false;
}

int SWGL::xinputVibrate(int leftVal/*=65535*/, int rightVal/*=65535*/, int port/*=0*/){
    if (SWGL::win.enabled_flags & 2) {
        XINPUT_VIBRATION Vibration;
        ZeroMemory(&Vibration, sizeof(XINPUT_VIBRATION));
    
        Vibration.wLeftMotorSpeed = leftVal;
        Vibration.wRightMotorSpeed = rightVal;
    
        XInputSetState(port, &Vibration);
    }
    else {
        SWGL::messageBox("Can't call `xinputConnected()` - XInput isn't init in the program", "Make sure to include the `XINPUT_ENABLED` flag  in your `SWGL::Window` variable, otherwise no Xinput functions will be executed.", 0, MB_OK);
    }
    return 0;
}

SWGL::point SWGL::xinputGetTriggerValues(int port/*=0*/) {
    if (SWGL::win.enabled_flags & 2) {
        XINPUT_STATE state;
        ZeroMemory(&state, sizeof(XINPUT_STATE));
            
        XInputGetState(port, &state);
        return {state.Gamepad.bLeftTrigger, state.Gamepad.bRightTrigger};
    }
    else {
        SWGL::messageBox("Can't call `xinputGetTriggerValues()` - XInput isn't init in the program", "Make sure to include the `XINPUT_ENABLED` flag  in your `SWGL::Window` variable, otherwise no Xinput functions will be executed.", 0, MB_OK);
    }
    return {-1, -1};
}

SWGL::point SWGL::xinputGetBattery(int port/*=0*/) {
    if (SWGL::win.enabled_flags & 2) {
        XINPUT_BATTERY_INFORMATION state;
        ZeroMemory(&state, sizeof(XINPUT_BATTERY_INFORMATION));
            
        XInputGetBatteryInformation(port, BATTERY_DEVTYPE_GAMEPAD , &state);
        return {state.BatteryType, state.BatteryLevel};
    }
    else {
        SWGL::messageBox("Can't call `xinputGetBattery()` - XInput isn't init in the program", "Make sure to include the `XINPUT_ENABLED` flag  in your `SWGL::Window` variable, otherwise no Xinput functions will be executed.", 0, MB_OK);
    }
    return {-1, -1};
} 