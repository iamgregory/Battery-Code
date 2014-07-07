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
    if(uartPrint)Serial.println("Override");
    int sVal=input.indexOf("_");
    int eVal=input.indexOf("_",sVal+1);
    String overrideString=input.substring(sVal+1,eVal);
    int flagNum=overrideString.toInt();
    if(flagNum>0 && flagNum<23){
      unsigned long flagOverTempo = 1<<(flagNum-1);
      flagOverride=flagOverride | flagOverTempo;
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
    if(!stopUntil){
      contactorsOn=false;
      if (!balanceOn){
        Serial.print(BMCcommand);
        int sVal=input.indexOf("_");
        int eVal=input.indexOf("_",sVal+1);
        String bal2string=input.substring(sVal+1,eVal);
        int bal2int=bal2string.toInt();
        float tempo = bal2int*0.0001;
        if(tempo>=volLowBalAlarm){
          balance2Vol=tempo;
          balanceMode();
        }
        else{
          if(uartPrint)Serial.println("bad bal2vol!");
          volLowBalAlarmFlag = true;
        }
      }
      balanceCal(); // if balancing mode is on, then calculate which cells need to be discarged
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
      integer1=drtModuleString.toInt();  // store requested module as an integer
      eVal=input.indexOf('\n',sVal+1); //find the end of the command
      string2=input.substring(sVal+1,eVal); // grab the requested layer
      integer2=drtLayerString.toInt();  // store requested module as an integer
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
  priorityMode();                   //sets contactors according to the mode and flags
}


/*------------------------------------------------------------------------------
 * void priorityMode(void)
 * looks the bmu mode and priority then acts accordingly
 *----------------------------------------------------------------------------*/
 
 void priorityMode(void){
   
   switch(flagPriority){
     case 1:
       stopMode();
       stopUntil=true;
       Serial.println("Should Stop!");
       break;
     case 2:
     case 3:
       overrideCount++;
       if(overrideCount >=overrideTLimit){
         stopMode();
         stopUntil=true;
       }
       break;
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
      if(uartPrint) Serial.println("Stop");
      stopBal();
      chargeOn=false;
      driveOn=false;
      stopOn=true;
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
      if(uartPrint) Serial.println("Drive");
      stopBal();
      stopOn=false;
      chargeOn=false;
      driveOn=true;
      overrideCount=0;
      conOnTime=0;
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
      overrideCount=0;
      conOnTime=0;
      timeoutCount=0;
      chargeDoneFalg=false;
      BMESelfTest();
    }
    contactorsOn=true;
    if( charge2Vol <= maxVol && current<=doneCur){
      chargeDoneFalg=true;
    }
   }
 }
 


 /*------------------------------------------------------------------------------
 * void balanceMode(void)
 * puts system into Balance mode
 *----------------------------------------------------------------------------*/
 void balanceMode(void){
    if(uartPrint){
      Serial.println("Balance to: ");
      Serial.println(balance2Vol,4);
    }
    balanceMax=maxVol;
    for(int j=0;j<BMENum;j++){
     if(!BME[j].dataCheck){
       for(int i=0;i<cellNum;i++){
         if(BME[j].fVol[i]-balance2Vol > volTolerance){
           BME[j].balFlag[i]=1;
         }
         else {
           BME[j].balFlag[i]=0;
         }
       }
     }
    }
    BMESelfTest();
    driveOn=false;
    stopOn=false;
    chargeOn=false;
    balanceOn=true;
    overrideCount=0;
    timeoutCount=0;
    balDoneFlag=false;
 }
 
 /*------------------------------------------------------------------------------
 * void stopBal(void)
 * balances the virtual cells of a battery with in a +/- 5mv
 *----------------------------------------------------------------------------*/
 void stopBal(void){
   int j,i;
   for(j=0;j<BMENum;j++){   
     BME[j].DCC=0;               // set the 3 bit DCC registry to "000" 
     WRCFG((BMEdata&) BME[j]);  
     for(i=0;i<cellNum;i++){
       BME[j].balFlag[i]=false;
     }
   }
   balanceOn=false;
 }
 
 
 /*------------------------------------------------------------------------------
 * void balanceCal(void)
 * balances the virtual cells of a battery with in a +/- 5mv 
 * conversion BNME.vol*1.5mV
 *----------------------------------------------------------------------------*/
 void balanceCal(void){
   int i,j;
   boolean balOn=false;
   balTempControl();
   for(j=0;j<BMENum;j++){
     if(!BME[j].dataCheck){
       //BME[j].DCC=0;
       for(i=0;i<cellNum;i++){
         if(BME[j].fVol[i]-balance2Vol > volTolerance && BME[j].balFlag[i]){
           BME[j].DCC= BME[j].DCC | (1<<(2-i));    // balance by enabling the bit flag corresponding to the i-th virtual layer
           balOn=true;
         }
         if(BME[j].fVol[i]-balance2Vol <= volBalStop || BME[j].balTempCon){
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
     if(balDoneCount>=25) balDoneFlag=true;
   }
 }
 
 /*------------------------------------------------------------------------------
 * void balTempControl(void)
 * temperature check on all cells, heat sinks, and chip temperatures for 
 * temperature warnings and temperature errors
 *----------------------------------------------------------------------------*/
void balTempControl(void){
 
 for(int j=0;j<BMENum;j++){                         // goes through all BMEs
   if(!BME[j].dataCheck){       // check if BME is communicating
     BME[j].balTempCon=false;
     for (int i=0;i<cellNum;i++){                   // goes through all virtual cells in a BME
       if(!BME[j].ignoreT[i] && BME[j].fTemp[i] >= tempVCWarn-5){
         BME[j].balTempCon=true;
       }
     }
     if(!BME[j].ignoreT[3] && BME[j].fTemp[3] >= tempHSWarn-5){ 
       BME[j].balTempCon=true;
     }
     if(!BME[j].ignoreiT && BME[j].fiTemp >= tempTiWarn-5){
       BME[j].balTempCon=true;
     }
   }
 } 
}


