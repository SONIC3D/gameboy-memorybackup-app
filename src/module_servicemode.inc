; =============== S U B R O U T I N E =======================================

; 按住Start+Select+B+A，则进入Service Mode
; 打印Flash ROM芯片信息和游戏卡带的SRAM Bank信息
enterSvcMode:
    call    joypadRead              ; 读取按键
                                    ; 返回值：
                                    ;   (以下返回值Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A)
                                    ;   a: 本周期按下了的按键(这些键的bit被置为1)
                                    ;   b: 上周期没有按下，而本周期按下了的按键(这些键的bit被置为1)
    ld      a, [byte_C000]          ; 本周期按下了的按键(这些键的bit被置为1)
                                    ; Bit7/6/5/4/3/2/1/0对应下/上/左/右/Start/Select/B/A
    cp      $F
    ret     nz                      ; 如果不相等，就返回（即如果不是Start/Select/B/A都按下，且方向键未按下的状态，就返回，不再继续）

.keysAllMatched:
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
    ld      a, 4
    ld      [byte_C00F], a          ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      hl, strFactoryTest      ; "Factory test...\n"
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
    call    SF040_ReadId            ; 读取存储芯片(28SF040)的厂商/设备ID
                                    ; 返回值
                                    ;   h: 28SF040的Device Code,应当为$04
                                    ;   l: 28SF040的厂商ID, 应当$BF

.storeSP_1:                         ; if sp=0xFFFC, then [0xC00C]=0xFC, [0xC00D]=0xFF
    ld      [word_C00C], sp

.saveRegs:
    ld      sp, $C00C
    push    hl                      ; [0xC00A]=hl
                                    ;    [0xC00A]=l=28SF040 Manufacturer ID
                                    ;    [0xC00B]=h=28SF040 Device ID
    push    de                      ; [0xC008]=de
    push    bc                      ; [0xC006]=bc
    push    af                      ; [0xC004]=af

.restoreSP_1:
    ld      a, [word_C00C]
    ld      l, a                    ; l=[0xC00C]=0xFC
    ld      a, [word_C00C+1]
    ld      h, a                    ; h=[0xC00D]=0xFF
    ld      sp,    hl

.printFlashInfo:                    ; 在0x691的strPrintf函数中，用来作为第1个格式化符号对应的参数
    ld      hl,    C010_PrintArgv1
    ld      a, [byte_C00B]
    ldi     [hl], a                 ; [0xC010]=[0xC00B]
    ld      a, [byte_C00A]          ; enterSvcMode中用来临时存放28SF040 Manufacturer ID
    ldi     [hl], a                 ; [0xC011]=[0xC00A]
    ld      hl,    strFlashId       ; "Flash ID = %2%2\n"
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

.checkGameSvSRAM:                   ; 似乎是为了检测有多少个SRAM Bank可用
    call    detectBankCnt
                                    ; 整个逻辑是:
                                    ;   切到GameCart模式
                                    ;   测试第0个SRAM Bank是否可读写
                                    ;    不可读写就直接返回，Z Flag清空，为nz
                                    ;   备份[0xA000]到[0xC053]
                                    ;   写入测试字符串(16个字符)到第0个Bank的[0xA000]
                                    ;   循环赋值[0x4000], 从0x01到0x0F(注意0x4000是MBC1/3/5)，所以不支持MBC6)
                                    ;    测试SRAM Bank是否可读写，不可读写直接走结束流程
                                    ;    循环读取[0xA000]开始到[0xA00F]，比对其中的所有字节是否和测试字符串相符合
                                    ;      相符则走到结束流程，不相符则继续测试下一个Bank
                                    ;   循环结束
                                    ;   结束流程:
                                    ;    从[0xC053]恢复备份[0xA000]
                                    ;    切到MemoryBackupCart模式
                                    ;    把最后一轮的b寄存器值保存到[0xC038]
                                    ;
    ld      a, [byte_C038]          ; 存放的是当前插的游戏卡带，最后可用的SRAM的Bank编号数量(最后一个可用Bank的序号+1)

.storeSP_2:                         ; if sp=0xFFFC, then [0xC00C]=0xFC, [0xC00D]=0xFF
    ld      [word_C00C], sp
    ld      sp,    $C00C
    push    hl                      ; [0xC00A] = hl
    push    de                      ; [0xC008] = de
    push    bc                      ; [0xC006] = bc
    push    af                      ; [0xC004] = af
                                    ;    [0xC004] = f
                                    ;    [0xC005] = a = [byte_C038]  // Game SRAM Bank count

.restoreSP_2:
    ld      a, [word_C00C]
    ld      l, a
    ld      a, [word_C00C+1]
    ld      h, a
    ld      sp,    hl

.printGameSvBank:                   ; 在0x691的strPrintf函数中，用来作为第1个格式化符号对应的参数
    ld      hl,    C010_PrintArgv1
    ld      a, [byte_C005]
    ldi     [hl], a
    ld      hl,    strRamBanks      ; "RAM banks = %d\n"
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

.loopInfine:
    jr        @
; End of function enterSvcMode


; =============== S U B R O U T I N E =======================================

; Display credits info and initial formating 28SF040.

creditsFmtFlash:
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
    ld      hl, strCredits          ; "     Design:\n       D.Barwin\n       M"...
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
    ld      hl, strDatelCopyRt      ; "  (C)Datel D&D 1999"
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
.formatSF040:
    ld      hl, $D800
    ld      bc, $800
    xor     a
    call    memFillWith_A           ; 赋值源a中数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to fill: bc
                                    ;   Value: a
                                    ;   DestAddr:[hl]
    call    wrSF040_0x7F000         ; Copy WRAM:1:[0xD800-0xDFFF] to 28SF040:[0x7F000-0x7F7FF].
                                    ; 保存Sector分配表到FlashROM
    ld      hl, C063_fnameCache     ; [0xC063-0xC762]:
                                    ;   每$10个字节一组，存放N组卡带中已经备份数据的文件名，总数量在$C763中。
                                    ;   总共$70组，即112组，但是实际好像只用了100组也就是1600字节(0x640字节)
                                    ;   用$700只是为了写入28SF040时，对齐256字节的扇区
    ld      bc, $700
    ld      a, $20 ; ' '
    call    memFillWith_A           ; 赋值源a中数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to fill: bc
                                    ;   Value: a
                                    ;   DestAddr:[hl]
    call    wrSF040_0x7F800         ; Copy WRAM:0:[0xC063-0xC762] to 28SF040:[0x7F800-0x7FEFF].
    ld      hl, byte_C763           ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。
    ld      bc, $FF
    xor     a
    call    memFillWith_A           ; 赋值源a中数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to fill: bc
                                    ;   Value: a
                                    ;   DestAddr:[hl]
.initOptionValue:
    ld      a, $FF
    ld      [C773_OptVal01], a      ; Option选项值，存放菜单显示方式，非0时，表示用图标显示主菜单，为0时表示用纯文字。
                                    ; 初始化为0xFF，即图标方式显示。
    ld      [C774_OptVal02], a      ; Option选项值，存放颜色显示方式，非0时，表示用反转色彩。
                                    ; 初始化为0xFF，即反转色彩方式显示。
    ld      [C775_OptVal03], a      ; Option选项值，是否启用声音，非0时，表示启用声音。
                                    ; 初始化为0xFF，即启用声音。
    call    wrSF040_0x7FF00         ; Set [0xDFF0-0xDFFF] to 0xFF.
                                    ; Create checksum for [0xC763-0xC861] and store the checksum at [0xC862].
                                    ; Copy WRAM-0:[0xC763-0xC862] to 28SF040:[0x7FF00-0x7FFFF].
    ld      c, 3

.delay3secs:
    call    delay1second            ; 这个函数设置Timer为4096Hz，然后设置Timer计数器初始为0x80(即还剩0x80计数到0xFF、发生中断)
                                    ; 然后发生0x20次中断后返回，即0x80 * 0x20 = 0x1000即4096计数，即延时1秒后函数返回。
    dec     c
    jr      nz, .delay3secs
    ret
; End of function creditsFmtFlash


; =============== S U B R O U T I N E =======================================

; 这个函数设置Timer为4096Hz，然后设置Timer计数器初始为0x80(即还剩0x80计数到0xFF、发生中断)
; 然后发生0x20次中断后返回，即0x80 * 0x20 = 0x1000即4096计数，即延时1秒后函数返回。

delay1second:
    xor     a
    ld      [FF07_TimCtrl], a       ; Timer set to 4096Hz, stopped.
    ld      a, $80 ; '�'            ; Timer counter will reset to 0x80 when it runs overflow(reaches 0xFF).
    ld      [FF06_TimModulo], a     ; FF06 - TMA - Timer Modulo (R/W)
                                    ; When the TIMA overflows, this data will be loaded.
    ld      [FF05_TimCounter], a    ; FF05 - TIMA - Timer counter (R/W)
                                    ; This timer is incremented by a clock frequency specified by the TAC register ($FF07). When the value overflows (gets bigger    than FFh) then it will be reset    to the value specified in TMA (FF06), and an interrupt will be requested, as described below.
    ld      a, 4                    ; Start Timer.
    ld      [FF07_TimCtrl], a       ; FF07 - TAC - Timer Control (R/W)
                                    ;   Bit 2    - Timer Stop  (0=Stop, 1=Start)
                                    ;   Bits 1-0 - Input Clock Select
                                    ;         00:   4096 Hz    (~4194 Hz SGB)
                                    ;         01: 262144 Hz  (~268400 Hz SGB)
                                    ;         10:  65536 Hz   (~67110 Hz SGB)
                                    ;         11:  16384 Hz   (~16780 Hz SGB)
    ld      b, $20 ; ' '            ; Wait 0x20(32) times of Timer Int Req happen
.clrTimerIntReq:
    ld      a, [FF0F_IntrptFlag]    ; FF0F - IF - Interrupt Flag (R/W)
                                    ;   Bit 0: V-Blank  Interrupt Request (INT 40h)  (1=Request)
                                    ;   Bit 1: LCD STAT Interrupt Request (INT 48h)  (1=Request)
                                    ;   Bit 2: Timer    Interrupt Request (INT 50h)  (1=Request)
                                    ;   Bit 3: Serial   Interrupt Request (INT 58h)  (1=Request)
                                    ;   Bit 4: Joypad   Interrupt Request (INT 60h)  (1=Request)
    and     $FB
    ld      [FF0F_IntrptFlag], a    ; FF0F - IF - Interrupt Flag (R/W)
                                    ;   Bit 0: V-Blank  Interrupt Request (INT 40h)  (1=Request)
                                    ;   Bit 1: LCD STAT Interrupt Request (INT 48h)  (1=Request)
                                    ;   Bit 2: Timer    Interrupt Request (INT 50h)  (1=Request)
                                    ;   Bit 3: Serial   Interrupt Request (INT 58h)  (1=Request)
                                    ;   Bit 4: Joypad   Interrupt Request (INT 60h)  (1=Request)

.waitTimerReq:
    ld      a, [FF0F_IntrptFlag]    ; FF0F - IF - Interrupt Flag (R/W)
                                    ;   Bit 0: V-Blank  Interrupt Request (INT 40h)  (1=Request)
                                    ;   Bit 1: LCD STAT Interrupt Request (INT 48h)  (1=Request)
                                    ;   Bit 2: Timer    Interrupt Request (INT 50h)  (1=Request)
                                    ;   Bit 3: Serial   Interrupt Request (INT 58h)  (1=Request)
                                    ;   Bit 4: Joypad   Interrupt Request (INT 60h)  (1=Request)
    bit     2, a
    jr      z, .waitTimerReq

.timerReqHappen:
    dec     b
    jr      nz, .clrTimerIntReq
    ret
; End of function delay1second
