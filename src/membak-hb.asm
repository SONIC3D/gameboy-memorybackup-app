INCLUDE "include/hardware.inc"
INCLUDE "include/membak.inc"

SECTION "ServiceProgramCode", ROM0[$0]
UnUsed_0x0000:
    ret
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

    ds      $40 - @, 0              ; Alignment

VBlank:
    jp      VIntFunc
VIntFunc:                           ; TODO: point to the real VIntFunc
    ds      $48 - @, 0              ; Alignment
; ---------------------------------------------------------------------------
LCD_STAT:
    reti
    ds      $50 - @, 0              ; Alignment
; ---------------------------------------------------------------------------
Timer:
    reti
    ds      $58 - @, 0              ; Alignment
; ---------------------------------------------------------------------------
Serial:
    reti
    ds      $60 - @, 0              ; Alignment
; ---------------------------------------------------------------------------
Joypad:
    reti

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

    ds      $100 - @, 0              ; Alignment
; ---------------------------------------------------------------------------

SECTION "UserProgramCode", ROM0[$100]
; =============== S U B R O U T I N E =======================================
GlobalEntry_100:
    nop
    jp      mainEntry
mainEntry:                          ; TODO: point to the real mainEntry
; End of function GlobalEntry_100

; ---------------------------------------------------------------------------
    ds $150 - @, 0                  ; Make room for logo and header info
; ---------------------------------------------------------------------------

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

; =============== S U B R O U T I N E =======================================

; Write [0xA000]=([0xA000] xor 0xFF) and re-read to test if it is writable.
; Return
;   z:
;     set(z):  A000 is Read/Write supported.
;     clr(nz): A000 failed in Read/Write test.
tst_0xA000_RW:
    ld      a, [A000_CartSramSt]
    cpl
    ld      [A000_CartSramSt], a
    ld      c, a                    ; c = ([0xA000] ^ $FF)
                                    ; [0xA000] = c
    ld      a, [A000_CartSramSt]
    cp      c
    ret     nz                      ; return if (c != [0xA000])
    cpl
    ld      [A000_CartSramSt], a    ; [0xA000] = ([0xA000] ^ $FF)
    xor     a
    ret
; End of function tst_0xA000_RW

; =============== S U B R O U T I N E =======================================

; 似乎是为了检测有多少个SRAM    Bank可用
;
; 整个逻辑是:
;   切到GameCart模式
;   测试第0个SRAM Bank是否可读写
;     不可读写就直接返回，Z Flag清空，为nz
;   备份[0xA000]到[0xC053]
;   写入测试字符串(16个字符)到第0个Bank的[0xA000]
;   循环赋值[0x4000], 从0x01到0x0F(注意0x4000是MBC1/3/5)，所以不支持MBC6)
;     测试SRAM Bank是否可读写，不可读写直接走结束流程
;     循环读取[0xA000]开始到[0xA00F]，比对其中的所有字节是否和测试字符串相符合
;    相符则走到结束流程，不相符则继续测试下一个Bank
;   循环结束
;   结束流程:
;     从[0xC053]恢复备份[0xA000]
;     切到MemoryBackupCart模式
;     把最后一轮的b寄存器值保存到[0xC038]
;
detectBankCnt:
    ld      a, $C0
    ld      [IOPort_0x2000], a      ; 0x2000 I/O Port:
                                    ;   Write
                                    ;    bit7..bit6:
                                    ;      $C0:
                                    ;        Switch to Game Cart    Mode. All access to address like $4000 passed to Game Cart.
                                    ;      $80:
                                    ;        Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;    bit5:
                                    ;      Unknown. Always be 0.
                                    ;    bit4..bit0:
                                    ;      bit4..bit0 of    input value to the 74HC373. Which would    be latched and out put to A18..A14 of 28SF040.
    xor     a
    ld      [byte_4000], a          ; [0x2000] = $C0, [0x4000] = 0; Switch to GameCart SRAM Bank 0
    call    tst_0xA000_RW           ; Write [0xA000]=([0xA000] xor 0xFF) and re-read to test if it is writable.
                                    ; Return
                                    ;   z:
                                    ;    set(z):     A000 is Read/Write supported.
                                    ;    clr(nz): A000 failed in    Read/Write test.
    ld      a, 0
    ld      [byte_C038], a          ; [0xC038] = 0
    ret     nz                      ; Return if 0xA000 is    failed in R/W test.
    ld      hl, A000_CartSramSt
    ld      de, byteArr_C053
    ld      bc, $10                 ; 从卡带SRAM范围[0xA000]备份复制一个字节到[0xC053]
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]
    ld      hl, strSramWriteTst     ; "Mushroom/Kingpin"
    ld      de, A000_CartSramSt
    ld      bc, $10                 ; 从测试字符串复制一个字节到卡带SRAM范围[0xA000]
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]
    ld      b, 1
.nextSramBank:
    ld      a, b
    cp      $10
    jr      nc, .endOfDetect      ; Jump if (b >= $10)
    ld      a, b
    ld      [byte_4000], a          ; [0x2000] = $C0, [0x4000] = b;    //    b=1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
    call    tst_0xA000_RW           ; Write [0xA000]=([0xA000] xor 0xFF) and re-read to test if it is writable.
                                    ; Return
                                    ;   z:
                                    ;    set(z):     A000 is Read/Write supported.
                                    ;    clr(nz): A000 failed in    Read/Write test.
    jr      nz, .endOfDetect         ; Jump if 0xA000 is failed in    R/W test.
    ld      hl, A000_CartSramSt
    ld      de, strSramWriteTst     ; "Mushroom/Kingpin"
.loopCmpTstStr:
    ldi     a, [hl]
    ld      c, a                    ; Read a byte    from [0xA000]
    ld      a, [de]
    inc     de
    cp      c
    jr      nz, .incrSramBank        ; Jump if loop read compare failed
    ld      a, l
    cp      $10
    jr      c, .loopCmpTstStr        ; Jump if (l < $10)
.endOfDetect:
    push    bc                      ; b中是当前正在检查可用性的卡带RAM的编号
                                    ; c无意义(有可能是最后一个检查完毕的字符的值，如果可写入性检测失败，也有可能是其他值)
                                    ; 在0x1D1 pop到af
    xor     a
    ld      [byte_4000], a          ; [0x2000] = $C0, [0x4000] = 0; Switch to GameCart SRAM Bank 0
    ld      hl, byteArr_C053
    ld      de, A000_CartSramSt
    ld      bc, $10                 ; 从[0xC053]恢复备份复制一个字节到卡带SRAM范围[0xA000]
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]
    ld      a, $80 ; '�'
    ld      [IOPort_0x2000], a      ; 0x2000 I/O Port:
                                    ;   Write
                                    ;    bit7..bit6:
                                    ;      $C0:
                                    ;        Switch to Game Cart    Mode. All access to address like $4000 passed to Game Cart.
                                    ;      $80:
                                    ;        Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;    bit5:
                                    ;      Unknown. Always be 0.
                                    ;    bit4..bit0:
                                    ;      bit4..bit0 of    input value to the 74HC373. Which would    be latched and out put to A18..A14 of 28SF040.
    pop     af                      ; Data pushed    at 0x1BB
    ld      [byte_C038], a          ; 存放的是当前插的游戏卡带，最后可用的SRAM的Bank编号数量(最后一个可用Bank的序号+1)
    ret
    ; ---------------------------------------------------------------------------
.incrSramBank:
    inc        b
    jr        .nextSramBank
; End of function detectBankCnt




SECTION "StrData", ROM0[$2848]
strSramWriteTst:    db "Mushroom/Kingpin",0
