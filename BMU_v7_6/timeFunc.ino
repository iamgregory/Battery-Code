/*------------------------------------------------------------------------------
 * void timeCheck(void)
 * checks the loop time and makes the loop in a constant time interval. 
 *----------------------------------------------------------------------------*/
 
 void timeCheck(void){
   unsigned long currentTimeStamp;
   do{
    currentTimeStamp=micros();
    if(timeStamp > currentTimeStamp){                // check if a rollover has occured
      dLoopTime= ~(timeStamp - currentTimeStamp)+1;              // correct for rollover
    }
    else dLoopTime = currentTimeStamp-timeStamp;          // microseconds since "void Loop" began
    
    if (dLoopTime>(controlTime+50)){                // for debugging purposes
      if(uartPrint)Serial.println(dLoopTime);
    }
   
   }while(dLoopTime < controlTime);                 // kill time till next "void Loop" should execute
   BMCcommdt=BMCcommdt+dLoopTime;
 }
 
 /*------------------------------------------------------------------------------
 * boolean areWeThereYet(unsigned long referenceTimeStamp, long waitTime)
 * returns true if waitTime has elapsed since referenceTimeStamp
 * returns false if not
 *----------------------------------------------------------------------------*/
 boolean areWeThereYet(unsigned long referenceTimeStamp, long waitTime){
   unsigned long currentTimeStamp =micros();
   unsigned long timeSince=0;
   if(referenceTimeStamp > currentTimeStamp){                // check if a rollover has occured
      timeSince= ~(referenceTimeStamp - currentTimeStamp)+1;              // correct for rollover
    }
   else timeSince = currentTimeStamp-timeStamp;
   if (timeSince>=waitTime)
     return true;
   else
     return false;
 }
 
 

 
 
 
 
 
 
 
