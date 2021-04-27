;=============================================================================;
;                                                                             ;
; File           : llp1-1c.asm                                                ;
; Format         : COM                                                        ;
; Exercise       : Compilation, linking and debugging of assembly programs    ;
; Authors        : Maciej Kopa 230451 Dominik Czerwoniuk 230446, group 1      ;
; Date           : 22.03.20                                                   ;
; Notes          : Program which evaluates the expression: (a-b)*c/d          ;
;                                                                             ;
;=============================================================================;

				.MODEL TINY

MyCode          SEGMENT

                ORG    100h
                ASSUME   CS:MyCode, DS:MyCode, SS:MyCode

Start:									;data segment
                jmp 	Beginning

a               DB      20
b               DB      10
c               DB     100
d               DB       5
Result          DB      ?		
				
Beginning:								;code segment
                mov     al, a			;14h -> al
				mov 	bl, b			;0Ah -> bl
                sub     al, bl			;14h-0Ah=0Ah, 0Ah -> al
                mul     c				;al*64h=0E38h, 0E38h -> ax
                div     d				;ax/05h=C8h -> ax (al - quotient, ah - reminder)
				
				mov 	[Result], al 	;send final value to Result, C8h -> Result




                mov     ax, 4C00h       ; end the program and exit to DOS                                        
                int     21h             ; with no errors repored and code 0

MyCode          ENDS

                END     Start