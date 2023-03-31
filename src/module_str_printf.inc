; =============== S U B R O U T I N E =======================================

; 等待LCD H/V Blank，然后将a中的0-15数转为FontTile中十六进制数字字符0-9和A-F的编号，
; 赋给目标地址de(可以是VRAM)，并给de增1。最后光标右移1格。
;
; 可能用于输入备份文件名时，屏幕底部的输入框更新。
; Input:
;   a: Only accept low 4-bit as the input number value.
;   de: Target address, can be VRAM.
; Output:
;   de++;
;   [0xC00E]++;

printNumberChar:
    push    af                      ; popped at 0x610
    and     $F
    cp      $A
    jr      nc, .greatEquThan10

.lessThan10:                        ; 用于对应字库中0-9字符的Font tile
    add     a, $30 ; '0'
    jr      .printToScrWait
; ---------------------------------------------------------------------------

.greatEquThan10:
    add     a, $37 ; '7'            ; 用于对应字库中A-F字符的Font tile

.printToScrWait:
    push    af                      ; popped at 0x606

.waitLcdHVBlank:
    ld      a, [byte_FF41]          ; FF41 - STAT - LCDC Status (R/W)
                                    ;   Bit 6 - LYC=LY Coincidence Interrupt (1=Enable) (Read/Write)
                                    ;   Bit 5 - Mode 2 OAM Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 4 - Mode 1 V-Blank Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 3 - Mode 0 H-Blank Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 2 - Coincidence Flag    (0:LYC<>LY, 1:LYC=LY) (Read Only)
                                    ;   Bit 1-0 - Mode Flag (Mode 0-3, see below) (Read Only)
                                    ;        0: During H-Blank
                                    ;        1: During V-Blank
                                    ;        2: During Searching OAM-RAM
                                    ;        3: During Transfering Data to LCD Driver
    and     2
    jr      nz, .waitLcdHVBlank

.printToScr:                        ; pushed at 0x5FF
    pop     af
    ld      [de], a
    inc     de

.updtPrintCursor:                   ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
    ld      a, [byte_C00E]          ; 具体参考函数:
                                    ;   0x691 strPrintf
    inc     a
    ld      [byte_C00E], a          ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    pop     af                      ; pushed at 0x5F2
    ret
; End of function printNumberChar


; =============== S U B R O U T I N E =======================================

; 读取bc所指内存内的值，将这个字节对应的2位数的16进制数输出到屏幕
; 并自增bc值，使其指向下一个字节
;
; Input:
;   bc: 待打印的字节数据所在的内存地址。
;   de: 待输出到屏幕的BG    Map的VRAM地址。

printHexChar:
    ld      a, [bc]
    inc     bc
            swap    a
    call    printNumberChar         ; 等待LCD H/V Blank，然后将a中的0-15数转为FontTile中十六进制数字字符0-9和A-F的编号，
                                    ; 赋给目标地址de(可以是VRAM)，并给de增1。最后光标右移1格。
                                    ;
                                    ; 可能用于输入备份文件名时，屏幕底部的输入框更新。
                                    ; Input:
                                    ;   a: Only accept low 4-bit as the input number value.
                                    ;   de: Target address, can be VRAM.
                                    ; Output:
                                    ;   de++;
                                    ;   [0xC00E]++;
    swap    a
    call    printNumberChar         ; 等待LCD H/V Blank，然后将a中的0-15数转为FontTile中十六进制数字字符0-9和A-F的编号，
                                    ; 赋给目标地址de(可以是VRAM)，并给de增1。最后光标右移1格。
                                    ;
                                    ; 可能用于输入备份文件名时，屏幕底部的输入框更新。
                                    ; Input:
                                    ;   a: Only accept low 4-bit as the input number value.
                                    ;   de: Target address, can be VRAM.
                                    ; Output:
                                    ;   de++;
                                    ;   [0xC00E]++;
    ret
; End of function printHexChar


; =============== S U B R O U T I N E =======================================

; 读取bc所指内存内的2个字节值，将这2个字节对应的2个2位数的16进制数输出到屏幕
; Input:
;   bc:    待打印的2字节数据所在的起始内存地址。
;   de:    待输出到屏幕的BG    Map的起始VRAM地址。

print2HexChar:
    call    printHexChar            ; 读取bc所指内存内的值，将这个字节对应的2位数的16进制数输出到屏幕
                                    ; 并自增bc值，使其指向下一个字节
                                    ;
                                    ; Input:
                                    ;   bc: 待打印的字节数据所在的内存地址。
                                    ;   de: 待输出到屏幕的BG Map的VRAM地址。
    call    printHexChar            ; 读取bc所指内存内的值，将这个字节对应的2位数的16进制数输出到屏幕
                                    ; 并自增bc值，使其指向下一个字节
                                    ;
                                    ; Input:
                                    ;   bc: 待打印的字节数据所在的内存地址。
                                    ;   de: 待输出到屏幕的BG Map的VRAM地址。
    ret
; End of function print2HexChar


; =============== S U B R O U T I N E =======================================

; 将[bc]中所存的字节，以十进制形式输出到[de]所在的地址。用于打印字符串中的'%d'的格式化。
;   1. 打印输出到[de]的是对应每一位数的ASCII(正好也是Font tile编号)，
;      比如0，就输出0x30，9就输出0x39。
;   2. 因为bc中的字节值范围是[0-255]，所以输出内容可能是1到3位的数字字符，
;      所以实际可能输出在[de][de+1][de+2]的地址。
;

vramIntCpyBC2DE:
    push    hl                      ; poped at 0x66D
    ld      a, [bc]
    inc     bc
    ld      hl, 0
; -------------------------------------------------
; 将输入值a，循环除以100。
;   得到的商存入h，余数依然在a中
;   因为a的值在[0-255]，所以h的结果范围是[0-2]，余数a的结果范围在[0-99]

.loopDiv100:
    cp      100
    jr      c, .loopDiv10            ; Jump if ([bc] >= 0x64)
    inc     h
    sub     100
    jr      .loopDiv100
; ---------------------------------------------------------------------------
; -------------------------------------------------
; 将输入值a，循环除以10。
;   得到的商存入l，余数依然在a中
;   因为a的值在[0-99]，所以l的结果范围是[0-9]，余数a的结果范围在[0-9]

.loopDiv10:
    cp      10
    jr      c, process3digits
    inc     l
    sub     10
    jr      .loopDiv10
; ---------------------------------------------------------------------------
; -------------------------------------------------
; 输出3位数对应的ASCII(正好也是Font tile编号)到[de]
;   百位数在h中，十位数在l中，个位数在a中

process3digits:
    push    af                      ; popped at 0x668

.chk1stDigit:
    ld      a, h
    and     a
    jr      z, .lessThan100

.prepare1stDigit:
    add     a, $30 ; '0'
    push    af                      ; popped at 0x64C

.waitLcdHVBlank:
    ld      a, [byte_FF41]          ; FF41 - STAT - LCDC Status (R/W)
                                    ;   Bit 6 - LYC=LY Coincidence Interrupt (1=Enable) (Read/Write)
                                    ;   Bit 5 - Mode 2 OAM Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 4 - Mode 1 V-Blank Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 3 - Mode 0 H-Blank Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 2 - Coincidence Flag    (0:LYC<>LY, 1:LYC=LY) (Read Only)
                                    ;   Bit 1-0 - Mode Flag (Mode 0-3, see below) (Read Only)
                                    ;        0: During H-Blank
                                    ;        1: During V-Blank
                                    ;        2: During Searching OAM-RAM
                                    ;        3: During Transfering Data to LCD Driver
    and     2
    jr      nz, .waitLcdHVBlank

.output1stDigit:                    ; pushed at 0x645
    pop     af
    ld      [de], a
    inc     de

.chk2ndDigit:
    ld      a, l
    jr      .prepare2ndDigit
; ---------------------------------------------------------------------------

.lessThan100:
    ld      a, l
    and     a
    jr      z, .waitLcdHVBlank3

.prepare2ndDigit:
    add     a, $30 ; '0'
    push    af                      ; popped at 0x65F

.waitLcdHVBlank2:
    ld      a, [byte_FF41]          ; FF41 - STAT - LCDC Status (R/W)
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
    jr      nz, .waitLcdHVBlank2

.output2ndDigit:                    ; pushed at 0x658
    pop     af
    ld      [de], a
    inc     de

.waitLcdHVBlank3:
    ld      a, [byte_FF41]          ; FF41 - STAT - LCDC Status (R/W)
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
    jr      nz, .waitLcdHVBlank3

.output3rdDigit:                    ; pushed at 0x63E
    pop     af
    add     a, $30 ; '0'
    ld      [de], a
    inc     de
    pop     hl                      ; pushed at 0x626
    ret
; End of function vramIntCpyBC2DE


; =============== S U B R O U T I N E =======================================

; 复制一个[bc]处的字符到[de]。用于打印字符串中的'%c'的格式化。
;
; 完整逻辑:
;   等待LCD HVBlank，然后复制1个字节从源地址数据到目标地址，并对源/目标地址递增1。
;   目标地址可以是VRAM。
; 即:
;     [de++] = [bc++];
;

vramChrCpyBC2DE:
.waitLcdHVBlank:                    ; FF41 - STAT - LCDC Status (R/W)
    ld      a, [byte_FF41]          ;   Bit 6 - LYC=LY Coincidence Interrupt (1=Enable) (Read/Write)
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
    jr      nz, .waitLcdHVBlank      ; 复制一个[bc]处的字符到[de]。用于打印字符串中的'%c'的格式化。
                                    ;
                                    ; 完整逻辑:
                                    ;   等待LCD HVBlank，然后复制1个字节从源地址数据到目标地址，并对源/目标地址递增1。
                                    ;   目标地址可以是VRAM。
                                    ; 即:
                                    ;    [de++] = [bc++];
                                    ;
    ld      a, [bc]
    inc     bc
    ld      [de], a
    inc     de
    ret
; End of function vramChrCpyBC2DE


; =============== S U B R O U T I N E =======================================

; 复制一个[bc][bc+1]构成的地址处的字符串到[de]。用于打印字符串中的'%s'的格式化。
;
; 完整逻辑:
;   以[bc][bc+1]构成的地址作为源数据的起始地址，
;   循环等待LcdHVBlank然后从源数据中取值，
;   如果不为0，就把取到的值复制到[de]，遇到0则函数结束返回
;   (整体应该就是读取'\0'结尾的字符串)。
;
; Input:
;   bc: bc和bc+1中的数据构成源数据起始地址
;   de: 构成目的地起始地址
; Output
;   bc=bc+2
;   de=de+源数据中非0部分的字节长度
;
; Pseudo Code:
; {
;   push hl;
;   hl = [bc][bc+1];
;   bc+=2;
;   do {
;     waitLcdHVBlank;
;     a = [hl];
;     hl++;
;     if (a==0) {
;   pop hl;
;   return;
;     }
;     [de]=a;
;     de++;
;   }
; }

vramStrCpyBC2DE:
    push    hl
    ld      a, [bc]
    inc     bc
    ld      h, a
    ld      a, [bc]
    inc     bc
    ld      l, a

.waitLcdHVBlank:
    ld      a, [byte_FF41]          ; FF41 - STAT - LCDC Status (R/W)
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

.rdByteFromPtrHL:
    ldi     a, [hl]
    and     a
    jr      z, .readByteIs0

.wrByteToPtrDE:
    ld      [de], a
    inc     de
    jr      .waitLcdHVBlank
; ---------------------------------------------------------------------------

.readByteIs0:
    pop     hl
    ret
; End of function vramStrCpyBC2DE


; =============== S U B R O U T I N E =======================================

; 打印字符串到屏幕
;
; 参数:
; hl是待打印的字符串起始地址，字符串以'\0'结尾
; bc是用于hl指定的字符串中，格式化符号处所要代入的参数的地址
;
; 可用的格式化符号:
;   %0: 直接结束整个打印
;   %2: 输出2位16进制字符，如F6
;   %4: 输出4位16进制字符，如F6AB
;   %d: 输出10进制整数(根据输入值0-255，输出1到3位不等的字符)
;   %c: 输出一个字符
;   %s: 输出一个字符串，字符串以'\0'结尾
;
; [0xC00E]当前输出内容的列偏移坐标(共32列，根据卷轴状态，其中最左侧20列可见,从左到右0-0x13,最后一列0x1F)
; [0xC00F]当前输出内容的行偏移坐标(共32行，根据卷轴状态，屏幕上可见的共17行,从上到下0-0x11,最后一行0x1F)
;   注意换行符'0xA'，会使输出换行，会导致返回时[0xC00F]同一开始不同，用来表示调用完毕后输出内容的行偏移。

strPrintf:
    ld      bc, C010_PrintArgv1     ; 在0x691的strPrintf函数中，用来作为第1个格式化符号对应的参数
    push    hl
    ld      a, [byte_C00F]          ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      l, a
    ld      h, 0
    add     hl, hl
    add     hl, hl
    add     hl, hl
    add     hl, hl
    add     hl, hl                  ; hl = [0xC00F] * 32
    ld      a, [byte_C00E]          ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    add     a, l
    ld      l, a
    ld      a, 0
            adc        a, h
    ld      h, a                    ; hl += [0xC00E]     // hl = [0xC00F]*32 + [0xC00E]
    ld      de, $9800
    add     hl, de
    ld      d, h
    ld      e, l                    ; de = TileMap_0 + hl      // 即待输出的位置对应的BG Map格子的地址
    pop     hl

.readNextChar:
    ldi     a, [hl]
    and     a
    jr      z, .isPercent_0

.checkNewLnMark:
    cp      $A
    jr      nz, .checkFmtMark

.char0xA_newLine:
    push    hl
    ld      hl, $20 ; ' '
    add     hl, de
    ld      d, h
    ld      a, l
    and     $E0
    ld      e, a
    pop     hl
    ld      a, [byte_C00F]          ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    inc     a
    ld      [byte_C00F], a          ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
            xor        a
    ld      [byte_C00E], a          ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    jr      .readNextChar
; ---------------------------------------------------------------------------

.checkFmtMark:
    cp      $25    ; '%'
    jr      nz, .notFmtMark

.readFmtMarkChar:
    ldi     a, [hl]
    and     a
    jr      z, .isPercent_0

.checkPercent_2:
    cp      $32    ; '2'
    jr      nz, .checkPercent_4

.isPercent_2:                       ; 读取bc所指内存内的值，将这个字节对应的2位数的16进制数输出到屏幕
    call    printHexChar            ; 并自增bc值，使其指向下一个字节
                                    ;
                                    ; Input:
                                    ;   bc: 待打印的字节数据所在的内存地址。
                                    ;   de: 待输出到屏幕的BG Map的VRAM地址。
    jr      .readNextChar
; ---------------------------------------------------------------------------

.checkPercent_4:
    cp      $34    ; '4'
    jr      nz, .checkPercent_d

.isPercent_4:                       ; 读取bc所指内存内的2个字节值，将这2个字节对应的2个2位数的16进制数输出到屏幕
    call    print2HexChar           ; Input:
                                    ;   bc: 待打印的2字节数据所在的起始内存地址。
                                    ;   de: 待输出到屏幕的BG Map的起始VRAM地址。
    jr      .readNextChar
; ---------------------------------------------------------------------------

.checkPercent_d:
    cp      $64    ; 'd'
    jr      nz, .checkPercent_c

.isPercent_d:                       ; 将[bc]中所存的字节，以十进制形式输出到[de]所在的地址。用于打印字符串中的'%d'的格式化。
    call    vramIntCpyBC2DE         ;   1. 打印输出到[de]的是对应每一位数的ASCII(正好也是Font tile编号)，
                                    ;     比如0，就输出0x30，9就输出0x39。
                                    ;   2. 因为bc中的字节值范围是[0-255]，所以输出内容可能是1到3位的数字字符，
                                    ;     所以实际可能输出在[de][de+1][de+2]的地址。
                                    ;
    jr      .readNextChar
; ---------------------------------------------------------------------------

.checkPercent_c:
    cp      $63    ; 'c'
    jr      nz, .checkPercent_s

.isPercent_c:                       ; 复制一个[bc]处的字符到[de]。用于打印字符串中的'%c'的格式化。
    call    vramChrCpyBC2DE         ;
                                    ; 完整逻辑:
                                    ;   等待LCD HVBlank，然后复制1个字节从源地址数据到目标地址，并对源/目标地址递增1。
                                    ;   目标地址可以是VRAM。
                                    ; 即:
                                    ;    [de++] = [bc++];
                                    ;
    jr      .readNextChar
; ---------------------------------------------------------------------------

.checkPercent_s:
    cp      $73    ; 's'
    jr      nz, .notFmtMark

.isPercent_s:                       ; 复制一个[bc][bc+1]构成的地址处的字符串到[de]。用于打印字符串中的'%s'的格式化。
    call    vramStrCpyBC2DE         ;
                                    ; 完整逻辑:
                                    ;   以[bc][bc+1]构成的地址作为源数据的起始地址，
                                    ;   循环等待LcdHVBlank然后从源数据中取值，
                                    ;   如果不为0，就把取到的值复制到[de]，遇到0则函数结束返回
                                    ;   (整体应该就是读取'\0'结尾的字符串)。
                                    ;
                                    ; Input:
                                    ;   bc: bc和bc+1中的数据构成源数据起始地址
                                    ;   de: 构成目的地起始地址
                                    ; Output
                                    ;   bc=bc+2
                                    ;   de=de+源数据中非0部分的字节长度
    jr      .readNextChar
; ---------------------------------------------------------------------------

.notFmtMark:
    push    af

.waitLcdHVBlank:
    ld      a, [byte_FF41]          ; FF41 - STAT - LCDC Status (R/W)
                                    ;   Bit 6 - LYC=LY Coincidence Interrupt (1=Enable) (Read/Write)
                                    ;   Bit 5 - Mode 2 OAM Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 4 - Mode 1 V-Blank Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 3 - Mode 0 H-Blank Interrupt       (1=Enable) (Read/Write)
                                    ;   Bit 2 - Coincidence Flag    (0:LYC<>LY, 1:LYC=LY) (Read Only)
                                    ;   Bit 1-0 - Mode Flag    (Mode 0-3, see below) (Read Only)
                                    ;        0: During H-Blank
                                    ;        1: During V-Blank
                                    ;        2: During Searching OAM-RAM
                                    ;        3: During Transfering Data to LCD Driver
    and     2
    jr      nz, .waitLcdHVBlank

.printNormalChar:
    pop     af
    ld      [de], a
    inc     de
    ld      a, [byte_C00E]          ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    inc     a
    ld      [byte_C00E], a          ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    jr      .readNextChar
; ---------------------------------------------------------------------------

.isPercent_0:
    ret
; End of function strPrintf
