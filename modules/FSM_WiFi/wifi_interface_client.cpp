#include "wifi_interface_client.h"
#include "wifi_states.h"
#include <string>
using namespace std::chrono;

Wifi::Wifi(BufferedSerial *wifiSerialPort)
{
    wifiSerialPort_ = wifiSerialPort;
    resetConnectionIDs();
    startMainTimer();
    setState(new CWJAP);
}

Wifi::~Wifi(){
    delete state_;
}

void Wifi::setState(WifiStateInterface *state)
{
    if (this->state_ != nullptr) {
        delete this->state_;
    }

    this->state_ = state;
    this->state_->resetVariables();
    this->state_->setClientInterface(this);
}

void Wifi::TransitionTo(WifiStateInterface *state)
{
    this->tempBufferMessage_ = "";
    this->setState(state);
    this->state_->entry();
}

void Wifi::run()
{
    this->state_->entry();
}

void Wifi::stop()
{
    this->state_->exit();
}

void Wifi::setMessage(string msg)
{
    this->message_ = msg;
}

void Wifi::startMainTimer()
{
    this->timerWifi_.start();
}

void Wifi::stopMainTimer()
{
    this->timerWifi_.stop();
}

void Wifi::resetMainTimer()
{
    this->timerWifi_.reset();
}

double Wifi::readMainTimer()
{
    return duration_cast<milliseconds>(this->timerWifi_.elapsed_time()).count();
}

void Wifi::writeOnWiFiSerialPort()
{
    this->wifiSerialPort_->write(this->message_.c_str(), this->message_.length());
}

string Wifi::concatenateStringsToSendToGUI()
{
    string msg = "<!doctype html> <html> <head> <title>TP Final</title> <meta charset=\"UTF-8\"> ";
    msg += "<meta http-equiv=\"refresh\" content=\"5\" /> </head> <body style=\"text-align: left;\">";//
    msg += "<h1 style=\"color: #2f2fb1;\">Curso Embebidos ARM 2024</h1>";
    msg += "<h3>Trabajo Pr&aacutectico Final</h3>";
    msg += "<h3>Nombre: Alarma de seguridad para aberturas</h3>";
    msg += "<h3>Alumno: Fernando Augusto Gonz&aacutelez</h3>";
    msg += "<div style=\"font-weight: bold\">";
    msg +=    "<h2>Estado del sistema: " + this->getAlarmStateReport() + "</h2>";
    msg +=    "<h2>Sensor magnetico: " + this->getAlarmReedSwitchReport() + "</h2>";
    msg +=    "<h2>Sensor de distancia: " + this->getAlarmUltrasonicSensorReport() + "</h2>";
    msg +="</div> </body> </html>";
    return msg;
}

size_t Wifi::calculateSizeOfHTMLMessage()
{
    return this->concatenateStringsToSendToGUI().size();
}

void Wifi::setWifiSystemReport(string value)
{
    this->wifiSystemReport_ = value;
}

string Wifi::getWifiSystemReport()
{
    return this->wifiSystemReport_;
}

void Wifi::setWifiStateReport(string value)
{
    this->wifiStateReport_ = value;
}

string Wifi::getWifiStateReport()
{
    return this->wifiStateReport_;
}


void Wifi::setAlarmSystemReport(string value)
{
    this->alarmSystemReport_ = value;
}

string Wifi::getAlarmSystemReport()
{
    return this->alarmSystemReport_;
}


void Wifi::setAlarmStateReport(string value)
{
    this->alarmStateReport_ = value;
}

string Wifi::getAlarmStateReport()
{
    return this->alarmStateReport_;
}


void Wifi::setAlarmReedSwitchReport(string value)
{
    this->alarmReedSwitchReport_ = value;
}

string Wifi::getAlarmReedSwitchReport()
{
    return this->alarmReedSwitchReport_;
}


void Wifi::setAlarmUltrasonicSensorReport(string value)
{
    this->alarmUltrasonicSensorReport_ = value;
}

string Wifi::getAlarmUltrasonicSensorReport()
{
    return this->alarmUltrasonicSensorReport_;
}

void Wifi::deleteWifiReportsOnly()
{
    this->wifiSystemReport_ = "";
    this->wifiStateReport_ = "";
}

void Wifi::resetConnectionIDs()
{
    for (int i = 0; i < this->kMaxNumberOfConnectionsSupported; i++) {
        this->connectionIDs[i] = '\0';
    }
}
