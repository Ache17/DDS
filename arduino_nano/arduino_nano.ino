/* Reference 
 *  AD9833  https://www.analog.com/media/en/technical-documentation/data-sheets/AD9833.pdf
 *  AN-1070 https://www.analog.com/media/en/technical-documentation/application-notes/AN-1070.pdf
 */


#include <SPI.h>

// Registers for waveform type
#define SINE      0x2000
#define SQUARE    0x2028       
#define TRIANGLE  0x2002     

// standard SPI stuff
#define FSYNC 10
#define DATA  11
#define CLK   13


long freq = 10000;
const float clockFreq = 25000000.;
const float scale = 268435456. / clockFreq; 

// phase scale is indepenedent of clock
const float phaseScale = 11.378; 

// need to write to actually work !
void resetAD9833()
{
   writeAD9833(0x100);
   delay(10);
}

/*
 *  Standard SPI transfer of 16 bits
 */
void writeAD9833(int data)
{
  SPI.setDataMode(SPI_MODE2);

  digitalWrite(FSYNC, LOW);
  delayMicroseconds(10);


  SPI.transfer(data >> 8);       
  SPI.transfer(data & 0xff);         

  digitalWrite(FSYNC, HIGH);          
}

void writeAD9833Frequency(float frequency, int Waveform, float phase)
{
  // frequency (float ) to discrete 28 bit value
  long freqData = frequency * scale;

  
  // transfer 2 x 14 bit values ( LSB first)
  int MSB = (int)((freqData >> 14) & 0x3FFF); 
  int LSB = (int)(freqData & 0x3FFF);
  
  // setting as in application note
  LSB |= 0x4000;
  MSB |= 0x4000; 

  // write to FREQ register 0
  writeAD9833(0x2100);   

  // write LSB 
  writeAD9833(LSB);

  // write MSB
  writeAD9833(MSB);  

  // write phase 
  writeAD9833(0xC000 | int(phase * phaseScale));      
  
  // write waveform
  writeAD9833(Waveform);            
}
 
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  SPI.begin();
  delay(50);
  writeAD9833Frequency(freq, SINE, 0);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (Serial.available() > 0)
  {
  float freq = Serial.parseFloat();
  float phase = Serial.parseFloat();
  if (freq != 0) 
    writeAD9833Frequency(freq, SINE, phase);
  }
}
