/*
 * This sketch reads N_POTIS potentiometers and
 * outputs their values as the amplitudes of
 * VTP channels via serial (binary VTP).
 * 
 * That binary stream can for example be recorded using
 * a terminal software such as moserial and disassembled
 * using the vtp-disassemble tool.
 */
 
#include <vtp_codec.h>
 
#define N_POTIS (2)
#define POTI_PINS { A1, A2 }
#define SAMPLING_DELAY_MS (50)
#define POTI_HYSTERESIS (5)

const int pins[N_POTIS] = POTI_PINS;

int amplitudes[N_POTIS];
unsigned long currentTime, timeOfLastWrite;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  currentTime = timeOfLastWrite = 0;
}

void loop() {
  for (int i = 0; i < N_POTIS; i++) {
    int potiValue = analogRead(pins[i]);
    if(!areWithinHysteresis(amplitudes[i], potiValue, POTI_HYSTERESIS)) {
      amplitudes[i] = potiValue;
      writeVTPAmplitudeToSerial(i+1, amplitudes[i]);
    }
  }
  
  delay(SAMPLING_DELAY_MS);
  currentTime += SAMPLING_DELAY_MS;
}

bool areWithinHysteresis(int x, int y, int hysteresis) {
  return abs(x-y) <= hysteresis;
}

void encodeAndWriteVTPToSerial(const VTPInstructionV1* instruction) {  
  VTPInstructionWord encodedInstruction;
  
  vtp_encode_instruction_v1(instruction, &encodedInstruction); //Note: Error checking was omitted
  
  Serial.write((encodedInstruction >> 24) & 0xFF);
  Serial.write((encodedInstruction >> 16) & 0xFF);
  Serial.write((encodedInstruction >> 8) & 0xFF);
  Serial.write(encodedInstruction & 0xFF);
  
  timeOfLastWrite = currentTime;
}

void writeVTPAmplitudeToSerial(int channel, int amplitude) {
  if (currentTime - timeOfLastWrite > 1023)
    writeVTPInrementTimeToSerial();
  
  VTPInstructionV1 instruction;
  
  instruction.code = VTP_INST_SET_AMPLITUDE;
  instruction.params.format_b.channel_select = channel;
  instruction.params.format_b.time_offset = currentTime - timeOfLastWrite;
  instruction.params.format_b.parameter_a = amplitude;
  
  encodeAndWriteVTPToSerial(&instruction);
}

void writeVTPInrementTimeToSerial() {
  VTPInstructionV1 instruction;
  
  instruction.code = VTP_INST_INCREMENT_TIME;
  instruction.params.format_a.parameter_a = currentTime - timeOfLastWrite;
  
  encodeAndWriteVTPToSerial(&instruction);
}
