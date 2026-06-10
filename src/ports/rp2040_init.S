@ SPDX-License-Identifier: MIT
@ Copyright (c) 2026 Ferdinand ATI
@ VectOS — Lightweight preemptive RTOS for RP2040
@ https://github.com/Ferdinaelectro1/VectOS

.syntax unified    @ Mandatory for using modern ARM syntax
.thumb             @ Mandatory to compile in Thumb mode for the Cortex-M0+
.section .text     @ Indicates that what follows is executable code
.align 2           @ Aligns code on 4-byte boundaries
.global start_first_task
.type start_first_task, %function
.thumb_func        @ Tells the linker that this function runs in Thumb mode
start_first_task:
    @ Stop the CPU right here to validate
    @B .                       @ If you uncomment this line and the LED turns on: ASM is reached!
    @ R0 holds the first argument passed from C,
    @ which is the address of 'currentTCB' (a pointer to a pointer: TCB**)
    LDR     R1, [R0]        @ R1 = currentTCB (address of task 1's TCB)
    LDR     R0, [R1]        @ R0 = currentTCB->stack_ptr (retrieves our 0x200017A8!)
    @ --- SOFTWARE CONTEXT RESTORE (R4-R11) ---
    @ On Cortex-M0+, LDMIA can only pop R0-R7.
    @ We work around this by first popping R4-R7, then loading R8-R11 into R4-R7 to move them.
    MOVS   R1, #16          @ Load immediate 16 into R1 (to skip the 8 registers R0-R3, R12, LR, PC, xPSR)
    ADDS   R0, R0, R1       @ Advance by 16 bytes to skip the 4 R4-R7 slots (which hold the fake R8-R11 values)
    LDMIA   R0!, {R4-R7}    @ Pop R4, R5, R6, R7 from the task stack, which contain the R8-R11 values
    MOV     R8, R4          @ Move those values into their actual high registers
    MOV     R9, R5
    MOV     R10, R6
    MOV     R11, R7
    MOVS    R1, #32         @ Temporarily load immediate 32 into R1
    SUBS    R0, R0, R1      @ Go back 32 bytes to return to the beginning of the task stack at R4,
                            @ because LDMIA also advanced R0 by 16 during the previous pop
    LDMIA   R0!, {R4-R7}    @ Pop the next 4 registers (R4-R7) from the task stack
    MOVS   R1, #16          @ Load immediate 16 into R1
    ADDS   R0, R0, R1       @ Advance by 16 bytes past R4-R7, landing at R0 in the hardware stack
    @ At this point (X+32), the hardware stack contains: [R0, R1, R2, R3, R12, LR, PC, xPSR]
    @ PC is located 24 bytes above the address currently pointed to by R0.
    MOVS    R1, #24         @ Offset to reach the PC slot
    ADDS    R0, R0, R1      @ R0 now points exactly at [PC] in the stack
    LDMIA   R0!, {R1}       @ R1 = load the PC value (holds the address of the task function to start)
                            @ R0 now points at [xPSR] (X+28 from the start of the HW stack, i.e. X+60)
    MOVS    R2, #4          @ Skip the xPSR slot (4 bytes) to fully drain the stack
    ADDS    R0, R0, R2      @ R0 now points just past the task stack (stack is empty)
    @ --- CPU REGISTER CONFIGURATION ---
    MSR     PSP, R0         @ Update the CPU PSP with the current stack address (now pointing past the stack)
    MOVS    R0, #2          @ Load 0x02 to indicate the CPU must use PSP as stack pointer, not MSP
    MSR     CONTROL, R0     @ Write to the CONTROL register: force the CPU to use PSP
    ISB                     @ Instruction Synchronization Barrier (flush the CPU pipeline to apply the change)
    BX      R1              @ Jump directly to the Task 1 address stored in R1!
                            @ The CPU automatically unstacks R0-R3, R12, LR, PC, xPSR from the PSP