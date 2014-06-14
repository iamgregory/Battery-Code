
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
  debugCommand(inputString);
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

/*------------------------------------------------------------------------------
 * void getCommand(void)
 * does the command that is sent from the GUI
 *----------------------------------------------------------------------------*/
void debugCommand(String input){
  
  if(input.indexOf("cle") >=0) {
      clearFlags();            //clear flags
  }
  
  if(input.indexOf("sto") >=0)//checks for Stop
  {
    stopMode();
  }
  
  else if(input.indexOf("dri") >=0)//checks for start
  {
    driveMode();
  }
  
  else if(input.indexOf("ove") >=0)//checks for hdata
  {
    if(uartPrint)Serial.println("Override");
    int sVal=input.indexOf("_");
    int eVal=input.indexOf("_",sVal+1);
    String overrideString=input.substring(sVal+1,eVal);
    int flagNum=overrideString.toInt();
    if(flagNum>0 && flagNum<23){
      unsigned long flagOverTempo = 1<<(flagNum-1);
      flagOverride=flagOverride | flagOverTempo;
      if(uartPrint)Serial.println(flagOverride,HEX);
      overrideCount=0;
    }
  }
  else if(input.indexOf("ign") >=0)//checks for hdata
  { 
    flagBMU=~(~flagBMU | (0xE));
    flagOverride=~(~flagOverride | (0xE));
    flagIgnoreTemp=true;
  }
  
  
  else if(input.indexOf("cha") >=0)//checks for Charge
  {
    chargeMode();
  }
  
  else if(input.indexOf("bal") >=0)//checks for Balance
  {
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
    
    if (!balanceOn){
      
      int sVal=input.indexOf("_");
      int eVal=input.indexOf("_",sVal+1);
      String bal2string=input.substring(sVal+1,eVal);
      int bal2int=bal2string.toInt();
      float temp = bal2int*0.0001;
      if(temp>=volLowBalAlarm){
        if(uartPrint)Serial.println("Balance");
        balance2Vol=temp;
        balanceMode();
      }
      else{
        if(uartPrint)Serial.println("bad bal2vol!");
        volLowBalAlarmFlag = true;
      }
    }
  }
  
  
  else if(input.indexOf("pri") >=0){
    int sVal=input.indexOf(" ");
    if(sVal>0){
      int eVal=input.indexOf(" ",sVal+1);
      String ignoreBMEString=input.substring(sVal+1,eVal);
      int i = ignoreBMEString.toInt();
      if(i>0 && i<15)testBME((BMEdata&)BME[i-1]);
      else testBMU();
    }
    else{
      testBMU();
      for(int i=0;i<14;i++){
        printOutBME((BMEdata&)BME[i]);
      }
    }
  }
  
  else if(input.indexOf("fon") >=0){
        fanOn=true; 
        if(uartPrint)Serial.println("Fan On ");

  }
  else if(input.indexOf("fof") >=0){
    fanOn=false; 
    if(uartPrint)Serial.println("Fan Off ");
  }
  else if(input.indexOf("don") >=0) {
    dischargeTest();
    if(uartPrint)Serial.println("Resistors On ");
  }
  else if(input.indexOf("dof") >=0) {
    stopBal();
    if(uartPrint)Serial.println("Resistors Off ");
  }
  else if(input.indexOf("help") >=0) {
    if(uartPrint){
      Serial.println("stop");
      Serial.println("drive");
      Serial.println("charge");
      Serial.println("balance_bal2int_");
      Serial.println("override_overflagInt_");
      Serial.println("ignore");
      Serial.println("clear or reset");
      Serial.println("print_x_ to read data from x bme");
      Serial.println("fon to turn fan on");
      Serial.println("fof to turn fan off");
      Serial.println("don to turn resistors on");
      Serial.println("dof to turn resistors off");
    }
  }
}

 /*------------------------------------------------------------------------------
 *  void dischargeTest(void)
 * discharges all the virtual cells of a battery
 *----------------------------------------------------------------------------*/
 
 void dischargeTest(void){
   int j=0;
   for(j=0;j<BMENum;j++){           // for all BMEs            
     BME[j].DCC= 7;                // set 3 bit DCC registry to "111" 
   } 
 }
