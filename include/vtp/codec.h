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
