/*
created 10/3/2013
 modified 4/8/20143333333333333
 by Nima Ghods
 
 BMU code made to communicate with the BME's 
 Circuit: BMU Shield v3_4
          BME V5_5 attached to arduino due
 */

//included libraries:
  #include <SPI.h>           //use Serial Peripheral Interface (SPI) to communicate to BMEs, included in arduino environment
  //#include <SD.h>
  #include <Ethernet.h>      //use Ethernet to communicate to BMC, included in arduino environment
  #include "BMU.h"           //all BMU variables
  
  
  void setup() {   
    pinInital();    // configure arduino due pins
    Serial.println("\n \t\t BMU 7.4\n");    // print the code version
    Serial.println("Enter Command (help for a list of commands)");
  }
  
  void loop() 
  {
    timeStamp=micros();                // microseconds since board initialized, overflow/rollover after ~11.9 hours (2^32-1 uS)
                                       // returned in 1 microsecond resolution
    loopCount=loopCount%countLimit+1;  // counts the number of loops up to countLimit
    
    if(loopCount%bmeSelfTestTime==0)  BMESelfTest();    // run self check on all BMEs runs once every 5 min
    
    if(Rtest)  dischargeTest();   // for testing discharge
    
    if(balanceOn) balanceCal(); // if balancing mode is on, then calculate which cells need to be balanced
    
    getBMEData();               // gets data from all BMEs 
    calStateBME();              // calculates state of BME's
 
    getBMUData();               //gets data for the half-string
    calStateBMU();              // calculates the state of the half-string
    
    checkFlags();               //checks and sets flags
    
    if(loopCount%bmcComTime==0)  BMCcomm();         //send and receive information through ethernet to BMC  every 1 sec

    priorityMode();             //sets contactors according to the mode and flags
    
//    if (BMCcommdt< micros()-timeStamp) BMCcommdt= micros()-timeStamp;
    timeCheck();                //tries to keep loop time roughly constant
  }
  
