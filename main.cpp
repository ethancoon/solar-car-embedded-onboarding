#include "DigitalOut.h"
#include "PowerAuxCANInterface.h"
#include "Printing.h"
#include "ThisThread.h"
#include "log.h"
#include "pindef.h"
#include <mbed.h>
#include <rtos.h>

#define LOG_LEVEL          LOG_DEBUG
#define MAIN_LOOP_PERIOD   1s
#define ERROR_CHECK_PERIOD 100ms
#define FLASH_PERIOD       500ms

// TODO: add DRO pin to pindef.h and in main() set dro=1

PowerAuxCANInterface vehicle_can_interface(MAIN_CAN_RX, MAIN_CAN_TX,
                                           MAIN_CAN_STBY);

bool flashLeftTurnSignal, flashRightTurnSignal, flashHazards, flashBrakeLights = false;
Thread signalFlashThread;

DigitalOut brake_lights(BRAKE_LIGHT_EN);
DigitalOut leftTurnSignal(LEFT_TURN_EN);
DigitalOut rightTurnSignal(RIGHT_TURN_EN);
DigitalOut bms_strobe(BMS_STROBE_EN);
DigitalOut drl(DRL_EN);

void signalFlashHandler() {
    while (true) {
        if (flashHazards || flashLeftTurnSignal || flashRightTurnSignal) {
            if (flashHazards) {
                leftTurnSignal = !leftTurnSignal;
                rightTurnSignal = !leftTurnSignal;
            } else if (flashLeftTurnSignal) {
                leftTurnSignal = !leftTurnSignal;
                rightTurnSignal = false;
            } else if (flashRightTurnSignal) {
                rightTurnSignal = !rightTurnSignal;
                leftTurnSignal = false;
            } else {
                leftTurnSignal = false;
                rightTurnSignal = false;
            }
        }
    }
    // Change lights based on CAN information
    
}

AnalogIn fan_tach(FanTach);
AnalogIn brake_light_current(BRAKE_LIGHT_CURRENT);
AnalogIn headlight_current(DRL_CURRENT);
AnalogIn bms_strobe_current(BMS_STROBE_CURRENT);
AnalogIn left_turn_current(LEFT_TURN_CURRENT);
AnalogIn right_turn_current(RIGHT_TURN_CURRENT);

int main() {
    log_set_level(LOG_LEVEL);
    log_debug("Start main()");

    signalFlashThread.start(signalFlashHandler);
    drl = 1;
    while (true) {
        log_debug("Main thread loop");

        ThisThread::sleep_for(MAIN_LOOP_PERIOD);
    }
}

void PowerAuxCANInterface::handle(ECUPowerAuxCommands *can_struct) {
    // Process CAN data
    flashBrakeLights = can_struct->brakeLights;
    flashLeftTurnSignal = can_struct->leftTurnSignal;
    flashRightTurnSignal = can_struct->rightTurnSignal;
    flashHazards = can_struct->hazards;
}


