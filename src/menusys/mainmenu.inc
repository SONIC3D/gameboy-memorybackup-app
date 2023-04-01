; =============== S U B R O U T I N E =======================================


displayMainMenu:

; FUNCTION CHUNK AT 11EE SIZE 0000005B BYTES

    call    clearOAM                ; Fill [0xFE00-0xFE9F] OAM address range with data 0x00.
    ld      a, [C774_OptVal02]      ; Option选项值，存放颜色显示方式，非0时，表示用反转色彩。
                                    ; 初始化为0xFF，即反转色彩方式显示。
    or      a
    jr      z, .optNormalColor

.optReverseColor:
    ld      a, $1B
    ld      [byte_FF47], a          ; FF47 - BGP - BG Palette Data (R/W) - Non CGB Mode Only
                                    ; This register assigns gray shades to the color numbers of the BG and Window    tiles.
                                    ;   Bit 7-6 - Shade for Color  Number 3
                                    ;   Bit 5-4 - Shade for Color  Number 2
                                    ;   Bit 3-2 - Shade for Color  Number 1
                                    ;   Bit 1-0 - Shade for Color  Number 0
                                    ; The    four possible gray shades are:
                                    ;   0     White
                                    ;   1     Light gray
                                    ;   2     Dark gray
                                    ;   3     Black
                                    ; In CGB Mode    the Color Palettes are taken from CGB Palette Memory instead.
    ld      a, $AA ; '�'
    ld      [byte_FF48], a          ; FF48 - OBP0 - Object Palette 0 Data    (R/W) -    Non CGB    Mode Only
                                    ;   This register assigns gray shades    for sprite palette 0. It works exactly as BGP (FF47), except that the lower two    bits aren't used because sprite data 00 is transparent.
    jr      .readOption
; ---------------------------------------------------------------------------

.optNormalColor:
    ld      a, $E4 ; '�'
    ld      [byte_FF47], a          ; FF47 - BGP - BG Palette Data (R/W) - Non CGB Mode Only
                                    ; This register assigns gray shades to the color numbers of the BG and Window    tiles.
                                    ;   Bit 7-6 - Shade for Color  Number 3
                                    ;   Bit 5-4 - Shade for Color  Number 2
                                    ;   Bit 3-2 - Shade for Color  Number 1
                                    ;   Bit 1-0 - Shade for Color  Number 0
                                    ; The    four possible gray shades are:
                                    ;   0     White
                                    ;   1     Light gray
                                    ;   2     Dark gray
                                    ;   3     Black
                                    ; In CGB Mode    the Color Palettes are taken from CGB Palette Memory instead.
    ld      a, $55 ; 'U'
    ld      [byte_FF48], a          ; FF48 - OBP0 - Object Palette 0 Data    (R/W) -    Non CGB    Mode Only
                                    ;   This register assigns gray shades    for sprite palette 0. It works exactly as BGP (FF47), except that the lower two    bits aren't used because sprite data 00 is transparent.

.readOption:
    ld      a, [C773_OptVal01]      ; Option选项值，存放菜单显示方式，非0时，表示用图标显示主菜单，为0时表示用纯文字。
                                    ; 初始化为0xFF，即图标方式显示。
    or      a
    jp      nz, menuInIcon

.menuInText:
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
    ld      hl, strMegaMemTitle      ; "   GB MegaMem v1.0"
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
    ld      a, 5
    ld      [byte_C00F], a          ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      hl, strMainMenuTxt      ; "  1.Backup\n  2.Restore\n  3.Delete\n  "...
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
    call    calcSpaceUsage          ; 计算备份空间的占用情况，将计算值存入[0xC03C]，数值实际应该在[0-$40]内，即[0-64]。
                                    ; 计算[0xD800-0xD8FF]的范围，共有多少个字节非0，该计数存入bc。
                                    ; [0xC03C] = (int)((bc+8)/32);
    call    drawCapacityBar         ; 根据0xC03C内的值，画出对应的存档空间占用比例的进度条。
    ld      a, $38 ; '8'
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
    push    af
    ld      a, [byte_FFFF]          ; FFFF - IE - Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt    Enable    (INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt    Enable    (INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt    Enable    (INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt    Enable    (INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt    Enable    (INT 60h)  (1=Enable)
    ld      [byte_C039], a          ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt    Enable (R/W)
    pop     af
    ld      a, $5D ; ']'            ; 0xB5D will be jumped to on next vblank.(By code at 0x264)
    ld      [byte_C035], a          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
    ld      a, $B
    ld      [byte_C036], a          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
    push    af
    ld      a, [byte_C039]          ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt    Enable (R/W)
    ld      [byte_FFFF], a          ; FFFF - IE - Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt    Enable    (INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt    Enable    (INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt    Enable    (INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt    Enable    (INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt    Enable    (INT 60h)  (1=Enable)
    pop     af
    ret
; End of function displayMainMenu


; =============== S U B R O U T I N E =======================================

; Referenced as function pointer by code at 0xB4B.
; 文字界面版的主菜单的按键响应逻辑函数

keyMainMenuTxt:
; FUNCTION CHUNK AT 0D18 SIZE 0000001B BYTES

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
    ld      a, $10
    ld      [byte_FE02], a          ; SpriteAttr[0].Tile/Pattern_Number
    call    joypadRead              ; 读取按键
                                    ; 返回值：
                                    ;   (以下返回值Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A)
                                    ;   a: 本周期按下了的按键(这些键的bit被置为1)
                                    ;   b: 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
    ld      a, [byte_C001]          ; 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
                                    ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A
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
    add     a, 8
    cp      $61
    jr      c, .cursorToTop
    ld      a, $38

.cursorToTop:
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
    sub     8
    cp      $38
    jr      nc, .cursorToBottom
    ld      a, $60

.cursorToBottom:
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

.notKeyDown_DirU:
    bit     0, a
    ret     z                       ; 如果[$C001].bit0是0(本周期没有按"A"键)，就直接退出函数

.KeyDown_A:
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
    cp      $38    ; '8'
    jp      z, mainMenuItem_1

.notMenuItm_1:
    cp      $40    ; '@'
    jp      z, mainMenuItem_2

.notMenuItm_2:
    cp      $48    ; 'H'
    jp      z, mainMenuItem_3

.notMenuItm_3:
    cp      $50    ; 'P'
    jp      z, mainMenuItem_4

.notMenuItm_4:
    cp      $58    ; 'X'
    jp      z, mainMenuItem_5

.notMenuItm_5:
    jp      mainMenuItem_6
; ---------------------------------------------------------------------------

mainMenuItem_1:
    ld      a, [byte_C038]          ; 存放的是当前插的游戏卡带，最后可用的SRAM的Bank编号数量(最后一个可用Bank的序号+1)
    or      a
    jr      z, .backupError         ; 游戏卡带没有SRAM

.hasOnCartSRAM_1:                   ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。
    ld      a, [byte_C763]
    cp      $63    ; 'c'
    jr      nc, .backupError        ; 备份存档数已经>=99，就跳错误

.displayBakUI:                      ; 备份存档功能的界面，输入存档名称等
    call    dispBackupUI
    ret
; ---------------------------------------------------------------------------

.backupError:
    call    playSfxError
    ret
; ---------------------------------------------------------------------------

mainMenuItem_2:
    ld      a, [byte_C763]          ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。
    or      a
    jr      z, .restoreError        ; 没有任何备份存档

.bakFileExist_1:                    ; 存放的是当前插的游戏卡带，最后可用的SRAM的Bank编号数量(最后一个可用Bank的序号+1)
    ld      a, [byte_C038]
    or      a
    jr      z, .restoreError        ; 游戏卡带没有SRAM

.dispRestoreUI:
    ld      a, 1
    ld      [byte_C02A], a          ; 该变量为0，表示主菜单中选了Delete
                                    ; 该变量为1，表示主菜单中选了Restore
    call    dispRestorDelUI         ; Display Restore/Delete UI
                                    ; [0xC02A] = 1: MainMenu -> Restore
                                    ; [0xC02A] = 0: MainMenu -> Delete
    ret
; ---------------------------------------------------------------------------

.restoreError:
    call    playSfxError
    ret
; ---------------------------------------------------------------------------

mainMenuItem_3:
    ld      a, [byte_C763]          ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。
    or      a
    jr      z, .noBackupFile_2

.bakFileExist_2:
    xor     a
    ld      [byte_C02A], a          ; 该变量为0，表示主菜单中选了Delete
                                    ; 该变量为1，表示主菜单中选了Restore
    call    dispRestorDelUI         ; Display Restore/Delete UI
                                    ; [0xC02A] = 1: MainMenu -> Restore
                                    ; [0xC02A] = 0: MainMenu -> Delete
    ret
; ---------------------------------------------------------------------------

.noBackupFile_2:
    call    playSfxError
    ret
; ---------------------------------------------------------------------------

mainMenuItem_4:
    call    dispOptionUI            ; Display Option Menu
    ret
; End of function keyMainMenuTxt


; ===========================================================================
; ===========================================================================
; DONE: Insert Save Del/Reset menu code here
INCLUDE "menusys/save_del_rst_menu.inc"


; ===========================================================================
; ===========================================================================
SECTION "MainMenu_Part_2", ROM0[$0D18]
; ---------------------------------------------------------------------------
; START OF FUNCTION CHUNK FOR keyMainMenuTxt
mainMenuItem_5:
    ld      a, [byte_C038]          ; 存放的是当前插的游戏卡带，最后可用的SRAM的Bank编号数量(最后一个可用Bank的序号+1)
    or      a
    jr      z, .noOnCartSRAM_2

.hasOnCartSRAM_2:
    xor     a
    ld      [byte_C041], a          ; 该变量为0，表示主菜单中选了GameCart SaveDelete删除游戏卡存档
                                    ; 该变量为1，主菜单中选了Reset格式化
                                    ; 该变量为2, 主菜单中选了Delete Backup删除备份文件
    call    dispConfirmUI           ; Display confirmation UI for    Backup Delete /    GameCart_Save_Delete / FactoryReset.
    ret
; ---------------------------------------------------------------------------

.noOnCartSRAM_2:
    call    playSfxError
    ret
; ---------------------------------------------------------------------------

mainMenuItem_6:
    ld      a, 1
    ld      [byte_C041], a          ; 该变量为0，表示主菜单中选了GameCart SaveDelete删除游戏卡存档
                                    ; 该变量为1，主菜单中选了Reset格式化
                                    ; 该变量为2, 主菜单中选了Delete Backup删除备份文件
    call    dispConfirmUI           ; Display confirmation UI for    Backup Delete /    GameCart_Save_Delete / FactoryReset.
    ret
; END OF FUNCTION CHUNK FOR keyMainMenuTxt
; ---------------------------------------------------------------------------

; ===========================================================================
; ===========================================================================
INCLUDE "menusys/mainmenu_string.inc"






; ===========================================================================
; ===========================================================================
; DONE: Insert Backup Restore menu code here
INCLUDE "menusys/backup_menu.inc"
INCLUDE "menusys/restore_menu.inc"





; ===========================================================================
; ===========================================================================
SECTION "MainMenu_Part_3", ROM0[$11EE]
; START OF FUNCTION CHUNK FOR displayMainMenu
menuInIcon:
    ld      hl, $9800
    ld      bc, $400
    ld      a, $7F ; ''
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
    ld      hl, strMegaMemTitle     ; "   GB MegaMem v1.0"
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
    ld      hl, strMainMenuIcon
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
    call    calcSpaceUsage          ; 计算备份空间的占用情况，将计算值存入[0xC03C]，数值实际应该在[0-$40]内，即[0-64]。
                                    ; 计算[0xD800-0xD8FF]的范围，共有多少个字节非0，该计数存入bc。
                                    ; [0xC03C] = (int)((bc+8)/32);
    call    drawCapacityBar         ; 根据0xC03C内的值，画出对应的存档空间占用比例的进度条。
    xor     a
    ld      [byte_C02D], a          ; 存放图标形式的MainMenu界面的被光标选中的项目的ID，取值范围0-5。
                                    ; 第1行3个图标从左到右依次是0-2，第2行从左到右依次是3-5。
    call    dispCsorIconTxt         ; 设置Icon风格主界面中光标位置对应的屏幕下方文字
                                    ; [0xC02D]指定了当前选中的项目序号，取值范围从0-5(待确认)
                                    ;   0-2对应第1行从左到右3个图标
                                    ;   3-5对应第2行从左到右3个图标
    ld      a, $18
    ld      [byte_C03E], a          ; 菜单光标的X坐标(在BgMap中的列偏移)
                                    ; 0x79: 对应图标主菜单的最右侧一列的列偏移
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
    push    af
    ld      a, [byte_FFFF]          ; FFFF - IE -    Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt    Enable    (INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt    Enable    (INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt    Enable    (INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt    Enable    (INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt    Enable    (INT 60h)  (1=Enable)
    ld      [byte_C039], a          ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt    Enable (R/W)
    pop     af
    ld      a, $49 ; 'I'            ; 0x1249 will be jumped to on next vblank.(By code at 0x264)
    ld      [byte_C035], a          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
    ld      a, $12
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
; END OF FUNCTION CHUNK FOR displayMainMenu

; =============== S U B R O U T I N E =======================================

; Referenced as function pointer by code at 0x1237.
; 图标界面版的主菜单的按键响应逻辑函数

keyMainMenuIcon:
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
    ld      [byte_FE04], a          ; SpriteAttr[1].PosY
    ld      [byte_FE08], a
    ld      [byte_FE0C], a
    add     a, 8
    ld      [byte_FE10], a
    ld      [byte_FE14], a
    ld      [byte_FE18], a
    ld      [byte_FE1C], a
    add     a, 8
    ld      [byte_FE20], a
    ld      [byte_FE24], a
    ld      [byte_FE28], a
    ld      [byte_FE2C], a
    add     a, 8
    ld      [byte_FE30], a
    ld      [byte_FE34], a
    ld      [byte_FE38], a
    ld      [byte_FE3C], a
    ld      a, [byte_C03E]          ; 菜单光标的X坐标(在BgMap中的列偏移)
                                    ; 0x79: 对应图标主菜单的最右侧一列的列偏移
    ld      [byte_FE01], a          ; SpriteAttr[0].PosX
    ld      [byte_FE11], a
    ld      [byte_FE21], a
    ld      [byte_FE31], a
    add     a, 8
    ld      [byte_FE05], a          ; SpriteAttr[1].PosX
    ld      [byte_FE15], a
    ld      [byte_FE25], a
    ld      [byte_FE35], a
    add     a, 8
    ld      [byte_FE09], a
    ld      [byte_FE19], a
    ld      [byte_FE29], a
    ld      [byte_FE39], a
    add     a, 8
    ld      [byte_FE0D], a
    ld      [byte_FE1D], a
    ld      [byte_FE2D], a
    ld      [byte_FE3D], a
    xor     a
    ld      [byte_FE02], a          ; SpriteAttr[0].Tile/Pattern_Number
    ld      [byte_FE06], a          ; SpriteAttr[1].Tile/Pattern_Number
    ld      [byte_FE0A], a
    ld      [byte_FE0E], a
    ld      [byte_FE12], a
    ld      [byte_FE16], a
    ld      [byte_FE1A], a
    ld      [byte_FE1E], a
    ld      [byte_FE22], a
    ld      [byte_FE26], a
    ld      [byte_FE2A], a
    ld      [byte_FE2E], a
    ld      [byte_FE32], a
    ld      [byte_FE36], a
    ld      [byte_FE3A], a
    ld      [byte_FE3E], a
    ld      a, $80 ; '�'
    ld      [byte_FE03], a          ; SpriteAttr[0].Attributes/Flags:
                                    ;   Bit7   OBJ-to-BG Priority    (0=OBJ Above BG, 1=OBJ Behind BG color 1-3)
                                    ;         (Used for both BG and Window. BG color 0 is always    behind OBJ)
                                    ;   Bit6   Y flip         (0=Normal, 1=Vertically mirrored)
                                    ;   Bit5   X flip         (0=Normal, 1=Horizontally mirrored)
                                    ;   Bit4   Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
                                    ;   Bit3   Tile VRAM-Bank  **CGB Mode    Only**       (0=Bank 0, 1=Bank 1)
                                    ;   Bit2-0 Palette number  **CGB Mode    Only**       (OBP0-7)
    ld      [byte_FE07], a          ; SpriteAttr[1].Attributes/Flags
    ld      [byte_FE0B], a
    ld      [byte_FE0F], a
    ld      [byte_FE13], a
    ld      [byte_FE17], a
    ld      [byte_FE1B], a
    ld      [byte_FE1F], a
    ld      [byte_FE23], a
    ld      [byte_FE27], a
    ld      [byte_FE2B], a
    ld      [byte_FE2F], a
    ld      [byte_FE33], a
    ld      [byte_FE37], a
    ld      [byte_FE3B], a
    ld      [byte_FE3F], a
    call    joypadRead              ; 读取按键
                                    ; 返回值：
                                    ;   (以下返回值Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A)
                                    ;   a: 本周期按下了的按键(这些键的bit被置为1)
                                    ;   b: 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
    ld      a, [byte_C001]          ; 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
                                    ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A
    bit     7, a
    jr      z, .notKeyDown_DirD     ; 如果[$C001].bit7是0(本周期没有按"下"键)，就跳

.KeyDown_DirDown:                   ; 菜单光标的Y坐标(在BgMap中的行偏移)
    ld      a, [byte_C03F]          ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    add     a, $28 ; '('
    cp      $51    ; 'Q'
    ret     nc                      ; 如果已经是第2行Icon了，就返回

.setCursorToRow2:                   ; 菜单光标的Y坐标(在BgMap中的行偏移)
    ld      [byte_C03F], a          ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset

.setSelIconId_1:                    ; 存放图标形式的MainMenu界面的被光标选中的项目的ID，取值范围0-5。
    ld      a, [byte_C02D]          ; 第1行3个图标从左到右依次是0-2，第2行从左到右依次是3-5。
    add     a, 3
    ld      [byte_C02D], a          ; 存放图标形式的MainMenu界面的被光标选中的项目的ID，取值范围0-5。
                                    ; 第1行3个图标从左到右依次是0-2，第2行从左到右依次是3-5。
    jr      .dispIconDesc
; ---------------------------------------------------------------------------

.notKeyDown_DirD:
    bit     6, a
    jr      z, .notKeyDown_DirU

.KeyDown_DirUp:                     ; 菜单光标的Y坐标(在BgMap中的行偏移)
    ld      a, [byte_C03F]          ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset
    sub     $28    ; '('
    cp      $28    ; '('
    ret     c                       ; 如果已经是第1行Icon了，就返回

.setCursorToRow1:                   ; 菜单光标的Y坐标(在BgMap中的行偏移)
    ld      [byte_C03F], a          ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                                    ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                                    ; 0x38  对应主菜单1 Backup
                                    ; 0x40  对应主菜单2 Restore
                                    ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                                    ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                                    ; 0x58  对应主菜单5 Delete Game Save
                                    ; 0x60  对应主菜单6 Reset

.setSelIconId_2:                    ; 存放图标形式的MainMenu界面的被光标选中的项目的ID，取值范围0-5。
    ld      a, [byte_C02D]          ; 第1行3个图标从左到右依次是0-2，第2行从左到右依次是3-5。
    sub     3
    ld      [byte_C02D], a          ; 存放图标形式的MainMenu界面的被光标选中的项目的ID，取值范围0-5。
                                    ; 第1行3个图标从左到右依次是0-2，第2行从左到右依次是3-5。
    jr      .dispIconDesc
; ---------------------------------------------------------------------------

.notKeyDown_DirU:
    bit     4, a
    jr      z, .notKeyDown_DirR

.KeyDown_DirRt:                      ; 菜单光标的X坐标(在BgMap中的列偏移)
    ld      a, [byte_C03E]          ; 0x79: 对应图标主菜单的最右侧一列的列偏移
    add     a, $30
    cp      $79
    ret     nc                      ; 如果a>=0x79，就返回

.mvCursor2NxtCol:                    ; 菜单光标的X坐标(在BgMap中的列偏移)
    ld      [byte_C03E], a          ; 0x79: 对应图标主菜单的最右侧一列的列偏移

.setSelIconId_3:                    ; 存放图标形式的MainMenu界面的被光标选中的项目的ID，取值范围0-5。
    ld      a, [byte_C02D]          ; 第1行3个图标从左到右依次是0-2，第2行从左到右依次是3-5。
    inc     a
    ld      [byte_C02D], a          ; 存放图标形式的MainMenu界面的被光标选中的项目的ID，取值范围0-5。
                                    ; 第1行3个图标从左到右依次是0-2，第2行从左到右依次是3-5。
    jr      .dispIconDesc
; ---------------------------------------------------------------------------

.notKeyDown_DirR:
    bit     5, a
    jr      z, .notKeyDown_DirL

.KeyDown_DirLt:                      ; 菜单光标的X坐标(在BgMap中的列偏移)
    ld      a, [byte_C03E]          ; 0x79: 对应图标主菜单的最右侧一列的列偏移
    sub     $30
    cp      $71
    ret     nc                      ; 如果a>=0x71，就返回(a减掉0x30后，因为小于0，而负溢出继而变大的情况)

.mvCsor2PrevCol:                    ; 菜单光标的X坐标(在BgMap中的列偏移)
    ld      [byte_C03E], a          ; 0x79: 对应图标主菜单的最右侧一列的列偏移

.setSelIconId_4:                    ; 存放图标形式的MainMenu界面的被光标选中的项目的ID，取值范围0-5。
    ld      a, [byte_C02D]          ; 第1行3个图标从左到右依次是0-2，第2行从左到右依次是3-5。
    dec     a
    ld      [byte_C02D], a          ; 存放图标形式的MainMenu界面的被光标选中的项目的ID，取值范围0-5。
                                    ; 第1行3个图标从左到右依次是0-2，第2行从左到右依次是3-5。
    jr      .dispIconDesc
; ---------------------------------------------------------------------------

.notKeyDown_DirL:
    bit     0, a
    ret     z                       ; 如果本周期没有按"A"键)，就返回

.KeyDown_A:
    call    playSfxOK

.chkSelectedItem:                   ; 存放图标形式的MainMenu界面的被光标选中的项目的ID，取值范围0-5。
    ld      a, [byte_C02D]          ; 第1行3个图标从左到右依次是0-2，第2行从左到右依次是3-5。
    or      a
    jr      z, .selMenuItm_1

.chkIfMenuItm_2:
    cp      1
    jr      z, .selMenuItm_2

.chkIfMenuItm_3:
    cp      2
    jr      z, .selMenuItm_3

.chkIfMenuItm_4:
    cp      3
    jr      z, .selMenuItm_4

.chkIfMenuItm_5:
    cp      4
    jr      z, .selMenuItm_5

.goSelMenuItm_6:
    jr      .selMenuItm_6
; ---------------------------------------------------------------------------

.dispIconDesc:
    call    dispCsorIconTxt         ; 设置Icon风格主界面中光标位置对应的屏幕下方文字
                                    ; [0xC02D]指定了当前选中的项目序号，取值范围从0-5(待确认)
                                    ;   0-2对应第1行从左到右3个图标
                                    ;   3-5对应第2行从左到右3个图标
    call    playSfxOK
    ret
; ---------------------------------------------------------------------------

.selMenuItm_1:
    jp      mainMenuItem_1
; ---------------------------------------------------------------------------
    ret
; ---------------------------------------------------------------------------

.selMenuItm_2:
    jp      mainMenuItem_2
; ---------------------------------------------------------------------------
    ret
; ---------------------------------------------------------------------------

.selMenuItm_3:
    jp      mainMenuItem_3
; ---------------------------------------------------------------------------
    ret
; ---------------------------------------------------------------------------

.selMenuItm_4:
    jp      mainMenuItem_4
; ---------------------------------------------------------------------------
    ret
; ---------------------------------------------------------------------------

.selMenuItm_5:
    jp      mainMenuItem_5
; ---------------------------------------------------------------------------
    ret
; ---------------------------------------------------------------------------

.selMenuItm_6:
    jp      mainMenuItem_6
; End of function keyMainMenuIcon

; ---------------------------------------------------------------------------
    ret

; =============== S U B R O U T I N E =======================================

; 设置Icon风格主界面中光标位置对应的屏幕下方文字
; [0xC02D]指定了当前选中的项目序号，取值范围从0-5(待确认)
;   0-2对应第1行从左到右3个图标
;   3-5对应第2行从左到右3个图标

dispCsorIconTxt:
    ld      hl, arrStrPtr_147B
    ld      a, [byte_C02D]          ; 存放图标形式的MainMenu界面的被光标选中的项目的ID，取值范围0-5。
                                    ; 第1行3个图标从左到右依次是0-2，第2行从左到右依次是3-5。
    add     a, a
    ld      e, a
    ld      d, 0
    add     hl, de                  ; hl = 0x147B    + [0xC02D]
    ldi     a, [hl]
    ld      e, a
    ld      a, [hl]
    ld      d, a                    ; de = [HL+1][HL]
    push    de
    pop     hl                      ; hl = de
    ld      a, 0
    ld      [byte_C00E], a          ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      a, $D
    ld      [byte_C00F], a          ; [0xC00E][0xC00F] = 000D
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
; End of function dispCsorIconTxt

; ---------------------------------------------------------------------------
strMainMenuIcon:
    db  $20, $20, $89, $8A, $8B, $8C, $20, $20, $8D, $8E, $8F, $90, $20, $20, $BD, $BE, $BF, $C0,  $A; 0
    db  $20, $20, $95, $96, $97, $98, $20, $20, $99, $9A, $9B, $9C, $20, $20, $C9, $CA, $CB, $CC,  $A; 0
    db  $20, $20, $A1, $A2, $A3, $A4, $20, $20, $A5, $A6, $A7, $A8, $20, $20, $D5, $D6, $D7, $D8,  $A; 0
    db  $20, $20, $AD, $AE, $AF, $B0, $20, $20, $B1, $B2, $B3, $B4, $20, $20, $E1, $E2, $E3, $E4,  $A; 0
    db  $A          ; 0
    db  $20, $20, $91, $92, $93, $94, $20, $20, $C1, $C2, $C3, $C4, $20, $20, $B9, $BA, $BB, $BC,  $A; 0
    db  $20, $20, $9D, $9E, $9F, $A0, $20, $20, $CD, $CE, $CF, $D0, $20, $20, $C5, $C6, $C7, $C8,  $A; 0
    db  $20, $20, $A9, $AA, $AB, $AC, $20, $20, $D9, $DA, $DB, $DC, $20, $20, $D1, $D2, $D3, $D4,  $A; 0
    db  $20, $20, $B5, $B6, $B7, $B8, $20, $20, $E5, $E6, $E7, $E8, $20, $20, $DD, $DE, $DF, $E0,   0; 0

arrStrPtr_147B:
    db LOW(aBackup),        HIGH(aBackup)           ; db $87,$14 => 0x1487
    db LOW(aRestore),       HIGH(aRestore)          ; db $9D,$14 => 0x149D
    db LOW(aDelete_0),      HIGH(aDelete_0)         ; db $B3,$14 => 0x14B3
    db LOW(aOptions),       HIGH(aOptions)          ; db $C9,$14 => 0x14C9
    db LOW(aEraseGameSav_0),HIGH(aEraseGameSav_0)   ; db $DF,$14 => 0x14DF
    db LOW(aReset_0),       HIGH(aReset_0)          ; db $F5,$14 => 0x14F5

aBackup:            db "       Backup        ",0
aRestore:           db "       Restore       ",0
aDelete_0:          db "       Delete        ",0
aOptions:           db "       Options       ",0
aEraseGameSav_0:    db "   Erase game save   ",0
aReset_0:           db "        Reset        ",0
