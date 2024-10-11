#include "alarm_interface_client.h"
#include "alarm_states.h"

using namespace std::chrono;

void Ready::entry()
{
    if (this->firstTimeEnteringTheState_ == true) {
        this->firstTimeEnteringTheState_ = false;

        startMainTime_ = this->alarm_->readMainTimer();

        this->alarm_->setAccess(false);

        this->alarm_->setStateReport("LISTO");
        this->alarm_->setSystemReport("PREPARADO PARA ACTIVAR. INGRESE CONTRASEÑA");
    } else {
        this->inState();
    }
}

void Ready::inState()
{
    currentMainTime_ = this->alarm_->readMainTimer();
    elapsedMainTime_ = currentMainTime_ - startMainTime_;

    if (elapsedMainTime_ > kMatrixKeypadReadingPeriod){// in milliseconds
        startMainTime_ = currentMainTime_;
        for(int i=0;i<=3;i++) { //Write 0 in one row for enable it and 1 in the others to disable them 
            if (i == this->alarm_->getRowIndexValue()) {
                this->alarm_->setRowValue(i, 0);
            } else {
                this->alarm_->setRowValue(i, 1);
            }
        }

        for(int i=0;i<=3;i++) { // Read columns sequentially
            if (this->alarm_->getColumnValue(i) == 0) {
                char temp_keyPressed = this->alarm_->getKey(this->alarm_->getRowIndexValue(),i);
                if (temp_keyPressed != 'A') {
                    this->alarm_->storePasswordEntered(temp_keyPressed); //store pressed key in the array
                    this->alarm_->setSystemReport("");
                } else {
                    if (this->alarm_->checkIfPasswordEnteredIsCorrect() == true) {
                        this->alarm_->setAccess(true);
                        this->alarm_->setSystemReport("CONTRASEÑA CORRECTA");
                    } else {
                        this->alarm_->setSystemReport("CONTRASEÑA INCORRECTA");
                        this->alarm_->resetIndexPassEntered();
                    }
                }
            }
        }
        this->alarm_->incrementRowIndexValue();
        this->exit();
    }
}

void Ready::exit()
{
    if (this->alarm_->getAccess() == true) {
        this->alarm_->resetPasswordEntered();
        this->alarm_->TransitionTo(new Activating);
    } else if ( \
    this->alarm_->getDistanceMeasured() < this->alarm_->kWarningDistance || \
    this->alarm_->readReedSwitchValue() == this->alarm_->kReedSwitchValueWhenIsOpened) {
        this->alarm_->TransitionTo(new NotReady);
    }
}