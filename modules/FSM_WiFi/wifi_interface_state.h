#pragma once
#include <string>

class Wifi;
const int kMaxLengthBuffer = 512;

class WifiStateInterface 
{
    protected:
        Wifi *wifi_;
        bool firstTimeEnteringTheState_;
        double elapsedMainTime_;
        double startMainTime_;
        double currentMainTime_;
        
        std::string response_;
        size_t readingProccessResult_ = 0;
        
        double elapsedWarningTime_ = 0;
        double startWarningTime_ = 0;
        double currentWarningTime_ = 0;
        const int kWaitPeriod = 1000;
        
        size_t indexOfTheFirstWordToSearchFor = std::string::npos;
        size_t indexOfTheSecondWordToSearchFor = std::string::npos;
        std::string ATCommand_;
        char buffer_[kMaxLengthBuffer] = {NULL};
        int attemps_ = 2;

    public:
        virtual ~WifiStateInterface(){
        };

        void resetVariables() 
        {
            this->firstTimeEnteringTheState_ = true;
        }

        void setClientInterface(Wifi *wifiInstance)
        {
            this->wifi_ = wifiInstance;
        };

        virtual void entry() = 0;
        virtual void inState() = 0;
        virtual void send() = 0;
        virtual void exit() = 0;
        virtual void processResponse() = 0;
};