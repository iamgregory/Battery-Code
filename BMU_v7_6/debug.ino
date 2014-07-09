
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
  Serial.println(dLoopTime/1000000.0);
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
 * void debugCommand(void)
 * does the command that is sent from the GUI
 *----------------------------------------------------------------------------*/
void debugCommand(String input){
  
  if(input.indexOf("pri") >=0){
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
    BMCcommand="";
    if(uartPrint)Serial.println("Resistors Off ");
  }
  else if(input.indexOf("d1") >=0) {
    
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
   
  else if(input.indexOf("drt") >=0){
    if(uartPrint)Serial.println("Discharge Test");
    int sVal=input.indexOf("_"); //find the first underscore
    int eVal=input.indexOf('\n',sVal+1); //find the end of the command
    String drtString=input.substring(sVal+1,eVal); //grab command after "drt_"
    drtString=String("deb_1_" + drtString);
    BMCcommand=drtString;
  }
  else if(input.indexOf("flag") >=0){
    int sVal=input.indexOf("_"); //find the first underscore
    int eVal=input.indexOf('\n',sVal+1); //find the end of the command
    String flagDebugString=input.substring(sVal+1,eVal); //grab command after "drt_"
    int flagDebugInt = flagDebugString.toInt();
    flagDebugTest(flagDebugInt);
  }
  else if(input.indexOf("fake") >=0){  
    if(input.indexOf("v") >=0){
      int sVal=input.indexOf("p");
      sVal=input.indexOf("_",sVal+1); //find the first underscore
      int eVal=input.indexOf('\n',sVal+1); //find the end of the command
      String theValueString=input.substring(sVal+1,eVal); //grab command after "drt_"
      int theValueInt= theValueString.toInt();
      if(uartPrint)Serial.println("Faking voltage");
      float theValueFloat= theValueInt*0.0001;
      fakeSensorData(theValueFloat,1);
    }
    else if(input.indexOf("t") >=0){
      int sVal=input.indexOf("t");
      sVal=input.indexOf("_",sVal+1); //find the first underscore
      int eVal=input.indexOf('\n',sVal+1); //find the end of the command
      String theValueString=input.substring(sVal+1,eVal); //grab command after "drt_"
      int theValueInt= theValueString.toInt();
      if(uartPrint)Serial.println("Faking temp");
      float theValueFloat= theValueInt*0.0001;
      fakeSensorData(theValueFloat,2);
    }
    else if(input.indexOf("p") >=0){
      int sVal=input.indexOf("p");
      sVal=input.indexOf("_",sVal+1); //find the first underscore
      int eVal=input.indexOf('\n',sVal+1); //find the end of the command
      String theValueString=input.substring(sVal+1,eVal); //grab command after "drt_"
      int theValueInt= theValueString.toInt();
      float theValueFloat= theValueInt*0.0001;
      if(uartPrint)Serial.println("Faking pressure");
      fakeSensorData(theValueFloat,3);
    }
    else{
      fakeTempFlag=false;
      fakeVolFlag=false;
      fakePressFlag=false;
    }
  }
  else {
    BMCcommand=input; //regular mode commands like charge, balance, stop, clear,
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
 
 
 /*------------------------------------------------------------------------------
 *  void dischargeResistorTest(int module, int layer)
 * enables the <module>'s <resistor> so discharging occcurs
 * prints the voltage of each layer in the whole module
 *----------------------------------------------------------------------------*/
 
 void dischargeResistorTest(int module,int layer){
   if (0<module<=BMENum) {  // make sure the command is Module 1-14
      BME[module-1].balFlag[layer-1]= 1;
      if (1) { //(BME[module-1].fVol[layer-1] > 0) { //make sure voltage isnt too low volLowWarn
        if (layer >122) {
          BME[module-1].DCC= BME[module-1].DCC=7;    // turn  on all layers
        }
        else if (layer >22) {
          BME[module-1].DCC= BME[module-1].DCC=3;    // turn  on all layers
        }
        else if (layer >12) {
          BME[module-1].DCC= BME[module-1].DCC=5;    // turn  on all laye
        }
        else if (layer >11) {
          BME[module-1].DCC= BME[module-1].DCC=6;    // turn  on all laye
        }
        else if (layer >0) {
          BME[module-1].DCC= BME[module-1].DCC | (1<<(3-layer));    // turn on the moudle layer's resistor
        }
        int j=0;
        for(j=0;j<cellNum;j++){           // print voltage of each cell with tab spacers "vol1 \t vol2 \tb vol3 \n"            
          Serial.print(BME[module-1].fVol[j],4); // four decimal precision on voltages
          Serial.print("\t");
        }
        Serial.println("");      // prints carriage return
      }
      else {  // if voltage is too low, stop the discharge resistor test
        BME[module-1].DCC= 0;   // stop balancing by disabling the bit flag corresponding to the i-th virtual layer
        Serial.print("Voltage too low. Try another cell or recharge.");  
        BMCcommand = "stop";
      }
   } 
 }
 
  /*------------------------------------------------------------------------------
 *  void flagDebugTest(void)
 * discharges all the virtual cells of a battery
 *----------------------------------------------------------------------------*/
 
 void flagDebugTest(const int &flag){
   if (0<flag && flag <33) flagBMU= 0 | (1<<(flag-1)); 
 }
 
 /*------------------------------------------------------------------------------
 *  void fakeSensorData(void)
 * discharges all the virtual cells of a battery
 *----------------------------------------------------------------------------*/
 
 void fakeSensorData(const float &reading, const int &datatype){
   switch (datatype) {
    case 1:  // voltage
      fakeVolFlag= true;
      fakeVol=reading;
      break;
    case 2: //temp
      fakeTempFlag=true;
      fakeTemp=reading;
      break;
    case 3: //pressure
      fakePressFlag=true;
      fakePress=reading;
      break;
    default: 
      // if nothing else matches, do the default
      // default is optional
      break;
    }
 }
 
 

