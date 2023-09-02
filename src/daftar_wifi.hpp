#define DAFTAR_WIFI_HPP
#ifndef DAFTAR_WIFI_HPP

#include "wifi.h"
class DaftarWifi{
    private:
        Wifi[10] daftarWifi;
        int len;
    public:
        DaftarWifi(){
            len = 0;
        };
        ~DaftarWifi(){

        };
        void add(Wifi wifi){
            if (len < 10){
                daftarWifi[len] = wifi;
                len++;
            }
        }
}

#endif