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
 * This sketch showcases the emulation of a sampling-like interface for reading VTP streams.
 *
 * VTP is a format with incremental updates between states, whereas in
 * sampling you get an absolute state update for each step.
 * Using the function vtp_fold_until_v1, one can emulate a sampling-like
 * interface by calculating the state changes until a given point in time.
 *
 * This sketch will play a sample VTP pattern and output state snapshots as CSV via Serial.
 *
 * Feel free to try different sampling rates and more complex patterns.
 */
 
#define N_CHANNELS (4)
#define N_INSTRUCTIONS (8)
#define SAMPLING_RATE_HZ (20)
#define LIMIT_MILLISECONDS (3000)

#define STEP_MILLISECONDS (1000/SAMPLING_RATE_HZ)
#define SEPARATOR ", "

#include <vtp_fold.h>

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

VTPInstructionV1 decodedInstructions[N_INSTRUCTIONS];


VTPAccumulatorV1 accumulator;
const VTPInstructionV1* currentInstruction;
unsigned long timeElapsed;


void setup() {
  static unsigned int amplitudes[N_CHANNELS], frequencies[N_CHANNELS];
  
  Serial.begin(57600);
  while(!Serial);
  
  // Initializing the accumulator for N_CHANNELS, with 0ms elapsed and amplitudes and frequencies all set to zero
  accumulator.n_channels = N_CHANNELS;
  accumulator.amplitudes = amplitudes;
  accumulator.frequencies = frequencies;
  accumulator.milliseconds_elapsed = 0; //Please note that this stands for the cumulative amount of milliseconds of all VTP Instructions that were previously processed, not for the actual current point in time.
  
  memset(amplitudes, 0, N_CHANNELS);
  memset(frequencies, 0, N_CHANNELS);
  
  // Initializing status variables
  currentInstruction = decodedInstructions;
  timeElapsed = 0;
  
  // Decode instructions
  if (vtp_decode_instructions_v1(instructions, decodedInstructions, N_INSTRUCTIONS) != VTP_OK) {
    Serial.println("Invalid instructions - could not decode!");
    while(1);
  }
  
  printStatusTableHeader();
}

void loop() {  
  if (timeElapsed >= LIMIT_MILLISECONDS) {
    Serial.println("Done");
    while(1);
  }
  
  sample();
  
  printStatusTableRow();  
  
  delay(STEP_MILLISECONDS);
  timeElapsed += STEP_MILLISECONDS;
}



// This function acts as if we would request the next sample for a pattern,
// while in reality it calculates the current pattern state incrementally
// from the remaining VTP instructions
void sample() {
  VTPError error;
  
  size_t nProcessed;
  if ((error = vtp_fold_until_v1(&accumulator, currentInstruction, decodedInstructions + N_INSTRUCTIONS - currentInstruction, timeElapsed, &nProcessed)) != VTP_OK) {
    Serial.print("Error while folding - Code ");
    Serial.print(error);
    Serial.print("\n");
  }  
  currentInstruction += nProcessed;  
}

void printStatusTableHeader() {
  int i;
  
  Serial.print("time elapsed    ");
  Serial.print(SEPARATOR);
  for (i=0; i < accumulator.n_channels; i++) {
    Serial.print("amp ch");
    Serial.print(i);
    Serial.print(SEPARATOR);
  }
  for (i=0; i < accumulator.n_channels; i++) {
    Serial.print("frq ch");
    Serial.print(i);
    Serial.print(SEPARATOR);
  }
  
  Serial.print("\n");
}

void printStatusTableRow() {
  char time_buffer[17];
  
  sprintf(time_buffer, "%14lums", timeElapsed);
  
  Serial.print(time_buffer);
  Serial.print(SEPARATOR);
  
  printAccumulatorArray(accumulator.amplitudes);  
  printAccumulatorArray(accumulator.frequencies);
  
  Serial.print("\n");
}

void printAccumulatorArray(unsigned int* array) {
  char value_buffer[8];
  for (int i=0; i < accumulator.n_channels; i++) {
    sprintf(value_buffer, "%7u", array[i]);
    Serial.print(value_buffer);
    Serial.print(SEPARATOR);
  }
}
