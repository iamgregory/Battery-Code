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
   if(minVol<2.760){
     stopMode();
     stopUntil=true;
     Serial.println("voltage too low for testing ==> Stop!");
   }
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
      conOnTime=0;
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
      if(uartPrint) Serial.println("Charge");
      stopBal();
      driveOn=false;
      stopOn=false;
      chargeOn=true;
      overrideCount=0;
      conOnTime=0;
    }
    relayOn();
   }
 }
 
/*------------------------------------------------------------------------------
 * void chargeMode(void)
 * puts system into Charge mode
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
    driveOn=false;
    stopOn=false;
    chargeOn=false;
    balanceOn=true;
    overrideCount=0;
   }
 }
 
 /*------------------------------------------------------------------------------
 * void stopBal(void)
 * balances the virtual cells of a battery with in a +/- 5mv
 *----------------------------------------------------------------------------*/
 void stopBal(void){
   int j,i;
   for(j=0;j<BMENum;j++){   
     BME[j].DCC=0; 
     WRCFG((BMEdata&) BME[j]);  
     for(i=0;i<cellNum;i++){
       BME[j].balFlag[i]=false;
     }
   }
   balanceOn=false;
 }
 
 
 /*------------------------------------------------------------------------------
 * void balance(void)
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
           BME[j].DCC= BME[j].DCC | (1<<i);    // balance
         }
         if(BME[j].vol[i]-balance2Vol <= volBalStop){
           BME[j].DCC= BME[j].DCC & byte(!(1<<i));   // stop balancing
         }
       }
     }
     else{
       BME[j].DCC=0;
     }
   }
 }
 
 /*------------------------------------------------------------------------------
 * void dicharge(void)
 * dicharges all the virtual cells of a battery
 *----------------------------------------------------------------------------*/
 
 void dischargeTest(void){
   int j=0;
   int ronTime=2;
//      int ronTime=loopCount%2;
   for(j=0;j<BMENum;j++){
      BME[j].DCC= 0;
   }
   if ((disNum/ronTime)<(14*ronTime)) BME[(disNum/ronTime)].DCC= 7;
   disNum++;
 }
 
