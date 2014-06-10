/*------------------------------------------------------------------------------
 * void BMCcomm(void)
 * Sends Battery information and receives commands over ethernet
 * data out: dt, SOC, total voltage, BME voltages, and BME temperatures
 * data in: start, stop, balance, fan, data
 *-----------------------------------------------------------------------------*/

void BMCcomm()
{
  String BMCcommad="";//clearing string for next read
  bmcComFlag=true;
 // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    while (client.available()) {
      char c = client.read();
      if (BMCcommad.length() < 50) {
        BMCcommad += c; //store characters to string 
      }    
    }
    if(BMCcommad.indexOf("cle") >=0) {
      clearFlags();            //clear flags
      BMCcommad="sto";
    }
    sendData((EthernetClient&) client);
//    if(uartPrint)Serial.println(BMCcommad);
    BMCcommdt=0;
    bmcComFlag=false;
    getCommand(BMCcommad);
  }
//  if(!bmcComFlag) Serial.println("Not Communicating to BMC");
}

/*------------------------------------------------------------------------------
 * void getCommand(void)
 * does the command that is sent from the GUI
 *----------------------------------------------------------------------------*/
void getCommand(String input){
  
  
  if(input.indexOf("sto") >=0)//checks for Stop
  {
    stopMode();
  }
  
  else if(input.indexOf("dri") >=0)//checks for start
  {
    driveMode();
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
  
  else if(input.indexOf("ove") >=0)//checks for hdata
  {
    if(uartPrint)Serial.println("Override");
    int sVal=input.indexOf("_");
    int eVal=input.indexOf("_",sVal+1);
    String overrideString=input.substring(sVal+1,eVal);
    flagOverride=overrideString.toInt();
    if(uartPrint)Serial.println(flagOverride,HEX);
    overrideCount=0;
  }
  
  else if(input.indexOf("ign") >=0)//checks for hdata
  {
    if(uartPrint)Serial.println("Ignore");
    if(input.indexOf("pre") >=0){
      flagBMU=~(~flagBMU | (1<<6));
      ignorePres=true;
    }
    else if(input.indexOf("vmi") >=0){
      flagBMU=~(~flagBMU | (1<<13));
      ignoreVall=true;
    }
    else{
      int sVal=input.indexOf("_");
      int eVal=input.indexOf("_",sVal+1);
      String ignoreBMEString=input.substring(sVal+1,eVal);
      int j = ignoreBMEString.toInt();
      if(input.indexOf("vol") >=0){
        flagBMU=~(~flagBMU | (0x1FC0));
        sVal=input.indexOf("_",eVal+1);
        int eVal=input.indexOf("_",sVal+1);
        String ignoreBMEString=input.substring(sVal+1,eVal);
        int i = ignoreBMEString.toInt();
        if (j>0 && j<BMUNum && i>0 && i<cellNum) BME[j-1].ignoreVol[i-1]=true;
      }
      else if(input.indexOf("tem") >=0){
        flagBMU=~(~flagBMU | (0xE));
        sVal=input.indexOf("_",eVal+1);
        int eVal=input.indexOf("_",sVal+1);
        String ignoreBMEString=input.substring(sVal+1,eVal);
        int i = ignoreBMEString.toInt();
        if (j>0 && j<BMUNum && i>0 && i<4) BME[j].ignoreT[i]=true;
      }
      else if(input.indexOf("ite") >=0){
        flagBMU=~(~flagBMU | (0xE));
        if (j>0 && j<BMUNum) BME[j].ignoreiT=true;
      }
      else if(input.indexOf("vre") >=0){
        flagBMU=~(~flagBMU | (1<<12));
        if (j>0 && j<BMUNum) BME[j].ignoreVref=true;
      }
      else if(input.indexOf("vsu") >=0){
        flagBMU=~(~flagBMU | (1<<13));
        if (j>0 && j<BMUNum) BME[j].ignoreVSum=true;
      }
    }
    overrideCount=0;
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
        testBME((BMEdata&)BME[i]);
  //        printOutBME((BMEdata&)BME[i]);
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
  else if(input.indexOf("cle") >=0) {
    clearFlags();            //clear flags
    stopMode();
  }
  else if(input.indexOf("don") >=0) {
    Rtest=true;
    disNum=0;
    if(uartPrint)Serial.println("Resistors On ");
  }
  else if(input.indexOf("dof") >=0) {
    stopBal();
    Rtest=false;
    if(uartPrint)Serial.println("Resistors Off ");
  }
  else if(input.indexOf("help") >=0) {
    if(uartPrint){
      Serial.println("stop");
      Serial.println("drive");
      Serial.println("charge");
      Serial.println("balance_bal2int_");
      Serial.println("override_overflagInt_");
      Serial.println("ignoreSentype_bmeNum_cellnum_");
      Serial.println("clear");
      Serial.println("print to read data");
      Serial.println("fon to turn fan on");
      Serial.println("fof to turn fan off");
      Serial.println("don to turn resistors on");
      Serial.println("dof to turn resistors off");
    }
  }
}

  

/*------------------------------------------------------------------------------
 * void sendData(void)
 * send flag, dt, SOC, bottle voltage, current, cell voltage and temperaturs
 * information out to a client
 *----------------------------------------------------------------------------*/
void sendData(EthernetClient& _Client){
  int i,j;
  
  _Client.print(flagBMU,HEX);
  _Client.print(',');
  _Client.print(BMCcommdt);
  _Client.print(',');
  _Client.print(BMUNum+1);
  _Client.print(',');
  _Client.print(SOC,2);
  _Client.print(',');
  _Client.print(totalVoltage,2);
//  _Client.print(volSum,2);
//  _Client.print(154.14);
  _Client.print(',');
  _Client.print(current,3);
  _Client.print(',');
  _Client.print(minVol,4);
  _Client.print(',');
  _Client.print(maxVol,4);
  _Client.print(',');
  _Client.print(maxTemp,2);
  _Client.print(',');
  _Client.print(pressure,2);
  _Client.print(',');
  _Client.print(presRate,2);
  
//  int testnum=36700+random(-85,85);
  
  for(j=0;j<BMENum;j++){
    for(i=0;i<cellNum;i++){
      _Client.print(',');
      _Client.print(BME[j].vol[i],HEX);
//      _Client.print(testnum,HEX);
    }
  }
  for(j=0;j<BMENum;j++){
    _Client.print(',');
    _Client.print(BME[j].vSum,HEX);
//    _Client.print(testnum,HEX);
  }
  for(j=0;j<BMENum;j++){
    _Client.print(',');
    _Client.print(BME[j].vref2,HEX);
//    _Client.print(testnum,HEX);
  }
  for(j=0;j<BMENum;j++){
    for(i=0;i<4;i++){
      _Client.print(',');
      _Client.print(BME[j].temp[i],HEX);
//      _Client.print(testnum>>1,HEX);
    }
  }
  for(j=0;j<BMENum;j++){
    _Client.print(',');
    _Client.print(BME[j].iTemp,HEX);
//    _Client.print(22000,HEX);
  }
  _Client.print('\n');
}

/*------------------------------------------------------------------------------
 * void sendHeader(void)
 * sand flag, number of BMEs, and variables names out to a client
 *----------------------------------------------------------------------------*/
void sendHeader(EthernetClient& _Client){
  int i,j;
  _Client.print("Flag,dt,BMUNum,SOC,Total_V,Current,minV,maxV,maxT,pressure,ratePressure");
  for(j=1;j<=BMENum;j++){
    for(i=1;i<=cellNum;i++){
      _Client.print(",B");
      _Client.print(j);
      _Client.print("V");
      _Client.print(i);
    }
  }
  for(j=0;j<BMENum;j++){
    _Client.print(",B");
    _Client.print(j);
    _Client.print("Vsum");
  }
  for(j=0;j<BMENum;j++){
    _Client.print(",B");
    _Client.print(j);
    _Client.print("Vref2");
  }
  for(j=1;j<=BMENum;j++){
    for(i=1;i<=4;i++){
      _Client.print(",B");
      _Client.print(j);
      _Client.print("T");
      _Client.print(i);
    }
  }
  for(j=1;j<=BMENum;j++){
    _Client.print(",B");
    _Client.print(j);
    _Client.print("TI");
  }
  _Client.print('\n');
}
