.syntax unified
.thumb
.section .text
.align 2
.global PendSV_Handler
.type PendSV_Handler, %function
.thumb_func
PendSV_Handler:
    @ --- 1. SAVE THE CURRENT TASK CONTEXT ---
    MRS     R0, PSP          @ R0 = read the stack pointer of the currently running task
    @ On Cortex-M0+, we must manually move the pointer back to make room
    @ for R4-R11 (8 registers * 4 bytes = 32 bytes)
    MOVS    R1, #32
    SUBS    R0, R0, R1       @ R0 now points to the bottom of our save area, i.e. R4
    @ First save the real R4-R7 using STMIA (R0 advances by 16)
    STMIA   R0!, {R4-R7}     
    @ Move R8-R11 into R4-R7 so we can save them as well
    MOV     R4, R8
    MOV     R5, R9
    MOV     R6, R10
    MOV     R7, R11
    STMIA   R0!, {R4-R7}     @ R0 advances by another 16 (R0 is back to the original PSP value)
    @ We must store this new stack_ptr (currently R0 - 32) into the TCB
    SUBS    R0, R0, R1       @ R0 goes back down by 32 to point to the beginning (R4 slot)
    LDR     R1, =current_task_tcb_ptr  @ R1 = address of the global variable "currentTCB" (TCB**)
    LDR     R2, [R1]         @ R2 = address of the current task's TCB (TCB*)
    STR     R0, [R2]         @ TCB->stack_ptr = R0 (save task 1's stack pointer)
    @ --- 2. CALL THE C SCHEDULER ---
    PUSH    {R1, LR}         @ Save the address of "currentTCB" and the interrupt LR (0xFFFFFFFD)
    BL      vTaskSwitchContext @ LEGAL C CALL: picks the next task and updates "currentTCB"
    POP     {R1}
    POP     {R3}
    MOV     LR, R3           @ Restore our registers intact after the C code
    @ --- 3. RESTORE THE NEW TASK CONTEXT ---
    LDR     R2, [R1]         @ R2 = address of the NEW TCB (updated by C)
    LDR     R0, [R2]         @ R0 = new TCB->stack_ptr (0x2000xxxx)
    @ Restore logic using R3:
    MOVS    R3, #16          
    ADDS    R0, R0, R3       @ R0 advances by 16 bytes and points to R8
    LDMIA   R0!, {R4-R7}     @ Pop R8-R11 into R4-R7
    MOV     R8, R4          
    MOV     R9, R5
    MOV     R10, R6
    MOV     R11, R7
    MOVS    R3, #32          
    SUBS    R0, R0, R3       @ R0 goes back to the beginning (points to R4)
    LDMIA   R0!, {R4-R7}     @ Pop the real R4-R7. R0 now points to the Hardware Stack!
    @ Position R0 on the Hardware Stack (Base + 32)
    MOVS    R3, #16
    ADDS    R0, R0, R3
    @ No need to go back up by 32 here, because the second LDMIA left R0 exactly
    @ at the start of the Hardware Stack (R0-R3, etc.)
    MSR     PSP, R0          @ Update the PSP with the new task's stack pointer
    BX      LR               @ MAGIC INTERRUPT RETURN!
                             @ Since the CPU is in Handler mode, it reads LR (0xFFFFFFFD),
                             @ automatically unstacks the Hardware Stack from PSP and launches Task 2!
.global SVC_Handler
.type SVC_Handler, %function
.thumb_func
SVC_Handler:
    @ Here we are GUARANTEED to be in privileged mode!
    LDR     R0, =0xE000ED04     @ Address of the ICSR
    LDR     R1, =0x10000000     @ Bit 28 (PENDSVSET)
    STR     R1, [R0]            @ Force the PendSV trigger!
    BX      LR                  @ Exit SVC, PendSV will execute immediately after!