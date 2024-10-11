
#include "wifi_interface_state.h"
#include "mbed.h"
#include <string>
#include <map>

#define CONNECTIONS_SUPPORTED 5

const int kNumerOfkeysForReports = 6;


class Wifi {
    private:
        WifiStateInterface *state_;
        string message_;
        LowPowerTimer timerWifi_;
        
        int indexPasswordEntered_;

        string wifiSystemReport_;
        string wifiStateReport_;
        string alarmSystemReport_;
        string alarmStateReport_;
        string alarmReedSwitchReport_;
        string alarmUltrasonicSensorReport_;
        string HTMLMessage_;
        size_t HTMLMessageLength_;
        
    public:
        BufferedSerial *wifiSerialPort_;
        string tempBufferMessage_;
        Wifi(BufferedSerial *wifiSerialPort);
        ~Wifi();

        void setState(WifiStateInterface *state);
        void TransitionTo(WifiStateInterface *state);
        void run();
        void stop();
        void setMessage(string msg);

        void startMainTimer();
        void stopMainTimer();
        void resetMainTimer();
        double readMainTimer();

        void writeOnWiFiSerialPort();
        const int kMaxNumberOfConnectionsSupported = 5;
        char* connectionIDs = new char[CONNECTIONS_SUPPORTED];
        int currentIndexOfConnectionIDsArray = 0;
        int countdownForWaitState = 10;

        string getOutsideMessageForState();
        string getOutsideMessageForWindow();
        string getOutsideMessageForUltrasonic();
        string concatenateStringsToSendToGUI();
        size_t calculateSizeOfHTMLMessage();

        void setWifiSystemReport(string value);
        string getWifiSystemReport();

        void setWifiStateReport(string value);
        string getWifiStateReport();

        void setAlarmSystemReport(string value);
        string getAlarmSystemReport();

        void setAlarmStateReport(string value);
        string getAlarmStateReport();

        void setAlarmReedSwitchReport(string value);
        string getAlarmReedSwitchReport();

        void setAlarmUltrasonicSensorReport(string value);
        string getAlarmUltrasonicSensorReport();

        void deleteAllReports();
        void deleteWifiReportsOnly();

        void resetConnectionIDs();
};