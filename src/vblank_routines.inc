; =============== S U B R O U T I N E =======================================

; 循环halt等待中断发生，直到VBlank发生1次后，本函数返回。
waitVBlankOccur:
    xor     a
    ld      [byte_C037], a          ; 每次VBlank函数发生时，该变量会被设成1
    halt
    nop
    ld      a, [byte_C037]          ; 每次VBlank函数发生时，该变量会被设成1
    or      a
    jr      z, waitVBlankOccur      ; 循环halt等待中断发生，直到VBlank发生1次后，本函数返回。
    ret
; End of function waitVBlankOccur

; =============== S U B R O U T I N E =======================================

; 该函数在VBlank中断发生时被调用。
VIntFunc:
    push    af
    ld      a, 1
    ld      [byte_C037], a          ; 每次VBlank函数发生时，该变量会被设成1
    pop     af
    reti
; End of function VIntFunc
