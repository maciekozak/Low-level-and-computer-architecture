;=============================================================================;
;                                                                             ;
; File           : llp1-1e.asm                                                ;
; Format         : EXE                                                        ;
; Exercise       : Compilation, linking and debugging of assembly programs    ;
; Authors        : Maciej Kopa 230451 Dominik Czerwoniuk 230446, group 1	  ;
; Date           : 22.03.2020                                                 ;
; Notes          : Program which finds the largest number in a table 		  ;
;                                                                             ;
;=============================================================================;

                .MODEL  SMALL

.data


TABLEN          EQU     12 																;12 values in array - length of the array
Tab				DB      01h, 02h, 00h, 10h, 12h, 33h, 15h, 09h, 11h, 08h, 0Ah, 00h 		;array "Tab" declaration
Largest         DB      ?																;result - largest number in the array


.code

Start:
                mov     ax, @data			;DS register update
                mov     ds, ax

				mov 	si, 00h				;setting counter as = 0
				mov     al, Tab[00h]		;sending 1st value in Tab to al
MyLoop:
				inc 	si					;increment counter by 1
                cmp     si, TABLEN			;check if we passed all the elements in the table
				jz		Ending				;jump to the end if previous comparison is true (checks flags)
				cmp		Tab[si], al			;check if next variable in the array is bigger than this in al						
                jl		MyLoop				;"jump if smaller", if previous codition is not met we jump straight to the next irritation of the loop
                mov 	al, Tab[si]			;if previous condition (line 34) is met we save the value from Tab to al (updating the biggest number)
				jmp 	MyLoop				;go back to loop


Ending:         
				mov     Largest, al			;save al to Largest 

                mov     ax, 4C00h 			;quit program with code 0
                int     21h


.stack 

                DB      100h DUP (?)


                END  Start

