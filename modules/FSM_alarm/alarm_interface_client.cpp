#include "alarm_interface_client.h"
#include "alarm_states.h"
using namespace std::chrono;

Alarm::Alarm
        ( DigitalOut (*keypadRows)[4], DigitalIn (*keypadColumns)[4], \
          PinName alertLedPin, PinName buzzerPin, PinName hornPin, \
          PinName triggerUltrasonicPin, PinName echoUltrasonicPin, PinName reedSwitchPin \
        ): keypadRows_(*keypadRows), keypadColumns_(*keypadColumns), \
          triggerUltrasonic_(triggerUltrasonicPin), echoUltrasonic_(echoUltrasonicPin), \
          alertLed_(alertLedPin), horn_(hornPin), buzzer_(buzzerPin),\
          reedSwitchPin_(reedSwitchPin)
{

    keypadIndexRow_ = 0;
    accessGranted_ = false;
    echoUltrasonic_.rise(callback(this, &Alarm::echoRise_));    // Attach handler to the rising interruptIn edge
    echoUltrasonic_.fall(callback(this, &Alarm::echoFall_));  // Attach handler to the falling interruptIn edge
    tickerUltrasonic_.attach(callback(this, &Alarm::firesUltrasonicBurst_), 500ms);
    reedSwitchPin_.mode(PullUp);
    startMainTimer();
    setState(new NotReady);
}

Alarm::~Alarm(){
    delete state_;
}

void Alarm::setState(AlarmStateInterface *state)
{
    if (this->state_ != nullptr) {
        delete this->state_;
    }

    this->state_ = state;
    this->state_->resetVariables();
    this->state_->setClientInterface(this);
}

void Alarm::TransitionTo(AlarmStateInterface *state)
{
    this->setState(state);
    this->state_->entry();
}

void Alarm::run()
{
    this->state_->entry();
}

void Alarm::stop()
{
    if (this->state_ != nullptr) {
        delete this->state_;
    }
}

void Alarm::startMainTimer()
{
    this->timerAlarm_.start();
}

void Alarm::stopMainTimer()
{
    this->timerAlarm_.stop();
}

void Alarm::resetMainTimer()
{
    this->timerAlarm_.reset();
}

double Alarm::readMainTimer()
{
    return duration_cast<milliseconds>(this->timerAlarm_.elapsed_time()).count();
}

int Alarm::getRowIndexValue()
{
    return this->keypadIndexRow_;
}

void Alarm::setRowValue(int index, int value)
{
    this->keypadRows_[index].write(value);
}

void Alarm::incrementRowIndexValue()
{
    this->keypadIndexRow_++;
    if (this->keypadIndexRow_ >= 4) {
        this->keypadIndexRow_ = 0;
    }
}

int Alarm::getColumnValue(int index)
{
    return this->keypadColumns_[index].read();
}


char Alarm::getKey(int row, int col)
{
    return this->matrixKeypad_[row][col];
}

void Alarm::setAccess(bool value)
{
    this->accessGranted_ = value;
}

bool Alarm::getAccess()
{
    return this->accessGranted_;
}

void Alarm::storePasswordEntered(char pressedKey)
{
    if (this->indexPasswordEntered_ > 3) {
        return;
    } else {
        this->passwordEntered_[this->indexPasswordEntered_] = pressedKey;
        this->indexPasswordEntered_++;
    }
}

bool Alarm::checkIfPasswordEnteredIsCorrect()
{
    for(int i=0;i<=3;i++) {
        if (this->passwordEntered_[i] != this->correctPassword_[i]) {
            return false;
        }
    }
    return true;
}

void Alarm::resetPasswordEntered()
{
    for(int i=0;i<=3;i++) {
        this->passwordEntered_[i] = '0';
    }
    return;
}

void Alarm::resetIndexPassEntered()
{
    this->indexPasswordEntered_ = 0;
}

void Alarm::toggleAlertLedValueAndWriteOnIt() {
    this->alertLedValue_ = !(this->alertLedValue_);
    this->alertLed_.write(this->alertLedValue_);
}

void Alarm::setAlertLedValueAndWriteOnIt(int value)
{
    this->alertLedValue_ = value;
    this->alertLed_.write(this->alertLedValue_);

}

void Alarm::toggleBuzzerValueAndWriteOnIt()
{
    this->buzzerValue_ = !(this->buzzerValue_);
    this->buzzer_.write(this->buzzerValue_);
}

void Alarm::setBuzzerValueAndWriteOnIt(int value)
{
    this->buzzer_.write(value);
}

void Alarm::turnOnHorn()
{
    this->horn_.write(1);
}

void Alarm::turnOffHorn()
{
    this->horn_.write(0);
}

double Alarm::getDistanceMeasured()
{
    return this->distanceMeasured_;
}

void Alarm::echoFall_() {
    this->timeEnd_ = (this->timerUltrasonic_.elapsed_time()).count();
    this->timerUltrasonic_.stop();
    this->distanceMeasured_ = this->kHalfOfSpeedOfSound_ * (this->timeEnd_ - this->timeBegin_);
}

void Alarm::echoRise_()
{
    this->timerUltrasonic_.reset();
    this->timerUltrasonic_.start();
    this->timeBegin_ = (this->timerUltrasonic_.elapsed_time()).count();
}

void Alarm::firesUltrasonicBurst_()
{
    this->triggerUltrasonic_.write(1);
    this->timeoutUltrasonic_.attach(callback(this, &Alarm::stopUltrasonicBurst_), 10us);
}

void Alarm::stopUltrasonicBurst_()
{
    this->triggerUltrasonic_.write(0);
}

int Alarm::readReedSwitchValue(){
    return this->reedSwitchPin_.read();
}

void Alarm::setSystemReport(string value) 
{
    this->systemReport_ = value;
}

string Alarm::getSystemReport() 
{
    return this->systemReport_;
}

void Alarm::setStateReport(string value) 
{
    this->stateReport_ = value;
}

string Alarm::getStateReport() 
{
    return this->stateReport_;
}

void Alarm::setReedSwitchReport(string value) 
{
    this->reedSwitchReport_ = value;
}

string Alarm::getReedSwitchReport() 
{
    return this->reedSwitchReport_;
}

void Alarm::setUltrasonicSensorReport(string value) 
{
    this->ultrasonicSensorReport_ = value;
}

string Alarm::getUltrasonicSensorReport() 
{
    return this->ultrasonicSensorReport_;
}

void Alarm::deleteAllReports()
{
    this->setSystemReport("");
    this->setStateReport("");
    this->setReedSwitchReport("");
    this->setUltrasonicSensorReport("");
}