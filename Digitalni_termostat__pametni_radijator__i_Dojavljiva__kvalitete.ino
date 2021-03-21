#defineBLYNK_PRINTSerial
#include<Wire.h>
#include<Adafruit_Sensor.h>
#include"Adafruit_SGP30.h"
//#include <WiFi.h>
#include<ESP8266WiFi.h>
#include<ESP8266mDNS.h>
#include<WiFiUdp.h>
#include<ArduinoOTA.h>
#include<BlynkSimpleEsp8266.h>
#include<DHT.h>
#include<math.h>
#define DHTPIN 0  

// You shouldgetAuth Token intheBlynk App.
// Go to the Project Settings (nuticon).
charauth[] ="hx5CVXD0lRMkPGTYxewxNXlPsTGRM2s6";

// Your WiFi credentials.
// Set password to "" for opennetworks.
charssid[] ="";
charpass[] ="";
bool stanje=HIGH,kalibracija=HIGH,kalibracija1=HIGH;
bool a=HIGH,a1=HIGH,n1=0,n2=0,n3=0,n4=0, notifikacije=HIGH;
constint Grijanje1 = 12;
constint Grijanje2 = 13;
int temp1=22,temp2=21,stupanj_grijanja,voc,co2;
int co2_visok,co2_previsok,voc_visok,voc_previsok;
floattemp_offset=0.5,h,t;
int b=0,brojac_baseline  = 0,c=0;//brojaci
uint16_tTVOC_base, eCO2_base;//varijable za slanje vrijednosti mqtt-om
//String co2_baza,voc_baza;//varijable za primanje baseline vrijednosti mqtt
int co2b,vocb;//tip varijable koji prima sgp.setIAQBaseline(co2b, vocb);


Adafruit_SGP30 sgp; 


BLYNK_CONNECTED() {
  // Requestthelateststatefromthe server
   Blynk.syncVirtual(V3);
   Blynk.syncVirtual(V1);
   Blynk.syncVirtual(V2);
   Blynk.syncVirtual(V4);
   Blynk.syncVirtual(V9);
   Blynk.syncVirtual(V8);
   Blynk.syncVirtual(V11); 
   Blynk.syncVirtual(V14); 
   Blynk.syncVirtual(V15); 
   Blynk.syncVirtual(V16); 
   Blynk.syncVirtual(V20); 
   Blynk.syncVirtual(V21); 
   Blynk.syncVirtual(V22); 
   Blynk.syncVirtual(V23); 
   Blynk.syncVirtual(V24); 
   Blynk.syncVirtual(V25); 
   Blynk.syncVirtual(V26);      

  // Alternatively, youcouldoverride server stateusing:
  //Blynk.virtualWrite(V2, ledState);
}
BLYNK_WRITE(V3) {//botun auto/manual grijanje


  stanje=param.asInt();
  }
  BLYNK_WRITE(V20) {//botun kalibracija


 kalibracija=param.asInt();
  }
   BLYNK_WRITE(V21) {//botun kalibracija


 kalibracija1=param.asInt();
  }
  BLYNK_WRITE(V15) {


  if((eCO2_base=!0))
  co2b=param.asInt();

  }
   BLYNK_WRITE(V16) {
  if(TVOC_base=!0)

  vocb=param.asInt();
  }


BLYNK_WRITE(V2) {//reset botun
  if((param.asInt()==1))
  {
    ESP.restart();
  }

 }

BLYNK_WRITE(V1)
 {
if((param.asInt()==1)&&(stanje==LOW))
  {
     digitalWrite(Grijanje1,HIGH);
     a=HIGH;
  }
  if ((param.asInt()==0)&&(stanje==LOW))
  {
     digitalWrite(Grijanje1,LOW);
     a=LOW;
  }
 }
BLYNK_WRITE(V8)
 {
if((param.asInt()==1)&&(stanje==LOW))
  {
     digitalWrite(Grijanje2,HIGH);
     a1=HIGH;
  }
  if ((param.asInt()==0)&&(stanje==LOW))
  {
     digitalWrite(Grijanje2,LOW);
     a1=LOW;
  }

  
  
 }
  BLYNK_WRITE(V4)
 {
  temp1=param.asInt();
 }
  BLYNK_WRITE(V9)
 {

  temp2=param.asInt();
 }
  BLYNK_WRITE(V11)
 {
  temp_offset=param.asFloat();
 }
  BLYNK_WRITE(V22)
 {
  co2_visok=param.asInt();
 }
  BLYNK_WRITE(V23)
 {
  co2_previsok=param.asInt();
 }
BLYNK_WRITE(V24)
 {
  voc_visok=param.asInt();
 }
  BLYNK_WRITE(V25)
 {
  voc_previsok=param.asInt();
 }
BLYNK_WRITE(V26)
 {
  notifikacije=param.asInt();
 }
// Uncommentwhatevertypeyou'reusing!
#define DHTTYPE DHT11     // DHT 11
//#define DHTTYPE DHT22   // DHT 22, AM2302, AM2321
//#define DHTTYPE DHT21   // DHT 21, AM2301

DHTdht(DHTPIN, DHTTYPE);
BlynkTimertimer;

// ThisfunctionsendsArduino'sup time everysecond to Virtual Pin (5).
// In theapp, Widget'sreadingfrequencyshouldbe set to PUSH. Thismeans
// thatyoudefine how often to send data to Blynk App.
voidsendSensor()
{

  h =dht.readHumidity();
  t =dht.readTemperature(); // or dht.readtemp1erature(true) for Fahrenheit
  
  // You cansendanyvalue at any time.
  // Pleasedon'tsend more that 10 values per second.
  Blynk.virtualWrite(V1, a);
  Blynk.virtualWrite(V8, a1);
  Blynk.virtualWrite(V7,stupanj_grijanja);
  Blynk.virtualWrite(V5, t);
  Blynk.virtualWrite(V6, h);
  Blynk.virtualWrite(V10,WiFi.RSSI()); 
  Blynk.virtualWrite(V19,temp_offset);
  int callFunction2 =timer.setTimeout(1000, sendSensor2);
}

void sendSensor2() 
{

  co2= sgp.eCO2;
  voc=sgp.TVOC;
  Blynk.virtualWrite(V1, a);
  Blynk.virtualWrite(V8, a1);
  Blynk.virtualWrite(V7,stupanj_grijanja);
  Blynk.virtualWrite(V12, co2);
  Blynk.virtualWrite(V13,voc); 
  Blynk.virtualWrite(V15, eCO2_base);
  Blynk.virtualWrite(V16,TVOC_base);
  Blynk.virtualWrite(V17,co2b );
  Blynk.virtualWrite(V18,vocb);
  int callFunction3 =timer.setTimeout(1000, sendSensor3);
   // your sensor2 codehere  
   // THREE
}
void sendSensor3() 
{
  Blynk.virtualWrite(V1, a);
  Blynk.virtualWrite(V8, a1);
  Blynk.virtualWrite(V7,stupanj_grijanja);
  // your sensor2 codehere  
   // THREE
}




voidsetup()
{
  // Debugconsole
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid,pass);   
  sgp.begin();
  dht.begin();
  pinMode(Grijanje1,OUTPUT);
  pinMode(Grijanje2,OUTPUT);
  digitalWrite(Grijanje1,HIGH);
  digitalWrite(Grijanje2,HIGH);
  


  // You canalsospecify server:
  //Blynk.begin(auth, ssid, pass, "blynk-cloud.com", 8442);
  //Blynk.begin(auth, ssid, pass, IPAddress(192,168,1,100), 8442);
   Blynk.config(auth);
   Blynk.connect();

  // Setup a function to becalledeverysecond
  timer.setInterval(3000L,sendSensor);

  ArduinoOTA.onStart([]() {
    Serial.println("Start");
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsignedintprogress,unsignedint total) {
    Serial.printf("Progress: %u%%\r", (progress/ (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_terror) {
    Serial.printf("Error[%u]: ",error);
    if (error== OTA_AUTH_ERROR) Serial.println("AuthFailed");
    elseif (error== OTA_BEGIN_ERROR) Serial.println("BeginFailed");
    elseif (error== OTA_CONNECT_ERROR) Serial.println("ConnectFailed");
    elseif (error== OTA_RECEIVE_ERROR) Serial.println("ReceiveFailed");
    elseif (error== OTA_END_ERROR) Serial.println("EndFailed");
  });
  ArduinoOTA.setHostname("Grijanje i kvalieteta zraka");
  ArduinoOTA.begin();

}

voidloop()
{
ArduinoOTA.handle();

if (sgp.IAQmeasure())
  {
    sgp.IAQmeasureRaw();
    sgp.setHumidity(getAbsoluteHumidity(t,h));
    if (Blynk.connected()) 
    {
    
     if((c==0)&&(kalibracija1==LOW))//samo prvi prolazak kroz petlju(nakon resetiranja arduina)postavi baseline vrijednosti
     {
       sgp.setIAQBaseline(co2b,vocb);
       Serial.println("CO2 BAZA prava");
       Serial.println(eCO2_base);  
       Serial.println("CO2 BAZA primjena");
       Serial.println(co2b);
       Serial.println("Postavljene referentne vrijednosti");
       c=1;
     } 
  
     if(kalibracija==LOW)
     {   
      brojac_baseline++;
      if (brojac_baseline== 18000)//cca 55min 12000
          { 
            Serial.println("Dohvati nove referente vrijednosti(nakon sat vremena)");
            brojac_baseline= 0;
            if (!sgp.getIAQBaseline(&eCO2_base,&TVOC_base)) 
            {}
            else
            {
              Serial.println("Prave vrijednosti");
              Serial.println("CO2 BAZA");
              Serial.println(eCO2_base);
              Serial.println("VOC BAZA ");
              Serial.println(TVOC_base);
              sgp.IAQmeasure();    
             }
          } 
    }}}
    
     if((digitalRead(Grijanje1)==HIGH)&&(digitalRead(Grijanje2)==HIGH))
      stupanj_grijanja=0;
     if((digitalRead(Grijanje1)==HIGH)&&(digitalRead(Grijanje2)==LOW))
      stupanj_grijanja=1;
     if((digitalRead(Grijanje1)==LOW)&&(digitalRead(Grijanje2)==HIGH))
      stupanj_grijanja=2;
     if((digitalRead(Grijanje1)==LOW)&&(digitalRead(Grijanje2)==LOW))
       stupanj_grijanja=3;

  
     if ((temp1>15)&&(temp1<30)&&(temp1>dht.readTemperature() )&&(dht.readTemperature()>7 )&&(dht.readTemperature()<30))
       {
        if(stanje==HIGH)
          {//AKO JE NA AUTOMATIKU
           digitalWrite(Grijanje1,LOW);
          }
        }
     else
       {
        if((stanje==HIGH)&&(temp1+temp_offset)<(dht.readTemperature()))
         {
        digitalWrite(Grijanje1,HIGH);
         }
       }



      if ((temp2>15)&&(temp2<30)&&(temp2>dht.readTemperature() )&&(dht.readTemperature()>7 )&&(dht.readTemperature()<30))
      {
        if(stanje==HIGH)
        {//AKO JE NA AUTOMATIKU
          digitalWrite(Grijanje2,LOW);
        }
      }
      else
      {      
        if((stanje==HIGH)&&(temp2+temp_offset)<(dht.readTemperature()))
        {
        digitalWrite(Grijanje2,HIGH);
        }
      }

      if (stanje==LOW)
      { 
        digitalWrite(Grijanje1,a);
        digitalWrite(Grijanje2,a1);       
      }
      if(notifikacije==HIGH)
      {
        if ((co2<co2_visok))
           n1=0;
        if ((co2<co2_previsok))
           n2=0;
        if ((voc<voc_visok))
           n3=0;
        if ((voc<voc_previsok))
           n4=0;
           
           
        if ((co2>co2_visok)&&(n1==0))
           {
            n1=1;
            Blynk.notify("CO2 BOKUN VIŠJI");
           } 
        
        if ((co2>co2_previsok)&&(n2==0))
           {
            n2=1;
            Blynk.notify("CO2 VISOK, OTVOR PONISTRU!!");
           }
        if ((voc>voc_visok)&&(n3==0))
           {
            n3=1;
            Blynk.notify("KVALITETA ZRAKA SLABIJA");
           } 
        
        if ((voc>voc_previsok)&&(n4==0))
           {
            n4=1;
            Blynk.notify("KVALITETA ZRAKA SLABA, OTVOR PONISTRU!!");
           }   
        }  
      
      /*if((co2>800)&&(co2<1000))
       Blynk.notify("CO2 BOKUN VIŠJI");
      if(co2>1000)
       Blynk.notify("CO2 VISOK, OTVOR PONISTRU!!");
     
      if((voc>200)&&(voc<300))
       Blynk.notify("KVALITETA ZRAKA SLABIJA");
      if(voc>300)
       Blynk.notify("KVALITETA ZRAKA SLABA, OTVOR PONISTRU!!");*/
       
  
      Blynk.run();
      timer.run();
}
uint32_tgetAbsoluteHumidity(float temperature,floathumidity) 
{//dobivanje apsolune vlažnosti potrebne za SGP korekciju mjerenja 
// approximation formula fromSensirion SGP30 Driver Integrationchapter 3.15
constfloatabsoluteHumidity= 216.7f * ((humidity/ 100.0f) * 6.112f *exp((17.62f * temperature) / (243.12f + temperature)) / (273.15f + temperature)); // [g/m^3]
constuint32_tabsoluteHumidityScaled=static_cast<uint32_t>(1000.0f *absoluteHumidity); // [mg/m^3]
returnabsoluteHumidityScaled;
} 
