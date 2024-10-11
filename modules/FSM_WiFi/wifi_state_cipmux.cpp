#include "wifi_interface_client.h"
#include "wifi_states.h"
#include <string>

using namespace std::chrono;

void CIPMUX::entry()
{
    if (this->firstTimeEnteringTheState_ == true) {
        this->firstTimeEnteringTheState_ = false;
        this->wifi_->deleteWifiReportsOnly();
        startMainTime_ = this->wifi_->readMainTimer();
        this->wifi_->setWifiStateReport("CIPMUX");
        this->ATCommand_ = "AT+CIPMUX=1\r\n";
        this->wifi_->tempBufferMessage_ = "";
        this->send();
    } else {
        this->inState();
    }
}

void CIPMUX::inState()
{
    currentMainTime_ = this->wifi_->readMainTimer();
    elapsedMainTime_ = currentMainTime_ - startMainTime_;

    if (this->wifi_->wifiSerialPort_->readable()) {
        this->readingProccessResult_ = this->wifi_->wifiSerialPort_->read(this->buffer_, kMaxLengthBuffer);
  
        if (this->readingProccessResult_ > 0) { 
            this->wifi_->tempBufferMessage_.append(this->buffer_, this->readingProccessResult_);
            this->wifi_->setWifiSystemReport("\r\nInicio de respuesta\r\n" + this->wifi_->tempBufferMessage_ + "\r\nFin de respuesta");
        } else if (this->readingProccessResult_ < 0) { // indicates error
            this->wifi_->setWifiSystemReport("Reading error \r\n");
            this->exit();
        }
    }

    if (elapsedMainTime_ >= this->kWaitPeriod) {
        startMainTime_ = currentMainTime_;
        this->processResponse();
        this->exit();
    }
}

void CIPMUX::send()
{
    this->wifi_->setMessage(this->ATCommand_);
    this->wifi_->writeOnWiFiSerialPort();
}

void CIPMUX::exit()
{
    if (this->indexOfTheFirstWordToSearchFor != std::string::npos) {  
        this->wifi_->TransitionTo(new CIPSERVER);
    } else {
        this->wifi_->TransitionTo(new CIPMUX);
    }
}

void CIPMUX::processResponse()
{
    this->indexOfTheFirstWordToSearchFor = this->wifi_->tempBufferMessage_.find("OK");
}