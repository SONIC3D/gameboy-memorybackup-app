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


SECTION "VariablesC022", WRAM0[$C022]
byte_C022::     ds 1        ; 写入这个地址值的低5bit是上次被74HC373锁存的值的低5bit，是作为28SF040的A18..A14的值。
                            ; 保存在这里，以便之后可以渠道上次对28SF040进行的Bank切换。
                            ; (28SF040因此被分成2^5 = 32个Bank)
byte_C023::     ds 1        ; 0x85B中:
                            ;   用于存放在游戏卡SRAM中出现次数最少的Byte值。
                            ;   当有多个Byte值出现次数都最少且相等，则存放的是这些Byte值中最小的一个。
                            ;   比如假设0x66和0x77的出现次数都是0次，则只会保存0x66这个值。
                            ; 0x8DB中:
                            ;   使用0x85B调用后留下的该值，作为RLE压缩数据的指令字，
                            ;   对于要RLE压缩的数据:
                            ;    第1个字节就是这个[0xC023]的值，第2个字节是循环次数，第3字节是被重复的数据
                            ; 0xA25中:
                            ;   用于存放从对应ID的第一个FlashROM的Sector的第1个字节的值。
                            ;   作为RLE解压指令字的字节。
                            ;   后续遇到读取到的字节和这个指令字相等的情况，就意味着要继续读2个字节来作RLE解压。
                            ;   如果后续读取到的不是这个指令字，就当作直接数据。
byte_C024::     ds 1        ; 本地变量:
                            ;   0x8DB中可能是用来存放当前某个字节值出现了多少次重复，以便作为RLE压缩数据写入FlashROM,
                            ;   如果某个字节值重复次数超过0xFF次，那么会先以0xFF作为RLE压缩的参数，写入FlashROM，
                            ;   然后后续的重复，重新开始记次。
byte_C025::     ds 1        ; 本地变量：
                            ;   0x8DB函数中用来保存参考字节，即上一次读到的非重复的字节。
                            ;   用于和之后的字节进行比较来决定是否对后续数据进行RLE压缩。
byte_C026::     ds 1        ; 本地变量：
                            ;   0x8DB函数中用来保存每次循环读取到的最新1个字节。
byte_C027::     ds 1        ; 在多个存档备份、恢复、搜索函数中用来标志本次执行结果是否失败，以便后续逻辑可以取出作为参考值。
                            ;   比如:
                            ;    用在了搜索某个Id的存档备份FlashROM Sector时
                            ;    用在了搜索空闲的FlashROM Sector时
                            ; 值为1是表示之前的执行结果是失败或者错误。


SECTION "VariablesC02A", WRAM0[$C02A]
byte_C02A::     ds 1        ; 该变量为0，表示主菜单中选了Delete
                            ; 该变量为1，表示主菜单中选了Restore
byte_C02B::     ds 1        ; 在Restore或Delete界面，按A键时，光标选中的备份文件的顺序号+1
                            ; (不是当前屏幕内的序号，是在所有备份中的顺序号)
                            ; 这个序号>=1，且<=([0xC763])

SECTION "VariablesC02D", WRAM0[$C02D]
byte_C02D::     ds 1        ; 存放图标形式的MainMenu界面的被光标选中的项目的ID，取值范围0-5。
                            ; 第1行3个图标从左到右依次是0-2，第2行从左到右依次是3-5。
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

SECTION "VariablesC03C", WRAM0[$C03C]
byte_C03C::     ds 1        ; 备份空间的占用情况，数值实际应该在[0-$40]内，即[0-64]。

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
byte_C040::     ds 1        ; 局部变量:
                            ;   0x85B和0xA25函数中:
                            ;    表示当前设置给Game Cart的SRAM Bank ID
                            ;   0x1034(listSavFilename)函数中:
                            ;    表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
byte_C041::     ds 1        ; 该变量为0，表示主菜单中选了GameCart SaveDelete删除游戏卡存档
                            ; 该变量为1，主菜单中选了Reset格式化
                            ; 该变量为2, 主菜单中选了Delete Backup删除备份文件

SECTION "VariablesC042", WRAM0[$C042]
C042_bufFileNam::           ; 共$10字节。用于存放备份功能时，用户输入的文件名字符串，以便用于和卡中已有存档名进行比较。
                ds 1        ; 0
                ds 1        ; 1
                ds 1        ; 2
                ds 1        ; 3
                ds 1        ; 4
                ds 1        ; 5
                ds 1        ; 6
                ds 1        ; 7
                ds 1        ; 8
                ds 1        ; 9
                ds 1        ; $A
                ds 1        ; $B
                ds 1        ; $C
                ds 1        ; $D
                ds 1        ; $E
                ds 1        ; $F
C043_Unused::   ds 1

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
