#include "alarm_interface_client.h"
#include "alarm_states.h"

using namespace std::chrono;

void Warning::entry()
{
    if (this->firstTimeEnteringTheState_ == true) {
        this->firstTimeEnteringTheState_ = false;

        startMainTime_ = this->alarm_->readMainTimer();
        this->startBuzzerTime_ = startMainTime_;

        this->alarm_->setStateReport("WARNING \r\n");
        this->alarm_->setSystemReport("");
    } else {
        this->inState();
    }
}

void Warning::inState()
{
    currentMainTime_ = this->alarm_->readMainTimer();
    elapsedMainTime_ = currentMainTime_ - startMainTime_;

    this->currentBuzzerTime_ = currentMainTime_;
    this->elapsedBuzzerTime_ = currentBuzzerTime_ - startBuzzerTime_;

    if (elapsedBuzzerTime_ >= this->alarm_->kBuzzerPeriod) {
        startBuzzerTime_ = currentBuzzerTime_;

        this->alarm_->toggleBuzzerValueAndWriteOnIt();
    }

    this->exit();
}

void Warning::exit()
{
    if (this->alarm_->readReedSwitchValue() == this->alarm_->kReedSwitchValueWhenIsOpened) {
        this->alarm_->deleteAllReports();
        this->alarm_->TransitionTo(new Danger);
    } else if (this->elapsedMainTime_ > this->alarm_->kWarningPeriod) {
        this->alarm_->setBuzzerValueAndWriteOnIt(0);
        this->alarm_->deleteAllReports();
        this->alarm_->TransitionTo(new Danger);
    } else if (this->alarm_->getDistanceMeasured() > this->alarm_->kWarningDistance) {
            this->alarm_->setBuzzerValueAndWriteOnIt(0);
            this->alarm_->deleteAllReports();
            this->alarm_->TransitionTo(new OnAlert);
    } 
}