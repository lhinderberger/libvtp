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
 * This sketch decodes VTP Binary Instruction Words on the Arduino
 * and prints their meaning to the serial output.
 */
 
#define N_INSTRUCTIONS (8)

#include <vtp_codec.h>

/*
 * Corresponding VTP Assembly Code:
 *
 * freq ch* 234
 * amp ch* 123
 * freq ch2 345
 *
 * freq +50ms ch2 456
 * freq ch1 789
 *
 * time +2000ms
 * amp ch* 234
 * freq ch2 567
 */
const VTPInstructionWord instructions[N_INSTRUCTIONS] = {
  0x100000ea, 0x2000007b, 0x10200159, 0x1020c9c8,
  0x10100315, 0x000007d0, 0x200000ea, 0x10200237
};


bool instructionsPrinted;

void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  instructionsPrinted = false;
}

void loop() {
  if (!instructionsPrinted) {
    printInstructions();
    instructionsPrinted = true;
  }
}

void printInstructions() {
  VTPInstructionV1 instruction;
  
  for (size_t i = 0; i < N_INSTRUCTIONS; i++) {    
    VTPError error = vtp_decode_instruction_v1(instructions[i], &instruction);
    if (error != VTP_OK) {
      Serial.print("Error decoding instruction word: Code ");
      Serial.print(error);
      Serial.print("\n\n");
    }
    else {
      switch (instruction.code) {
        case VTP_INST_INCREMENT_TIME:
          Serial.print("Increment Time\n");
          printParametersA(&instruction.params.format_a);
          break;
        case VTP_INST_SET_AMPLITUDE:
          Serial.print("Set Amplitude\n");
          printParametersB(&instruction.params.format_b);
          break;
        case VTP_INST_SET_FREQUENCY:
          Serial.print("Set Frequency\n");
          printParametersB(&instruction.params.format_b);
          break;        
      }
    }
  }
}

void printParametersA(VTPInstructionParamsA* params) {
  Serial.print("Parameter A: ");
  Serial.print(params->parameter_a);
  Serial.print("\n\n");
}

void printParametersB(VTPInstructionParamsB* params) {
  if (params->time_offset > 0) {
    Serial.print("Time offset: +");
    Serial.print(params->time_offset);
    Serial.print("ms\n");
  }
  
  Serial.print("Channel: ");
  if (params->channel_select == 0) {
    Serial.print("all");
  }
  else {
    Serial.print(params->channel_select);
  }
  Serial.print("\n");
  
  Serial.print("Parameter A: ");
  Serial.print(params->parameter_a);
  Serial.print("\n\n");  
}
