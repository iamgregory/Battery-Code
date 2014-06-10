/*------------------------------------------------------------------------------
 * void timeCheck(void)
 * checks the loop time and makes the loop in a constant time interval. 
 *----------------------------------------------------------------------------*/
 
 void timeCheck(void){
   do{
    dloopTime= micros()-timeStamp;
    if (dloopTime>(controlTime+50)){ 
      Serial.println(dloopTime);
    }
   
   }while(dloopTime < controlTime);
   BMCcommdt=BMCcommdt+dloopTime;
 }
 
 

 
 
 
 
 
 
 
