
#include "alarm_interface_state.h"
#include "mbed.h"
#include <string>
#include <map>

using namespace std;

const int kNumberOfRowsOfMatrixKeypad = 4;
const int kNumberOfColumnsOfMatrixKeypad = 4;
const int kNumberOfPasswordDigits = 4;
const int kLEDAlertperiod = 1000;
const int kMatrixKeypadReadingPeriod = 70; // milliseconds
const int kWaitperiod = 1000;

class Alarm {
    private:
        AlarmStateInterface *state_;
        
        int keypadIndexRow_;
        DigitalOut (&keypadRows_)[kNumberOfRowsOfMatrixKeypad];
        DigitalIn (&keypadColumns_)[kNumberOfColumnsOfMatrixKeypad];
        char matrixKeypad_[kNumberOfRowsOfMatrixKeypad][kNumberOfColumnsOfMatrixKeypad] = {
            {'1','2','3','A'},
            {'4','5','6','B'},
            {'7','8','9','C'},
            {'*','0','#','D'}
        };

        bool accessGranted_;
        char correctPassword_[kNumberOfPasswordDigits] = {'8', '5', '6', '6'};
        char passwordEntered_[kNumberOfPasswordDigits] = {'0', '0', '0', '0'};
        int indexPasswordEntered_;

        DigitalOut alertLed_;
        DigitalOut horn_;
        DigitalOut buzzer_;
        DigitalOut triggerUltrasonic_;
        InterruptIn echoUltrasonic_;
        DigitalIn reedSwitchPin_;
        
        void echoRise_();
        void echoFall_();
        void firesUltrasonicBurst_();
        void stopUltrasonicBurst_();

        Ticker tickerUltrasonic_;
        Timeout timeoutUltrasonic_;
        Timer timerUltrasonic_;
        double timeBeginUltrasonic_;
        double timeEndUltrasonic_;
        double distanceMeasuredUltrasonic_;
        int buzzerValue_;
        int alertLedValue_;
        const double kHalfOfSpeedOfSound_ = 0.017; //0.017 = 0.034/2 in cm/us
        string systemReport_;
        string stateReport_;
        string reedSwitchReport_;
        string ultrasonicSensorReport_;

        LowPowerTimer timerAlarm_;
        
        double timeBegin_;
        double timeEnd_;
        double distanceMeasured_;
        
    public:
        Alarm
        ( DigitalOut (*keypadRows)[4], DigitalIn (*keypadColumns)[4], \
          PinName alertLedPin, PinName buzzerPin, PinName hornPin, \
          PinName triggerUltrasonicPin, PinName echoUltrasonicPin, PinName reedSwitchPin \
        );
        ~Alarm();

        const int kWarningDistance = 35; //in centimeters
        const int kWarningPeriod = 20000;
        const int kBuzzerPeriod = 1000;
        const int kReedSwitchValueWhenIsOpened = 1;
        //ReedSwitchValueWhenIsOpened is 0 if the pin is in pulldown mode
        const int kReedSwitchValueWhenIsClosed = 0;
        const int kActivatingPeriod = 15;
        
        void setState(AlarmStateInterface *state);
        void TransitionTo(AlarmStateInterface *state);
        void run();
        void stop();
        void startMainTimer();
        void stopMainTimer();
        void resetMainTimer();
        double readMainTimer();

        int getRowIndexValue();
        void incrementRowIndexValue();
        void setRowValue(int index, int value);
        int getColumnValue(int index);
        char getKey(int row, int col);

        void setAccess(bool value);
        bool getAccess();
        bool checkIfPasswordEnteredIsCorrect();
        void resetPasswordEntered();
        void resetIndexPassEntered();
        void storePasswordEntered(char pressedKey);
        
        void toggleBuzzerValueAndWriteOnIt();
        void setBuzzerValueAndWriteOnIt(int value);
        void turnOnHorn();
        void turnOffHorn();

        double getDistanceMeasured();
        int readReedSwitchValue();
        void toggleAlertLedValueAndWriteOnIt();
        void setAlertLedValueAndWriteOnIt(int value);

        void setSystemReport(string value);
        string getSystemReport();

        void setStateReport(string value);
        string getStateReport();

        void setReedSwitchReport(string value);
        string getReedSwitchReport();

        void setUltrasonicSensorReport(string value);
        string getUltrasonicSensorReport();

        void deleteAllReports();
};