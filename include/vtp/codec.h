#ifndef LIBVTP_CODEC_H
#define LIBVTP_CODEC_H

#include <stddef.h>
#include <vtp/error.h>
#include <vtp/instruction_types.h>

VTPError vtp_decode_instruction_v1(VTPInstructionWord instruction, VTPInstructionV1* out);
VTPError vtp_encode_instruction_v1(const VTPInstructionV1* instruction, VTPInstructionWord* out);

VTPError vtp_decode_instructions_v1(const VTPInstructionWord instructions[], VTPInstructionV1 out[], size_t n);
VTPError vtp_encode_instructions_v1(const VTPInstructionV1 instructions[], VTPInstructionWord out[], size_t n);

#endif
