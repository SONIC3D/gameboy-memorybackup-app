;SECTION "SubMenu Module Options", ROM0[$150B]

; =============== S U B R O U T I N E =======================================

; Display Option Menu

dispOptionUI:
    call    waitVBlankOccur         ; 循环halt等待中断发生，直到VBlank发生1次后，本函数返回。
    call    clearOAM                ; Fill [0xFE00-0xFE9F] OAM address range with data 0x00.
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
    ld      hl, strOptionTitle      ; "      Options"
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
    ld      hl, C773_OptVal01       ; Option选项值，存放菜单显示方式，非0时，表示用图标显示主菜单，为0时表示用纯文字。
                                    ; 初始化为0xFF，即图标方式显示。
    ld      de, C02F_TmpOptVal1     ; 存放Option界面用户点Save前，用户所设置的临时值。
                                    ; 4个字节分别对应：使用图标菜单、反向色盘、音效开启、不允许重复文件名
    ld      bc, 4
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]
    ld      a, 4

.prtOptionItem:
    ld      [byte_C02E], a          ; 存放Option界面的光标纵坐标，取值范围0-4
    call    printOptionItem         ; 根据[0xC02E]的编号(当前光标所指的选项)，打印输出Option界面的5个选项中的对应的一个，和其末尾的选项叉状态。
                                    ; [0xC02E]中内容会由调用方放入，值的范围是0-4。option菜单创建主逻辑中会依次从4到0，递减放入。
    ld      a, [byte_C02E]          ; 存放Option界面的光标纵坐标，取值范围0-4
    sub     1
    jr      nc, .prtOptionItem

.setInitCsorPos:
    xor     a
    ld      [byte_C02E], a          ; 存放Option界面的光标纵坐标，取值范围0-4
    call    setOptCursorPos         ; 设置Option界面中箭头光标的坐标位置
                                    ; [0xC02E]指定了当前选中的项目序号，取值范围从0-4

.savInterruptEnb:
    push    af
    ld      a, [byte_FFFF]          ; FFFF - IE - Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt Enable (INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt Enable (INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt Enable (INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt Enable (INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt Enable (INT 60h)  (1=Enable)
    ld      [byte_C039], a          ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt Enable (R/W)
    pop     af

.setNextVBlkFunc:                    ; 0x1567 will be jumped to on next vblank.(By code at 0x264)
    ld      a, $67 ; 'g'
    ld      [byte_C035], a          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
    ld      a, $15
    ld      [byte_C036], a          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。

.recInterruptEnb:
    push    af
    ld      a, [byte_C039]          ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt Enable (R/W)
    ld      [byte_FFFF], a          ; FFFF - IE - Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt Enable (INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt Enable (INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt Enable (INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt Enable (INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt Enable (INT 60h)  (1=Enable)
    pop     af
    ret
; End of function dispOptionUI


; =============== S U B R O U T I N E =======================================

; Referenced as function pointer by code at 0x1555.
; Option界面按键响应逻辑
keyOptionUI:
    ld      a, [byte_C03F]          ; 菜单光标的Y坐标(在BgMap中的行偏移)
                                    ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------> 对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    ld      [byte_FE00], a          ; SpriteAttr[0].PosY
    ld      a, [byte_C03E]          ; 菜单光标的X坐标(在BgMap中的列偏移)
                                    ; 0x79: 对应图标主菜单的最右侧一列的列偏移
    ld       [byte_FE01], a         ; SpriteAttr[0].PosX
    ld       a, $10
    ld       [byte_FE02], a         ; SpriteAttr[0].Tile/Pattern_Number
    call    joypadRead              ; 读取按键
                                    ; 返回值：
                                    ;   (以下返回值Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A)
                                    ;   a: 本周期按下了的按键(这些键的bit被置为1)
                                    ;   b: 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
    ld      a, [byte_C001]          ; 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
                                    ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A
    bit     1, a
    jr      z, .notKeyDown_B         ; 如果[$C001].bit1是0(本周期没有按"B"键)，就跳

.KeyDown_B:
    call    displayMainMenu
    ret
; ---------------------------------------------------------------------------

.notKeyDown_B:
    bit     7, a
    jr      z, .notKeyDown_DirD      ; 如果[$C001].bit7是0(本周期没有按"下"键)，就跳

.KeyDown_DirDown:                    ; 存放Option界面的光标纵坐标，取值范围0-4
    ld      a, [byte_C02E]
    inc     a
    cp      5
    jr      c, .savOptCursorPos

.movCursor2First:
    xor     a
    jr      .savOptCursorPos
; ---------------------------------------------------------------------------

.notKeyDown_DirD:
    bit     6, a
    jr      z, .notKeyDown_DirU      ; 如果[$C001].bit6是0(本周期没有按"上"键)，就跳

.KeyDown_DirUp:                      ; 存放Option界面的光标纵坐标，取值范围0-4
    ld      a, [byte_C02E]
    sub     1
    jr      nc, .savOptCursorPos

.movCursor2Last:
    ld      a, 4

.savOptCursorPos:
    ld      [byte_C02E], a          ; 存放Option界面的光标纵坐标，取值范围0-4
    call    setOptCursorPos         ; 设置Option界面中箭头光标的坐标位置
                                    ; [0xC02E]指定了当前选中的项目序号，取值范围从0-4
    call    playSfxOK
    ret
; ---------------------------------------------------------------------------

.notKeyDown_DirU:
    bit     0, a
    ret     z                       ; 如果本周期没有按"A"键)，就返回

.KeyDown_A:
    call    playSfxOK

.chkSelectedItem:                   ; 存放Option界面的光标纵坐标，取值范围0-4
    ld      a, [byte_C02E]
    cp      4
    jr      z, .optionDoSave

.selItem_1_to_4:                    ; 存放Option界面用户点Save前，用户所设置的临时值。
    ld      hl, C02F_TmpOptVal1     ; 4个字节分别对应：使用图标菜单、反向色盘、音效开启、不允许重复文件名
    ld      a, [byte_C02E]          ; 存放Option界面的光标纵坐标，取值范围0-4
    ld      e, a
    ld      d, 0
    add     hl, de
    ld      a, [hl]
    cpl
    ld      [hl], a
    call    printOptionItem         ; 根据[0xC02E]的编号(当前光标所指的选项)，打印输出Option界面的5个选项中的对应的一个，和其末尾的选项叉状态。
                                    ; [0xC02E]中内容会由调用方放入，值的范围是0-4。option菜单创建主逻辑中会依次从4到0，递减放入。
    ret
; ---------------------------------------------------------------------------

.optionDoSave:
    ld      hl, C02F_TmpOptVal1     ; 存放Option界面用户点Save前，用户所设置的临时值。
                                    ; 4个字节分别对应：使用图标菜单、反向色盘、音效开启、不允许重复文件名
    ld      de, C773_OptVal01       ; Option选项值，存放菜单显示方式，非0时，表示用图标显示主菜单，为0时表示用纯文字。
                                    ; 初始化为0xFF，即图标方式显示。
    ld      bc, 4
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]
    call    wrSF040_0x7FF00         ; Set [0xDFF0-0xDFFF] to 0xFF.
                                    ; Create checksum for [0xC763-0xC861] and store the checksum at [0xC862].
                                    ; Copy WRAM-0:[0xC763-0xC862] to 28SF040:[0x7FF00-0x7FFFF].
    call    displayMainMenu
    ret
; End of function keyOptionUI

; ---------------------------------------------------------------------------
    call    displayMainMenu
    ret

; =============== S U B R O U T I N E =======================================

; 根据[0xC02E]的编号(当前光标所指的选项)，打印输出Option界面的5个选项中的对应的一个，和其末尾的选项叉状态。
; [0xC02E]中内容会由调用方放入，值的范围是0-4。option菜单创建主逻辑中会依次从4到0，递减放入。

printOptionItem:
    ld      a, [byte_C02E]          ; 存放Option界面的光标纵坐标，取值范围0-4
    add     a, a
    add     a, 4
    ld      [byte_C00F], a          ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    xor     a
    ld      [byte_C00E], a          ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      hl, arrStrOptionItm
    ld      a, [byte_C02E]          ; 存放Option界面的光标纵坐标，取值范围0-4
    add     a, a
    ld      e, a
    ld      d, 0
    add     hl, de
    ldi     a, [hl]
    ld      e, a
    ld      a, [hl]
    ld      d, a
    push    de                      ; Popped at 0x162A, as hl
    ld      hl, C02F_TmpOptVal1     ; 存放Option界面用户点Save前，用户所设置的临时值。
                                    ; 4个字节分别对应：使用图标菜单、反向色盘、音效开启、不允许重复文件名
    ld      a, [byte_C02E]          ; 存放Option界面的光标纵坐标，取值范围0-4
    ld      e, a
    ld      d, 0
    add     hl, de
    ld      a, [hl]

.chkOptionValue:
    or      a
    ld      a, $D
    jr      z, .storeSP

.optionValueIs_1:
    ld      a, $E

.storeSP:
    ld      [word_C00C], sp         ; if sp=0xFFFC, then [0xC00C]=0xFC, [0xC00D]=0xFF

.saveRegs:
    ld      sp, $C00C
    push    hl                      ; [0xC00A]=hl
    push    de                      ; [0xC008]=de
    push    bc                      ; [0xC006]=bc
    push    af                      ; [0xC004] = af
                                    ;    [0xC004] = f
                                    ;    [0xC005] = a = $D(if option value is 0)    // Empty square    in font    tiles
                                    ;             = $E(if option value is 1)    // Cross square    in font    tiles

.restoreSP:
    ld      a, [word_C00C]
    ld      l, a
    ld      a, [word_C00C+1]
    ld      h, a
    ld      sp, hl

.prepareParam1:                     ; 在0x691的strPrintf函数中，用来作为第1个格式化符号对应的参数
    ld      hl, C010_PrintArgv1
    ld      a, [byte_C005]
    ldi     [hl], a

.doPrint:
    pop     hl                      ; Pushed at 0x15FD, as de
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
    ret
; End of function printOptionItem


; =============== S U B R O U T I N E =======================================

; 设置Option界面中箭头光标的坐标位置
; [0xC02E]指定了当前选中的项目序号，取值范围从0-4

setOptCursorPos:
    ld      hl, optionItemPosY      ; Option界面的5行Item在BG Map的Y坐标，用这个Y坐标去显示最左侧的指向光标
    ld      a, [byte_C02E]          ; 存放Option界面的光标纵坐标，取值范围0-4
    ld      e, a
    ld      d, 0
    add     hl, de
    ld      a, [hl]
    ld      [byte_C03F], a          ; 菜单光标的Y坐标(在BgMap中的行偏移)
                                    ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    ld      a, $10
    ld      [byte_C03E], a          ; 菜单光标的X坐标(在BgMap中的列偏移)
                                    ; 0x79: 对应图标主菜单的最右侧一列的列偏移
    ret
; End of function setOptCursorPos

; ---------------------------------------------------------------------------
arrStrOptionItm:
    db  LOW(strOptionItm1), HIGH(strOptionItm1)     ; db $60,$16 => 0x1660
    db  LOW(strOptionItm2), HIGH(strOptionItm2)     ; db $75,$16 => 0x1675
    db  LOW(strOptionItm3), HIGH(strOptionItm3)     ; db $8A,$16 => 0x168A
    db  LOW(strOptionItm4), HIGH(strOptionItm4)     ; db $9F,$16 => 0x169F
    db  LOW(strOptionItm5), HIGH(strOptionItm5)     ; db $B4,$16 => 0x16B4

optionItemPosY:                     ; Option界面的5行Item在BG Map的Y坐标，用这个Y坐标去显示最左侧的指向光标
    db  $30                         ; 0
    db  $40                         ; 1
    db  $50                         ; 2
    db  $60                         ; 3
    db  $70                         ; 4

strOptionTitle:     db "      Options",0
strOptionItm1:      db "  1.Icons         %c",0
strOptionItm2:      db "  2.Inverse       %c",0
strOptionItm3:      db "  3.Sound         %c",0
strOptionItm4:      db "  4.Unique names  %c",0
strOptionItm5:      db "  5.Save & exit",0
