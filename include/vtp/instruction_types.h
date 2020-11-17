#ifndef LIBVTP_INSTRUCTION_TYPES_H
#define LIBVTP_INSTRUCTION_TYPES_H

enum _VTPInstructionCode {
    VTP_INST_INCREMENT_TIME = 0,
    VTP_INST_SET_FREQUENCY = 1,
    VTP_INST_SET_AMPLITUDE = 2
};
typedef enum _VTPInstructionCode VTPInstructionCode;

struct _VTPInstructionParamsA {
    unsigned long parameter_a;
};
typedef struct _VTPInstructionParamsA VTPInstructionParamsA;

struct _VTPInstructionParamsB {
    unsigned char channel_select;
    unsigned int time_offset;
    unsigned int parameter_a;
};
typedef struct _VTPInstructionParamsB VTPInstructionParamsB;


union _VTPInstructionParamsV1 {
    VTPInstructionParamsA format_a;
    VTPInstructionParamsB format_b;
};
typedef union _VTPInstructionParamsV1 VTPInstructionParamsV1;


struct _VTPInstructionV1 {
    VTPInstructionCode code;
    VTPInstructionParamsV1 params;
};
typedef struct _VTPInstructionV1 VTPInstructionV1;

typedef unsigned long VTPInstructionWord;

#endif