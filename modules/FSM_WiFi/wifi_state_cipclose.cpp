#include "wifi_interface_client.h"
#include "wifi_states.h"
#include <string>

using namespace std::chrono;

void CIPCLOSE::entry()
{
    if (this->firstTimeEnteringTheState_ == true) {
        this->firstTimeEnteringTheState_ = false;
        this->wifi_->deleteWifiReportsOnly();
        startMainTime_ = this->wifi_->readMainTimer();
        this->wifi_->setWifiStateReport("CIPCLOSE");
        this->wifi_->tempBufferMessage_ = "";
        this->send();
    } else {
        this->inState();
    }
}

void CIPCLOSE::inState()
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

void CIPCLOSE::send()
{
    std::string ID{this->wifi_->connectionIDs[this->wifi_->currentIndexOfConnectionIDsArray]};
    this->ATCommand_ = "AT+CIPCLOSE=";
    this->ATCommand_ += ID;
    this->ATCommand_ += "\r\n";
    this->wifi_->setMessage(this->ATCommand_);
    this->wifi_->writeOnWiFiSerialPort();
}


void CIPCLOSE::exit()
{
    if (this->indexOfTheFirstWordToSearchFor != std::string::npos && \
        this->indexOfTheSecondWordToSearchFor != std::string::npos) {  
        this->wifi_->currentIndexOfConnectionIDsArray += 1;
        if (this->wifi_->currentIndexOfConnectionIDsArray > 4) {
            this->wifi_->currentIndexOfConnectionIDsArray = 0;
            this->wifi_->resetConnectionIDs();
            this->wifi_->TransitionTo(new WAIT);
        } else {
            this->wifi_->TransitionTo(new CIPSEND);
        }
    } else {
        this->attemps_--;
        if (this->attemps_ <= 0) {
            this->wifi_->TransitionTo(new CIPCLOSEALL);
        } else {
            this->inState();
        }
    }
}

void CIPCLOSE::processResponse()
{
    this->indexOfTheFirstWordToSearchFor = this->wifi_->tempBufferMessage_.find("CLOSED");
    this->indexOfTheSecondWordToSearchFor = this->wifi_->tempBufferMessage_.find("OK");
}