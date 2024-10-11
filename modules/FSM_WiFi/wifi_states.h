#include "wifi_interface_state.h"
#include <string>
#include "mbed.h"

class CWJAP : public WifiStateInterface
{
    public:
        void entry()            override;
        void exit()             override;
        void inState()          override;
        void send()             override;
        void processResponse()  override;
};

class CIPMUX : public WifiStateInterface
{
    public:
        void entry()            override;
        void exit()             override;
        void inState()          override;
        void send()             override;
        void processResponse()  override;
};


class CIPSERVER : public WifiStateInterface
{
    public:
        void entry()            override;
        void exit()             override;
        void inState()          override;
        void send()             override;
        void processResponse()  override;
};

class WAIT : public WifiStateInterface
{
    public:
        void entry()            override;
        void exit()             override;
        void inState()          override;
        void send()             override;
        void processResponse()  override;
    private:
        bool ThereIsAnyConnectionAlive();
};

class CIPSTATE : public WifiStateInterface
{
    public:
        void entry()            override;
        void exit()             override;
        void inState()          override;
        void send()             override;
        void processResponse()  override;
    private:
        string wordToSearchFor_ = "+CIPSTATE:";
        size_t startingPosition_ = string::npos;
        size_t currentPosition_ = string::npos;
        size_t searchIDConnection_(size_t startingPosition);
        void saveIDConnection_(size_t indexOfWordFounded, int indexArray);
        void resetIDsSaved_();
        bool ThereIsAnyConnectionAlive();
        string temp = ""; //borrar despues
};

class TEST : public WifiStateInterface
{
    public:
        void entry()            override;
        void exit()             override;
        void inState()          override;
        void send()             override;
        void processResponse()  override;
    private:
        string wordToSearchFor_ = "+CIPSTATE:";
        size_t startingPosition_ = string::npos;
        size_t currentPosition_ = string::npos;
        size_t searchIDConnection_(size_t startingPosition);
        void saveIDConnection_(size_t indexOfWordFounded, int indexArray);
        void resetIDsSaved_();
        string temp = ""; //borrar despues
};

class CIPSEND : public WifiStateInterface
{
    public:
        void entry()            override;
        void exit()             override;
        void inState()          override;
        void send()             override;
        void processResponse()  override;
};


class SENDHTML : public WifiStateInterface
{
    public:
        void entry()            override;
        void exit()             override;
        void inState()          override;
        void send()             override;
        void processResponse()  override;
};

class CIPCLOSE : public WifiStateInterface
{
    public:
        void entry()            override;
        void exit()             override;
        void inState()          override;
        void send()             override;
        void processResponse()  override;
};

class CIPCLOSEALL : public WifiStateInterface
{
    public:
        void entry()            override;
        void exit()             override;
        void inState()          override;
        void send()             override;
        void processResponse()  override;
};