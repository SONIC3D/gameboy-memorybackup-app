;SECTION "Module RAM VRAM Operation", ROM0[$0061]

; =============== S U B R O U T I N E =======================================

; 赋值源a中数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
; Parameters:
;   LengthOfByte to fill: bc
;   Value: a
;   DestAddr:[hl]
;
; Pseudo Code:
;   b =    (b+1)%0xFF;
;   c =    (c+1)%0xFF;
;   while (true) {
;     c--;
;     if (c==0)    {
;    b--;
;    if (b==0) {
;      return;
;    } else {
;      c=256;
;    }
;     }
;     [hl++] = a
;   }
;
memFillWith_A:
    inc     b
    inc     c
    jr      .decLow8bit
.valueAssign:
    ldi     [hl], a
.decLow8bit:
    dec     c
    jr      nz, .valueAssign
.decHigh8bit:
    dec     b                       ; 这里b递减后，只要不是0，就会执行到c再递减的地方，
                                    ; 这样c会从0变为0xFF，所以就相当于从高位b借位了
    jr      nz, .valueAssign
    ret
; End of function memFillWith_A

; =============== S U B R O U T I N E =======================================

; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
; Parameters:
;   LengthOfByte to Copy: bc
;   SrcAddr: [hl]
;   DestAddr:[de]
;
; Pseudo Code:
;   b = (b+1)%256;
;   c = (c+1)%256;
;   while (true) {
;     c--;
;     if (c==0) {
;       b--;
;       if (b==0) {
;         return;
;       } else {
;         c=256;
;       }
;     }
;     [de++] = [hl++]
;   }
;
memcpyHL2DE:
    inc     b
    inc     c
    jr      .decLow8bit
.valueAssign:
    ldi     a, [hl]
    ld      [de], a
    inc     de
.decLow8bit:
    dec     c
    jr      nz, .valueAssign
.decHigh8bit:                       ; 这里b递减后，只要不是0，就会执行到c再递减的地方，
    dec     b                       ; 这样c会从0变为0xFF，所以就相当于从高位b借位了
    jr      nz, .valueAssign
    ret
; End of function memcpyHL2DE

; =============== S U B R O U T I N E =======================================

; 赋值源地址数据到目标地址和[目标地址+1]中，总赋值字节数为bc中存放的16bit值，b是高8bit
; Parameters:
;   LengthOfByte to Copy: bc
;   SrcAddr: [hl]
;   DestAddr:[de]
;
; Pseudo Code:
;   b = (b+1)%256;
;   c = (c+1)%256;
;   while (true) {
;     c--;
;     if (c==0) {
;       b--;
;       if (b==0) {
;         return;
;       } else {
;         c=256;
;       }
;     }
;     [de++] = [hl];
;     [de++] = [hl++];
;   }
;
memcpyHL2DE2:
    inc     b
    inc     c
    jr      .decLow8bit
.valueAssign:
    ldi     a, [hl]
    ld      [de], a
    inc     de
    ld      [de], a
    inc     de
.decLow8bit:
    dec     c
    jr      nz, .valueAssign
.decHigh8bit:
    dec     b
    jr      nz, .valueAssign
    ret
; End of function memcpyHL2DE2


; =============== S U B R O U T I N E =======================================

; 赋值源a中数据到目标地址中(目标地址可以是VRAM)，总赋值字节数为bc中存放的16bit值，b是高8bit
; 为了确保目标地址是VRAM地址(8000h-9FFFh)或OAM(FE00h-FE9Fh)地址时也可写入，本函数会确保在LCD的HVBlank发生期间操作。
;
; Parameters:
;   LengthOfByte to fill: bc
;   Value: a
;   DestAddr:[hl]
;
; Pseudo Code:
;   b = (b+1)%256;
;   c = (c+1)%256;
;   while (true) {
;     c--;
;     if (c==0) {
;       b--;
;       if (b==0) {
;         return;
;       } else {
;         c=256;
;       }
;     }
;     disableInterrupt();
;     while(LcdNotDuringHVBlank()) {
;       // empty loop
;     }
;     [hl] = a;
;     enableInterrupt();
;   }
;

vramFillWith_A:
    inc     b
    inc     c
    jr      .decLow8bit
.valueAssignWait:
    push    af
    di
.waitLcdHVBlank:
    ld      a, [byte_FF41]          ; FF41 - STAT    - LCDC Status (R/W)
                                    ;   Bit 6 - LYC=LY Coincidence Interrupt (1=Enable) (Read/Write)
                                    ;   Bit 5 - Mode 2 OAM Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 4 - Mode 1 V-Blank Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 3 - Mode 0 H-Blank Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 2 - Coincidence Flag    (0:LYC<>LY, 1:LYC=LY) (Read Only)
                                    ;   Bit 1-0 -    Mode Flag    (Mode 0-3, see below) (Read Only)
                                    ;        0: During H-Blank
                                    ;        1: During V-Blank
                                    ;        2: During Searching OAM-RAM
                                    ;        3: During Transfering Data to LCD Driver
    and     2
    jr      nz, .waitLcdHVBlank     ; Jump if not in mode 0/1.
                                    ; (In mode 0/1, CPU can access both the display RAM (8000h-9FFFh) and OAM (FE00h-FE9Fh))
.valueAssign:
    pop     af
    ldi     [hl], a
    ei
.decLow8bit:
    dec     c
    jr      nz, .valueAssignWait
.decHigh8bit:
    dec     b
    jr      nz, .valueAssignWait
    ret
; End of function vramFillWith_A


; =============== S U B R O U T I N E =======================================

; 赋值源地址数据到目标地址中(目标地址可以是VRAM)，总赋值字节数为bc中存放的16bit值，b是高8bit
; 为了确保目标地址是VRAM地址(8000h-9FFFh)或OAM(FE00h-FE9Fh)地址时也可写入，本函数会确保在LCD的HVBlank发生期间操作。
;
; Parameters:
;   LengthOfByte to Copy: bc
;   SrcAddr: [hl]
;   DestAddr:[de]
;
; Pseudo Code:
;   b =    (b+1)%256;
;   c =    (c+1)%256;
;   while (true) {
;     c--;
;     if (c==0)    {
;    b--;
;    if (b==0) {
;      return;
;    } else {
;      c=256;
;    }
;     }
;     disableInterrupt();
;     while(LcdNotDuringHVBlank()){
;    // empty loop
;     }
;     [de++] = [hl++]
;     enableInterrupt();
;   }
;

vramcpyHL2DE:
    inc     b
    inc     c
    jr      .decLow8bit
.valueAssignWait:
    di
.waitLcdHVBlank:
    ld      a, [byte_FF41]          ; FF41 - STAT    - LCDC Status (R/W)
                                    ;   Bit 6 - LYC=LY Coincidence Interrupt (1=Enable) (Read/Write)
                                    ;   Bit 5 - Mode 2 OAM Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 4 - Mode 1 V-Blank Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 3 - Mode 0 H-Blank Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 2 - Coincidence Flag    (0:LYC<>LY, 1:LYC=LY) (Read Only)
                                    ;   Bit 1-0 -    Mode Flag    (Mode 0-3, see below) (Read Only)
                                    ;        0: During H-Blank
                                    ;        1: During V-Blank
                                    ;        2: During Searching OAM-RAM
                                    ;        3: During Transfering Data to LCD Driver
    and     2
    jr      nz, .waitLcdHVBlank
.valueAssign:
    ldi     a, [hl]
    ld      [de], a
    ei
    inc     de
.decLow8bit:
    dec     c
    jr      nz, .valueAssignWait
.decHigh8bit:
    dec     b
    jr      nz, .valueAssignWait
    ret
; End of function vramcpyHL2DE

;    ds      $100 - @, 0              ; Alignment
; ---------------------------------------------------------------------------
