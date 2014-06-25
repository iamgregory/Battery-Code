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
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
    
    if (!balanceOn){
      
      int sVal=input.indexOf("_");
      int eVal=input.indexOf("_",sVal+1);
      String bal2string=input.substring(sVal+1,eVal);
      int bal2int=bal2string.toInt();
      float temp = bal2int*0.0001;
      if(temp>=volLowBalAlarm){
        if(uartPrint)Serial.println("Balance");
        balance2Vol=temp;
        balanceMode();
      }
      else{
        if(uartPrint)Serial.println("bad bal2vol!");
        volLowBalAlarmFlag = true;
      }
    }
    balanceCal(); // if balancing mode is on, then calculate which cells need to be discarged
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
   
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
    
    if(!stopOn){
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
    relayOn();
   }
 }
 
 /*------------------------------------------------------------------------------
 * void chargeMode(void)
 * puts system into Charge mode
 *----------------------------------------------------------------------------*/
 void chargeMode(void){
   if(!stopUntil){
    if(!chargeOn){
      if(uartPrint) Serial.print("Charge to");
      if(uartPrint) Serial.println(charge2Vol);
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
    relayOn();
   }
   if(abs(charge2Vol-maxVol)<.01 && current<=doneCur){
     chargeDoneFalg=true;
   }
 }
 
/*------------------------------------------------------------------------------
 * void relayOn(void))
 * puts the relay on?
 *----------------------------------------------------------------------------*/
void relayOn(void){
  digitalWrite(relay1, HIGH);
  if(conOnTime<1) conOnTime++;
  else digitalWrite(relay2, HIGH);

}

 /*------------------------------------------------------------------------------
 * void balanceMode(void)
 * puts system into Balance mode
 *----------------------------------------------------------------------------*/
 void balanceMode(void){
   digitalWrite(relay1, LOW);
   digitalWrite(relay2, LOW);
   if(!stopUntil){
    if(uartPrint) Serial.println(balance2Vol);
    if (balance2Vol < minVol) balance2Vol=minVol;
    for(int j=0;j<BMENum;j++){
     if(!BME[j].dataCheck){
       for(int i=0;i<cellNum;i++){
         if(BME[j].vol[i]-balance2Vol > volTolerance){
           BME[j].balFlag[i]=1;
         }
         else {
           BME[j].balFlag[i]=0;
         }
       }
     }
    }
    if(!balanceOn){
      driveOn=false;
      stopOn=false;
      chargeOn=false;
      balanceOn=true;
      overrideCount=0;
      timeoutCount=0;
      BMESelfTest();
    }
   }
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
   for(j=0;j<BMENum;j++){
     if(!BME[j].dataCheck){
       //BME[j].DCC=0;
       for(i=0;i<cellNum;i++){
         if(BME[j].vol[i]-balance2Vol > volTolerance && BME[j].balFlag[i]){
           BME[j].DCC= BME[j].DCC | (1<<i);    // balance by enabling the bit flag corresponding to the i-th virtual layer
         }
         if(BME[j].vol[i]-balance2Vol <= volBalStop){
           BME[j].DCC= BME[j].DCC & byte(!(1<<i));   // stop balancing by disabling the bit flag corresponding to the i-th virtual layer
         }
       }
     }
     else{
       BME[j].DCC=0;
     }
   }
 }
