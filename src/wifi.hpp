#define WIFI
#ifndef WIFI
#include <string>

class Wifi
{
private:
    /* data */
    std::string ssid;
    std::string password;
public:
    Wifi(std::string ssid, std::string password);
    ~Wifi();
};

Wifi::Wifi(std::string ssid, std::string password)
{
    this->ssid =  ssid;
    this->password = password;
}

Wifi::~Wifi()
{
}

#endif