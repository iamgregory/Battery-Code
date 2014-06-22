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
  
  if(input.indexOf("sto") >=0)//checks for stop
  {
    stopMode();
  }
  
  else if(input.indexOf("dri") >=0)//checks for drive
  {
    driveMode();
  }
  
  else if(input.indexOf("ove") >=0)//checks for override
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
  
  else if(input.indexOf("ign") >=0)//checks for ignore
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
  _Client.print(flagPriority);
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
