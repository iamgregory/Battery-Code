/*------------------------------------------------------------------------------
 * void calState(void)
 * uses the measured data
 * to calculate the state of the battery and check errors
 *-----------------------------------------------------------------------------*/
 
 void calStateBMU(void){
  
  socCal();                              //calculates the state of charge
  presRate=rateCal(pressure,presOld);    // calculates pressure rate
  presOld=pressure;                      //set the old pressure value to the new one
 }
 
 /*------------------------------------------------------------------------------
 * void calState(void)
 * uses the measured data
 * to calculate the state of the battery and check errors
 *-----------------------------------------------------------------------------*/
 
 void calStateBME(void){
  
  minVol=findMinV();                    //updates min cell voltage and total battery-string voltage
  maxVol=findMaxV();
  maxTemp=findMaxT();                  // updates the max temperature reading
  volSumCal();                       // sums all the vertual cell voltages into modules and half-strin voltage
 }
 
 /*------------------------------------------------------------------------------
 * void rateCal(void)
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
         if(BME[j].fVol[i] < _mVol && !BME[j].ignoreVol[i]){
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
         if(BME[j].fVol[i] > _mVol && !BME[j].ignoreVol[i]){
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
 * returns the sums all the vertual cell voltages
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

