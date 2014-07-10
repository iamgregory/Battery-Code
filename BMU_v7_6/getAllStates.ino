
/*------------------------------------------------------------------------------
 * void measCalAllstates(void)
 * Measures and calculates all states of half string
 *-----------------------------------------------------------------------------*/
 void measCalAllstates(void){
//  if(loopCount%bmeSelfTestTime==0)  BMESelfTest();    // run self check on all BMEs runs once every 5 min
    
  getBMEData();               // gets data from all BMEs 
  calStateBME();              // calculates state of BME's
 
  getBMUData();               //gets data for the half-string
  calStateBMU();              // calculates the state of the half-string
 }

 /*------------------------------------------------------------------------------
 * void getBMUData(void)
 * Measures and calculates data on a half string level 
 *-----------------------------------------------------------------------------*/
 void getBMUData(void){
  
  
  totalVoltage=analogRead(tVolInPin)*volConst;
  fwLeak=!digitalRead(frontWPin);
  pressure=analogRead(presIn1Pin)*presConst-presOffset;
  bwLeak=!digitalRead(backWPin);
  current0=analogRead(cur0InPin);
  delay(1);
  current=(analogRead(curInPin)-(float)current0)*curConst;
  if (abs(current)<=.6) current=0;
 }
 /*------------------------------------------------------------------------------
 * void calStateBMU(void)
 * uses the measured data
 * to calculate the state of the battery and check errors
 *-----------------------------------------------------------------------------*/
 
 void calStateBMU(void){
  
  socCal();                              //calculates the state of charge
  presRate=rateCal(pressure,presOld);    // calculates pressure rate
  presOld=pressure;                      //set the old pressure value to the new one
 }
 
 /*------------------------------------------------------------------------------
 * void getBMEData(void)
 * Gets the measured data
 * Collects voltage, flag and temperature readings of the BME's.
 *-----------------------------------------------------------------------------*/
 void getBMEData(void){
  int i;
   
  for(i=0;i<BMENum;i++){ 
    BME[i].dataCheck=false;
  }
  
  CLRCELL(0);          // wake the bme 
  
  ADCV(0,0);          //  broadcast to all channels
  delayMicroseconds(BMEConDelay1);
  
  for(int i=0;i<BMENum;i++){ 
    RDCVA((BMEdata&) BME[i]);
  }
  
  // get cell temperatures
  ADAX(0,0);
  delayMicroseconds(BMEConDelay1);
  
  for(int i=0;i<BMENum;i++){
    RDAUXA((BMEdata&) BME[i]);
    RDAUXB((BMEdata&) BME[i]);  
  }
  
  // get chip temperatures, sum of battery module 
  ADSTAT(0,0);
  delayMicroseconds(BMEConDelay2);
  for(int i=0;i<BMENum;i++){
    RDSTATA((BMEdata&) BME[i]);
    RDSTATB((BMEdata&) BME[i]);
  }
  
  for(i=0;i<BMENum;i++){               // Cycles through BME's
    BME[i].GPIO=0x0f|((!fanOn)<<4);          // Sets the GPIO to 0 or 1 for the multiplexer
    WRCFG((BMEdata&) BME[i]);          // Sends out the GPIO command
  }

if (balanceOn) saturateBalanceVoltage();
  for(int i=0;i<BMENum;i++){
    int2float((BMEdata&) BME[i]); // passes pointer to BME[i]
  }
  
  if (fakeTempFlag)
    BME[1].fTemp[1]=fakeTemp;
  if (fakeVolFlag)
    BME[1].fVol[1]=fakeVol;
  if (fakePressFlag)
    pressure=fakePress;
}

 /*------------------------------------------------------------------------------
 * void calStateBME(void)
 * uses the measured data
 * to calculate the state of the battery and check errors
 *-----------------------------------------------------------------------------*/
 
 void calStateBME(void){
  
  minVol=findMinV();                    //updates min cell voltage and total battery-string voltage
  if(!balanceOn) maxVol=findMaxV();
  maxTemp=findMaxT();                  // updates the max temperature reading
  volSumCal();                       // sums all the virtual cell voltages into modules and half-strin voltage
 }

 /*------------------------------------------------------------------------------
 * void BMESelfTest(void)
 * clears the ADC registers and performs a self-test on each BME
 * checks the digital filter of each BME
 *-----------------------------------------------------------------------------*/
 void BMESelfTest(void){
  int i;
  
  for(i=0;i<BMENum;i++){           //sets flags to false
    BME[i].dataCheck=false;
    BME[i].muxCheck=false;
    BME[i].volSelfCheck=false;
    BME[i].AuxSelfCheck=false;
    BME[i].StatSelfCheck=false;
  }
  
  CLRCELL(0);          // wake the bme
  CLRCELL(0);          // clears all battery voltage data
  CLRAUX(0);           // clear all battery module temperature readings
  CLRSTAT(0);          // clear all battery module status register

  
  CVST(0);                // start testing voltage sensor's digital filter
  delayMicroseconds(BMEConDelay1);
  for(int i=0;i<BMENum;i++){ 
    RDCVST((BMEdata&) BME[i]); //check results of self test
  }
  
  AXST(0);                // start testing auxiliary sensor's digital filter
  delayMicroseconds(BMEConDelay1);
  for(int i=0;i<BMENum;i++){ 
    RDAXSTA((BMEdata&) BME[i]);  //check results of self test
  }
  
  STATST(0);              // start testing Status sensor's digital filter
  delayMicroseconds(BMEConDelay2);
  DIAGN(0);
  delayMicroseconds(BMEConDelay2);
  for(int i=0;i<BMENum;i++){ 
    RDSTATSTA((BMEdata&) BME[i]);  //check results of self test
    RDSTATSTB((BMEdata&) BME[i]);  //check results of self test
  }
  
 }
 
//////////////////////////// sub Functions //////////////////////// 
 
 /*------------------------------------------------------------------------------
 *  float rateCal(float value, float valueLast)
 * takes in the current and last data point
 * returns the rate
 *-----------------------------------------------------------------------------*/
 
 float rateCal(float value, float valueLast){
   float valueRate=0;
   valueRate=(value-valueLast)/dt;
   
   return valueRate;
 }
 
 /*------------------------------------------------------------------------------
 * float findMinV(void)
 * finds the lowest voltage cell
 *----------------------------------------------------------------------------*/
 float findMinV(void){
   int i,j;
   float _mVol=7.0;
   for(j=0;j<BMENum;j++){
     if(!BME[j].dataCheck){
       for(i=0;i<cellNum;i++){
         if(BME[j].fVol[i] < _mVol){
           _mVol=BME[j].fVol[i];
         }
       }
     }
   }
   return _mVol;
 }

/*------------------------------------------------------------------------------
 * float findMaxV(void)
 * finds the lowest voltage cell
 *----------------------------------------------------------------------------*/
 float findMaxV(void){
   int i,j;
   float _mVol=0;
   for(j=0;j<BMENum;j++){
     if(!BME[j].dataCheck){
       for(i=0;i<cellNum;i++){
         if(BME[j].fVol[i] > _mVol){
           _mVol=BME[j].fVol[i];
         }
       }
     }
   }
   return _mVol;
 }
 
 /*------------------------------------------------------------------------------
 * float findMaxT(void)
 * returns the highest cell temperature
 *----------------------------------------------------------------------------*/
 float findMaxT(void){
   int i,j;
   float _mTemp=-300;
   for(j=0;j<BMENum;j++){
     if(!BME[j].dataCheck){
       for(i=0;i<3;i++){
         if(BME[j].fTemp[i] > _mTemp){
           _mTemp=BME[j].fTemp[i];
         }
       }
     }
   }
   return _mTemp;
 }
 
 /*------------------------------------------------------------------------------
 * void volSumCal(void)
 * returns the sums all the virtual cell voltages
 *----------------------------------------------------------------------------*/
 void volSumCal(void){
   int i,j;
   volSum=0.0;
   for(j=0;j<BMENum;j++){
     BME[j].modSum=0.0;
     for(i=0;i<cellNum;i++){  
       BME[j].modSum+=BME[j].fVol[i];
     }
//     Serial.println(BME[j].modSum);
     volSum+=BME[j].modSum;
   }
 }
 
 
 /*------------------------------------------------------------------------------
 *void saturateBalanceVoltage(void)
 * saturates voltages if balance mode is on
 *----------------------------------------------------------------------------*/
 void saturateBalanceVoltage(void){ 
   int i,j;
   for(j=0;j<BMENum;j++){
     for(i=0;i<cellNum;i++){  
       if(BME[j].vol[i]> (int)(maxVol*10000)) BME[j].vol[i]=(int)(maxVol*10000);
       else if(BME[j].vol[i]< (int)((balance2Vol-.005)*10000)) BME[j].vol[i]=(int)((balance2Vol-.005)*10000);
     }
   }
 }
