#if defined(__aarch64__) && !defined(__arm64__)
#define __arm64__ 1
#endif

#if defined(__APPLE__)
.macro function name
    .text
    .align 4
    .global _\name
    _\name:
.endm
#else
.macro function name
    .text
    .align 4
    .global \name
    .type   \name, %function
    \name:
.endm
#endif
