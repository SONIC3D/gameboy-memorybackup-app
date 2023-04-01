;SECTION "SubMenu Module Save DeleteReset", ROM0[$0C09]

; =============== S U B R O U T I N E =======================================

; Display confirmation UI for Backup Delete / GameCart_Save_Delete / FactoryReset.

dispConfirmUI:
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

.setPrintOffset:
    ld      a, 0
    ld      [byte_C00E], a          ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      a, 6
    ld      [byte_C00F], a          ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf

.checkMode:                         ; 该变量为0，表示主菜单中选了GameCart SaveDelete删除游戏卡存档
    ld      a, [byte_C041]          ; 该变量为1，主菜单中选了Reset格式化
                                    ; 该变量为2, 主菜单中选了Delete Backup删除备份文件
    or      a
    jr      z, .dispErsGameSv

.isSelectedReset:
    cp      1
    jr      z, .dispFactoryRst

.dispBackupDel:                     ; "     Delete     "
    ld      hl, strDelete_2
    jr      .storeSP
; ---------------------------------------------------------------------------

.dispErsGameSv:
    ld      hl, strEraseGameSv      ; "Erase game save "
    jr      .storeSP
; ---------------------------------------------------------------------------

.dispFactoryRst:
    ld      hl, strReset            ; "      Reset     "

.storeSP:
    ld      [word_C00C], sp         ; if sp=0xFFFC, then [0xC00C]=0xFC, [0xC00D]=0xFF

.saveRegs:
    ld      sp, $C00C
    push    hl                      ; [0xC00A]=hl
                                    ;    [0xC00A] = h = 待作为格式化参数打印的字符串地址的高8bit
                                    ;    [0xC00B] = l = 待作为格式化参数打印的字符串地址的高8bit
    push    de                      ; [0xC008]=de
    push    bc                      ; [0xC006]=bc
    push    af                      ; [0xC004] = af

.restoreSP:
    ld      a, [word_C00C]
    ld      l, a
    ld      a, [word_C00C+1]
    ld      h, a
    ld      sp, hl

.prepareParam1:                     ; 在0x691的strPrintf函数中，用来作为第1个格式化符号对应的参数
    ld      hl, C010_PrintArgv1
    ld      a, [byte_C00B]
    ldi     [hl], a                 ; 待作为格式化参数打印的字符串地址的低8bit
    ld      a, [byte_C00A]          ; enterSvcMode中用来临时存放28SF040 Manufacturer ID
    ldi     [hl], a                 ; 待作为格式化参数打印的字符串地址的高8bit
    ld      hl, strYesNoConfirm
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

.setCursorPos:
    ld      a, $50 ; 'P'
    ld      [byte_C03F], a          ; 菜单光标的Y坐标(在BgMap中的行偏移)
                                    ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    ld      a, $48 ; 'H'
    ld      [byte_C03E], a          ; 菜单光标的X坐标(在BgMap中的列偏移)
                                    ; 0x79: 对应图标主菜单的最右侧一列的列偏移

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

.setNextVBlkFunc:                   ; 0xC82 will be jumped to on next vblank.(By code at 0x264)
    ld      a, $82 ; '�'
    ld      [byte_C035], a          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
    ld      a, $C
    ld      [byte_C036], a          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。

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
    ret
; End of function dispConfirmUI


; =============== S U B R O U T I N E =======================================

; Referenced as    function pointer by code at 0xC70.
; GameCart存档删除/FactoryReset界面的按键响应逻辑

keySvDelRstlUI:
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
    ld      a, $10                  ;        0x10是向右箭头的Tile
    ld      [byte_FE02], a          ; SpriteAttr[0].Tile/Pattern_Number

.checkKeys:                         ; 读取按键
    call    joypadRead              ; 返回值：
                                    ;   (以下返回值Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A)
                                    ;   a: 本周期按下了的按键(这些键的bit被置为1)
                                    ;   b: 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
    ld      a, [byte_C001]          ; 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
                                    ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A
    bit     7, a
    jr      z, .notKey_DirDown      ; 如果[$C001].bit7是0(本周期没有按"下"键)，就跳

.KeyDown_DirDown:                   ; 菜单光标的Y坐标(在BgMap中的行偏移)
    ld      a, [byte_C03F]          ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------> 对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    add     a, 8
    cp      $51    ; 'Q'
    ret     nc                      ; Return if the cursor already pointed to the last item.

.movCursorToNo:                     ; 菜单光标的Y坐标(在BgMap中的行偏移)
    ld      [byte_C03F], a          ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------> 对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    call    playSfxOK
    ret
; ---------------------------------------------------------------------------

.notKey_DirDown:
    bit     6, a
    jr      z, .notKey_DirUp        ; 如果[$C001].bit6是0(本周期没有按"上"键)，就跳

.KeyDown_DirUp:                     ; 菜单光标的Y坐标(在BgMap中的行偏移)
    ld      a, [byte_C03F]          ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------> 对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    sub     8
    cp      $48    ; 'H'
    ret     c                       ; Return if the cursor already pointed to the first item.

.movCursorToYes:                    ; 菜单光标的Y坐标(在BgMap中的行偏移)
    ld      [byte_C03F], a          ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------> 对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    call    playSfxOK
    ret
; ---------------------------------------------------------------------------

.notKey_DirUp:
    bit     0, a
    jr      z, .notKey_A             ; 如果[$C001].bit0是0(本周期没有按"A"键)，就跳

.KeyDown_A:
    call    playSfxOK
    ld      a, [byte_C03F]          ; 菜单光标的Y坐标(在BgMap中的行偏移)
                                    ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------> 对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    cp      $50    ; 'P'            ; 光标位置和No所在的BgMap行偏移值0x50作比较
    jr      z, .backToMainMenu

.cursorPoint2Yes:                   ; 该变量为0，表示主菜单中选了GameCart SaveDelete删除游戏卡存档
    ld      a, [byte_C041]          ; 该变量为1，主菜单中选了Reset格式化
                                    ; 该变量为2, 主菜单中选了Delete Backup删除备份文件
    or      a
    jr      z, .goGameSvDelete

.isSelectedReset:
    cp      1
    jr      z, .goFactoryReset

.goDeleteBackup:                    ; 执行删除Backup的某个存档
    call    doDeleteBackup
    jr      .backToMainMenu
; ---------------------------------------------------------------------------

.goGameSvDelete:
    call    doGameCartSvDel         ; 执行删除卡带内的存档
    jr      .backToMainMenu
; ---------------------------------------------------------------------------

.goFactoryReset:
    call    doFactoryReset          ; 对28SF040进行全片擦除，然后跳回程序头部重新开始卡带程序
                                    ; 因为擦除后，编目部分校验和不对，程序从头执行后会另外触发初始化逻辑:
                                    ;   0x24E
    jr      .backToMainMenu
; ---------------------------------------------------------------------------

.notKey_A:
    bit     1, a
    ret     z                       ; 本周期没有按下"B"键，则直接返回
                                    ; 如果按下了，则继续下面的返回主菜单流程

.backToMainMenu:
    call    displayMainMenu
    ret
; End of function keySvDelRstlUI


; =============== S U B R O U T I N E =======================================

; 执行删除卡带内的存档

doGameCartSvDel:
    ld      a, $C0 ; '�'
    ld      [IOPort_0x2000], a      ; 0x2000 I/O Port:
                                    ;   Write
                                    ;    bit7..bit6:
                                    ;      $C0:
                                    ;        Switch to Game Cart Mode. All access to address like $4000 passed to Game Cart.
                                    ;      $80:
                                    ;        Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;    bit5:
                                    ;      Unknown. Always be 0.
                                    ;    bit4..bit0:
                                    ;      bit4..bit0 of input value to the 74HC373. Which would  be latched and out put to A18..A14 of 28SF040.
    ld      d, 0

.nextRamBank:
    ld      a, d
    ld      [byte_4000], a
    ld      hl, $A000
    ld      bc, $2000
    xor     a
    call    memFillWith_A           ; 赋值源a中数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to fill: bc
                                    ;   Value: a
                                    ;   DestAddr:[hl]
    inc     d
    ld      a, [byte_C038]          ; 存放的是当前插的游戏卡带，最后可用的SRAM的Bank编号数量(最后一个可用Bank的序号+1)
    cp      d
    jr      nc, .nextRamBank
    ld      a, $80 ; '�'
    ld      [IOPort_0x2000], a      ; 0x2000 I/O Port:
                                    ;   Write
                                    ;    bit7..bit6:
                                    ;      $C0:
                                    ;        Switch to Game Cart Mode. All access to address like $4000 passed to Game Cart.
                                    ;      $80:
                                    ;        Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;    bit5:
                                    ;      Unknown. Always be 0.
                                    ;    bit4..bit0:
                                    ;      bit4..bit0 of input value to the 74HC373. Which would  be latched and out put to A18..A14 of 28SF040.
    ret
; End of function doGameCartSvDel


; =============== S U B R O U T I N E =======================================

; 对28SF040进行全片擦除，然后跳回程序头部重新开始卡带程序
; 因为擦除后，编目部分校验和不对，程序从头执行后会另外触发初始化逻辑:
;   0x24E

doFactoryReset:
    call    delay1second            ; 这个函数设置Timer为4096Hz，然后设置Timer计数器初始为0x80(即还剩0x80计数到0xFF、发生中断)
                                    ; 然后发生0x20次中断后返回，即0x80 * 0x20 = 0x1000即4096计数，即延时1秒后函数返回。
    call    SF040_ChipErase
    jp      GlobalEntry_100
; End of function doFactoryReset
