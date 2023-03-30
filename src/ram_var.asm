SECTION "Variables", WRAM0
byte_C000::	    ds 1        ; 本周期按下了的按键(这些键的bit被置为1)
                            ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A
byte_C001::	    ds 1        ; 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
                            ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A

SECTION "VariablesC037", WRAM0[$C037]
byte_C037::     ds 1        ; 每次VBlank函数发生时，该变量会被设成1
byte_C038::     ds 1        ; 存放的是当前插的游戏卡带，最后可用的SRAM的Bank编号数量(最后一个可用Bank的序号+1)

SECTION "VariablesC053", WRAM0[$C053]
byteArr_C053::  ds $10      ; 用来在测试GameCart的SRAM可写性时，备份原有SRAM中数据的临时存储空间


