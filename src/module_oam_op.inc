;SECTION "Module OAM Operation", ROM0[$0150]

; =============== S U B R O U T I N E =======================================

; Fill [0xFE00-0xFE9F] OAM address range with data 0x00.
clearOAM:
    ld      hl, $FE00
.nextByte:
    xor     a
    ld      [hl], a
    inc     l
    ld      a, l
    cp      $A0                     ; OAM range is $FE00-$FE9F
    jr      c,  .nextByte
    ret
; End of function clearOAM
