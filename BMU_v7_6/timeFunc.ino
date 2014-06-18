/*------------------------------------------------------------------------------
 * void timeCheck(void)
 * checks the loop time and makes the loop in a constant time interval. 
 *----------------------------------------------------------------------------*/
 
 void timeCheck(void){
   unsigned long currentTimeStamp;
   do{
    currentTimeStamp=micros();
    if(timeStamp > currentTimeStamp){                // check if a rollover has occured
      dloopTime= ~(timeStamp - currentTimeStamp)+1;              // correct for rollover
    }
    else dloopTime = currentTimeStamp-timeStamp;          // microseconds since "void Loop" began
    
    if (dloopTime>(controlTime+50)){                // for debugging purposes
      if(uartPrint)Serial.println(dloopTime);
    }
   
   }while(dloopTime < controlTime);                 // kill time till next "void Loop" should execute
   BMCcommdt=BMCcommdt+dloopTime;
 }
 
 

 
 
 
 
 
 
 
