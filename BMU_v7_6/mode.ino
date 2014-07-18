/*------------------------------------------------------------------------------
 * void checkMode(void)
 * does the command that is sent from the GUI
 *----------------------------------------------------------------------------*/
void checkMode(String input){
  
  if(input.indexOf("sto") >=0)//checks for stop
  {
    stopMode();
  }
  
  else if(input.indexOf("dri") >=0)//checks for drive
  {
    driveMode();
  }
  
  else if(input.indexOf("ove") >=0)//checks for override
  {
    int eVal,sVal,temp,flagNum;
    String overrideString="";
    sVal=input.indexOf("_");  // first underscore
    temp=input.indexOf("_",sVal+1); //location of next possible underscore
    eVal=input.indexOf('\n');  // end location
    while(temp>0){  // while there are more underscores  
      overrideString=input.substring(sVal+1,temp);
      flagNum=overrideString.toInt();
      if(flagNum>0 && flagNum<23){  // if the flagNum is valid
        flagOverride=flagOverride | (1<<(flagNum-1)); //set override flag
        if(uartPrint)Serial.print("Override: ");
        if(uartPrint)Serial.println(flagOverride,HEX);
        overrideCount=0;
      }
      sVal=temp;  // update indices for underscores
      temp=input.indexOf("_",sVal+1);
    }
    overrideString=input.substring(sVal+1,eVal); // grab string from last underscore to end location
      flagNum=overrideString.toInt();
      if(flagNum>0 && flagNum<23){
        flagOverride=flagOverride | (1<<(flagNum-1));
        if(uartPrint)Serial.print("Override: ");
        if(uartPrint)Serial.println(flagOverride,HEX);
        overrideCount=0;
      }
  }
  
  else if(input.indexOf("ign") >=0)//checks for ignore
  { 
    flagBMU=~(~flagBMU | (0xE));
    flagOverride=~(~flagOverride | (0xE));
    flagIgnoreTemp=true;
  }
  
  
  else if(input.indexOf("cha") >=0)//checks for Charge
  {
    int sVal=input.indexOf("_");
    int eVal=input.indexOf('\n',sVal+1);
    String cha2string=input.substring(sVal+1,eVal);
    int cha2int=cha2string.toInt();
    charge2Vol = cha2int*0.0001;
    chargeMode();
  }
  
  else if(input.indexOf("bal") >=0)//checks for Balance
  { 
    if(!stopUntil )
    {  contactorsOn=false;
      if (!balanceOn){ 
        int sVal=input.indexOf("_");
        int eVal=input.indexOf("_",sVal+1);
        String bal2string=input.substring(sVal+1,eVal);
        int bal2int=bal2string.toInt();
        balance2Vol = bal2int*0.0001;        
        if(balance2Vol>volLowBalAlarm){
          balanceMode();
        }
        else{
          if(uartPrint)Serial.println("bad bal2vol!");
        }
      }
      else if(!balRelaxFlag){   // check to see if the system has relaxed
        if( areWeThereYet(balanceTimeStamp,balanceRelaxTime) ){ 
          balRelaxFlag=true;   
          if(uartPrint) Serial.println("Relaxed and Ready to Balance");
        }
      }
      else if ( areWeThereYet(balanceTimeStamp,balanceCheckTime) ){ 
        disableResistors();
        if ( realBalDataFlag ){  // if a loop has happened since the resistors were disabled
          if(uartPrint) Serial.println("balanceCal()");
          balanceCal(); // if balancing mode is on, then calculate which cells need to be discarged
        }
      }
    balTempControl();
   }
  }
  
  else if(input.indexOf("deb") >=0)//checks for debug test commands
  { // debug commands look like deb_<debugmode>_<additional commands>
    int sVal =input.indexOf("_"); // grab first underscore location
    int eVal=input.indexOf("_",sVal+1); // grab second underscore location
    String debugModeString=input.substring(sVal+1,eVal); //grab the debug mode as a string
    int debugMode=debugModeString.toInt();  // store debug mode as an int
    String string1="";
    String string2="";
    int integer1 =0;
    int integer2 =0;
    
    switch (debugMode) {
    case 1:  // discharge resistor test
      if (sVal>0){ 
      sVal=input.indexOf("_",eVal+1); //find the second underscore
      string1=input.substring(eVal+1,sVal); //grab the requested module
      integer1=string1.toInt();  // store requested module as an integer
      eVal=input.indexOf('\n',sVal+1); //find the end of the command
      string2=input.substring(sVal+1,eVal); // grab the requested layer
      integer2=string2.toInt();  // store requested module as an integer
      dischargeResistorTest(integer1,integer2);
      break;
    case 2: //do something when var equals 2
       //callSomeDebugCommand();
     // break;
    default: 
      // if nothing else matches, do the default
      // default is optional
      break;
    }
  }
  } 
  priorityMode();                   //sets contactors according to the mode and flags
}


/*------------------------------------------------------------------------------
 * void priorityMode(void)
 * looks the bmu mode and priority then acts accordingly
 *----------------------------------------------------------------------------*/
 
 void priorityMode(void){
   setPriority();
   switch(flagPriority){
     case 1:
       stopMode();
       stopUntil=true;
       Serial.println("Should Stop!");
       break;
     case 2:
       overrideCount++;
       if(overrideCount >=overrideTLimit){
         stopMode();
         stopUntil=true;
       }
       break;
     case 3:
     case 4:
     case 5:
       break;
     default:
       break;
   } 
   
   // delete for final version ===> 
   if(minVol<2.760){
     stopMode();
     stopUntil=true;
     if(uartPrint) Serial.println("voltage too low for testing ==> Stop!");
   } 
   //<===
 }
 
 /*------------------------------------------------------------------------------
 * void stopMode(void)
 * puts system into Stop mode
 *----------------------------------------------------------------------------*/
 void stopMode(void){
    contactorsOn=false;
    if(!stopOn){
      if(uartPrint) Serial.print(BMCcommand);
      if(uartPrint) Serial.println("Stop Mode Entered.");
      stopBal();
      chargeOn=false;
      driveOn=false;
      stopOn=true;
      conOnTime=0;
      modeInfo.currentMode=STOPMODE;
      modeTimeReset();
      BMESelfTest();
    }
 }
 
 /*------------------------------------------------------------------------------
 * void driveMode(void)
 * puts system into Drive mode
 *----------------------------------------------------------------------------*/
 void driveMode(void){
   if(!stopUntil){
    if(!driveOn) {
      if(uartPrint) Serial.println("Drive Mode Entered.");
      stopBal();
      stopOn=false;
      chargeOn=false;
      driveOn=true;
      overrideCount=0;
      conOnTime=0;
      modeInfo.currentMode=DRIVEMODE;
      modeTimeReset();
      BMESelfTest();
    }
    contactorsOn=true;
   }
 }
 
 /*------------------------------------------------------------------------------
 * void chargeMode(void)
 * puts system into Charge mode
 *----------------------------------------------------------------------------*/
 void chargeMode(void){
   if(!stopUntil){
    if(!chargeOn){
      if(uartPrint) Serial.print("Charge to: ");
      if(uartPrint) Serial.println(charge2Vol,4);
      stopBal();
      driveOn=false;
      stopOn=false;
      chargeOn=true;
      if(uartPrint) Serial.println("Charge Mode Entered.");
      modeInfo.currentMode=CHARGEMODE;
      modeTimeReset();
      overrideCount=0;
      conOnTime=0;
      chargeDoneFlag=false;
      BMESelfTest();
    }
    contactorsOn=true;
    if( charge2Vol <= maxVol && current<=doneCur){
      chargeDoneFlag=true;
    }
   }
 }
 


 /*------------------------------------------------------------------------------
 * void balanceMode(void)
 * puts system into Balance mode
 *----------------------------------------------------------------------------*/
 void balanceMode(void){
    if(uartPrint){
      Serial.print("Balance to: ");
      Serial.println(balance2Vol,4);
    }
//    for(int j=0;j<BMENum;j++){
//     if(!BME[j].dataCheck){
//       for(int i=0;i<cellNum;i++){
//         if(BME[j].fVol[i]>balance2Vol){
//           BME[j].balFlag[i]=1;
//         }
//         else {
//           BME[j].balFlag[i]=0;
//         }
//       }
//     }
//    }
    BMESelfTest();
    driveOn=false;
    stopOn=false;
    modeInfo.currentMode=BALANCEMODE;
    modeTimeReset();
    chargeOn=false;
    balanceOn=true;
    overrideCount=0;
    balanceTimeStamp=micros(); //make sure balanceCal executes the first time through
    balDoneFlag=false;
    realBalDataFlag=false;
    balRelaxFlag=false;
    conOnTime=0;
 }
 
 /*------------------------------------------------------------------------------
 * void stopBal(void)
 * stops all balancing by disconnecting all resistors
 *----------------------------------------------------------------------------*/
 void stopBal(void){
   int j,i;
   disableResistors();
   for(j=0;j<BMENum;j++){    
     for(i=0;i<cellNum;i++){
       BME[j].balFlag[i]=false;  // clear all the balancing flags
     }
   }
   balanceOn=false;
 }
 
  /*------------------------------------------------------------------------------
 * void disableResistors(void)
 * set the DCC registries to 0 and communicate that to all BMEs
 --------------------------------------------------------------------------*/
 void disableResistors(void){
   int j;
   for(j=0;j<BMENum;j++){   
     BME[j].DCC=0;               // set the 3 bit DCC registry to "000" 
     WRCFG((BMEdata&) BME[j]);  
   }
 }
 
 
 /*------------------------------------------------------------------------------
 * void balanceCal(void)
 * balances the virtual cells of a battery with in a +/- volTolerance/2
 *----------------------------------------------------------------------------*/
 void balanceCal(void){
   int i,j;
   boolean balOn=false;
   balanceTimeStamp=micros();
   realBalDataFlag=true;
   balTempControl();
   maxVol=findMaxV();
   for(j=0;j<BMENum;j++){
     if(!BME[j].dataCheck){
       //BME[j].DCC=0;
       for(i=0;i<cellNum;i++){
         if(BME[j].fVol[i]- balance2Vol > volTolerance){
           BME[j].DCC= BME[j].DCC | (1<<(2-i));    // balance by enabling the bit flag corresponding to the i-th virtual layer
           balOn=true;
         }
         else if (BME[j].fVol[i] <= balance2Vol) {
           BME[j].DCC= BME[j].DCC & byte(!(1<<(2-i)));   // stop balancing by disabling the bit flag corresponding to the i-th virtual layer
         }
       }
     }
     else{
       BME[j].DCC=0;
     }
   }
   if(balOn) balDoneCount=0;
   else{
     balDoneCount++;
     if(balDoneCount>=16){
       balDoneFlag=true;
       if(uartPrint) Serial.println("Balancing Done");
       //if(uartPrint) BMCcommand="stop";
     }
   }
 }
 
 /*------------------------------------------------------------------------------
 * void balTempControl(void)
 * temperature check on all cells, heat sinks, and chip temperatures for 
 * temperature warnings and temperature errors
 *----------------------------------------------------------------------------*/
void balTempControl(void){
 fanOn = false;
 for(int j=0;j<BMENum;j++){                         // goes through all BMEs
   if(!BME[j].dataCheck){       // check if BME is communicating
     BME[j].balTempCon=false;
     for (int i=0;i<cellNum;i++){                   // goes through all virtual cells in a BME
       if(!BME[j].ignoreT[i] && BME[j].fTemp[i] >= tempVCWarn-5){
         BME[j].balTempCon=true;
            if(uartPrint){
              Serial.print("TempVCWarn- BME ");
              Serial.print(j);
              Serial.print(" layer ");
              Serial.print(i);
              Serial.print(": \t");
              Serial.println(BME[j].fTemp[i]);
            }
       }
     }
     if(!BME[j].ignoreT[3] && BME[j].fTemp[3] >= tempHSWarn-5){ 
       BME[j].balTempCon=true;
       if(uartPrint){
              Serial.print("tempHSWarn- BME ");
              Serial.print(j);
              Serial.print(": \t");
              Serial.println(BME[j].fTemp[3]);
            }
     }
     if(!BME[j].ignoreiT && BME[j].fiTemp >= tempTiWarn-5){
       BME[j].balTempCon=true;
       if(uartPrint){
              Serial.print("tempTiWarn- BME ");
              Serial.print(j);
              Serial.print(": \t");
              Serial.println(BME[j].fiTemp);
            }
     }
   }
   if (BME[j].balTempCon) {
     BME[j].DCC=0;
     fanOn = true;
   }
 } 
 
}



