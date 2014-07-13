/*------------------------------------------------------------------------------
 * void timeCheck(void)
 * checks the loop time and makes the loop in a constant time interval. 
 *----------------------------------------------------------------------------*/
 
 void timeCheck(void){
   if(areWeThereYet(timeStamp, controlTime)){
     unsigned long currentTimeStamp =micros();
     unsigned long timeSince=0;
     if(timeStamp > currentTimeStamp){                // check if a rollover has occured
        timeSince= ~(timeStamp - currentTimeStamp)+1;              // correct for rollover
     }
     else{
       timeSince = currentTimeStamp-timeStamp;
     }
     Serial.print("Loop running late by: ");
     Serial.print(timeSince-controlTime);
     Serial.println(" microseconds!");
   }
   else{
     do{
     }while(!areWeThereYet(timeStamp, controlTime));                 // kill time till next "void Loop" should execute
   }
 }
 
 
 /*------------------------------------------------------------------------------
 * boolean areWeThereYet(const unsigned long &referenceTimeStamp, const long &waitTime)
 * returns true if waitTime has elapsed since referenceTimeStamp
 * returns false if not
 *----------------------------------------------------------------------------*/
 boolean areWeThereYet(const unsigned long &referenceTimeStamp, const long &waitTime){
   unsigned long timeSince= timeElapsed(referenceTimeStamp);
   if (timeSince>=waitTime)
     return true;
   else
     return false;
 }
 
 long timeElapsed(const unsigned long &referenceTimeStamp){
   unsigned long currentTimeStamp =micros();
   unsigned long timeSince=0;
   if(referenceTimeStamp > currentTimeStamp){                // check if a rollover has occured
      timeSince= ~(referenceTimeStamp - currentTimeStamp)+1;              // correct for rollover
    }
   else timeSince = currentTimeStamp-referenceTimeStamp;
   return timeSince;
 }
 
/*------------------------------------------------------------------------------
 * void modeTimeReset(void)
 *
 *----------------------------------------------------------------------------*/
 
 void modeTimeReset(void){
   modeInfo.hours=0;
   modeInfo.minutes=0;
   modeInfo.microseconds=0;
   modeInfo.timeKeepingStamp=micros();
 }
 
 
 

 
 
 
 
 
 
 
