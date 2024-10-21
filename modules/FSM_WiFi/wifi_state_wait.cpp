#include "wifi_interface_client.h"
#include "wifi_states.h"
#include <string>

using namespace std::chrono;

void WAIT::entry()
{
    if (this->firstTimeEnteringTheState_ == true) {
        this->firstTimeEnteringTheState_ = false;
        this->wifi_->deleteWifiReportsOnly();
        startMainTime_ = this->wifi_->readMainTimer();
        this->wifi_->setWifiStateReport("WAIT");
        this->wifi_->tempBufferMessage_ = "";
    } else {
        this->inState();
    }
}

void WAIT::inState()
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

void WAIT::send()
{
}

void WAIT::exit()
{
    if (this->indexOfTheFirstWordToSearchFor != std::string::npos) {  
        this->wifi_->TransitionTo(new CIPSTATE);
    } else if (this->ThereIsAnyConnectionAlive() == true) {
        this->wifi_->TransitionTo(new CIPSEND);
    } else {
        this->wifi_->countdownForWaitState--;
        if (this->wifi_->countdownForWaitState <= 0) {
            this->wifi_->countdownForWaitState = 10;
            this->wifi_->TransitionTo(new CIPSTATE);
        } else {
           this->wifi_->TransitionTo(new WAIT); 
        }
        
    }
}

void WAIT::processResponse()
{
    this->indexOfTheFirstWordToSearchFor = this->wifi_->tempBufferMessage_.find(",CONNECT");
}

bool WAIT::ThereIsAnyConnectionAlive()
{
    for (int i = 0; i < this->wifi_->kMaxNumberOfConnectionsSupported; i++) {
        if (this->wifi_->connectionIDs[i] != '\0') {
            return true;
        }
    }
    return false;
}