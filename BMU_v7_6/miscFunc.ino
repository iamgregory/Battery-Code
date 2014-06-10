/*------------------------------------------------------------------------------
 * void timeCheck(void)
 * checks the loop time and makes the loop in a constant time interval. 
 *----------------------------------------------------------------------------*/
 
 void timeCheck(void){
   unsigned long currentTimeStamp;
   do{
     currentTimeStamp=micros();
    dloopTime= currentTimeStamp-timeStamp;          // microseconds since "void Loop" began
    if(timeStamp> currentTimeStamp){                // check if a rollover has occured
      dloopTime= dloopTime - maxULong;              // correct for rollover
    }
    if (dloopTime>(controlTime+50)){                // for debugging purposes??
      Serial.println(dloopTime);
    }
   
   }while(dloopTime < controlTime);                 // kill time till next "void Loop" should execute
   BMCcommdt=BMCcommdt+dloopTime;
 }
 
 

 
 
 
 
 
 
 
