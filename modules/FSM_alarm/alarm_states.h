#include "alarm_interface_state.h"
#include <string>
#include "mbed.h"

class Ready : public AlarmStateInterface
{
    public:
        void entry()                        override;
        void exit()                         override;
        void inState()                      override;
};

class NotReady : public AlarmStateInterface
{
    public:
        void entry()                        override;
        void exit()                         override;
        void inState()                      override;
};

class Activating : public AlarmStateInterface
{
    public:
        void entry()                        override;
        void exit()                         override;
        void inState()                      override;
    private:
        int countdown_;
        int ledValue_;
};

class Danger : public AlarmStateInterface
{
    public:
        void entry()                        override;
        void exit()                         override;
        void inState()                      override;
    private:
        bool readyToComparePasswords_;
        int indexPasswordEntered_;
        int hornValue_ = 0;
};

class OnAlert : public AlarmStateInterface
{
    public:
        void entry()                        override;
        void exit()                         override;
        void inState()                      override;
    private:
        int ledValue_ = 0;
};

class Warning : public AlarmStateInterface
{
    public:
        void entry()                        override;
        void exit()                         override;
        void inState()                      override;
    private:
        double elapsedBuzzerTime_ = 0;
        double startBuzzerTime_ = 0;
        double currentBuzzerTime_ = 0;
};