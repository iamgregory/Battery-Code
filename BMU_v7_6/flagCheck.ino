/*------------------------------------------------------------------------------
 * void checkFlags(void)
 * Checks and sets all BMU flags burritos
 *----------------------------------------------------------------------------*/
void checkFlags(void){
  leakFlag=false;
  if(fwLeak || bwLeak) leakFlag=true;  //hecks the leak sensors
  bmeCommCheck();   //checks the communication of all BME's
  bmeFlagCheck();   //checks the BME's self-checks flags
  volCheck();      //checks the voltage for over and under voltage
  tempCheck();     //checks the temperature for over temperature
  currentCheck();  //checks the current 
  pressurCheck();      //checks additional sensors
  timeoutCheck();
  setFlag();       // sets the flag to be sent out 
}

/*------------------------------------------------------------------------------
 * void timeoutCheck()
 * time out after 10 hours of chargeing or balancing
 *----------------------------------------------------------------------------*/
 void timeoutCheck(){
   unsigned long timeSince= timeElapsed(modeInfo.timeKeepingStamp);  // update time in the mode
   modeInfo.timeKeepingStamp=micros();
   modeInfo.microseconds+=timeSince;   
   if (modeInfo.microseconds>ONEMINUTE){
       modeInfo.microseconds-=ONEMINUTE;
       modeInfo.minutes++;
   }
   if (modeInfo.minutes>60){
       modeInfo.minutes-=60;
       modeInfo.hours++;
   }
   timeoutFlag=false;      // Check if charging or balance time > 10 hours
   if(modeInfo.currentMode==CHARGEMODE || modeInfo.currentMode==BALANCEMODE){
     if (modeInfo.hours>=10) timeoutFlag= true;  // set Timeout flag
   }
//   if(uartPrint)Serial.print("Mode(");
//   if(uartPrint)Serial.print(modeInfo.currentMode);
//   if(uartPrint)Serial.print("), Hours: ");
//   if(uartPrint)Serial.print(modeInfo.hours);
//   if(uartPrint)Serial.print(",Minutes: ");
//   if(uartPrint)Serial.print(modeInfo.minutes);
//   if(uartPrint)Serial.print(",uSecs: ");
//   if(uartPrint)Serial.println(modeInfo.microseconds);

 }

/*------------------------------------------------------------------------------
 * void currentCheck(void)
 * checks the current sensor reading
 *----------------------------------------------------------------------------*/
 void currentCheck(){
   driveCurflag=false;          //Current >20 durring Drive
   chargeCurFlag=false;      //Current > 92A or current <2A during Charge
   stopCurFlag=false;     //abs(Current)>1A

   if(driveOn && current >= inCurLimit) driveCurflag= true;
   if(chargeOn && current >= highInCur && current >= highOutCur) chargeCurFlag= true;
   if((stopOn || balanceOn) && current >= inOutCur) stopCurFlag= true;
 }

/*------------------------------------------------------------------------------
 * void pressurCheck(void)
 * checks the pressure sensor and the calculated pressure rate
 *----------------------------------------------------------------------------*/
 void pressurCheck(){
   presRateFlag= false;    //Pressure rate > .25 PSI/sec
   presFlag =false;      // Pressure < 0.5 PSI or Pressure  > 5.0 PSI
   if(abs(presRate)>=presRateHigh) presRateFlag= true;
   if((pressure>=presHighLimit) || (pressure<=presLowLimit)) presFlag= true;
 }

/*------------------------------------------------------------------------------
 * void bmeCommCheck(void)
 * checks the communication of all BME's
 *----------------------------------------------------------------------------*/
 void bmeCommCheck(void){
   bmeComFlag=false;  // Communication failure occurs between BMU and BME
   
   for(int j=0;j<BMENum;j++){                         // goes through all BMEs
     if(BME[j].dataCheck) bmeComFlag=true;          // set communication error flag
   }     
 }

/*------------------------------------------------------------------------------
 * void bmeFlagCheck(void)
 * checks the BME's self-checks flags
 *----------------------------------------------------------------------------*/
 void bmeFlagCheck(void){
   bmeAlarmFlag=false; // If any cell over/under voltage failures or self-check failures are sent from BME 
   bmeComFlag=false;  // Communication failure occurs between BMU and BME
   
   for(int j=0;j<BMENum;j++){                         // goes through all BMEs
     if(!BME[j].dataCheck){                           // check if BME is communicating
       for (int i=0;i<cellNum;i++){                   // goes through all virtual cells in a BME
         if(BME[j].uFlag[i] || BME[j].oFlag[i]) bmeAlarmFlag=true;
       }
       if(BME[j].muxCheck || BME[j].volSelfCheck || 
           BME[j].AuxSelfCheck || BME[j].StatSelfCheck) bmeAlarmFlag=true;
     }
     else bmeComFlag=true;          // set communication error flag
   }     
 }

/*------------------------------------------------------------------------------
 * void tempCheck(void)
 * temperature check on all cells, heat sinks, and chip temperatures for 
 * temperature warnings and temperature errors
 *----------------------------------------------------------------------------*/
void tempCheck(void){
 // set flags to false before starting the temperature check
 tempAlarmFlag=false;
 tempWarnFlag=false;
 tempFailFlag=false;
 
 //
 for(int j=0;j<BMENum;j++){                         // goes through all BMEs
   if(!BME[j].dataCheck){                           // check if BME is communicating
     for (int i=0;i<cellNum;i++){                   // goes through all virtual cells in a BME
       if(!BME[j].ignoreT[i]){
         if(BME[j].fTemp[i] <=-42 || BME[j].fTemp[i]== 'nan'){  //check and set temperature sensor failure flag
           if(flagIgnoreTemp) BME[j].ignoreT[i]=true;
           else tempFailFlag= true;
         }
         else if(BME[j].fTemp[i] >= tempVCWarn){           // check virtual cell temperature for temperature warning
           if(BME[j].fTemp[i] >= tempVCAlarm){        // check virtual cell temperature for temperature error
             if(flagIgnoreTemp) BME[j].ignoreT[i]=true;
             else tempAlarmFlag=true;                          // set temperature alarm flag
           }
           else {
             if(flagIgnoreTemp) BME[j].ignoreT[i]=true;
             else tempWarnFlag=true;                             // set temperature warnning flag
           }
         }
       }
     }
     if(!BME[j].ignoreT[3]){
       if( BME[j].fiTemp <=-42 || BME[j].fTemp[3]== 'nan' ){ //check and set temperature sensor failure flag
         if(flagIgnoreTemp) BME[j].ignoreT[3]=true;
         else tempFailFlag= true; 
       } 
       else if( BME[j].fTemp[3] >= tempHSWarn){        // check heat sink and chip temperature for temperature warning
         if(BME[j].fTemp[3] >= tempHSAlarm){    // check heat sink and chip temperature for temperature warning
             if(flagIgnoreTemp) BME[j].ignoreT[3]=true;
             else tempAlarmFlag=true;                          // set temperature alarm flag
         }
         else{
           if(flagIgnoreTemp) BME[j].ignoreT[3]=true;
           else tempWarnFlag=true;                             // set temperature warnning flag
         }
       }
     }
     if(!BME[j].ignoreiT){
       if(BME[j].fiTemp >= tempTiWarn){
         if(BME[j].fiTemp >= tempTiAlarm){
            if(flagIgnoreTemp) BME[j].ignoreiT=true;
             else tempAlarmFlag=true;                          // set temperature alarm flag
         }
         else{
           tempWarnFlag=true;                             // set temperature warnning flag
         }
       }
     }
   }
 } 
}

/*------------------------------------------------------------------------------
 * void volCheck(void)
 * Checks the Voltage for all the batteries and sets a flag if it is outside 
 * the required range also checks balance recommendation
 *----------------------------------------------------------------------------*/
void volCheck(void){
  // set flags to false before starting the voltage check
  volHighAlarmFlag =false;      //Any VC voltage > 4.205 V 
  balRecFlag=false;             //Any VC voltage < 3.9 V
  volLowWarnFlag =false;     //Any VC voltage < 3.2 V
  volLowBalAlarmFlag = false;
  volLowAlarmFlag =false;    //Any VC voltage < 3.0 V
  deadBatAlarmFlag=false;    //Any VC voltage < 2.5 V
  volFailFlag =false;      //Any VC voltage < .1 V or >6.5 or Vref2<2.978 or>3.020
  volMisFlag =false;      /*5V difference between overall half-string voltage and sum of half-string VC voltages or
                                     50mV difference between battery module voltage and sum of its VC voltages*/
  
  for(int j=0;j<BMENum;j++){                // goes through all BMEs
   if(!BME[j].dataCheck){                   // check if BME is communicating
     if ( (BME[j].fVref2 > 3.020) || (BME[j].fVref2 < 2.978)){
       volFailFlag = true;             // set voltage failure flag
     } 
     if(abs(BME[j].modSum-BME[j].fVSum)>=volModMismatch) volMisFlag =true; 
   }
  }
  if(abs(totalVoltage-volSum)>=volMismatch) volMisFlag =true;
  if(maxVol >= 6.5 ){     // check virtual cell voltage sensor for failure 
       volFailFlag = true;             // set voltage failure flag
  } 
  else if((maxVol >= volHighAlarm) | (chargeOn && maxVol>=(charge2Vol+0.01))){  // check virtual cell voltage for high voltage flag
    volHighAlarmFlag  = true;          // set high voltage error flag
    if(uartPrint) Serial.println(maxVol,4);
  }  
  
  if(minVol <= 0.0) volFailFlag = true;             // set voltage failure flag
  else if(minVol <= deadBatAlarm && !driveOn) deadBatAlarmFlag=true;  // set dead battery alarm
  else if(minVol <= volLowAlarm && !chargeOn) volLowAlarmFlag=true;    // low voltage alarm
  else if(minVol <= volLowWarn  && !chargeOn) volLowWarnFlag = true;    //  low voltage warning      
  else if(balance2Vol <= volLowBalAlarm  && BMCcommand.indexOf("bal") >=0) volLowBalAlarmFlag = true;    //  low voltage warning
   
  if((maxVol-minVol)>=balRecVol && !balanceOn && minVol>=balRecLimit ){
    balRecFlag=true;    // set balance recomanded flag
  }

        

}

/*------------------------------------------------------------------------------
 * void setFlag(void)
 * Sets the flag
 *----------------------------------------------------------------------------*/
 void setFlag(void){
   if(leakFlag) flagBMU=flagBMU | 1;    // fwleak and bwleak get set in getBMUdata
   if(tempWarnFlag) flagBMU=flagBMU | (1<<1);
   if(tempAlarmFlag) flagBMU=flagBMU | (1<<2);
   if(tempFailFlag) flagBMU=flagBMU | (1<<3);
   if(presRateFlag) flagBMU=flagBMU | (1<<4);
   if(presFlag) flagBMU=flagBMU | (1<<5); 
   if(volHighAlarmFlag) flagBMU=flagBMU | (1<<6);
   if(volLowBalAlarmFlag) flagBMU=flagBMU | (1<<7);
   if(volLowWarnFlag) flagBMU=flagBMU | (1<<8);
   if(volLowAlarmFlag) flagBMU=flagBMU | (1<<9);
   if(deadBatAlarmFlag) flagBMU=flagBMU | (1<<10);
   if(volFailFlag) flagBMU=flagBMU | (1<<11);
   if(volMisFlag) flagBMU=flagBMU | (1<<12);
//   if(bmeAlarmFlag) flagBMU=flagBMU | (1<<13);
//   if(bmeComFlag) flagBMU=flagBMU | (1<<14);
//   if(bmcComFlag) flagBMU=flagBMU | (1<<15);
   if(driveCurflag) flagBMU=flagBMU | (1<<16);
   if(chargeCurFlag) flagBMU=flagBMU | (1<<17);
   if(stopCurFlag) flagBMU=flagBMU | (1<<18);
   if(timeoutFlag) flagBMU=flagBMU | (1<<19);
   if(chargeDoneFlag) flagBMU=flagBMU | (1<<20);      
   if(balDoneFlag) flagBMU=flagBMU | (1<<21);
   if(balRecFlag) flagBMU=flagBMU | (1<<22); 
   
   flagBMU= flagBMU & ~flagOverride;
//   if(uartPrint){
//     Serial.print(minVol,4);
//     Serial.print('\t');
//     Serial.println(maxVol,4);
//   }
//   if(uartPrint && flagBMU !=0) Serial.println(flagBMU,HEX);
 }
 
 /*------------------------------------------------------------------------------
 * void setPriority(void)
 * Sets the flag
 *----------------------------------------------------------------------------*/
 void setPriority(void){
   
   flagPriority=0;
   
   if(driveOn){
     if((flagBMU & 0x400000) !=0) flagPriority=5;   //  B10000000000000000000000
     if((flagBMU & 0x00910A) !=0) flagPriority=4;  //  
     if((flagBMU & 0x016A75) !=0) flagPriority=2;    
   }
   if(stopOn){
     if((flagBMU & 0x400000) !=0) flagPriority=5;   //  B10000000000000000000000
     if((flagBMU & 0x00D92A) !=0) flagPriority=4;  //  
     if((flagBMU & 0x042655) !=0) flagPriority=3;  //  
   }  
   if(balanceOn){
     if((flagBMU & 0x200000) !=0) flagPriority=5;   //  B10100000000000000000000
     if((flagBMU & 0x00102A) !=0) flagPriority=2;  //  B00000000000000000101010
     if((flagBMU & 0x0CEF75) !=0) flagPriority=1;  //  B00010101111100011010101
   }
   if(chargeOn){
     if((flagBMU & 0x500000) !=0) flagPriority=5;   //  B01000000000000000000000
     if((flagBMU & 0x00002A) !=0) flagPriority=2;   // 
     if((flagBMU & 0x0AFC55) !=0) flagPriority=1;  //  
   }  
 }
 
 /*------------------------------------------------------------------------------
 * void clearFlags(void)
 * clears all flags
 *----------------------------------------------------------------------------*/
 void clearFlags(void){
   if(uartPrint)Serial.println("Clear");
   flagBMU=0;
   flagPriority=0;
   stopUntil=false; 
   fanOn= false;
   flagOverride=0;
   overrideCount=0;
   chargeDoneFlag=false;
   balDoneFlag=false;
   leakFlag=false;
   //debug stuff
   fakeVolFlag=false;
   fakePressFlag=false;
   fakeTempFlag=false;
   fakeTotVolFlag=false;
   fakeModVolFlag=false;
   fakeCurFlag=false;
   
   for(int j;j<BMENum;j++){                // goes through all BMEs
     BME[j].ignoreiT=false;
     for (int i=0;i<4;i++){
       BME[j].ignoreT[i]=false;
     }
   }
 }
