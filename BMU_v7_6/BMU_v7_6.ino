/*
created 10/3/2013
 modified 4/8/2014
 by Nima Ghods
 
 BMU code made to communicate with the BME's 
 Circuit: BMU Shield v3_4
          BME V5_5 attached to arduino due
 */

//included libraries:
  #include <SPI.h>           //use SPI to communicate to BMEs
  //#include <SD.h>
  #include <Ethernet.h>      //use Ethernet to communicate to BMC
  #include "BMU.h"           //all BMU variables
  
  
  void setup() {   
    pinInital();    // configure arduino due pins
    Serial.println("\n \t\t BMU 7.4\n");    // print the code version
    Serial.println("Enter Command (help for a list of commands)");
  }
  
  void loop() 
  {
    timeStamp=micros();
    loopCount=loopCount%countLimit+1;  // counts the number of loops up to countLimit
    
    if(loopCount%bmeSelfTestTime==0) BMESelfTest();    // run self check on all BMEs runs once every 5 min
    
    if(Rtest)dischargeTest();   // for thisting discharge
    
    if(balanceOn) balanceCal(); // if balancing is on then calculate which cells need to be balanced
    getBMEData();               // gets the data from all BMEs 
    calStateBME();              // calculates the state of BME's
 
    getBMUData();              //gets/ calculates data for the half string
    calStateBMU();          // calculates the state of half-string
    
    checkFlags();    //checks and sets flags
    
    if(loopCount%bmcComTime==0) BMCcomm();         //sends and receives information through ethernet to BMC runs every 1 sec

    priorityMode();          //sets contactors acourding to the mode and flags
    
//    if (BMCcommdt< micros()-timeStamp) BMCcommdt= micros()-timeStamp;
    timeCheck();    //sets the loop time
  }
  
