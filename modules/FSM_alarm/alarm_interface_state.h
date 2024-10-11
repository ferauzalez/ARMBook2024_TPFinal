#pragma once

class Alarm;

class AlarmStateInterface 
{
    protected:
        Alarm *alarm_;
        bool firstTimeEnteringTheState_;
        double elapsedMainTime_;
        double startMainTime_;
        double currentMainTime_;
        double elapsedUltrasonicTime_;
        double startUltrasonicTime_;
        double currentUltrasonicTime_;

    public:
        virtual ~AlarmStateInterface(){
        };

        void resetVariables() 
        {
            this->firstTimeEnteringTheState_ = true;
        }

        void setClientInterface(Alarm *alarmInstance)
        {
            this->alarm_ = alarmInstance;
        };

        virtual void entry()    = 0;
        virtual void inState()  = 0;
        virtual void exit()     = 0;
};