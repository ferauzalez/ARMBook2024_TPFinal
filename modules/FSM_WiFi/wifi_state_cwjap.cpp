#include "wifi_interface_client.h"
#include "wifi_states.h"
#include <string>

using namespace std::chrono;

void CWJAP::entry()
{
    if (this->firstTimeEnteringTheState_ == true) {
        this->firstTimeEnteringTheState_ = false;
        this->wifi_->deleteWifiReportsOnly();
        startMainTime_ = this->wifi_->readMainTimer();
        this->wifi_->setWifiStateReport("CWJAP");
        this->ATCommand_ = "AT+CWJAP?\r\n";
        this->wifi_->tempBufferMessage_ = "";
        this->send();
    } else {
        this->inState();
    }
}

void CWJAP::inState()
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

void CWJAP::send()
{
    this->wifi_->setMessage(this->ATCommand_);
    this->wifi_->writeOnWiFiSerialPort();
}

void CWJAP::exit()
{
    if (this->indexOfTheFirstWordToSearchFor != std::string::npos && \
        this->indexOfTheSecondWordToSearchFor != std::string::npos) {  
        this->wifi_->TransitionTo(new CIPMUX);
    } else {
        this->wifi_->TransitionTo(new CWJAP);
    }
}

void CWJAP::processResponse()
{
    this->indexOfTheFirstWordToSearchFor = this->wifi_->tempBufferMessage_.find("+CWJAP:");
    this->indexOfTheSecondWordToSearchFor = this->wifi_->tempBufferMessage_.find("OK");
}