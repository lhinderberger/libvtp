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
 */

#ifndef LIBVTP_INSTRUCTION_TYPES_H
#define LIBVTP_INSTRUCTION_TYPES_H

/*
 * Instruction types in this file are defined according to the VTPv1 specification draft
 */

enum eVTPInstructionCode {
    VTP_INST_INCREMENT_TIME = 0,
    VTP_INST_SET_FREQUENCY = 1,
    VTP_INST_SET_AMPLITUDE = 2
};
typedef enum eVTPInstructionCode VTPInstructionCode;

struct sVTPInstructionParamsA {
    unsigned long parameter_a;
};
typedef struct sVTPInstructionParamsA VTPInstructionParamsA;

struct sVTPInstructionParamsB {
    unsigned char channel_select;
    unsigned int time_offset;
    unsigned int parameter_a;
};
typedef struct sVTPInstructionParamsB VTPInstructionParamsB;


union uVTPInstructionParamsV1 {
    VTPInstructionParamsA format_a;
    VTPInstructionParamsB format_b;
};
typedef union uVTPInstructionParamsV1 VTPInstructionParamsV1;


struct sVTPInstructionV1 {
    VTPInstructionCode code;
    VTPInstructionParamsV1 params;
};
typedef struct sVTPInstructionV1 VTPInstructionV1;

typedef unsigned long VTPInstructionWord;

#endif
