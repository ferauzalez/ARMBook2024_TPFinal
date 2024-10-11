#include "alarm_interface_client.h"
#include "alarm_states.h"

using namespace std::chrono;

void NotReady::entry()
{
    if (this->firstTimeEnteringTheState_ == true) {
        this->firstTimeEnteringTheState_ = false;
        startMainTime_ = this->alarm_->readMainTimer();
        this->alarm_->setStateReport("No está listo");
    } else {
        this->inState();
    }
}

void NotReady::inState()
{
    currentMainTime_ = this->alarm_->readMainTimer();
    elapsedMainTime_ = currentMainTime_ - startMainTime_;

    if (elapsedMainTime_ >= 1000) {
        startMainTime_ = currentMainTime_;

        if (this->alarm_->getDistanceMeasured() < this->alarm_->kWarningDistance) {
            this->alarm_->setUltrasonicSensorReport("Obstáculo detectado");
        } else {
            this->alarm_->setUltrasonicSensorReport("Despejado");
        }

        if (this->alarm_->readReedSwitchValue() == this->alarm_->kReedSwitchValueWhenIsOpened) {
            this->alarm_->setReedSwitchReport("Ventana abierta");
        } else {
            this->alarm_->setReedSwitchReport("Ventana cerrada");
        }

        if (this->alarm_->getDistanceMeasured() >= this->alarm_->kWarningDistance && \
            this->alarm_->readReedSwitchValue() == this->alarm_->kReedSwitchValueWhenIsClosed) {
            this->exit();
        }
    }
}

void NotReady::exit()
{
    this->alarm_->deleteAllReports();
    this->alarm_->TransitionTo(new Ready);
}