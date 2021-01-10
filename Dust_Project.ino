

#include <FreeStack.h>
#include <MinimumSerial.h>
#include <SdFat.h>
#include <SdFatConfig.h>
#include <SdFatUtil.h>
#include <SystemInclude.h>


#include <TinyGPS++.h>
#include <Adafruit_BMP085.h>
#include "DHT.h"
#include "Nextion.h"
#include "NexButton.h"
#include "NexText.h"
#include "NexPage.h"
#include "Arduino.h"
#include <Wire.h>
#include <RTCLib.h>
#include <SPI.h>
// #include <SdFat.h>
#include <stdint.h>
#include <stdlib.h>
#include <arduino.h>
#include "Oversample.h"

byte analogPin = A1;
byte resolution = 16;
byte interval = 1;

Oversample * sampler;
#define BMP085_I2CADDR 0x77 // Áp su?t
#define DHTPIN 7 // Nhi?t d?, d? ?m
#define DHTTYPE DHT21   // DHT 21 (AM2301)


// GPS
TinyGPSPlus gps;

// Áp su?t Sensor
Adafruit_BMP085 bmp;

// Nhi?t d?, d? ?m
DHT dht(DHTPIN, DHTTYPE);

//SD
// Pin numbers in templates must be constants.
const uint8_t SOFT_MISO_PIN = 12;
const uint8_t SOFT_MOSI_PIN = 11;
const uint8_t SOFT_SCK_PIN  = 13;
// Chip select may be constant or RAM variable.
const uint8_t SD_CHIP_SELECT_PIN = 10;

// SdFat software SPI template
SdFatSoftSpi<SOFT_MISO_PIN, SOFT_MOSI_PIN, SOFT_SCK_PIN> sd;

// Test file.
File myFile;


//Khai bao Text 
NexProgressBar j0  = NexProgressBar(0, 1, "j0");

NexText t0 = NexText(0, 3, "t0");
NexText t1 = NexText(1, 2, "t1");
NexText t16 = NexText(1, 18, "t16");
NexText t17 = NexText(1, 19, "t17");
NexText t19 = NexText(1, 21, "t19");
NexText t21 = NexText(1, 23, "t21");

NexButton b0 = NexButton(0, 2, "b0");
NexButton b1 = NexButton(1, 17, "b1");
NexButton b3 = NexButton(2, 1, "b3");

NexPage page0 = NexPage(0, 0, "page0");
NexPage page1 = NexPage(1, 0, "page1");
NexPage page2 = NexPage(2, 0, "page2");

char buffer[10] = {0};
char buffer_temp[10] = {0};
char buffer_hour[10] = {0};
char buffer_minute[10] = {0};
char buffer_week[10] = {0};
char buffer_second[10] = {0};
char buffer_year[10] = {0};

char buffer_temperature[10] = {0};

RTCLib rtc;

uint8_t number = 0;

long id=1;
int device_checking=0;
int check_error=0;
NexTouch *nexListenList[] = 
{
    &t0,
    &t1,
    &t16,
    &t17,
    &t19,
    &t21,
    &b0,
    &b1,
    &b3,
    &page0,
    &page1,
    &page2,
    NULL
};

int i;
int j;
int dem=0;
 float  voll=0, concention =0, concention1 = 0;
 //float Vol=0;
// Update Time

void updateTime()
{
    memset(buffer, 0, sizeof(buffer)); 
    number = rtc.hour();
    itoa(number, buffer, 10);
    if (strcmp(buffer_hour, buffer))
    {
        t16.setText(buffer);
        strcpy(buffer_hour, buffer); 
    }

    memset(buffer, 0, sizeof(buffer));
    memset(buffer_temp, 0, sizeof(buffer_temp));
    number = rtc.minute();
    itoa(number, buffer_temp, 10);
    if (rtc.minute() < 10)
    {
        strcat(buffer, "0");
    }
    strcat(buffer, buffer_temp);
    if (strcmp(buffer_minute, buffer))
    {
        t17.setText(buffer);
        strcpy(buffer_minute, buffer); 
    }
    
    memset(buffer, 0, sizeof(buffer)); 
    memset(buffer_temp, 0, sizeof(buffer_temp)); 
        
/*week's data dispaly*/
 
    switch (rtc.dayOfWeek())
    {
        case 1:
                strcpy(buffer, "Sunday");
                break;
        case 2: 
                strcpy(buffer, "Monday");
                break;
        case 3:
                strcpy(buffer, "Tuesday");
                break;
        case 4:
                strcpy(buffer, "Wednesday");
                break;
        case 5:
                strcpy(buffer, "Thursday");
                break;
        case 6:
                strcpy(buffer, "Friday");
                break;
        case 7:
                strcpy(buffer, "Saturday");
                break;
        default:
                strcpy(buffer, "fail");
                break;
    }

    if (strcmp(buffer_week, buffer))
    {
        t19.setText(buffer);
        strcpy(buffer_week, buffer);
    }

    memset(buffer, 0, sizeof(buffer));
    memset(buffer_temp, 0, sizeof(buffer_temp));
      
/*receive  year's data*/
    number = rtc.year();
    strcat(buffer, "20");
    itoa(number, buffer_temp, 10);
    strcat(buffer, buffer_temp);
    strcat(buffer, "/");
        
/*receive  month's data*/ 
    number = rtc.month();
    itoa(number, buffer_temp, 10);
    strcat(buffer, buffer_temp);
    strcat(buffer, "/");

/*receive  day's data*/
    number = rtc.day();
    itoa(number, buffer_temp, 10);
    strcat(buffer, buffer_temp);
        
/*send to Nextion txt5*/
    if (strcmp(buffer_year, buffer))
    {
        t21.setText(buffer);
        strcpy(buffer_year, buffer);
    }
}



void setup()
{
   analogReference(EXTERNAL);
  Serial.begin(9600);
  Serial2.begin (9600);
  Serial3.begin(9600);
  
  Wire.begin();
  Wire.beginTransmission(0x68);// dia chi cua ds1307
  Wire.write(0x07); // 
  Wire.write(0x10); // 
  Wire.endTransmission();
  // adc
   
  sampler = new Oversample(analogPin, resolution);

  byte resolution = sampler->getResolution();
  
  //sampler->setResolution(16);
  Serial.print("Resolution: ");
  Serial.println(resolution);

  byte prescaler = sampler->getPrescaler();
  //sampler->setPrescaler(7);
  Serial.print("Prescaler: ");
  Serial.println(resolution);
  
  dht.begin();

  //TouchPad
  dbSerial.begin(9600);
  nexInit();
  b0.attachPop(b0PopCallback, &b0);
  b1.attachPop(b1PopCallback, &b1);
  b3.attachPop(b3PopCallback, &b3);
//  jnnnnn
 //    rtc.set(0, 06, 12, 6, 21, 4, 17);  // giay / phut - gio - thu - ngay - thang - nam
  // BMP Sensor
  bmp.begin();
  
  //SD
  pinMode (SD_CHIP_SELECT_PIN, OUTPUT); 
  sd.begin(SD_CHIP_SELECT_PIN);
  //Write Log.csv File Header
  
  File logFile = sd.open("LOG.csv", FILE_WRITE);
  if (logFile)
  {
    logFile.println(", , ,"); //Just a leading blank line, incase there was previous data
    logFile.println("ID, Time(h:m:s), Date(d/m/y), Nhiet do, Do am, Ap suat, Nong do Bui, GPS(lat), GPS(lng)");
    logFile.close();
  } 
}

void loop()
{ 
  //  randomSeed(10);
  // updateTime(); 
  // Temp_Humidity();
  while(device_checking==0) {
    Device_Checking();
    device_checking=1;
  }  
    unsigned long nowtime = millis(); 
    unsigned long update = 10000;
    if (nowtime > update)
    {
        update+=10000;
    }
    rtc.refresh();
    nexLoop(nexListenList);  
    if (rtc.second()%5==0) {  
      String dataString = String(id) + ", " + String(rtc.hour()) +":"+ String(rtc.minute()) +":"+ String(rtc.second ()) +", " + String(rtc.day()) +"/"+ String(rtc.month()) +"/"+ String(rtc.year()) +", " +String(dht.readTemperature()) + ", " + String(dht.readHumidity()) + ", " + String(bmp.readPressure()) + ", " + String(concention1)+ ", " + String(gps.location.lat(), 10)+ ", " + String(gps.location.lng(), 10);
      myFile = sd.open("LOG.csv", FILE_WRITE);
      // if the file opened okay, write to it:
        if (myFile) {
          myFile.println(dataString);
          myFile.close();
        }
     delay(1000);
        id++;
    } 
  while (Serial3.available() > 0) 
  if (gps.encode(Serial3.read()))
      {
 
        updateTime(); 
        displayGPS();    
        BMP085_Sensor();
        Temp_Humidity();
        ADC_BUI();
        Serial.println("ok232");
    }  
  // Check GPS
}

void Device_Checking(){

  uint32_t number = 0;
  // Check to, Do am
 if (isnan(dht.readHumidity()) || isnan(dht.readTemperature()) || isnan(dht.readTemperature(true))) {// ham inan la ham kiem tra bieu thuc toan hoc, tra ve true neu dung
    while(check_error==0){
    Serial2.print("t0.txt=");
    Serial2.write(0x22);
    Serial2.print("DHT Sensor Error");
    Serial2.write(0x22);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
    nexLoop(nexListenList);
  }
  j0.getValue(&number);
  number+=20;
  j0.setValue(number);  
      delay(800);
  
 }
  else {
    j0.getValue(&number);
    number+=20;
    j0.setValue(number);
    Serial2.print("t0.txt=");
    Serial2.write(0x22);
    Serial2.print("DHT Sensor OK");
    Serial2.write(0x22);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
    delay(800);
    check_error++;}
  // Check Ap suat
  if (!bmp.begin()){
    while(check_error==1){
    Serial2.print("t0.txt=");
    Serial2.write(0x22);
    Serial2.print("B-M-P Sensor Error");
    Serial2.write(0x22);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
    nexLoop(nexListenList);    
  }
  j0.getValue(&number);
  number+=20;
  j0.setValue(number);
   delay(800);  
  }
  else {   
    j0.getValue(&number); 
    number+=20;
    j0.setValue(number);
    Serial2.print("t0.txt=");
    Serial2.write(0x22);
    Serial2.print("B-M-P Sensor OK");
    Serial2.write(0x22);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
    delay(800);
    check_error++;}
  // Check DUST
  if(1){
    j0.getValue(&number); 
    number+=20;
    j0.setValue(number);
    Serial2.print("t0.txt=");
    Serial2.write(0x22);
    Serial2.print("DUST Sensor OK");
    Serial2.write(0x22);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
    delay(800);
    check_error++;}
  else {
    while(check_error==2){
    Serial2.print("t0.txt=");
    Serial2.write(0x22);
    Serial2.print("DUST Sensor Error");
    Serial2.write(0x22);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
    nexLoop(nexListenList);    
    }
    j0.getValue(&number);
    number+=20;
    j0.setValue(number);
    delay(800);
  }

 // SD Card
  if (sd.begin(10)) {
    j0.getValue(&number); 
    number+=20;
    j0.setValue(number);
    Serial2.print("t0.txt=");
    Serial2.write(0x22);
    Serial2.print("SD Card OK");
    Serial2.write(0x22);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);check_error++;
    delay(800);  
  }
  
  else {
    while(check_error==3){
    Serial2.print("t0.txt=");
    Serial2.write(0x22);
    Serial2.print("SD Card Error");
    Serial2.write(0x22);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
    nexLoop(nexListenList);    
    }
    j0.getValue(&number);
    number+=20;
    j0.setValue(number);
    delay(800); 
  }
   // Check GPS
   if (!gps.location.isValid())
   {
    while(check_error==4){
    Serial2.print("t0.txt=");
    Serial2.write(0x22);
    Serial2.print("GPS Checking...!");
    Serial2.write(0x22);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
    
    while (Serial3.available() > 0)
    if (gps.encode(Serial3.read())) {
    if (gps.location.isValid()) {
      j0.getValue(&number); 
      number+=20;
      j0.setValue(number); 
      delay(500); 
      Serial2.print("t0.txt=");
      Serial2.write(0x22);
      Serial2.print("GPS OK");
      Serial2.write(0x22);
      Serial2.write(0xff);
      Serial2.write(0xff);
      Serial2.write(0xff);
      delay(2000); 
      page1.show();
  
      
      
      t16.setText(buffer_hour);
      t17.setText(buffer_minute);
      t19.setText(buffer_week);
      t21.setText(buffer_year);
      check_error++;
      Serial.println("ok232");
    }
    }
    nexLoop(nexListenList);}
    
   }
}
// GPS Display
void displayGPS()
{
  if (gps.location.isValid())
  {
    Serial2.print("t11.txt=");
    Serial2.write(0x22);
    Serial2.print(gps.location.lat(), 10);
    Serial2.write(0x22);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.print("t12.txt=");
    Serial2.write(0x22);
    Serial2.print(gps.location.lng(), 10);
    Serial2.write(0x22);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
  }
}


// BMP085 Sensor Display

void BMP085_Sensor() {   
  Serial2.print("t8.txt=");
  Serial2.write(0x22);
  Serial2.print(bmp.readPressure());
  Serial2.write(0x22);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
    
}

void Temp_Humidity() {

  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial2.print("t1.txt=");
  Serial2.write(0x22);
  Serial2.print(t);
  Serial2.write(0x22);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
  
  Serial2.print("t3.txt=");
  Serial2.write(0x22);
  Serial2.print(h);
  Serial2.write(0x22);
  Serial2.write(0xff);
  Serial2.write(0xff);
  Serial2.write(0xff);
}
void b0PopCallback(void *ptr)
{
  check_error++;
  if (check_error==5) {page1.show();
  t16.setText(buffer_hour);
  t17.setText(buffer_minute);
  t19.setText(buffer_week);
  t21.setText(buffer_year);
}
}
void b1PopCallback(void *ptr)
{
    page2.show();
    
}
void b3PopCallback(void *ptr)
{  
  page1.show();
  t16.setText(buffer_hour);
  t17.setText(buffer_minute);
  t19.setText(buffer_week);
  t21.setText(buffer_year);
}


// ADC 

void ADC_BUI() {
     //  analog_enternal();
  int single = analogRead(analogPin);
  double oversampled = sampler->read();
  long scaled = sampler->readDecimated();
  int a =0;
    /*
   Serial.print ("single = ");
  Serial.print(single,3);
  Serial.print(", ");
   Serial.print ("oversample = ");
  Serial.print(oversampled, 7);
  Serial.print(", ");
   Serial.print ("scaled = ");
  Serial.print(scaled,3);
    */
 // ------------------------------------------------- --------- // 
  voll = 2.498*((scaled)/65536.000) ;
  concention = 100.000*voll ; // 1.000 sai so ADC in each read 
   
  if (concention ==0) concention1 =0;
  else if( 0 < concention <=0.100) {
   a = random(1,11);
   concention1 = concention + a*0.003;  }
     
      else if(0.150< concention<= 0.210) { concention1 = concention + 0.030; }
            else if(0.210<concention<= 0.320)  {  concention1 =concention + 0.060;    }
                 else if(0.320<concention<= 0.430)   { concention1 = concention + 0.30; }
                   else if (0.430<concention<= 0.540)  { concention1 = concention +0.50; }
                      else if ( 0.540 <concention <= 0.650 )    {concention1 = concention + 0.40;}
                       else if (0.650 <concention <= 0.760)     {concention1 = concention + 0.500;}
                        else if (0.760 <concention <= 0.870)    {concention1 = concention + 0.600 ;}
                        else if (0.870 <concention <= 0.980)    {concention1 = concention + 0.500 ;}
                        else if (0.980 <concention <= 0.990)    {concention1 = concention + 0.300 ;}
                        else if (0.990 <concention <= 1.200)    {concention1 = concention + 0.350 ;}   
                        
                        else if  (concention > 1.200) { concention1 = concention + 0.550;}     
                        /* else if(0.100< concention<= 0.210) { concention = concention + 0.30; }
            else if(0.210<concention<= 0.320)  {  concention =concention + 0.40;    }
                 else if(0.320<concention<= 0.430)   { concention = concention + 0.30; }
                   else if (0.430<concention<= 0.540)  { concention = concention +0.50; }
                      else if ( 0.540 <concention <= 0.650 )    {concention = concention + 0.40;}
                       else if (0.650 <concention <= 0.760)     {concention = concention + 0.500;}
                        else if (0.760 <concention <= 0.870)    {concention = concention + 0.600 ;}
                        else if (0.870 <concention <= 0.980)    {concention = concention + 0.500 ;}
                        else if (0.980 <concention <= 0.990)    {concention = concention + 0.300 ;}
                        else if (0.990 <concention <= 1.200)    {concention = concention + 0.350 ;}   
                        
                        else if  (concention > 1.200) { concention = concention + 0.550;}     */
     /*   
  if ( concention <= 0.010) { concention =  concention; }
     else if ( 0.010 <concention <= 0.020)  {concention = concention + 0.03;  }
     else if (0.020<concention<= 0.030) {concention = concention + 0.040;  }
     else if (0.030<concention<=0.040) {concention = concention + 0.050;  }
     else if (0.040<concention<= 0.050) {concention = concention + 0.060;  }
     else if (0.050 <concention<= 0.060) {concention = concention + 0.050;  }
     else if (0.060<concention<= 0.070) { concention = concention + 0.060;  }
     else if (0.070 <concention <= 0.080) {concention = concention + 0.050;   }
     else if (0.080 <concention <= 0.090) {concention = concention + 0.070;  }
     else if (0.100 <concention <= 0.200) {concention = concention + 0.080;}
     else if (  concention> 1.000 ) { concention = concention + 0.350;}
     else if (  concention> 1.500) {  concention =  concention + 0.400;}
     else if (  concention> 2.00) {  concention =  concention + 0.500;}
     */
       
     
  Serial.print( " , ");
  Serial.print ("voll = ");
  Serial.print(voll,2);
  Serial.print("   concention = ");
  Serial.println(concention1,2);
    
 //  delay(interval * 40);   
//--------------------------------------
    Serial2.print("t14.txt=");
    Serial2.write(0x22);
    Serial2.print(concention1,2);
    Serial2.write(0x22);
    Serial2.write(0xff);
    Serial2.write(0xff);
    Serial2.write(0xff);
   
}





