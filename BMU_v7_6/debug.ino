
 /*------------------------------------------------------------------------------
 * void printout(void)
 * prints information out on the screen for all BME's
 *----------------------------------------------------------------------------*/
 void serialEvent() {
  inputString = "";
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
//    Serial.println(inputString);
  }
  getCommand(inputString);
 }

/*------------------------------------------------------------------------------
 * void BMEcheck(void)
 * checks to see if BMEs are comunicating 
 *----------------------------------------------------------------------------*/
 void BMEcheck(){
   
   for(int i=0;i<BMENum;i++){
     if(BME[i].dataCheck){
       Serial.print("ERROR Communicating with BME ");
       Serial.println((i+1));
     }
     else{
       Serial.print("Found BME ");
       Serial.println((i+1));
     }
   } 
   Serial.println("--------------------------------------------------");
 }
 
 /*------------------------------------------------------------------------------
 * void printout(void)
 * prints information out on the screen for all BME's
 *----------------------------------------------------------------------------*/
 void printOut(){
   
   for(int i=0;i<BMENum;i++){
     if(BME[i].dataCheck){
       Serial.print("ERROR Communicating with BME ");
       Serial.println((i+1));
     }
     else{
       printOutBME((BMEdata&)BME[i]);
     }
   } 
 }
 
 /*------------------------------------------------------------------------------
 * void printout(void)
 * prints information out on the screen for one BME
 *----------------------------------------------------------------------------*/
void printOutBME(BMEdata& _BME){

  int i=0;
  
  Serial.print("-------------------------------------------------------------------------------------\n");
  if(_BME.dataCheck){
     Serial.print("ERROR Communicating with BME ");
     Serial.println(((_BME.addr-0x80)>>3)+1);
  }
  else{
     Serial.print("BME ");
     Serial.print(((_BME.addr-0x80)>>3)+1);
     
    Serial.print("  Voltages \n");
    Serial.print("Cell 1   \t\t   Cell 2 \t\t   Cell 3 \t\t Cell Sum \t\t V_ref2 \n");
    for(i=0;i<cellNum;i++){
      Serial.print(_BME.fVol[i],4);
      Serial.print(" V\t\t");
    }
    Serial.print(_BME.fVSum,3);
    Serial.print(" V\t\t");
    Serial.print(_BME.fVref2,4);
    Serial.println(" V");
//    Serial.print("BME ");
//    Serial.print(((_BME.addr-0x80)>>3)+1);
//    Serial.print(" Temperatures \n");
//    for(i=0;i<4;i++){
//      Serial.print(_BME.fTemp[i],2);
//      Serial.print(" C\t\t");
//    }
//    Serial.print(_BME.fiTemp,2);
//    Serial.print(" C\n");
  }
}


/*------------------------------------------------------------------------------
 * void testBMU(void)
 * prints information out on the screen for one BME
 *----------------------------------------------------------------------------*/
 void testBMU(){
  Serial.println("-------------------------------------------------------------------------------------");
  Serial.println("BMU sensors"); 
  Serial.print("Current:"); 
  if (current < 0){
    Serial.print('-');
    Serial.print(-1*current,2);
  }
  else{
    Serial.print(current,2);
  }
  Serial.print(" A\t Voltage:");
  Serial.print(totalVoltage,2);
  Serial.print(" V\t Pressure:");
  Serial.print(pressure,1);
  Serial.print(" PSIG\t waterleak: ");
  if(!fwLeak & !bwLeak) Serial.print("no leak");
  else if(!fwLeak & bwLeak) Serial.print("back leak");
  else if(fwLeak & !bwLeak) Serial.print("front leak");
  else if(fwLeak & bwLeak) Serial.print("both leak");
  Serial.print(" \t looptime (s): ");
  Serial.println(dloopTime/1000000.0);
  Serial.print(" \nFlag: ");
  Serial.print(flagBMU,HEX);
  Serial.print(" \t SOC: ");
  Serial.print(SOC,2);
  Serial.print(" %\t Vsum: ");
  Serial.print(volSum,2);
  Serial.print("V \t minVol: ");
  Serial.print(minVol,4);
  Serial.print("V \t maxVol: ");
  Serial.print(maxVol,4);
  Serial.print("V \t maxTemp: ");
  Serial.print(maxTemp,2);
  Serial.println(" C");
//  Serial.print(presRate,2);
  
  
  
 }
 
/*------------------------------------------------------------------------------
 * void testBME(void)
 * prints information out on the screen for one BME
 *----------------------------------------------------------------------------*/
void testBME(BMEdata& _BME){

  int i=0;
  
  int2float((BMEdata&) _BME);
  Serial.print("-------------------------------------------------------------------------------------\n");
  if(BME[i].dataCheck){
     Serial.print("ERROR Communicating with BME ");
     Serial.println(((_BME.addr-0x80)>>3)+1);
  }
  else{
     Serial.print("BME ");
     Serial.println(((_BME.addr-0x80)>>3)+1);
  }
  Serial.print(" Voltages \n");
  Serial.print("Cell 1   \t\t   Cell 2 \t\t   Cell 3 \t\t Cell Sum \t\t V_ref2 \n");
  for(i=0;i<cellNum;i++){
    Serial.print(_BME.fVol[i],4);
    Serial.print(" V\t\t");
  }
  Serial.print(_BME.fVSum,3);
  Serial.print(" V\t\t");
  Serial.print(_BME.fVref2,4);
  Serial.println(" V");
  Serial.print("BME ");
  Serial.print(((_BME.addr-0x80)>>3)+1);
  Serial.print(" Temperatures \n");
  for(i=0;i<4;i++){
    Serial.print(_BME.fTemp[i],1);
    Serial.print(" C\t\t");
  }
  Serial.print(_BME.fiTemp,1);
  Serial.print(" C\n");
  Serial.print("Over Voltage Flags C1 C2 C3\n");
  for(i=0;i<cellNum;i++){
    Serial.print(_BME.oFlag[i]);
    Serial.print("\t");
  }
  Serial.print("\nUnder Voltage Flags C1 C2 C3\n");
  for(i=0;i<cellNum;i++){
    Serial.print(_BME.uFlag[i]);
    Serial.print("\t");
  }
  Serial.print("\nSelf-tests \n");
  Serial.print("DataCheck \t MUX Decoder   \t   Volt Mem \t   GPIO Mem \t Status Mem \n");
  if(_BME.dataCheck) Serial.print("Fail\t\t");
  else Serial.print("Pass\t\t");
  if(_BME.muxCheck) Serial.print(" Fail\t\t   ");
  else Serial.print(" Pass\t\t   ");
  if(_BME.volSelfCheck) Serial.print("Fail\t\t   ");
  else Serial.print("Pass\t\t   ");
  if(_BME.AuxSelfCheck) Serial.print("Fail\t\t ");
  else Serial.print("Pass\t\t ");
  if(_BME.StatSelfCheck) Serial.println("Fail\t\t");
  else Serial.println("Pass\t\t");
}
