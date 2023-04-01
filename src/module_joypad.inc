;SECTION "Module Joypad", ROM0[$0001]

; =============== S U B R O U T I N E =======================================

; 读取按键
; 返回值：
;   (以下返回值Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A)
;   a: 本周期按下了的按键(这些键的bit被置为1)
;   b: 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
joypadRead:
    ld      a, $20                  ; ' '
    ld      [FF00_P1_Joypad], a     ; 按键读取模式切换到读取方向键 Bit3/2/1/0 对应 下/上/左/右
    ld      a, [FF00_P1_Joypad]
    ld      a, [FF00_P1_Joypad]
    cpl
    and     $F
    swap    a
    ld      b, a                    ; b中的Bit7/6/5/4现在对应下/上/左/右(按下的按键置1)
    ld      a, $10                  ; 按键读取模式切换到读取按键  Bit3/2/1/0 对应 Start/Select/B/A
    ld      [FF00_P1_Joypad], a
    ld      a, [FF00_P1_Joypad]
    ld      a, [FF00_P1_Joypad]
    ld      a, [FF00_P1_Joypad]
    ld      a, [FF00_P1_Joypad]
    ld      a, [FF00_P1_Joypad]
    ld      a, [FF00_P1_Joypad]
    cpl
    and     $F
    or      b
    ld      b, a                    ; b中的Bit7/6/5/4/3/2/1/0现在对应下/上/左/右/Start/Select/B/A(按下的按键置1)
    ld      a, [byte_C000]          ; a =    上周期的按键
    xor     b                       ; a =    上周期到本周期中，有变化的按键(有变化的按键bit置1)
    and     b                       ; a =    上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
    ld      [byte_C001], a          ; 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
                                    ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A
    ld      a, b
    ld      [byte_C000], a          ; 本周期按下了的按键(这些键的bit被置为1)
                                    ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A
    push    af
    ld      a, $30                  ; '0'
    ld      [FF00_P1_Joypad], a     ; 按键读取模式切换到不读取按键
    ld      a, [byte_C001]          ; 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
                                    ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A
    ld      b, a
    pop     af
    ret
; End of function joypadRead

;    ds      $40 - @, 0              ; Alignment
; ---------------------------------------------------------------------------
