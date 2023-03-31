SECTION "Variables", WRAM0
byte_C000::     ds 1        ; 本周期按下了的按键(这些键的bit被置为1)
                            ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A
byte_C001::     ds 1        ; 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
                            ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A

SECTION "VariablesC004", WRAM0[$C004]
byte_C004::     ds 1        ; 所有寄存器值的临时保存栈中，保存寄存器f的值
byte_C005::     ds 1        ; 所有寄存器值的临时保存栈中，保存寄存器a的值
byte_C006::     ds 1        ; 所有寄存器值的临时保存栈中，保存寄存器c的值
byte_C007::     ds 1        ; 所有寄存器值的临时保存栈中，保存寄存器b的值
byte_C008::     ds 1        ; 所有寄存器值的临时保存栈中，保存寄存器e的值
byte_C009::     ds 1        ; 所有寄存器值的临时保存栈中，保存寄存器d的值
byte_C00A::     ds 1        ; 所有寄存器值的临时保存栈中，保存寄存器l的值
byte_C00B::     ds 1        ; 所有寄存器值的临时保存栈中，保存寄存器h的值
word_C00C::     ds 2        ; 存放所有寄存器值的临时栈顶
byte_C00E::     ds 1        ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                            ; 具体参考函数:
                            ;   0x691 strPrintf
byte_C00F::     ds 1        ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                            ; 具体参考函数:
                            ;   0x691 strPrintf
C010_PrintArgv1::
                ds 1        ; 在0x691的strPrintf函数中，用来作为第1个格式化符号对应的参数


SECTION "VariablesC02E", WRAM0[$C02E]
byte_C02E::     ds 1        ; 存放Option界面的光标纵坐标，取值范围0-4
C02F_TmpOptVal1::           ; 存放Option界面用户点Save前，用户所设置的临时值。
                            ; 4个字节分别对应：使用图标菜单、反向色盘、音效开启、不允许重复文件名
                ds 1        ; 0
                ds 1        ; 1
                ds 1        ; 2
                ds 1        ; 3

SECTION "VariablesC035", WRAM0[$C035]
byte_C035::     ds 1        ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
byte_C036::     ds 1        ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
byte_C037::     ds 1        ; 每次VBlank函数发生时，该变量会被设成1
byte_C038::     ds 1        ; 存放的是当前插的游戏卡带，最后可用的SRAM的Bank编号数量(最后一个可用Bank的序号+1)
byte_C039::     ds 1        ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt    Enable (R/W)

SECTION "VariablesC03E", WRAM0[$C03E]
byte_C03E::     ds 1        ; 菜单光标的X坐标(在BgMap中的列偏移)
                            ; 0x79: 对应图标主菜单的最右侧一列的列偏移
byte_C03F::     ds 1        ; 菜单光标的Y坐标(在BgMap中的行偏移)
                            ; 文字菜单中，当前光标选择的菜单项在BgMap中的行偏移
                            ; 0x28  ------------------------------------>    对应图标主菜单的第1行图标
                            ; 0x38  对应主菜单1 Backup
                            ; 0x40  对应主菜单2 Restore
                            ; 0x48  对应主菜单3 Delete  或Yes/No对话框的Yes
                            ; 0x50  对应主菜单4 Option  或Yes/No对话框的No   或对应图标主菜单的第2行图标
                            ; 0x58  对应主菜单5 Delete Game Save
                            ; 0x60  对应主菜单6 Reset

SECTION "VariablesC053", WRAM0[$C053]
byteArr_C053::  ds $10      ; 用来在测试GameCart的SRAM可写性时，备份原有SRAM中数据的临时存储空间

SECTION "VariablesC063", WRAM0[$C063]
C063_fnameCache::   ds 1    ; [0xC063-0xC762]:
                            ;   每$10个字节一组，存放N组卡带中已经备份数据的文件名，总数量在$C763中。
                            ;   总共$70组，即112组，但是实际好像只用了100组也就是1600字节(0x640字节)
                            ;   用$700只是为了写入28SF040时，对齐256字节的扇区

SECTION "VariablesC763", WRAM0[$C763]
byte_C763::     ds 1        ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。

SECTION "VariablesC773", WRAM0[$C773]
C773_OptVal01:: ds 1        ; Option选项值，存放菜单显示方式，非0时，表示用图标显示主菜单，为0时表示用纯文字。
                            ; 初始化为0xFF，即图标方式显示。
C774_OptVal02:: ds 1        ; Option选项值，存放颜色显示方式，非0时，表示用反转色彩。
                            ; 初始化为0xFF，即反转色彩方式显示。
C775_OptVal03:: ds 1        ; Option选项值，是否启用声音，非0时，表示启用声音。
                            ; 初始化为0xFF，即启用声音。
C776_OptVal04:: ds 1        ; Option选项值，是否强制要求唯一文件名。为0时，表示允许同名文件。
                            ; 初始化为0x0，即允许同名文件。

SECTION "VariablesC862", WRAM0[$C862]
byte_C862::     ds 1        ; 存放0xAE + [0xC763]..[0xC861]共0xFE个字节的校验和
