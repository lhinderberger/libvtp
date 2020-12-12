/*
 * Copyright 2020 Lucas Hinderberger
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *
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
  unsigned char buffer[4];
  VTPInstructionWord encodedInstruction;
  
  vtp_encode_instruction_v1(instruction, &encodedInstruction); //Note: Error checking was omitted
  vtp_write_instruction_words(1, &encodedInstruction, buffer);
  
  Serial.write(buffer, 4);
  
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
