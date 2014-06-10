
 /*------------------------------------------------------------------------------
 * void getBMUData(void)
 * Measures and calculates data on a half string level 
 *-----------------------------------------------------------------------------*/
 void getBMUData(){
  
  
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
 * void BMESelfTest(void)
 * clears the ADC registers and performs a self-test on each BME
 * checks the digital filter of each BME
 *-----------------------------------------------------------------------------*/
 void BMESelfTest(){
  int i;
  
  for(i=0;i<BMENum;i++){ 
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

  
  CVST(0);                // start testing voltage sensor's digatal filter
  delayMicroseconds(BMEConDelay1);
  for(int i=0;i<BMENum;i++){ 
    RDCVST((BMEdata&) BME[i]); //check results of self test
  }
  
  AXST(0);                // start testing auxiliary sensor's digatal filter
  delayMicroseconds(BMEConDelay1);
  for(int i=0;i<BMENum;i++){ 
    RDAXSTA((BMEdata&) BME[i]);  //check results of self test
  }
  
  STATST(0);              // start testing Status sensor's digatal filter
  delayMicroseconds(BMEConDelay2);
  DIAGN(0);
  delayMicroseconds(BMEConDelay2);
  for(int i=0;i<BMENum;i++){ 
    RDSTATSTA((BMEdata&) BME[i]);  //check results of self test
    RDSTATSTB((BMEdata&) BME[i]);  //check results of self test
  }
  
 }
 /*------------------------------------------------------------------------------
 * void getBMEData(void)
 * Gets the measured data
 * Collects voltage, flag and temperature readings of the BME's.
 *-----------------------------------------------------------------------------*/
 void getBMEData(){
  int i;
   
  for(i=0;i<BMENum;i++){ 
    BME[i].dataCheck=false;
  }
  
  CLRCELL(0);          // wake the bme 
  
  ADCV(0,0);
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

  for(int i=0;i<BMENum;i++){
    int2float((BMEdata&) BME[i]);
  }
  
}


 
