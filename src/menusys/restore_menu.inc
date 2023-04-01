; =============== S U B R O U T I N E =======================================

; List backup filenames.
;  Max 10 line in a screen. From the screen line 4 to line 13(The 5th line to the 14th line).
;
; Input parameter:
;   a:
;   Start offset of the file entries.
;   (e.g. Set a=0x20, then list will start from 0x20 item of the entry list)
;   (Base value is 0)
;

listSavFilename:
    ld      [byte_C040], a          ; 局部变量:
                                    ;   0x85B和0xA25函数中:
                                    ;    表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;    表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)

.setPrtColOffset:
    ld      a, 0
    ld      [byte_C00E], a          ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf

.setPrtRowOffset:
    ld      a, 4
    ld      [byte_C00F], a          ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf

.prntNxtFilename:
    ld      a, [byte_C763]          ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。
    ld      b, a
    ld      a, [byte_C040]          ; 局部变量:
                                    ;   0x85B和0xA25函数中:
                                    ;    表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;    表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    cp      b
    ret     nc                      ; return if [0xC040] >= [0xC763]

.readCurrEntry:
    ld      l, a
    ld      h, 0
    add     hl, hl
    add     hl, hl
    add     hl, hl
    add     hl, hl                  ; hl = ([0xC040] << 4)
    ld      bc, C063_fnameCache     ; [0xC063-0xC762]:
                                    ;   每$10个字节一组，存放N组卡带中已经备份数据的文件名，总数量在$C763中。
                                    ;   总共$70组，即112组，但是实际好像只用了100组也就是1600字节(0x640字节)
                                    ;   用$700只是为了写入28SF040时，对齐256字节的扇区
    add     hl, bc                  ; hl = ([0xC040] << 4) + 0xC063
                                    ; e.g:
                                    ;   [0xC040]=      1, hl    = 0xC063 + 0x010
                                    ;   [0xC040]=      6, hl    = 0xC063 + 0x060
                                    ;   ...
                                    ;   [0xC040]=0x63, hl    = 0xC063 + 0x630

.copyCurrNam2Buf:                   ; 共$10字节。用于存放备份功能时，用户输入的文件名字符串，以便用于和卡中已有存档名进行比较。
    ld      de, C042_bufFileNam
    ld      bc, $10
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]

.endOfString:
    xor      a
    ld      [de], a                 ; de = $C052

.incrEntryOffset:                   ; 局部变量:
    ld      a, [byte_C040]          ;   0x85B和0xA25函数中:
                                    ;    表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;    表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    inc     a
    ld      [byte_C040], a          ; 局部变量:
                                    ;   0x85B和0xA25函数中:
                                    ;    表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;    表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
; ------------------------------------------
; Prepare calling printf
    ld      hl, C042_bufFileNam     ; 共$10字节。用于存放备份功能时，用户输入的文件名字符串，以便用于和卡中已有存档名进行比较。

.storeSP:                           ; if sp=0xFFFC, then [0xC00C]=0xFC, [0xC00D]=0xFF
    ld      [word_C00C], sp

.saveRegs:
    ld      sp, $C00C
    push    hl                      ; [0xC00A]=hl
                                    ;    [0xC00A] = h = 当前备份文件名字符串缓冲地址的高8bit
                                    ;    [0xC00B] = l = 当前备份文件名字符串缓冲地址的低8bit
    push    de                      ; [0xC008]=de
    push    bc                      ; [0xC006]=bc
    push    af                      ; [0xC004] = af
                                    ;    [0xC004] = f
                                    ;    [0xC005] = a = 当前备份文件的序号(从1开始计)

.restoreSP:
    ld      a, [word_C00C]
    ld      l, a
    ld      a, [word_C00C+1]
    ld      h, a
    ld      sp, hl

.prepareParams:                     ; 在0x691的strPrintf函数中，用来作为第1个格式化符号对应的参数
    ld      hl, C010_PrintArgv1
    ld      a, [byte_C005]
    ldi     [hl], a                 ; 当前打印中的备份文件的序号
    ld      a, [byte_C00B]
    ldi     [hl], a                 ; 当前打印中的备份文件名字符串地址的低8bit
    ld      a, [byte_C00A]          ; enterSvcMode中用来临时存放28SF040 Manufacturer ID
    ldi     [hl], a                 ; 当前打印中的备份文件名字符串地址的高8bit

.chkIdLength:                       ; 局部变量:
    ld      a, [byte_C040]          ;   0x85B和0xA25函数中:
                                    ;    表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;    表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    cp      10
    jr      nc, .dispLongId         ; Jump if ([0xC040] >= 0x0A)       // 使用2位数Id的格式化行表示10之后的数字

.dispShortId:                       ; "  %d.%s\n"
    ld      hl, strFmtSvName_S
    jr      .printSvFilename
; ---------------------------------------------------------------------------

.dispLongId:
    ld      hl, strFmtSvName_L      ; " %d.%s\n"

.printSvFilename:
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
    ld      a, [byte_C00F]          ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    cp      $E
    jr      c, .prntNxtFilename     ; Jump if Line offset    value in [0xC00F] is less than 0xE(14).
                                    ; This means list stored backup file name and print them from    line 4 to line 13.
                                    ; That's 10 lines in total.
            ret
; End of function listSavFilename


; =============== S U B R O U T I N E =======================================

; Display Restore/Delete UI
; [0xC02A] = 1: MainMenu -> Restore
; [0xC02A] = 0: MainMenu -> Delete

dispRestorDelUI:
    call    waitVBlankOccur         ; 循环halt等待中断发生，直到VBlank发生1次后，本函数返回。
    call    clearOAM                ; Fill [0xFE00-0xFE9F] OAM address range with    data 0x00.

.clrBgMap:
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
    ld      a, [byte_C02A]          ; 该变量为0，表示主菜单中选了Delete
                                    ; 该变量为1，表示主菜单中选了Restore
    and     a
    jr      z, .displayDelete

.displayRestore:                    ; "      Restore"
    ld      hl, strRestore
    jr      .print_UI_Title
; ---------------------------------------------------------------------------

.displayDelete:
    ld      hl, strDelete           ; "       Delete"

.print_UI_Title:
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

.printFilelist:
    xor      a
    ld      [byte_C02C], a          ; Restore/Del界面的按键响应函数中的一个CD时间，用来防止过快响应持续按住的上/下键
                                    ; 每次上下键读入后，这个值会设为6，然后每帧跑到该函数时，会对该值 -1，直到0为止。
                                    ; 为0后可以再次接受按住不放的上/下按键输入。
    ld      [byte_C028], a          ; 在Restore或Delete界面，备份文件名列表的第1行对应的全局文件编号
                                    ; (全局文件编号，以0为基)
    ld      [byte_C029], a          ; 在Restore或Delete界面，光标选中的备份文件的顺序号
                                    ; (不是当前屏幕内的序号，是在所有备份中的顺序号)
                                    ; 这个序号>=0，且<[0xC763]
    call    listSavFilename         ; List backup    filenames.
                                    ;  Max 10 line in a screen. From the screen line 4 to    line 13(The 5th    line to    the 14th line).
                                    ;
                                    ; Input parameter:
                                    ;   a:
                                    ;      Start    offset of the file entries.
                                    ;      (e.g.    Set a=0x20, then list will start from 0x20 item    of the entry list)
                                    ;      (Base    value is 0)
                                    ;

.intCursorPos:                      ; 光标放到0x30(屏幕行偏移6，即第7行)
    ld      a, $30 ; '0'
    ld      [byte_C03F], a          ; 菜单光标的Y坐标(在BgMap中的行偏移)
                                    ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    ld      a, 8                    ; 光标放到0x8(屏幕列偏移1，即第2列)
    ld      [byte_C03E], a          ; 菜单光标的X坐标(在BgMap中的列偏移)
                                    ; 0x79: 对应图标主菜单的最右侧一列的列偏移
; ----------------------------------------

.savInterruptEnb:
    push    af
    ld      a, [byte_FFFF]          ; FFFF - IE -    Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt    Enable    (INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt    Enable    (INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt    Enable    (INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt    Enable    (INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt    Enable    (INT 60h)  (1=Enable)
    ld      [byte_C039], a          ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt    Enable (R/W)
    pop     af
; ----------------------------------------

.setNextVBlkFunc:                   ; 0x1102 will    be jumped to on    next vblank.(By    code at    0x264)
    ld      a, 2
    ld      [byte_C035], a          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
    ld      a, $11
    ld      [byte_C036], a          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
; ----------------------------------------

.recInterruptEnb:
    push    af
    ld      a, [byte_C039]          ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt    Enable (R/W)
    ld      [byte_FFFF], a          ; FFFF - IE -    Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt    Enable    (INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt    Enable    (INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt    Enable    (INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt    Enable    (INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt    Enable    (INT 60h)  (1=Enable)
    pop     af
; ----------------------------------------
    ret
; End of function dispRestorDelUI


; =============== S U B R O U T I N E =======================================

; Referenced as function pointer by code at 0x10F0.
; Restore/Del界面的按键响应函数

keyRestoreDelUI:
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
    ld      a, [byte_C029]          ; 在Restore或Delete界面，光标选中的备份文件的顺序号
                                    ; (不是当前屏幕内的序号，是在所有备份中的顺序号)
                                    ; 这个序号>=0，且<[0xC763]
    cp      9
    jr      nc, .fileIdGThan9

.fileIdLEThan9:                     ; 让三角箭头光标出现在第2列，针对1位数序号
    ld      a, $10
    jr      .setCursorPosX
; ---------------------------------------------------------------------------

.fileIdGThan9:
    ld      a, 8                    ; 让三角箭头光标出现在第1列，针对两位数

.setCursorPosX:
    ld      [byte_FE01], a          ; SpriteAttr[0].PosX

.setCursorTile:                     ; 0x10号Tile是向右的箭头
    ld      a, $10
    ld      [byte_FE02], a          ; SpriteAttr[0].Tile/Pattern_Number

.readHoldKeyCD:                     ; Restore/Del界面的按键响应函数中的一个CD时间，用来防止过快响应持续按住的上/下键
    ld      a, [byte_C02C]          ; 每次上下键读入后，这个值会设为6，然后每帧跑到该函数时，会对该值 -1，直到0为止。
                                    ; 为0后可以再次接受按住不放的上/下按键输入。
    sub     1
    jr      c, .holdKeyCdUpdted

.updateHoldKeyCD:                   ; Restore/Del界面的按键响应函数中的一个CD时间，用来防止过快响应持续按住的上/下键
    ld      [byte_C02C], a          ; 每次上下键读入后，这个值会设为6，然后每帧跑到该函数时，会对该值 -1，直到0为止。
                                    ; 为0后可以再次接受按住不放的上/下按键输入。

.holdKeyCdUpdted:
    call    joypadRead              ; 读取按键
                                    ; 返回值：
                                    ;   (以下返回值Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A)
                                    ;   a: 本周期按下了的按键(这些键的bit被置为1)
                                    ;   b: 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
    ld      a, [byte_C02C]          ; Restore/Del界面的按键响应函数中的一个CD时间，用来防止过快响应持续按住的上/下键
                                    ; 每次上下键读入后，这个值会设为6，然后每帧跑到该函数时，会对该值 -1，直到0为止。
                                    ; 为0后可以再次接受按住不放的上/下按键输入。
    or      a
    jr      nz, .notKeyHold_DirU    ; 如果CoolDown时间还没有减到0就跳

.checkHoldingKey:                   ; 本周期按下了的按键(这些键的bit被置为1)
    ld      a, [byte_C000]          ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A
    bit     7, a
    jr      z, .notKeyHold_DirD     ; 如果[$C000].bit7是0(当前"下"键没有按住状态)，就跳

.KeyHold_DirDown:
    ld      a, 6
    ld      [byte_C02C], a          ; Restore/Del界面的按键响应函数中的一个CD时间，用来防止过快响应持续按住的上/下键
                                    ; 每次上下键读入后，这个值会设为6，然后每帧跑到该函数时，会对该值 -1，直到0为止。
                                    ; 为0后可以再次接受按住不放的上/下按键输入。
    ld      a, [byte_C763]          ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。
    ld      b, a
    ld      a, [byte_C029]          ; 在Restore或Delete界面，光标选中的备份文件的顺序号
                                    ; (不是当前屏幕内的序号，是在所有备份中的顺序号)
                                    ; 这个序号>=0，且<[0xC763]
    inc     a
    cp      b
    ret     nc                      ; return if cursor pointed to    the last backup    file.
                                    ; return if ([0xC029]+1) >= [0xC763]

.notTheLastFile:                    ; 在Restore或Delete界面，光标选中的备份文件的顺序号
    ld      [byte_C029], a          ; (不是当前屏幕内的序号，是在所有备份中的顺序号)
                                    ; 这个序号>=0，且<[0xC763]
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
    add     a, 8
    cp      $79    ; 'y'
    jr      c, .cursorY_MoveDn

.scrollFilelstDn:                   ; 在Restore或Delete界面，备份文件名列表的第1行对应的全局文件编号
    ld      a, [byte_C028]          ; (全局文件编号，以0为基)
    inc     a                       ; 这里没有检查上限?
                                    ;   因为前面代码中，已经读取了[0xC029]的内容，确保了光标所指的不是全局最后1条记录。
                                    ;   所以这里可以无需保护。
    ld      [byte_C028], a          ; 在Restore或Delete界面，备份文件名列表的第1行对应的全局文件编号
                                    ; (全局文件编号，以0为基)
    call    listSavFilename         ; List backup    filenames.
                                    ;  Max 10 line in a screen. From the screen line 4 to    line 13(The 5th    line to    the 14th line).
                                    ;
                                    ; Input parameter:
                                    ;   a:
                                    ;      Start    offset of the file entries.
                                    ;      (e.g.    Set a=0x20, then list will start from 0x20 item    of the entry list)
                                    ;      (Base    value is 0)
                                    ;
    ret
; ---------------------------------------------------------------------------

.cursorY_MoveDn:
    ld      [byte_C03F], a          ; 菜单光标的Y坐标(在BgMap中的行偏移)
                                    ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    ret
; ---------------------------------------------------------------------------

.notKeyHold_DirD:
    bit     6, a
    jr      z, .notKeyHold_DirU

.KeyHold_DirUp:
    ld      a, 6
    ld      [byte_C02C], a          ; Restore/Del界面的按键响应函数中的一个CD时间，用来防止过快响应持续按住的上/下键
                                    ; 每次上下键读入后，这个值会设为6，然后每帧跑到该函数时，会对该值 -1，直到0为止。
                                    ; 为0后可以再次接受按住不放的上/下按键输入。
    ld      a, [byte_C029]          ; 在Restore或Delete界面，光标选中的备份文件的顺序号
                                    ; (不是当前屏幕内的序号，是在所有备份中的顺序号)
                                    ; 这个序号>=0，且<[0xC763]
    sub     1
    ret     c                       ; return if cursor pointed to    the 1st    backup file.
                                    ; return if [0xC029]-1 < 0

.notTheFirstFile:                   ; 在Restore或Delete界面，光标选中的备份文件的顺序号
    ld      [byte_C029], a          ; (不是当前屏幕内的序号，是在所有备份中的顺序号)
                                    ; 这个序号>=0，且<[0xC763]
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
    sub     8
    cp      $30    ; '0'
    jr      nc, .cursorY_MoveUp     ; Jump if [0xC03F]>=0x30

.scrollFilelstUp:                   ; 在Restore或Delete界面，备份文件名列表的第1行对应的全局文件编号
    ld      a, [byte_C028]          ; (全局文件编号，以0为基)
    dec     a                       ; 这里没有检查下限?
                                    ;   因为前面代码中，已经读取了[0xC029]的内容，确保了光标所指的不是全局第0条记录。
                                    ;   所以这里可以无需保护。
    ld      [byte_C028], a          ; 在Restore或Delete界面，备份文件名列表的第1行对应的全局文件编号
                                    ; (全局文件编号，以0为基)
    call    listSavFilename         ; List backup    filenames.
                                    ;  Max 10 line in a screen. From the screen line 4 to    line 13(The 5th    line to    the 14th line).
                                    ;
                                    ; Input parameter:
                                    ;   a:
                                    ;      Start    offset of the file entries.
                                    ;      (e.g.    Set a=0x20, then list will start from 0x20 item    of the entry list)
                                    ;      (Base    value is 0)
                                    ;
    ret
; ---------------------------------------------------------------------------

.cursorY_MoveUp:
    ld      [byte_C03F], a          ; 菜单光标的Y坐标(在BgMap中的行偏移)
                                    ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    ret
; ---------------------------------------------------------------------------

.notKeyHold_DirU:
    ld      a, [byte_C001]          ; 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
                                    ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A
    bit     1, a
    jr      z, .notKeyDown_B        ; 如果[$C001].bit1是0(当前周期"B"键没有被按下)，就跳

.KeyDown_B:
    call    displayMainMenu
    ret
; ---------------------------------------------------------------------------

.notKeyDown_B:
    bit     0, a
    ret     z                       ; 如果[$C000].bit0是0(当前"A"键没有按住状态)，就直接返回

.KeyDown_A:
    call    playSfxOK
    ld      a, [byte_C029]          ; 在Restore或Delete界面，光标选中的备份文件的顺序号
                                    ; (不是当前屏幕内的序号，是在所有备份中的顺序号)
                                    ; 这个序号>=0，且<[0xC763]
    inc     a
    ld      [byte_C02B], a          ; 在Restore或Delete界面，按A键时，光标选中的备份文件的顺序号+1
                                    ; (不是当前屏幕内的序号，是在所有备份中的顺序号)
                                    ; 这个序号>=1，且<=([0xC763])
    ld      a, [byte_C02A]          ; 该变量为0，表示主菜单中选了Delete
                                    ; 该变量为1，表示主菜单中选了Restore
    or      a
    jr      z, .doDelete

.doRestore:                         ; 选好要恢复的备份，按A后，出现的Working等待处理中界面
    call    dispRstoWorking
    jr      .back2MainMenu
; ---------------------------------------------------------------------------

.doDelete:
    ld      a, 2
    ld      [byte_C041], a          ; 该变量为0，表示主菜单中选了GameCart SaveDelete删除游戏卡存档
                                    ; 该变量为1，主菜单中选了Reset格式化
                                    ; 该变量为2, 主菜单中选了Delete Backup删除备份文件
    call    dispConfirmUI           ; Display confirmation UI for  Backup Delete /    GameCart_Save_Delete / FactoryReset.
    ret
; ---------------------------------------------------------------------------

.back2MainMenu:
    call    displayMainMenu
    ret
; End of function keyRestoreDelUI

; ---------------------------------------------------------------------------
strFmtSvName_S: db "  %d.%s",$A,0
strFmtSvName_L: db " %d.%s",$A,0
strRestore:     db "      Restore",0
strDelete:      db "       Delete",0
