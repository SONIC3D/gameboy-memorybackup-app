; =============== S U B R O U T I N E =======================================


playSfxError:
    ld      a, [C775_OptVal03]      ; Option选项值，是否启用声音，非0时，表示启用声音。
                                    ; 初始化为0xFF，即启用声音。
    or      a
    ret     z

.soundFxIsOn:
    ld      a, $80 ; '�'
    ld      [byte_FF26], a          ; FF26 - NR52 - Sound on/off
    ld      a, $8F ; '�'
    ld      [byte_FF26], a          ; FF26 - NR52 - Sound on/off
    ld      a, $FF
    ld      [byte_FF25], a          ; FF25 - NR51 - Selection of Sound output terminal (R/W)
    ld      a, $77 ; 'w'
    ld      [byte_FF24], a          ; FF24 - NR50 - Channel control / ON-OFF / Volume (R/W)
    ld      a, 0
    ld      [byte_FF10], a          ; FF10 - NR10 - Channel 1 Sweep register (R/W)
    ld      a, $57 ; 'W'
    ld      [byte_FF11], a          ; FF11 - NR11 - Channel 1 Sound length/Wave pattern duty (R/W)
    ld      a, $F2 ; '�'
    ld      [byte_FF12], a          ; FF12 - NR12 - Channel 1 Volume Envelope (R/W)
    ld      a, $35 ; '5'
    ld      [byte_FF13], a          ; FF13 - NR13 - Channel 1 Frequency lo (Write Only)
    ld      a, $85 ; '�'
    ld      [byte_FF14], a          ; FF14 - NR14 - Channel 1 Frequency hi (R/W)
    ret
; End of function playSfxError


; =============== S U B R O U T I N E =======================================


playSfxOK:
    ld      a, [C775_OptVal03]      ; Option选项值，是否启用声音，非0时，表示启用声音。
                                    ; 初始化为0xFF，即启用声音。
    or      a
    ret     z

.soundFxIsOn:
    ld      a, $80 ; '�'
    ld      [byte_FF26], a          ; FF26 - NR52 - Sound on/off
    ld      a, $8F ; '�'
    ld      [byte_FF26], a          ; FF26 - NR52 - Sound on/off
    ld      a, $FF
    ld      [byte_FF25], a          ; FF25 - NR51 - Selection of Sound output terminal (R/W)
    ld      a, $77 ; 'w'
    ld      [byte_FF24], a          ; FF24 - NR50 - Channel control / ON-OFF / Volume (R/W)
    ld      a, 0
    ld      [byte_FF10], a          ; FF10 - NR10 - Channel 1 Sweep register (R/W)
    ld      a, $57 ; 'W'
    ld      [byte_FF11], a          ; FF11 - NR11 - Channel 1 Sound length/Wave pattern duty (R/W)
    ld      a, $F2 ; '�'
    ld      [byte_FF12], a          ; FF12 - NR12 - Channel 1 Volume Envelope (R/W)
    ld      a, $70 ; 'p'
    ld      [byte_FF13], a          ; FF13 - NR13 - Channel 1 Frequency lo (Write Only)
    ld      a, $87 ; '�'
    ld      [byte_FF14], a          ; FF14 - NR14 - Channel 1 Frequency hi (R/W)
    ret
; End of function playSfxOK
