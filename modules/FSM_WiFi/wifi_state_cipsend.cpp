#include "wifi_interface_client.h"
#include "wifi_states.h"
#include <string>

using namespace std::chrono;

void CIPSEND::entry()
{
    if (this->firstTimeEnteringTheState_ == true) {
        this->firstTimeEnteringTheState_ = false;
        this->wifi_->deleteWifiReportsOnly();
        startMainTime_ = this->wifi_->readMainTimer();
        this->wifi_->setWifiStateReport("CIPSEND");
        
        this->wifi_->tempBufferMessage_ = "";
        this->send();
    } else {
        this->inState();
    }
}

void CIPSEND::inState()
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

void CIPSEND::send()
{
    if (this->wifi_->connectionIDs[this->wifi_->currentIndexOfConnectionIDsArray] == '\0') {
        this->exit();
    } else {
        std::string ID{this->wifi_->connectionIDs[this->wifi_->currentIndexOfConnectionIDsArray]};
        this->ATCommand_ = "AT+CIPSEND=";
        this->ATCommand_ += ID;
        this->ATCommand_ += ",";
        this->ATCommand_ += to_string(this->wifi_->calculateSizeOfHTMLMessage()) + "\r\n";
        this->wifi_->setMessage(this->ATCommand_);
        this->wifi_->writeOnWiFiSerialPort();
    }
}

void CIPSEND::exit()
{
    if (this->wifi_->connectionIDs[this->wifi_->currentIndexOfConnectionIDsArray] == '\0') {
        this->wifi_->currentIndexOfConnectionIDsArray = 0;
        this->wifi_->resetConnectionIDs();
        this->wifi_->TransitionTo(new WAIT);
    } else if (this->indexOfTheFirstWordToSearchFor != std::string::npos && \
        this->indexOfTheSecondWordToSearchFor != std::string::npos) {  
        this->wifi_->TransitionTo(new SENDHTML);
    } else {
        this->attemps_--;
        if (this->attemps_ <= 0) {
            this->wifi_->TransitionTo(new CIPCLOSE);
        } else {
            this->inState();
        }
    }
}

void CIPSEND::processResponse()
{
    this->indexOfTheFirstWordToSearchFor = this->wifi_->tempBufferMessage_.find("OK");
    this->indexOfTheSecondWordToSearchFor = this->wifi_->tempBufferMessage_.find(">");
}