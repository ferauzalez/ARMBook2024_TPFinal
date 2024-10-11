#include "alarm_interface_client.h"
#include "alarm_states.h"

using namespace std::chrono;

void Activating::entry()
{
    if (this->firstTimeEnteringTheState_ == true) {
        this->firstTimeEnteringTheState_ = false;

        startMainTime_ = this->alarm_->readMainTimer();
        this->countdown_ = this->alarm_->kActivatingPeriod;
        this->ledValue_ = 0;
        this->alarm_->setStateReport("ACTIVANDO");
    } else {
        this->inState();
    }
}

void Activating::inState()
{
    currentMainTime_ = this->alarm_->readMainTimer();
    elapsedMainTime_ = currentMainTime_ - startMainTime_;

    if (elapsedMainTime_ >= kWaitperiod) {
        startMainTime_ = currentMainTime_;

        this->alarm_->setAlertLedValueAndWriteOnIt(1);
        
        this->alarm_->setSystemReport("ACTIVANDO ALARMA EN " + std::to_string(this->countdown_)\
         + " SEGUNDOS");//el ultimo espacio es necesario para que se imprima bien en pantalla \
         cuando la cuenta es de un dígito
         this->exit();
    }
}

void Activating::exit()
{
    if (this->countdown_ > 0) {
        this->countdown_--;
    } else {
        this->alarm_->deleteAllReports();
        this->alarm_->TransitionTo(new OnAlert);
    }
}