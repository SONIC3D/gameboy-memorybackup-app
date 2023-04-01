; =============== S U B R O U T I N E =======================================

; 备份存档功能的界面，输入存档名称等

dispBackupUI:
    call    waitVBlankOccur         ; 循环halt等待中断发生，直到VBlank发生1次后，本函数返回。
    call    clearOAM                ; Fill [0xFE00-0xFE9F] OAM address range with    data 0x00.
    ld      hl, $9800
    ld      bc, $400
    ld      a, $20 ; ' '
    call    vramFillWith_A          ; 赋值源a中数据到目标地址中(目标地址可以是VRAM)，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; 为了确保目标地址是VRAM地址(8000h-9FFFh)或OAM(FE00h-FE9Fh)地址时也可写入，本函数会确保在LCD的HVBlank发生期间操作。
                                    ;
                                    ; Parameters:
                                    ;   LengthOfByte to fill: bc
                                    ;   Value: a
                                    ;   DestAddr:[hl]
                                    ;
    ld      a, 0
    ld      [byte_C00E], a          ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      a, 0
    ld      [byte_C00F], a          ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      hl, $F7A
    call    strPrintf               ; 打印字符串到屏幕
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
    ld      a, 0
    ld      [byte_C00E], a          ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      a, 3
    ld      [byte_C00F], a          ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      hl, $F88
    call    strPrintf               ; 打印字符串到屏幕
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
    ld      a, 0
    ld      [byte_C00E], a          ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      a, $11
    ld      [byte_C00F], a          ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      hl, $FF4
    call    strPrintf               ; 打印字符串到屏幕
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
    ld      a, 2
    ld      [byte_C03A], a          ; Backup名称输入界面中，屏幕下方文件名的当前字符位光标，在这行第几个字符上。
                                    ; 注意起始字符是2，因为这行前面有2个空格。
                                    ; 如果输入文件名界面点End时发现光标在2上，则认为文件名不够长，不足以End。
                                    ; 最多可以到18，也就是总共文件名可以16个用户可输入字符(不包含额外的'\0')
    ld      a, $28 ; '('
    ld      [byte_C03F], a          ; 菜单光标的Y坐标(在BgMap中的行偏移)
                                    ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    ld      a, $20 ; ' '
    ld      [byte_C03E], a          ; 菜单光标的X坐标(在BgMap中的列偏移)
                                    ; 0x79: 对应图标主菜单的最右侧一列的列偏移
    ld      hl, C042_bufFileNam     ; 共$10字节。用于存放备份功能时，用户输入的文件名字符串，以便用于和卡中已有存档名进行比较。
    ld      bc, $10
    ld      a, $20 ; ' '
    call    memFillWith_A           ; 赋值源a中数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to fill: bc
                                    ;   Value: a
                                    ;   DestAddr:[hl]
    push    af
    ld      a, [byte_FFFF]          ; FFFF - IE -    Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt    Enable    (INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt    Enable    (INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt    Enable    (INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt    Enable    (INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt    Enable    (INT 60h)  (1=Enable)
    ld      [byte_C039], a          ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt    Enable (R/W)
    pop     af
    ld      a, $F4 ; '�'            ; 0xDF4 will be jumped to on next vblank.(By code at 0x264)
    ld      [byte_C035], a          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
    ld      a, $D
    ld      [byte_C036], a          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
    push    af
    ld      a, [byte_C039]          ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt    Enable (R/W)
    ld      [byte_FFFF], a          ; FFFF - IE -    Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt    Enable    (INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt    Enable    (INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt    Enable    (INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt    Enable    (INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt    Enable    (INT 60h)  (1=Enable)
    pop     af
    ret
; End of function dispBackupUI


; =============== S U B R O U T I N E =======================================

; Referenced as    function pointer by code at 0xDE2.
; 备份游戏存档界面中，按键响应逻辑，文件名输入之类的。

keyBackupUI:
    xor      a
    ld      [byte_FE04], a          ; SpriteAttr[1].PosY

.checkCursorY:                      ; 菜单光标的Y坐标(在BgMap中的行偏移)
    ld      a, [byte_C03F]          ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    cp      $78    ; 'x'
    jr      nz, .setCursorSpr

.checkCursorX:                      ; 菜单光标的X坐标(在BgMap中的列偏移)
    ld      a, [byte_C03E]          ; 0x79: 对应图标主菜单的最右侧一列的列偏移
    cp      $80    ; '�'
    jr      nz, .setCursorSpr

.cursorCharIsEnd:
    add     a, 8
    ld      [byte_FE05], a          ; SpriteAttr[1].PosX
    ld      a, [byte_C03F]          ; 菜单光标的Y坐标(在BgMap中的行偏移)
                                    ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    ld      [byte_FE04], a          ; SpriteAttr[1].PosY
    ld      a, $1D
    ld      [byte_FE06], a          ; SpriteAttr[1].Tile/Pattern_Number
    xor      a
    ld      [byte_FE07], a          ; SpriteAttr[1].Attributes/Flags

.setCursorSpr:
    ld      a, [byte_C03F]          ; 菜单光标的Y坐标(在BgMap中的行偏移)
                                    ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    ld      [byte_FE00], a          ; SpriteAttr[0].PosY
    ld      a, [byte_C03E]          ; 菜单光标的X坐标(在BgMap中的列偏移)
                                    ; 0x79: 对应图标主菜单的最右侧一列的列偏移
    ld      [byte_FE01], a          ; SpriteAttr[0].PosX
    xor      a
    ld      [byte_FE03], a          ; SpriteAttr[0].Attributes/Flags:
                                    ;   Bit7   OBJ-to-BG Priority    (0=OBJ Above BG, 1=OBJ Behind BG color 1-3)
                                    ;         (Used for both BG and Window. BG color 0 is always    behind OBJ)
                                    ;   Bit6   Y flip         (0=Normal, 1=Vertically mirrored)
                                    ;   Bit5   X flip         (0=Normal, 1=Horizontally mirrored)
                                    ;   Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
                                    ;   Bit3   Tile VRAM-Bank  **CGB Mode    Only**       (0=Bank 0, 1=Bank 1)
                                    ;   Bit2-0 Palette number  **CGB Mode    Only**       (OBP0-7)

.setPrintfPos:                      ; Backup名称输入界面中，屏幕下方文件名的当前字符位光标，在这行第几个字符上。
    ld      a, [byte_C03A]          ; 注意起始字符是2，因为这行前面有2个空格。
                                    ; 如果输入文件名界面点End时发现光标在2上，则认为文件名不够长，不足以End。
                                    ; 最多可以到18，也就是总共文件名可以16个用户可输入字符(不包含额外的'\0')
    ld      [byte_C00E], a          ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      a, $10
    ld      [byte_C00F], a          ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf

.getCursorTileId:                   ; 菜单光标的Y坐标(在BgMap中的行偏移)
    ld      a, [byte_C03F]          ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    sub     $28    ; '('
    sra     a
    ld      c, a
    sra     c
    sra     c
    sra     c
    sub     c
    ld      c, a                    ; a =    ([0xC03F] - 0x28)/2
                                    ; c =    a - a/8
    ld      a, [byte_C03E]          ; 菜单光标的X坐标(在BgMap中的列偏移)
                                    ; 0x79: 对应图标主菜单的最右侧一列的列偏移
    sub     $20    ; ' '
    sra     a
    sra     a
    sra     a
    sra     a
    add     a, c
    ld      c, a                    ; a =    ([0xC03E] - 0x20)/16
                                    ; c =    a + c
    ld      b, 0
    ld      hl, str_A_Z             ; "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    add     hl, bc
    ld      a, [hl]
    ld      [byte_FE02], a          ; SpriteAttr[0].Tile/Pattern_Number
    ld      [byte_C03D], a          ; Backup名称输入界面中，当前选中的字符的在FontTile中的编号

.endOfInputBar:                     ; Backup名称输入界面中，屏幕下方文件名的当前字符位光标，在这行第几个字符上。
    ld      a, [byte_C03A]          ; 注意起始字符是2，因为这行前面有2个空格。
                                    ; 如果输入文件名界面点End时发现光标在2上，则认为文件名不够长，不足以End。
                                    ; 最多可以到18，也就是总共文件名可以16个用户可输入字符(不包含额外的'\0')
    cp      $12
    jr      nc, .processKeys

.chkPrintableChr:                   ; Backup名称输入界面中，当前选中的字符的在FontTile中的编号
    ld      a, [byte_C03D]
    cp      $20    ; ' '
    jr      nc, .storeSP

.repInvalidChr:
    ld      a, $20 ; ' '
; ------------------------------------------------
; 这里将选中的字符打印到文件名输入框

.storeSP:
    ld      [word_C00C], sp         ; if sp=0xFFFC, then [0xC00C]=0xFC, [0xC00D]=0xFF

.saveRegs:
    ld      sp, $C00C
    push    hl                      ; [0xC00A] = hl
    push    de                      ; [0xC008] = de
    push    bc                      ; [0xC006] = bc
    push    af                      ; [0xC004] = af
                                    ;    [0xC004] = f
                                    ;    [0xC005] = a = 文件名输入界面用户当前光标选中的字符

.restoreSP:
    ld      a, [word_C00C]
    ld      l, a
    ld      a, [word_C00C+1]
    ld      h, a
    ld      sp, hl

.prepareParam1:                     ; 在0x691的strPrintf函数中，用来作为第1个格式化符号对应的参数
    ld      hl, C010_PrintArgv1
    ld      a, [byte_C005]
    ldi     [hl], a                 ; 文件名输入界面用户当前光标选中的字符
    ld      hl, strFmtString_3      ; "%c"
    call    strPrintf               ; 打印字符串到屏幕
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

.processKeys:
    call    joypadRead              ; 读取按键
                                    ; 返回值：
                                    ;   (以下返回值Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A)
                                    ;   a: 本周期按下了的按键(这些键的bit被置为1)
                                    ;   b: 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
    ld      a, [byte_C001]          ; 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
                                    ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A
    bit     1, a
    jr      z, .notKeyDown_B        ; 如果[$C001].bit1是0(本周期没有按"B"键)，就跳

.KeyDown_B:
    call    displayMainMenu
    ret
; ---------------------------------------------------------------------------

.notKeyDown_B:
    bit     7, a
    jr      z, .notKeyDown_DirD     ; 如果[$C001].bit7是0(本周期没有按"下"键)，就跳

.KeyDown_DirDown:
    call    playSfxOK
    ld      a, [byte_C03F]          ; 菜单光标的Y坐标(在BgMap中的行偏移)
                                    ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    add     a, $10
    cp      $79    ; 'y'
    jr      c, .updCursorPosY_2

.cursorToTopMost:
    ld      a, $28 ; '('

.updCursorPosY_2:
    ld      [byte_C03F], a          ; 菜单光标的Y坐标(在BgMap中的行偏移)
                                    ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    jr      .checkKeyDown_A
; ---------------------------------------------------------------------------

.notKeyDown_DirD:
    bit     6, a
    jr      z, .notKeyDown_DirU     ; 如果[$C001].bit6是0(本周期没有按"上"键)，就跳

.KeyDown_DirUp:
    call    playSfxOK
    ld      a, [byte_C03F]          ; 菜单光标的Y坐标(在BgMap中的行偏移)
                                    ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    sub     $10
    cp      $28    ; '('
    jr      nc, .updCursorPosY_1

.cursorToBtmMost:
    ld      a, $78 ; 'x'

.updCursorPosY_1:
    ld      [byte_C03F], a          ; 菜单光标的Y坐标(在BgMap中的行偏移)
                                    ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    jr      .checkKeyDown_A
; ---------------------------------------------------------------------------

.notKeyDown_DirU:
    bit     4, a
    jr      z, .notKeyDown_DirR     ; 如果[$C001].bit4是0(本周期没有按"右"键)，就跳

.KeyDown_DirRt:
    call    playSfxOK
    ld      a, [byte_C03E]          ; 菜单光标的X坐标(在BgMap中的列偏移)
                                    ; 0x79: 对应图标主菜单的最右侧一列的列偏移
    add     a, $10
    cp      $81    ; '�'
    jr      c, .updCursorPosX_1

.cursorToLtMost:
    ld      a, $20 ; ' '

.updCursorPosX_1:
    ld      [byte_C03E], a          ; 菜单光标的X坐标(在BgMap中的列偏移)
                                    ; 0x79: 对应图标主菜单的最右侧一列的列偏移
    jr      .checkKeyDown_A
; ---------------------------------------------------------------------------

.notKeyDown_DirR:
    bit     5, a
    jr      z, .checkKeyDown_A      ; 如果[$C001].bit5是0(本周期没有按"左"键)，就跳

.KeyDown_DirLt:
    call    playSfxOK
    ld      a, [byte_C03E]          ; 菜单光标的X坐标(在BgMap中的列偏移)
                                    ; 0x79: 对应图标主菜单的最右侧一列的列偏移
    sub     $10
    cp      $20    ; ' '
    jr      nc, .updCursorPosX_2

.cursorToRtMost:
    ld      a, $80 ; '�'

.updCursorPosX_2:
    ld      [byte_C03E], a          ; 菜单光标的X坐标(在BgMap中的列偏移)
                                    ; 0x79: 对应图标主菜单的最右侧一列的列偏移

.checkKeyDown_A:
    bit     0, a
    ret     z                       ; 如果本周期没有按"A"键)，就返回

.KeyDown_A:
    call    playSfxOK
    ld      a, [byte_C03D]          ; Backup名称输入界面中，当前选中的字符的在FontTile中的编号
    cp      $1B
    jr      nz, .notBackspace

.charIsBackspace:                   ; Backup名称输入界面中，屏幕下方文件名的当前字符位光标，在这行第几个字符上。
    ld      a, [byte_C03A]          ; 注意起始字符是2，因为这行前面有2个空格。
                                    ; 如果输入文件名界面点End时发现光标在2上，则认为文件名不够长，不足以End。
                                    ; 最多可以到18，也就是总共文件名可以16个用户可输入字符(不包含额外的'\0')
    dec     a
    cp      2
    ret     c
    ld      [byte_C03A], a          ; Backup名称输入界面中，屏幕下方文件名的当前字符位光标，在这行第几个字符上。
                                    ; 注意起始字符是2，因为这行前面有2个空格。
                                    ; 如果输入文件名界面点End时发现光标在2上，则认为文件名不够长，不足以End。
                                    ; 最多可以到18，也就是总共文件名可以16个用户可输入字符(不包含额外的'\0')

.clrFilenamInput:
    sub     2
    ld      c, a
    ld      b, 0
    ld      hl, C042_bufFileNam     ; 共$10字节。用于存放备份功能时，用户输入的文件名字符串，以便用于和卡中已有存档名进行比较。
    add     hl, bc
    ld      a, $20 ; ' '            ; Use 10 of $20 char (space) to fill input buffer.
    ld      [hl], a
    ret
; ---------------------------------------------------------------------------

.notBackspace:
    cp      $1C
    jr      nz, .notTheCharEnd

.charIsEnd:                         ; Backup名称输入界面中，屏幕下方文件名的当前字符位光标，在这行第几个字符上。
    ld      a, [byte_C03A]          ; 注意起始字符是2，因为这行前面有2个空格。
                                    ; 如果输入文件名界面点End时发现光标在2上，则认为文件名不够长，不足以End。
                                    ; 最多可以到18，也就是总共文件名可以16个用户可输入字符(不包含额外的'\0')
    cp      2
    jr      z, .failedToAddFile

.chkDuplication:                    ; 找出现有存档中是否有同名备份。
    call    checkIfNameDupe         ; 具体实现是以0xC763中的存档备份总数作为循环次数，从0xC063开始循环
                                    ; 每次读取0x10个字节和用户输入在0xC042中的文件名作比较。
                                    ;   输入值:
                                    ;    [0xC042]: 其中存放用户输入的文件名内容。
                                    ;   返回值:
                                    ;    a=0，表示找到同名的，此时hl就是这个存档文件名的偏移地址(在0xC063开始的范围)。
                                    ;    a=1，表示没有找到同名的（也可能是0xC776这个用途不明的字节，内容值为0导致）
    or      a
    jr      nz, .addBackupFile

.failedToAddFile:
    call    playSfxError
    ret
; ---------------------------------------------------------------------------

.addBackupFile:
    ld      a, [byte_C763]          ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。
    inc     a
    ld      [byte_C763], a          ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。
    dec     a
    ld      l, a
    ld      h, 0
    add     hl, hl
    add     hl, hl
    add     hl, hl
    add     hl, hl
    ld      bc, C063_fnameCache     ; [0xC063-0xC762]:
                                    ;   每$10个字节一组，存放N组卡带中已经备份数据的文件名，总数量在$C763中。
                                    ;   总共$70组，即112组，但是实际好像只用了100组也就是1600字节(0x640字节)
                                    ;   用$700只是为了写入28SF040时，对齐256字节的扇区
    add     hl, bc
    push    hl
    ld      hl, C042_bufFileNam     ; 共$10字节。用于存放备份功能时，用户输入的文件名字符串，以便用于和卡中已有存档名进行比较。
    pop     de
    ld      bc, $10
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]
    call    dispBkupWorking         ; 选好备份后并输入好文件名，对着End符号按A后，出现的Working等待处理中界面
    call    displayMainMenu
    ret
; ---------------------------------------------------------------------------

.notTheCharEnd:
    and     a
    jr      nz, .storeChar

.replaceUseSpace:
    ld      a, $20 ; ' '

.storeChar:
    ld      [byte_C03D], a          ; Backup名称输入界面中，当前选中的字符的在FontTile中的编号
    ld      a, [byte_C03A]          ; Backup名称输入界面中，屏幕下方文件名的当前字符位光标，在这行第几个字符上。
                                    ; 注意起始字符是2，因为这行前面有2个空格。
                                    ; 如果输入文件名界面点End时发现光标在2上，则认为文件名不够长，不足以End。
                                    ; 最多可以到18，也就是总共文件名可以16个用户可输入字符(不包含额外的'\0')
    sub     2
    ld      c, a
    ld      b, 0
    ld      hl, C042_bufFileNam     ; 共$10字节。用于存放备份功能时，用户输入的文件名字符串，以便用于和卡中已有存档名进行比较。
    add     hl, bc
    ld      a, [byte_C03D]          ; Backup名称输入界面中，当前选中的字符的在FontTile中的编号
    ld      [hl], a
    ld      a, [byte_C03A]          ; Backup名称输入界面中，屏幕下方文件名的当前字符位光标，在这行第几个字符上。
                                    ; 注意起始字符是2，因为这行前面有2个空格。
                                    ; 如果输入文件名界面点End时发现光标在2上，则认为文件名不够长，不足以End。
                                    ; 最多可以到18，也就是总共文件名可以16个用户可输入字符(不包含额外的'\0')
    inc     a
    cp      $13
    ret     nc                      ; return, if current pos is the last position    of the input control.

.movCursorToNext:                   ; Backup名称输入界面中，屏幕下方文件名的当前字符位光标，在这行第几个字符上。
    ld      [byte_C03A], a          ; 注意起始字符是2，因为这行前面有2个空格。
                                    ; 如果输入文件名界面点End时发现光标在2上，则认为文件名不够长，不足以End。
                                    ; 最多可以到18，也就是总共文件名可以16个用户可输入字符(不包含额外的'\0')
    ret
; End of function keyBackupUI

; ---------------------------------------------------------------------------
aBackup_0:        db "       Backup",0
aABCDEFGHIJKLMN:    db "   A B C D E F G",$A
    db $A
    db "   H I J K L M N",$A
    db $A
    db "   O P Q R S T U",$A
    db $A
    db "   V W X Y Z   .",$A
    db $A
    db "   0 1 2 3 4 5 6",$A
    db $A
    db "   7 8 9 # - ",$1B," ",$1C,$1D,0
    db "  ----------------",0
strFmtString_3:        db "%c",0
str_A_Z:        db "ABCDEFGHIJKLMNOPQRSTUVWXYZ",0
str_0_9_marks:        db ".0123456789#-",$1B,$1C
