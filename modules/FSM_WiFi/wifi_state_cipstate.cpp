#include "wifi_interface_client.h"
#include "wifi_states.h"
#include <string>

using namespace std::chrono;

void CIPSTATE::entry()
{
    if (this->firstTimeEnteringTheState_ == true) {
        this->firstTimeEnteringTheState_ = false;
        this->wifi_->deleteWifiReportsOnly();
        startMainTime_ = this->wifi_->readMainTimer();
        this->wifi_->setWifiStateReport("CIPSTATE");

        this->wifi_->resetConnectionIDs();
        
        this->ATCommand_ = "AT+CIPSTATE?\r\n";
        this->wifi_->tempBufferMessage_ = "";
        this->send();
        
    } else {
        this->inState();
    }
}

void CIPSTATE::inState()
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

void CIPSTATE::send()
{
    this->wifi_->setMessage(this->ATCommand_);
    this->wifi_->writeOnWiFiSerialPort();
}

void CIPSTATE::exit()
{
    if (this->indexOfTheFirstWordToSearchFor != std::string::npos &&
        this->ThereIsAnyConnectionAlive() == true) {
        this->wifi_->TransitionTo(new CIPSEND);
    } else {
        this->wifi_->TransitionTo(new WAIT);
    }
}

void CIPSTATE::processResponse()
{
    this->indexOfTheFirstWordToSearchFor = this->wifi_->tempBufferMessage_.find("OK");

    this->startingPosition_ = 0;
    for (int i = 0; i < this->wifi_->kMaxNumberOfConnectionsSupported; i++) {
        this->currentPosition_ = searchIDConnection_(this->startingPosition_);
        if (this->currentPosition_ != string::npos) {
            this->saveIDConnection_(this->currentPosition_, i);
            this->startingPosition_ = this->currentPosition_ + this->wordToSearchFor_.length();
        } else {
            break;
        }
    }

}

size_t CIPSTATE::searchIDConnection_(size_t startingPositionIndex)
{
    return this->wifi_->tempBufferMessage_.find(this->wordToSearchFor_, startingPositionIndex);
}

void CIPSTATE::saveIDConnection_(size_t indexOfWordFounded, int indexArray)
{
    size_t lengthOfIDConnection = 1;

    string substringWithOneElement = this->wifi_->tempBufferMessage_.substr(\
    indexOfWordFounded + this->wordToSearchFor_.length(), lengthOfIDConnection);

    this->wifi_->connectionIDs[indexArray] = substringWithOneElement[0];
}

void CIPSTATE::resetIDsSaved_()
{
    this->wifi_->resetConnectionIDs();
}

bool CIPSTATE::ThereIsAnyConnectionAlive()
{
    for (int i = 0; i < this->wifi_->kMaxNumberOfConnectionsSupported; i++) {
        if (this->wifi_->connectionIDs[i] != '\0') {
            return true;
        }
    }
    return false;
}