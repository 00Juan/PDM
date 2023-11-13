#ifndef __PDMChannel
#define __PDMChannel
#include <Arduino.h>
#include "Timer.h"

class PDMChannel
{
private:
    unsigned pin_control;
    float voltage; //V
    float current; //A
    unsigned energy; //kWh
    unsigned power; //W
    float max_current; //A
    unsigned opening_time;  //ms 
    unsigned status; //0-Failure 1-Closed 2-Open
    unsigned mode; //0-Manual 1-Automático

    bool open();
    bool close();
    bool reset();
    Timer timerCurrent;


    unsigned long int running_time, alarm_time;

public:
    PDMChannel(unsigned __pinControl,float __maxcur,unsigned _time);
    PDMChannel();
    ~PDMChannel();

    //Abrir canal
    bool cmdOpen;
    //Cerrar canal
    bool cmdClose;
    //Limpiar fallo y resetear energía total consumida
    bool cmdReset; 

    unsigned cmdStatus;
    unsigned cmdMode; //0-Manual 1-Automatic
    
    void start(float _voltage, float _current);


    void config(float _max_current,unsigned _opening_time);
    void config(unsigned __pin,float _max_current,unsigned _opening_time);
    void configMode(bool _mode);

    float stsVoltage() const;
    float stsCurrent() const;
    float stsMaxCurrent() const;
    unsigned stsOpeningTime() const;
    unsigned stsEnergy() const;
    unsigned stsPower() const;
    unsigned stsStatus() const;
    unsigned stsMode() const;


};



#endif