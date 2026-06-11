@ SPDX-License-Identifier: MIT
@ Copyright (c) 2026 Ferdinand ATI
@ VectOS — Lightweight preemptive RTOS for RP2040

.syntax unified    @ Mandatory for using modern ARM syntax
.thumb             @ Mandatory to compile in Thumb mode for the Cortex-M0+
.section .text     @ Indicates that what follows is executable code
.align 2           @ Aligns code on 4-byte boundaries
.global start_first_task
.type start_first_task, %function
.thumb_func        @ Tells the linker that this function runs in Thumb mode

start_first_task:
    @ R0 initially holds the address of &current_task_tcb_ptr (TCB**)
    LDR     R1, [R0]        @ R1 = current_task_tcb_ptr (Address of task 1's TCB)
    LDR     R0, [R1]        @ R0 = current_task_tcb_ptr->stack_ptr (Top of stack pointing to R4)

    @ --- EXTRACT TASK PARAMETER ---
    @ In task_init, the task argument (virtual R0) is placed right after the software context.
    @ It is located exactly at an offset of 32 bytes (8 registers x 4 bytes: R4-R11).
    MOVS    R2, #32
    LDR     R3, [R0, R2]    @ R3 = Read task argument from stack without modifying R0 pointer.
                            @ Keep this value safe in R3 for now.

    @ --- RESTORE SOFTWARE CONTEXT (R4-R11) ---
    @ On Cortex-M0+, LDMIA can only pop low registers (R0-R7).
    @ We bypass this by jumping ahead to pop high registers into R4-R7, then moving them.
    MOVS    R1, #16          
    ADDS    R0, R0, R1       
    LDMIA   R0!, {R4-R7}    @ Pop values meant for R8-R11 from the task stack
    MOV     R8, R4          @ Move them into their actual high registers
    MOV     R9, R5
    MOV     R10, R6
    MOV     R11, R7
    
    MOVS    R1, #32         
    SUBS    R0, R0, R1      
    LDMIA   R0!, {R4-R7}    @ Go back and pop low registers R4-R7 from the task stack
    
    MOVS    R1, #16          
    ADDS    R0, R0, R1      @ R0 now points to the beginning of the hardware stack frame (R0 slot)

    @ --- CONFIGURE PROCESS STACK POINTER (PSP) ---
    @ Since we execute a direct branch (BX) instead of an exception return, 
    @ we simulate a completely empty stack by pointing PSP just past the xPSR slot (+32 bytes).
    MOVS    R1, #32
    ADDS    R2, R0, R1      @ R2 = Final top-of-stack address (empty stack frame boundary)
    MSR     PSP, R2         @ Initialize the CPU PSP register with this stack address

    @ --- FETCH TASK ENTRY POINT (PC) ---
    @ The Program Counter (PC) slot is located 24 bytes above the current hardware stack base.
    MOVS    R1, #24
    LDR     R1, [R0, R1]    @ R1 = Loaded PC value (Address of the task function)

    @ --- LOAD TASK ARGUMENT INTO CPU R0 ---
    MOV     R0, R3          @ Move the stored argument from R3 into the actual CPU R0 register.
                            @ The task function will now naturally receive it as its first parameter.

    @ --- SWITCH CPU PRIVILEGE / STACK MODE ---
    MOVS    R2, #2          
    MSR     CONTROL, R2     @ Write to CONTROL register: Force the CPU to switch and use PSP
    ISB                     @ Instruction Synchronization Barrier: Flush pipeline to apply changes

    BX      R1              @ Branch directly to the task function! R0 contains the parameter.