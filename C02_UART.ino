//running Arduino 1.0 IDE
//MH-Z14 CO2 Module from Zhengzhou Winsen Electronics Technology Co., Ltd
//Seeeduino Stalker v2.3
//http://www.adafruit.com/products/878

//Michael Doherty - Bitponics
//Crys Moore
//2.7.12
//update 2.21.12

//simple sketch to get sensor readings via UART and print them to Adafruit 7 segment backpack.
//Sensor readings are time stamped and logged to the SD card. 


#include <SoftwareSerial.h>
#include <Wire.h>
#include "RTClib.h"
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include <SD.h>


//using softserial to send/recieve data over analog pins 0 & 1
SoftwareSerial mySerial(A0, A1); // RX, TX respectively
Adafruit_7segment matrix = Adafruit_7segment();
RTC_DS1307 RTC;
File dataFile = SD.open("datalog1.csv", FILE_WRITE);



byte cmd[9] = {
  0xFF,0x01,0x86,0x00,0x00,0x00,0x00,0x00,0x79}; 
char response[9]; 
const int chipSelect = 10;
unsigned long time;
int calibrationTime = 180; //warm up time for C02 sensor (3min = 180sec)



void setup() 
{
  Serial.begin(9600);
  mySerial.begin(9600);
  matrix.begin(0x70);
  Wire.begin();
  RTC.begin();
  matrix.setBrightness(10); //0-15
  
    //give the sensor some time to calibrate
  Serial.print("calibrating sensor (3min) ");
  for(int i = 0; i < calibrationTime; i++){
    Serial.print(".");
    delay(1000);
  }
  Serial.println(" done");
  Serial.println("SENSOR ACTIVE");
  delay(50);

  if (! RTC.isrunning())
  {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  // Serial.print("Initializing SD card...");
  pinMode(10, OUTPUT);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    return;
  }
  //Serial.println("card initialized.");
  //printHeader(); 
  //readSd(); //fuction to read the contents of the SD
  

  
  



}


void loop() 
{
  DateTime now = RTC.now();

  //PING CO2, READ RESPONSE, GET READINGS AND CONVERT TO PPM
  mySerial.write(cmd,9);
  mySerial.readBytes(response, 9);
  int responseHigh = (int) response[2];
  int responseLow = (int) response[3];
  int ppm = (256*responseHigh)+responseLow;

  //PRINT c02 OUT ON THE SEVSEG BACKPACK & SERIAL
  //Serial.println(ppm,DEC);
  //matrix.print(ppm,DEC);


  // create a string for ppm, cast the data, open up the file
  String ppmString = "";
  ppmString = String(ppm);


  time = millis(); //1s = 1000


      dataFile = SD.open("datalog1.csv", FILE_WRITE);
      if (dataFile) 
        {
          dataFile.print(ppmString);
          dataFile.print(',');
          dataFile.print(now.year(), DEC);
          dataFile.print(',');
          dataFile.print(now.month(), DEC);
          dataFile.print(',');
          dataFile.print(now.day(), DEC);
          dataFile.print(' ');
          dataFile.print(now.hour(), DEC);
          dataFile.print(',');
          dataFile.print(now.minute(), DEC);
          dataFile.print(',');
          dataFile.print(now.second(), DEC);
          dataFile.println();
          dataFile.close();
          Serial.println(ppmString);
        }  
      else 
      {
        Serial.println("error opening datalog1.csv");
       }  
}


void printHeader()
{
  ///open up the .csv, write a header, close file
  dataFile = SD.open("datalog1.csv", FILE_WRITE);
  if (dataFile) {
    {
      dataFile.print("ppm"); 
      dataFile.print(",");
      dataFile.print("year");
      dataFile.print(","); 
      dataFile.print("month");
      dataFile.print(",");
      dataFile.print("day");
      dataFile.print(",");
      dataFile.print("minute");
      dataFile.print(",");
      dataFile.print("second");
      dataFile.print(",");
      dataFile.println();
      dataFile.close();
      Serial.println("printing header for real...");
    }
  } 
}
///function to read back the contents of the file via Serial
void readSd()
{

  File dataFile = SD.open("datalog1.csv");

  // if the file is available, write to it:
  if (dataFile) {
    while (dataFile.available()) {
      Serial.write(dataFile.read());
    }
    dataFile.close();
  }  
  // if the file isn't open, pop up an error:
  else {
    Serial.println("error opening datalog1.csv");
  }
}














