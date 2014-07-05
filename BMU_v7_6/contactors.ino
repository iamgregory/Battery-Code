
/*------------------------------------------------------------------------------
 * void setContactors(void))
 * sets the contactors on or off
 *----------------------------------------------------------------------------*/
void setContactors(){
  if(contactorsOn) relayOn(); 
  else {
    digitalWrite(relay1, LOW);
    digitalWrite(relay2, LOW);
  }
}
/*------------------------------------------------------------------------------
 * void relayOn(void))
 * puts the relay on?
 *----------------------------------------------------------------------------*/
void relayOn(void){
  digitalWrite(relay1, HIGH);
  if(conOnTime<5) conOnTime++;
  else digitalWrite(relay2, HIGH);

}
