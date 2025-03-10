#pragma once

#define PT_SIZE_ON_STACK 288
#define PT_EPC           0
#define PT_RA            8
#define PT_SP            16
#define PT_GP            24
#define PT_TP            32
#define PT_T0            40
#define PT_T1            48
#define PT_T2            56
#define PT_S0            64
#define PT_S1            72
#define PT_A0            80
#define PT_A1            88
#define PT_A2            96
#define PT_A3            104
#define PT_A4            112
#define PT_A5            120
#define PT_A6            128
#define PT_A7            136
#define PT_S2            144
#define PT_S3            152
#define PT_S4            160
#define PT_S5            168
#define PT_S6            176
#define PT_S7            184
#define PT_S8            192
#define PT_S9            200
#define PT_S10           208
#define PT_S11           216
#define PT_T3            224
#define PT_T4            232
#define PT_T5            240
#define PT_T6            248
#define PT_STATUS        256
#define PT_BADADDR       264
#define PT_CAUSE         272
#define PT_ORIG_A0       280

#define CTX_RA  0
#define CTX_SP  8
#define CTX_S0  16
#define CTX_S1  24
#define CTX_S2  32
#define CTX_S3  40
#define CTX_S4  48
#define CTX_S5  56
#define CTX_S6  64
#define CTX_S7  72
#define CTX_S8  80
#define CTX_S9  88
#define CTX_S10 96
#define CTX_S11 104

#define TASK_KERNEL_SP 112
#define TASK_USER_SP   120
