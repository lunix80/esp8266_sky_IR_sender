
void custom_delay_usec(unsigned long uSecs) {
  if (uSecs > 4) {
    unsigned long start = micros();
    unsigned long endMicros = start + uSecs - 4;
    if (endMicros < start) { // Check if overflow
      while ( micros() > start ) {}
    }
    while ( micros() < endMicros ) {}
  }
}

void sendNibble(uint8_t nibble){
   mark(XMP_LEADER);
   space(XMP_NIBBLE_PREFIX + (nibble * XMP_UNIT));  
}

void  sendRaw (unsigned int buf[],  unsigned int len){
  
  for (int i = 0;  i < len;  i++) {
    mark(XMP_LEADER);
    space(XMP_NIBBLE_PREFIX + (buf[i] * XMP_UNIT));  
  }
  space(0);  // Always end with the LED off
}

void mark(unsigned int time) {
   long beginning = micros();
   while(micros() - beginning < time){
   digitalWrite(IRpin, LOW);// NORMAL
   delayMicroseconds(halfPeriodicTime);
   digitalWrite(IRpin, HIGH);
   delayMicroseconds(halfPeriodicTime); //38 kHz -> T = 26.31 microsec (periodic time), half of it is 13
  }
}

void space(unsigned long  time) {
  digitalWrite(IRpin, HIGH); // REVERSE
  if (time > 0) custom_delay_usec(time);
}


