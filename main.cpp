#include "mbed.h"
#include "alarm_interface_client.h"
#include "wifi_interface_client.h"
#include <string>

using namespace std::chrono;

const int kNumberOfParametersOfAlarmToReport = 4;
const int kNumberOfParametersOfWifiToReport = 2;

static DigitalOut keypad_row_0(D11); // R1
static DigitalOut keypad_row_1(D10); // R2
static DigitalOut keypad_row_2(D9);  // R3
static DigitalOut keypad_row_3(D12); // R4
static DigitalOut keypadRows[4] = \
{keypad_row_0, keypad_row_1, keypad_row_2, keypad_row_3};

static DigitalIn keypad_column_0(D7, PullUp); // C1
static DigitalIn keypad_column_1(D6, PullUp); // C2
static DigitalIn keypad_column_2(D5, PullUp); // C3
static DigitalIn keypad_column_3(D4, PullUp); // C4
static DigitalIn keypadColumns[4] = \
{keypad_column_0, keypad_column_1, keypad_column_2, keypad_column_3};

static BufferedSerial esp32uart(PA_9, PA_10, 115200); 
// PA_9 es UART1_TX, PA_10 es UART1_RX
static UnbufferedSerial pcUartClient(USBTX, USBRX, 115200);

PinName triggerUltrasonicPin = PA_0;
PinName echoUltrasonicPin = PA_1;
PinName ledPinForActivatingAndOnAlertStates = PA_4;
PinName hornPin = PC_1;
PinName buzzerPin = PB_0;
PinName reedSwitchPin = PB_3;

std::string messageToWriteOnPC = "";
LowPowerTimer timerReport;

Alarm *alarm = new Alarm
(   &keypadRows, &keypadColumns,\
    ledPinForActivatingAndOnAlertStates, buzzerPin, hornPin,\
    triggerUltrasonicPin, echoUltrasonicPin, reedSwitchPin \
);

Wifi *wifi = new Wifi(&esp32uart);

void updatePCReports();
void updateWifiReports();

int main(void)
{
    timerReport.start();
    double timerReportBegin = duration_cast<milliseconds>(timerReport.elapsed_time()).count();
    double timerReportCurrent;
    double timerReportElapsed;
 
    while (true) {
        timerReportCurrent = duration_cast<milliseconds>(timerReport.elapsed_time()).count();
        timerReportElapsed = timerReportCurrent - timerReportBegin;
        if (timerReportElapsed > 1000) {
            timerReportBegin = timerReportCurrent;
            updatePCReports();
        }
        alarm->run(); 
        wifi->run();
        updateWifiReports();          
    }
}

void updatePCReports()
{
    messageToWriteOnPC = "";
    messageToWriteOnPC.append("\r\n____________________________________________________");
    messageToWriteOnPC.append("\r\nCurso Embebidos ARM");
    messageToWriteOnPC.append("\r\nTP final - Alarma de seguridad para aberturas");
    messageToWriteOnPC.append("\r\nAlumno: Fernando Augusto Gonzalez");
    messageToWriteOnPC.append("\r\n____________________________________________________");
    messageToWriteOnPC.append(string("\r\nALARMA"));
    messageToWriteOnPC.append(string("\r\n  - Estado:             ") + alarm->getStateReport());
    messageToWriteOnPC.append(string("\r\n  - Sensor magnético:   ") + alarm->getReedSwitchReport());
    messageToWriteOnPC.append(string("\r\n  - Sensor ultrasónico: ") + alarm->getUltrasonicSensorReport());
    messageToWriteOnPC.append(string("\r\n  - Sistema:            ") + alarm->getSystemReport());
    messageToWriteOnPC.append(string("\r\nWi-Fi"));
    messageToWriteOnPC.append(string("\r\n  - Estado:             ") + wifi->getWifiStateReport());
    messageToWriteOnPC.append(string("\r\n  - Sistema:            ") + wifi->getWifiSystemReport());

    pcUartClient.write(messageToWriteOnPC.c_str(), messageToWriteOnPC.length());
}

void updateWifiReports()
{
    wifi->setAlarmStateReport(alarm->getStateReport());
    wifi->setAlarmReedSwitchReport(alarm->getReedSwitchReport());
    wifi->setAlarmUltrasonicSensorReport(alarm->getUltrasonicSensorReport());
    wifi->setAlarmSystemReport(alarm->getSystemReport());
}