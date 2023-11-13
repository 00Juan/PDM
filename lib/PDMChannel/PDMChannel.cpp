

#include "PDMChannel.h"

bool PDMChannel::open()
{
    digitalWrite(pin_control, false);
    return true;
}
bool PDMChannel::close()
{
    digitalWrite(pin_control, true);
    return true;
}
bool PDMChannel::reset()
{
    status = 2;return true;
}

//Crea el objeto onfigurando el pin de salida del MOSFET, corriente máxima y tiempo de apertura en caso de i>imax. 
//Arranque en modo automático y estado abierto
PDMChannel::PDMChannel(unsigned __pinControl, float __maxcur,unsigned _time) : pin_control{__pinControl}, voltage{}, current{}, energy{}, power{}, max_current{__maxcur}, opening_time{_time}, status{}
{
    config(__pinControl,__maxcur,_time);
    mode=1;
    status = 2;
};
//Constructor por defecto
//Arranque en modo automático y estado abierto
PDMChannel::PDMChannel() : pin_control{}, voltage{}, current{}, energy{}, power{}, max_current{}, opening_time{}, status{}
{
    mode =1;
    status = 2;
};

PDMChannel::~PDMChannel()
{
}

//Rutina de ejecución de la máquina de estados, gestión de los diferentes modos y las transiciones entre ellos.
void PDMChannel::start(float _voltage, float _current)
{
    voltage = _voltage;
    current = _current;
    // Gestión de la condición de alarma con el temporizador
    if (current >= max_current)
    {
        if (timerCurrent.state() == STOPPED)
        {
            timerCurrent.start();
        }
    }
    else
    {
        timerCurrent.stop();
    }

    // Transiciones
    if (mode == 1) // En modo automático
    {
        if (cmdOpen)
            status = 2;                   // A abierto. El comando de abrir tiene prioridad, corta el paso de la corriente en cualquier estado
        else if (cmdClose && status == 2) // De abierto a cerrado. Se cierra el circuito sólo si se está en estado abierto
        {
            status = 1;
        }
        else if (status == 0 && cmdReset) // De alarmado a abierto
        {
            status = 2;
        }
        else if (timerCurrent.read() >= opening_time&&status==1) // De cerrado a alarmado
        {
            status = 0;
            timerCurrent.stop();
            Serial.println("ALLL");
        }
    }
    else if (mode == 0) // Modo manual;
    {
        status = cmdStatus; // Para depuración; no son necesarias las condiciones de transición para cambiar de un estado a otro
    }

    // Salidas
    if (status == 0)
    {
        open();
    }
    else if (status == 1)
    {
        close();
    }
    else if (status == 2)
    {
        open();
    }

    // Se resetean las órdenes para que se recalculen en cada ciclo
    cmdClose = false;
    cmdOpen = false;
    cmdReset = false;
}

//Configura corriente máxima y tiempo de apertura 
void PDMChannel::config(float _max_current,unsigned _opening_time)
{
    
    max_current = _max_current;
    opening_time = _opening_time;
}

//Configura pin del mossfet, corriente máxima y tiempo de apertura 
void PDMChannel::config(unsigned __pin,float _max_current,unsigned _opening_time)
{
        pin_control=__pin;
        pinMode(pin_control, OUTPUT);
        max_current = _max_current;
        opening_time = _opening_time;
}

//Voltaje del canal
float PDMChannel::stsVoltage() const
{
    return voltage;
}
//Corriente del canal
float PDMChannel::stsCurrent() const
{
    return current;
}
//Corriente máxima configurada para canal
float PDMChannel::stsMaxCurrent() const
{
    return max_current;
}
//Tiempo de apertura configurado para el canal
unsigned PDMChannel::stsOpeningTime() const
{
    return opening_time;
}
//Energía total consumida desde que se abrió el canal por primera vez. Poner a cero con cmdReset
unsigned PDMChannel::stsEnergy() const
{
}
unsigned PDMChannel::stsPower() const
{
    return voltage * current;
}
unsigned PDMChannel::stsStatus() const
{
    return status;
}
unsigned PDMChannel::stsMode() const
{
    return mode;
}
