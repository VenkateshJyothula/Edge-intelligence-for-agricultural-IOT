#include "SoftwareSerial.h"

#include <EEPROM.h>

#define Enable_Pin 7

SoftwareSerial NPK_Mod(4,5);
SoftwareSerial Humidity_Mod(2,3); 

const byte Humidity_Enquiry[] = {0x01,0x03, 0x00, 0x02, 0x00, 0x01, 0x25, 0xCA};
const byte Nitrogen_frame[]={0x01,0x03,0x00,0x1E,0x00,0x01,0xE4,0x0C};
const byte Phosphorous_frame[]={0x01,0x03,0x00,0x1F,0x00,0x01,0xB5,0xCC};
const byte Potassium_frame[]={0x01,0x03,0x00,0x20,0x00,0x01,0x85,0xC0};
byte Answer_frame[11]={0x00}; 

float get_data_from_humidity_sensor(){
  //  Writing Enquiry Frame to  RS485 Module
  // digitalWrite(DE,HIGH);digitalWrite(RE,HIGH);   
  digitalWrite(Enable_Pin,HIGH);         
  for(byte i=0;i<8;i++){Humidity_Mod.write(Humidity_Enquiry[i]);} 
  //  Retrieving Answer Frame from RS485 Module 
  // digitalWrite(DE,LOW);digitalWrite(RE,LOW);
  digitalWrite(Enable_Pin,LOW); 
  for(byte i=0;i<7;i++){Answer_frame[i] = Humidity_Mod.read();}

  float ans=Answer_frame[3]*1.6+Answer_frame[4]*0.1;
  return ans;
}

float get_Nitrogen()
{
  //  Writing Enquiry Frame to  RS485 Module
  digitalWrite(Enable_Pin,HIGH);            
  for(byte i=0;i<8;i++){NPK_Mod.write(Nitrogen_frame[i]);} 
  //  Retrieving Answer Frame from RS485 Module 
  digitalWrite(Enable_Pin,LOW);
  for(byte i=0;i<7;i++){Answer_frame[i] = NPK_Mod.read();} 

  float ans=Answer_frame[3]*16+Answer_frame[4];
  return ans;
}

float get_Phosphorous()
{
  //  Writing Enquiry Frame to  RS485 Module
  digitalWrite(Enable_Pin,HIGH);            
  for(byte i=0;i<8;i++){NPK_Mod.write(Phosphorous_frame[i]);} 
  //  Retrieving Answer Frame from RS485 Module 
  digitalWrite(Enable_Pin,LOW);
  for(byte i=0;i<7;i++){Answer_frame[i] = NPK_Mod.read();}

  float ans=Answer_frame[3]*16+Answer_frame[4];
  return ans;
}

float get_Potassium()
{
  //  Writing Enquiry Frame to  RS485 Module
  digitalWrite(Enable_Pin,HIGH);            
  for(byte i=0;i<8;i++){NPK_Mod.write(Potassium_frame[i]);} 
  //  Retrieving Answer Frame from RS485 Module 
  digitalWrite(Enable_Pin,LOW);
  for(byte i=0;i<7;i++){Answer_frame[i] = NPK_Mod.read();}

  float ans=Answer_frame[3]*16+Answer_frame[4];
  return ans;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(Enable_Pin,OUTPUT);
  EEPROM.update(0, 1);   
}

void loop() {
  // put your main code here, to run repeatedly:
  
  float Nitrogen_Value=0.0,Phosphorous_Value=0.0,Potassium_Value=0.0;
  
  Humidity_Mod.begin(9600);  
  delay(250);
  float ans=get_data_from_humidity_sensor();
  delay(250);
  ans=get_data_from_humidity_sensor();
  delay(250);


  
  NPK_Mod.begin(9600);
  Nitrogen_Value=get_Nitrogen();
  delay(50);
  Nitrogen_Value=get_Nitrogen();
  //Serial.print("Nitrogen Answer Frame:-   ");for(int i=0;i<7;i++){ Serial.print(Answer_frame[i]);Serial.print(" ");}Serial.println(); 
  delay(50);Phosphorous_Value=get_Phosphorous();
  //Serial.print("Phosphorous Answer Frame:-"); for(int i=0;i<7;i++){Serial.print(Answer_frame[i]);Serial.print(" ");}Serial.println();
  delay(50);Potassium_Value=get_Potassium();  
  //Serial.print("Potassium Answer Frame:-  ");for(int i=0;i<7;i++){Serial.print(Answer_frame[i]);Serial.print(" ");}Serial.println();


  Serial.print("The value of moisture is given by: ");
  Serial.println(ans);  
  Serial.print("The value of Nitrogen is: ");
  Serial.print(Nitrogen_Value,3);
  Serial.println(" mg/kg");
  Serial.print("The value of Phosphorous is: ");
  Serial.print(Phosphorous_Value,3);
  Serial.println(" mg/kg");
  Serial.print("The value of Potassium is: ");
  Serial.print(Potassium_Value,3);
  Serial.println(" mg/kg\n");
  delay(250);
  if(Serial.available() > 0) {
    switch (Serial.read()) {
      case 'W': // W = memory write
        mem_write();
        break;
      case 'R': // R = memory read
        mem_read();
        break;
      case 'C': // D = memory reset
        mem_reset();
        break;
    }
  }
  delay(250);
}



byte mem_write() {

  byte addr = EEPROM.read(0);

  if(addr == 401) { // allocation 400 numbers of memory locations
    EEPROM.update(0, 1);
    addr = EEPROM.read(0);
  }

  byte mos, nitro, phos, pota;

  Humidity_Mod.begin(9600);  
  mos = get_data_from_humidity_sensor();
  delay(250);
  mos = get_data_from_humidity_sensor(); // variable for moisture
  EEPROM.update(addr, ((float(mos)/256)*100)); // memory write for moisture value
  addr = addr + 1;
  delay(250);
  Humidity_Mod.flush();

  NPK_Mod.begin(9600);

  nitro = get_Nitrogen();
  delay(250);
  nitro = get_Nitrogen(); // variable for nitrogen
  EEPROM.update(addr, nitro); //memory write for nitrogen value
  addr = addr + 1;
  delay(250);
  NPK_Mod.flush();

  phos = get_Phosphorous(); // variable for phosphorous
  EEPROM.update(addr, phos); //memory write for phosphorus value
  addr = addr + 1;
  delay(250);
  NPK_Mod.flush();

  pota = get_Potassium(); // variable for potassium
  EEPROM.update(addr, pota); // memory write for potassium value
  addr = addr + 1;
  delay(250);
  NPK_Mod.flush();

  EEPROM.update(0, addr); // updating the last memory location written

  Serial.println("Data saved in the memory!");

  delay(3000);
}

// Function for EEPROM memory reading

byte mem_read() {
  
  int addr = 1;

  Serial.print("\n");
  
  for (addr = 1; addr <= 400; addr++) {

    if (EEPROM.read(addr) == 0) {
      if (addr == 1) {
        Serial.println("No data found!");
      }
      break;
    }
    
    Serial.print("Data Set Number: ");
    Serial.println((addr / 4) + 1);

    Serial.print("Moisture: ");
    Serial.print(EEPROM.read(addr));
    Serial.println(" %");  
    addr = addr + 1;

    Serial.print("Nitrogen: ");
    Serial.print(EEPROM.read(addr));
    Serial.println(" mg/kg");
    addr = addr + 1;

    Serial.print("Phosphorus: ");
    Serial.print(EEPROM.read(addr));
    Serial.println(" mg/kg");
    addr = addr + 1;

    Serial.print("Potassium: ");
    Serial.print(EEPROM.read(addr));
    Serial.println(" mg/kg");

    Serial.print("\n");
 }

 if (addr == 1) {
   delay(500);
 }
 else {
   delay(10000); //the programme will wait for 20 seconds to capture the data
 }  
}

// Function for EEPROM memory deletion

byte mem_reset() {
  EEPROM.update(0, 1); // address Zero is to remember the last reading location
  // four simultaneous memory locations for four 
  // different parameters, total 100 sets of data can be saved
  for (int i = 1; i <= 400; i++) { 
    EEPROM.update(i, 0);
  }
  Serial.println("Memory loation, data reset successfully!");
  delay(2000);
}
