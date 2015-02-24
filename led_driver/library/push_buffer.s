;**********************************************
; Things
;**********************************************
PORTF 	EQU 0x400043FC
HIGH 	EQU 0x0004
LOW		EQU	0x0000

	export push_buffer
		
;**********************************************
; SRAM
;**********************************************
    AREA    SRAM, READWRITE
    align
        
;**********************************************
; Constant Variables (FLASH) Segment
;**********************************************
    AREA    FLASH, CODE, READONLY
    align
	
	
push_buffer PROC
	PUSH {R5-R10}
	
	MOV		R2, #HIGH
	MOV 	R3, #LOW
	
	MOV32	R4, PORTF
	MOV 	R5, #24
	MUL		R1, R1, R5
	MOV		R5, #0

BIT_LOOP
	MOV		R5, #0
	LDRB	R6, [R0]
	CMP		R6, #0
	MOVGT	R5, #1
	
	B		PUSH_BIT
BIT_DONE
	ADD		R0, R0, #1
	SUBS	R1, R1, #1
	BGT		BIT_LOOP
	B		DONE
	
	
PUSH_BIT
	STR		R2, [R4]
	
	; 18 NOPs plus CMP/STREQ = 20 instruction (0.25us)
	
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	CMP		R5, #0		; Drop if zero else stay high
	STREQ	R3, [R4]
	
	; 27 NOPs plus STR = 28 insr (0.35 us)
	
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	STR		R3, [R4]	; Always drop here
	
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	NOP
	B		BIT_DONE
	
DONE
	POP		{R5-R10}

	BX		LR
	
	ENDP
    align
    END 
