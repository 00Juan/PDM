#include <Arduino.h>
#include "PDMChannel.h"
#include <Wire.h>
#include "Beastdevices_INA3221.h"
#include "LiquidCrystal_I2C.h"

LiquidCrystal_I2C lcd(0x27, 20, 4); 
#define PRINT_DEC_POINTS 1  

//******Datos de los canales
PDMChannel channels[channelSize]={{25,0.3, 3000},{26,0.3, 3000},{27,0.5, 3000}};
float current[3];
float voltage[3];
unsigned channel_modes[3];
unsigned channel_status[3];
int mode = 0,dev=0,stat=0;
int cmo = 0, cmc = 0, cmr = 0;

//Módulo INA3221 para leer el voltaje y la corriente de los tres canales
Beastdevices_INA3221 ina3221(INA3221_ADDR41_VCC);
int const channelSize = 3;


//Funciones empleadas
void processSerialMessage();
int countCommas(String message);

//LCD
void lcdDrawIni();
void lcdDrawValues();

void setup()
{
  Serial.begin(115200);
  
  while (!Serial)
  {
    delay(1);
  }
  Wire.begin(4,22);
  ina3221.begin();
  ina3221.reset();

  ina3221.setShuntRes(100, 100, 100);

  lcd.init();                    
  lcd.backlight();
  lcdDrawIni();
}

void loop()
{
  //Lectura de los canales
  current[0] = ina3221.getCurrent(INA3221_CH1);
  voltage[0] = ina3221.getVoltage(INA3221_CH1);
  current[1] = ina3221.getCurrent(INA3221_CH2);
  voltage[1] = ina3221.getVoltage(INA3221_CH2);
  current[2] = ina3221.getCurrent(INA3221_CH3);
  voltage[2] = ina3221.getVoltage(INA3221_CH3);

  //Ejecución del método start() de los objetos
  for(int i=0;i<channelSize;i++)
  {
    channels[i].start(voltage[i],current[i]);
    channel_modes[i]=channels[i].stsMode();
    channel_status[i]=channels[i].stsStatus();
  }

  //Lectura de datos por serial para controlar los canales
  if (Serial.available() > 0)
  {
    processSerialMessage();
  }

  //Salida de los datos por el LCD
  lcdDrawValues();
  delay(200);
}

void processSerialMessage()
{
  String message = Serial.readStringUntil('\n');

  message.trim();

  if (message.length() == 6) {
    dev = message.charAt(0) - '0'; 
    mode = message.charAt(1) - '0';
    stat = message.charAt(2) - '0';
    cmo = message.charAt(3) - '0';
    cmc = message.charAt(4) - '0';
    cmr = message.charAt(5) - '0';

 
  } else {
    Serial.println("Error: Message does not have 6 digits.");
  }
  channels[dev].cmdMode=mode;
  channels[dev].cmdStatus=stat;
  channels[dev].cmdClose=(cmc==1);
  channels[dev].cmdOpen=(cmo==1);
  channels[dev].cmdReset=(cmr==1);
}


void lcdDrawIni()
{
  lcd.setCursor(0,0);
  lcd.print((String)"Canal1 Canal2 Canal3");
}

void lcdDrawValues()
{
  unsigned nsep=7;
  String s; char c;
  
  for(int i=0;i<3;i++)
  {
    lcd.setCursor(i*nsep,1);
    lcd.print(" ");
    lcd.print(voltage[i],PRINT_DEC_POINTS);
    lcd.print("V ");
    lcd.setCursor(i*nsep,2);
    lcd.print(" ");
    if(channels[i].stsStatus()!=2)
    {
      lcd.print(current[i],PRINT_DEC_POINTS);
    }
    else{
      lcd.print(channels[i].stsMaxCurrent(),1);
    }
    
    lcd.print("A ");
  }

  for(int j=0;j<3;j++)
  {
    if(channels[j].stsStatus()==0)  s="Err!";
    else if(channels[j].stsStatus()==1)  s="Cerr";
    else if(channels[j].stsStatus()==2)  s="Abrt";
    if(channels[j].cmdMode==0) c='M';
    else c='P';
    lcd.setCursor(j*nsep,3);
    lcd.print(c);
    lcd.print (" ");
    lcd.print(s);
    
  }
  delay(200);
  

}