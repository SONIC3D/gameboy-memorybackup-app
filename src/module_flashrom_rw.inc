; =============== S U B R O U T I N E =======================================

; 选好备份后并输入好文件名，对着End符号按A后，出现的Working等待处理中界面

dispBkupWorking:
    ld      a, [byte_C763]	        ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。
    cp      $64	; 'd'
    ret     nc			            ; Return if [0xC763]>=100

.fileCntNotFull:					    ; 循环halt等待中断发生，直到VBlank发生1次后，本函数返回。
    call    waitVBlankOccur

.clrBgMap:					        ; Fill [0xFE00-0xFE9F] OAM address range with	data 0x00.
    call    clearOAM
    ld      hl, $9800
    ld      bc, $400
    ld      a, $20 ; ' '
    call    vramFillWith_A	        ; 赋值源a中数据到目标地址中(目标地址可以是VRAM)，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; 为了确保目标地址是VRAM地址(8000h-9FFFh)或OAM(FE00h-FE9Fh)地址时也可写入，本函数会确保在LCD的HVBlank发生期间操作。
                                    ;
                                    ; Parameters:
                                    ;   LengthOfByte to fill: bc
                                    ;   Value: a
                                    ;   DestAddr:[hl]
                                    ;

.printWorkingStr:
    ld      a, 0
    ld      [byte_C00E], a	        ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      a, 6
    ld      [byte_C00F], a	        ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      hl, strWorking
    call    strPrintf		        ; 打印字符串到屏幕
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
    call    delay1second	        ; 这个函数设置Timer为4096Hz，然后设置Timer计数器初始为0x80(即还剩0x80计数到0xFF、发生中断)
                                    ; 然后发生0x20次中断后返回，即0x80 * 0x20	= 0x1000即4096计数，即延时1秒后函数返回。
    call    getLeastCntByte	        ; 从Game Cart的各个SRAM Bank读取数据
                                    ; 把读到的字节值$0-$FF的出现次数记录在$D700-$D7FF中
                                    ; 最后统计出现次数最少且最小的字节值，将其存放到0xC023
    call    core_Sram2Bak	        ; 保存GameCart的完整SRAM数据到FlashROM中。对于连续重复的字节，会尽可能使用RLE压缩。
                                    ; 对于含有多个Bank的游戏SRAM，也会依次切换Bank，确保所有Bank的数据都被备份到FlashROM中。
                                    ;
                                    ; 必要参数:
                                    ;   [0xC023]:
                                    ;	存放了整个GameCart的SRAM中出现次数最少的值，以这个值作为RLE压缩的指令字。
                                    ;	这个值由0x85B(getLeastCntByte)函数生成和赋值，所以需要在本函数执行前调用一次。
    ld      a, [byte_C027]	        ; 在多个存档备份、恢复、搜索函数中用来标志本次执行结果是否失败，以便后续逻辑可以取出作为参考值。
                                    ;   比如:
                                    ;	用在了搜索某个Id的存档备份FlashROM Sector时
                                    ;	用在了搜索空闲的FlashROM Sector时
                                    ; 值为1是表示之前的执行结果是失败或者错误。
    or      a
    ret     nz
    call    wrSF040_0x7F000	        ; Copy WRAM:1:[0xD800-0xDFFF]	to 28SF040:[0x7F000-0x7F7FF].
                                    ; 保存Sector分配表到FlashROM
    call    wrSF040_0x7F800	        ; Copy WRAM:0:[0xC063-0xC762]	to 28SF040:[0x7F800-0x7FEFF].
    call    wrSF040_0x7FF00	        ; Set	[0xDFF0-0xDFFF]	to 0xFF.
                                    ; Create checksum for  [0xC763-0xC861]	and store the checksum at [0xC862].
                                    ; Copy WRAM-0:[0xC763-0xC862]	to 28SF040:[0x7FF00-0x7FFFF].
    call    delay1second	        ; 这个函数设置Timer为4096Hz，然后设置Timer计数器初始为0x80(即还剩0x80计数到0xFF、发生中断)
                                    ; 然后发生0x20次中断后返回，即0x80 * 0x20	= 0x1000即4096计数，即延时1秒后函数返回。
    ret
; End of function dispBkupWorking


; =============== S U B R O U T I N E =======================================

; 从Game Cart的各个SRAM Bank读取数据
; 把读到的字节值$0-$FF的出现次数记录在$D700-$D7FF中
; 最后统计出现次数最少且最小的字节值，将其存放到0xC023

getLeastCntByte:
    ld      a, $C0 ; '�'
    ld      [IOPort_0x2000], a	    ; 0x2000 I/O Port:
                                    ;   Write
                                    ;	bit7..bit6:
                                    ;	  $C0:
                                    ;	    Switch to Game Cart	Mode. All access to address like $4000 passed to Game Cart.
                                    ;	  $80:
                                    ;	    Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;	bit5:
                                    ;	  Unknown. Always be 0.
                                    ;	bit4..bit0:
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would  be latched and out put to A18..A14 of 28SF040.

.clr_D700_D7FF:
    ld      hl, $D700
    ld      bc, $100
    xor      a
    call    memFillWith_A	        ; 赋值源a中数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to fill: bc
                                    ;   Value: a
                                    ;   DestAddr:[hl]

.initValues:
    xor      a
    ld      [byte_C040], a	        ; 局部变量:
                                    ;   0x85B和0xA25函数中:
                                    ;	表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;	表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    ld      hl, $D700

.setGameCartBank:
    ld      de, $A000
    ld      a, [byte_C040]	        ; 局部变量:
                                    ;   0x85B和0xA25函数中:
                                    ;	表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;	表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    ld      [byte_4000], a	        ; Set	game cart SRAM Bank to the specific value in a.

.rdNxtByteInBank:
    ld      a, [de]
    inc     de

.incByteCounter:					    ; l =	[$A000], [$A001], [$A002]...[$BFFF]
    ld      l, a
    ld      a, [hl]
    inc     a			            ; 将0xD7??的数据取出来，增1。
                                    ; 这里??表示当前从Game	Cart的当前SRAM地址[de]中读到的数据。
                                    ; 简单说，就是用$D700-$D7FF的256字节，依次存放，低8bit地址对应的数据的出现次数。
    jr      z, .checkBankRange	    ; 如果出现次数已经达到0xFF，即256，就不再对计数增1

.saveIncredCount:
    ld      [hl], a

.checkBankRange:
    ld      a, d
    cp      $C0	; '�'
    jr      c, .rdNxtByteInBank

.incGameCartBank:				    ; 存放的是当前插的游戏卡带，最后可用的SRAM的Bank编号数量(最后一个可用Bank的序号+1)
    ld      a, [byte_C038]
    ld      b, a
    ld      a, [byte_C040]	        ; 局部变量:
                                    ;   0x85B和0xA25函数中:
                                    ;	表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;	表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    inc     a
    ld      [byte_C040], a	        ; 局部变量:
                                    ;   0x85B和0xA25函数中:
                                    ;	表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;	表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    cp      b
    jr      c, .setGameCartBank

.lastBankReached:
    ld      l, 0
    ld      b, $FF

.nextByteCounter:
    ld      a, [hl]
    cp      b
    jr      nc, .checkUpLimit

.updLeastCntByte:				    ; 每当发现出现次数比现有b值更小的字节时，就将出现次数写入b
    ld      b, a
    ld      c, l		            ; 每当发现出现次数比现有b值更小的字节时，就将这个字节本身的值写入c

.checkUpLimit:
    inc     l
    jr      nz, .nextByteCounter

.saveResult:
    ld      a, c
    ld      [byte_C023], a	        ; 0x85B中:
                                    ;   用于存放在游戏卡SRAM中出现次数最少的Byte值。
                                    ;   当有多个Byte值出现次数都最少且相等，则存放的是这些Byte值中最小的一个。
                                    ;   比如假设0x66和0x77的出现次数都是0次，则只会保存0x66这个值。
                                    ; 0x8DB中:
                                    ;   使用0x85B调用后留下的该值，作为RLE压缩数据的指令字，
                                    ;   对于要RLE压缩的数据:
                                    ;	第1个字节就是这个[0xC023]的值，第2个字节是循环次数，第3字节是被重复的数据
                                    ; 0xA25中:
                                    ;   用于存放从对应ID的第一个FlashROM的Sector的第1个字节的值。
                                    ;   作为RLE解压指令字的字节。
                                    ;   后续遇到读取到的字节和这个指令字相等的情况，就意味着要继续读2个字节来作RLE解压。
                                    ;   如果后续读取到的不是这个指令字，就当作直接数据。

.swMapper2MemBak:
    ld      a, $80 ; '�'
    ld      [IOPort_0x2000], a	    ; 0x2000 I/O Port:
                                    ;   Write
                                    ;	bit7..bit6:
                                    ;	  $C0:
                                    ;	    Switch to Game Cart	Mode. All access to address like $4000 passed to Game Cart.
                                    ;	  $80:
                                    ;	    Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;	bit5:
                                    ;	  Unknown. Always be 0.
                                    ;	bit4..bit0:
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would  be latched and out put to A18..A14 of 28SF040.
    ret
; End of function getLeastCntByte


; =============== S U B R O U T I N E =======================================

; return a=0 if	found 0x00 within [0xD800-0xDFFF]
;   hl = 0xD800+de is the absolute address that	the value is 0x00.
;   de is the relative address(relative	to 0xD800) that	the value == [0xC02B].
;
; return a=1 if	not found any 0x00 within [0xD800-0xDFFF].
;   hl = 0xE000
;   de = 0x800
;   [0xC027] = 1
;
; 在0xD800-0xDFFF的文件分配表(Sector分配表)范围内搜索无存档数据的对应格
;   * 如果找到了，就返回a=0。
;     且此时hl就是文件分配表(Sector分配表)中的对应的绝对地址,其值从0xD800-0xDFFF。
;     de是相对地址(相对于0xD800)，其值从0x0-0x7FFF。
;     可以用该地址去进一步定位FlashROM中的Sector。
;   * 如果没有找到就返回a=1
;     且[0xC027]=1，这个变量会用于调用方后续判断本次搜索是否找到对应的空闲格

findUnusedInFAT:
    ld      hl, $D800
    ld      de, 0

.checkNextByte:
    ld      a, [hl]
    or      a
    ret     z

.valueNotZero:
    inc     de
    inc     hl
    ld      a, h
    cp      $E0
    jr      c, .checkNextByte
    ld      a, 1
    ld      [byte_C027], a	        ; 在多个存档备份、恢复、搜索函数中用来标志本次执行结果是否失败，以便后续逻辑可以取出作为参考值。
                                    ;   比如:
                                    ;	用在了搜索某个Id的存档备份FlashROM Sector时
                                    ;	用在了搜索空闲的FlashROM Sector时
                                    ; 值为1是表示之前的执行结果是失败或者错误。
    ret
; End of function findUnusedInFAT


; =============== S U B R O U T I N E =======================================

; 用途：
;   每次调用时将输入的字节先写入Sector写缓存中，
;   当写缓存凑满28SF040的Sector大小256字节时，就进行实际写FlashROM操作，
;   并重置写缓存的光标到0xD700的起始位置，等待下次的写入。
;
; 详细逻辑:
;   输入参数a
;   1. 将a写入缓存[de]。(实际使用中，de是0xD700-0xD7FF的256字节)
;   2. 然后e++
;      如果e未变成0，就直接返回。(z此时是clr即nz状态)
;      如果e变成了0，就表示已经凑满了一个Sector大小(0x100即256字节)，就继续第3步写FlashROM
;   3. 在Sector/File Allocation Table(0xD800-0xDFFF)找到空余空间
;      如果没有找到，则返回(z此时是clr即nz状态)
;      如果找到了，就继续第4步
;   4. 将在Sector/File Allocation Table(0xD800-0xDFFF)找到的第一个空余格子值标记为
;      当前存档总数量(即拿总数量作为下一个存档序号计数，该序号为>=1的数)
;   5. 然后把0xD700开始的0x100字节内容写入de对应的28SF040新扇区中(A18..A14已经提前锁存)。
;   6. 最后再de=0xD700
;
; Input:
;    a:	待存入de的值
;   de:	待保存a值的地址
; Output:
;   de:	当输入的de的e为0xFF时，输出的de重置为0xD700

fillSectr_wrFls:
    ld      [de], a
    inc     e
    ret     nz			            ; return if e+1 is not 0x00. (Won't return if e==0xFF)

.deIsEndOfSector:				    ; return a=0 if found	0x00 within [0xD800-0xDFFF]
    call    findUnusedInFAT	        ;   hl = 0xD800+de is	the absolute address that the value is 0x00.
                                    ;   de is the	relative address(relative to 0xD800) that the value == [0xC02B].
                                    ;
                                    ; return a=1 if not found any	0x00 within [0xD800-0xDFFF].
                                    ;   hl = 0xE000
                                    ;   de = 0x800
                                    ;   [0xC027] = 1
                                    ;
                                    ; 在0xD800-0xDFFF的文件分配表(Sector分配表)范围内搜索无存档数据的对应格
                                    ;   *	如果找到了，就返回a=0。
                                    ;	且此时hl就是文件分配表(Sector分配表)中的对应的绝对地址,其值从0xD800-0xDFFF。
                                    ;	de是相对地址(相对于0xD800)，其值从0x0-0x7FFF。
                                    ;	可以用该地址去进一步定位FlashROM中的Sector。
                                    ;   *	如果没有找到就返回a=1
                                    ;	且[0xC027]=1，这个变量会用于调用方后续判断本次搜索是否找到对应的空闲格
    or      a
    ret     nz			            ; return, if not found any unused Sector in Sector/File Allocation Table.

.foundFreeSector:				    ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。
    ld      a, [byte_C763]
    ld      [hl], a		            ; 将Sector/File Allocation Table中找到的空余扇区对应字节设为下一个存档文件的编号
                                    ; 该编号>=1，<=[0xC763]，该值在被搜索时，用来和[0xC02B]进行比较，可以参考
    ld      hl, $D700
    ld      bc, $100
    call    SF040_Cp2Sector         ; 复制指定长度数据到28SF040指定的Sector(复制数据的总字节数必须为256的整数倍)
                                    ; Parameters:
                                    ;   hl: 待复制到28SF040目标扇区的数据的起始地址
                                    ;   de: A18..A8 of target sector(Sector Address, SA) of 28SF040
                                    ;   bc: 总共复制多少字节
                                    ;	  实际c被忽略，所以这个参数必须是0x100(256)的整数倍
                                    ;	  即28SF040的Sector大小的整数倍
    ld      de, $D700
    ret
; End of function fillSectr_wrFls


; =============== S U B R O U T I N E =======================================

; 保存GameCart的完整SRAM数据到FlashROM中。对于连续重复的字节，会尽可能使用RLE压缩。
; 对于含有多个Bank的游戏SRAM，也会依次切换Bank，确保所有Bank的数据都被备份到FlashROM中。
;
; 必要参数:
;   [0xC023]:
;     存放了整个GameCart的SRAM中出现次数最少的值，以这个值作为RLE压缩的指令字。
;     这个值由0x85B(getLeastCntByte)函数生成和赋值，所以需要在本函数执行前调用一次。

core_Sram2Bak:
    ld      a, $C0 ; '�'
    ld      [IOPort_0x2000], a	    ; 0x2000 I/O Port:
                                    ;   Write
                                    ;	bit7..bit6:
                                    ;	  $C0:
                                    ;	    Switch to Game Cart	Mode. All access to address like $4000 passed to Game Cart.
                                    ;	  $80:
                                    ;	    Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;	bit5:
                                    ;	  Unknown. Always be 0.
                                    ;	bit4..bit0:
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would  be latched and out put to A18..A14 of 28SF040.

.setGmSramBank0:
    xor      a
    ld      [byte_4000], a	        ; Switch game	cart SRAM Bank to 0

.initOtherVars:					    ; 局部变量:
    ld      [byte_C040], a	        ;   0x85B和0xA25函数中:
                                    ;	表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;	表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    ld      [byte_C024], a	        ; 本地变量:
                                    ;   0x8DB中可能是用来存放当前某个字节值出现了多少次重复，以便作为RLE压缩数据写入FlashROM,
                                    ;   如果某个字节值重复次数超过0xFF次，那么会先以0xFF作为RLE压缩的参数，写入FlashROM，
                                    ;   然后后续的重复，重新开始记次。
    ld      [byte_C027], a	        ; 在多个存档备份、恢复、搜索函数中用来标志本次执行结果是否失败，以便后续逻辑可以取出作为参考值。
                                    ;   比如:
                                    ;	用在了搜索某个Id的存档备份FlashROM Sector时
                                    ;	用在了搜索空闲的FlashROM Sector时
                                    ; 值为1是表示之前的执行结果是失败或者错误。

.sw2MemBakMode_0:
    ld      a, $80 ; '�'
    ld      [IOPort_0x2000], a	    ; 0x2000 I/O Port:
                                    ;   Write
                                    ;	bit7..bit6:
                                    ;	  $C0:
                                    ;	    Switch to Game Cart	Mode. All access to address like $4000 passed to Game Cart.
                                    ;	  $80:
                                    ;	    Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;	bit5:
                                    ;	  Unknown. Always be 0.
                                    ;	bit4..bit0:
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would  be latched and out put to A18..A14 of 28SF040.

.saveRleCmdByte:
    ld      de, $D700
    ld      a, [byte_C023]	        ; 0x85B中:
                                    ;   用于存放在游戏卡SRAM中出现次数最少的Byte值。
                                    ;   当有多个Byte值出现次数都最少且相等，则存放的是这些Byte值中最小的一个。
                                    ;   比如假设0x66和0x77的出现次数都是0次，则只会保存0x66这个值。
                                    ; 0x8DB中:
                                    ;   使用0x85B调用后留下的该值，作为RLE压缩数据的指令字，
                                    ;   对于要RLE压缩的数据:
                                    ;	第1个字节就是这个[0xC023]的值，第2个字节是循环次数，第3字节是被重复的数据
                                    ; 0xA25中:
                                    ;   用于存放从对应ID的第一个FlashROM的Sector的第1个字节的值。
                                    ;   作为RLE解压指令字的字节。
                                    ;   后续遇到读取到的字节和这个指令字相等的情况，就意味着要继续读2个字节来作RLE解压。
                                    ;   如果后续读取到的不是这个指令字，就当作直接数据。
    ld      [de], a		            ; 保存RLE指令字到[0xD700]，以便作为整个备份存档的第1个Sector的第1个字节写入FlashROM。
    inc     e

.startRdGameSram:
    ld      hl, $A000
    ldi     a, [hl]
    ld      [byte_C025], a	        ; 本地变量：
                                    ;   0x8DB函数中用来保存参考字节，即上一次读到的非重复的字节。
                                    ;   用于和之后的字节进行比较来决定是否对后续数据进行RLE压缩。

.chkEndOfBakData:
    ld      a, [byte_C027]	        ; 在多个存档备份、恢复、搜索函数中用来标志本次执行结果是否失败，以便后续逻辑可以取出作为参考值。
                                    ;   比如:
                                    ;	用在了搜索某个Id的存档备份FlashROM Sector时
                                    ;	用在了搜索空闲的FlashROM Sector时
                                    ; 值为1是表示之前的执行结果是失败或者错误。
    or      a
    ret     nz			            ; 如果之前的写入过程中，触发了递进到下一个Sector的行为，
                                    ; 但是已经找不到更多空余的Sector，就会在这里直接返回，
                                    ; 结束整个备份档恢复到卡带的逻辑流程。
; ------------------------------------------------------------------
; 检查是否已经到了当前GameCart的SRAM	Bank的末尾(SRAM Bank范围是0xA000-0xBFFF)

.chkEndOfSramBnk:
    ld      a, h
    cp      $C0	; '�'
    jr      c, .readNextByte
; ------------------------------------------------------------------
; 检查GameCart是否还有更多SRAM Bank可以切换

.nxtGameSramBank:				    ; 存放的是当前插的游戏卡带，最后可用的SRAM的Bank编号数量(最后一个可用Bank的序号+1)
    ld      a, [byte_C038]
    ld      b, a
    ld      a, [byte_C040]	        ; 局部变量:
                                    ;   0x85B和0xA25函数中:
                                    ;	表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;	表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    inc     a
    cp      b
    jr      c, .incGameSramBank
; ------------------------------------------------------------------
; 已经到了GameCart的SRAM的末尾，直接收尾把剩余已经读取的数据写入Flash即可

.noMoreSramData:					    ; 本地变量:
    ld      a, [byte_C024]	        ;   0x8DB中可能是用来存放当前某个字节值出现了多少次重复，以便作为RLE压缩数据写入FlashROM,
                                    ;   如果某个字节值重复次数超过0xFF次，那么会先以0xFF作为RLE压缩的参数，写入FlashROM，
                                    ;   然后后续的重复，重新开始记次。
    or      a
    jr      nz, .wrRestDataRle

.wrRestDataNoRle:				    ; 本地变量：
    ld      a, [byte_C025]	        ;   0x8DB函数中用来保存参考字节，即上一次读到的非重复的字节。
                                    ;   用于和之后的字节进行比较来决定是否对后续数据进行RLE压缩。
    call    fillSectr_wrFls	        ; 用途：
                                    ;   每次调用时将输入的字节先写入Sector写缓存中，
                                    ;   当写缓存凑满28SF040的Sector大小256字节时，就进行实际写FlashROM操作，
                                    ;   并重置写缓存的光标到0xD700的起始位置，等待下次的写入。
                                    ;
                                    ; 详细逻辑:
                                    ;   输入参数a
                                    ;   1. 将a写入缓存[de]。(实际使用中，de是0xD700-0xD7FF的256字节)
                                    ;   2. 然后e++
                                    ;	 如果e未变成0，就直接返回。(z此时是clr即nz状态)
                                    ;	 如果e变成了0，就表示已经凑满了一个Sector大小(0x100即256字节)，就继续第3步写FlashROM
                                    ;   3. 在Sector/File	Allocation Table(0xD800-0xDFFF)找到空余空间
                                    ;	 如果没有找到，则返回(z此时是clr即nz状态)
                                    ;	 如果找到了，就继续第4步
                                    ;   4. 将在Sector/File Allocation Table(0xD800-0xDFFF)找到的第一个空余格子值标记为
                                    ;	 当前存档总数量(即拿总数量作为下一个存档序号计数，该序号为>=1的数)
                                    ;   5. 然后把0xD700开始的0x100字节内容写入de对应的28SF040新扇区中(A18..A14已经提前锁存)。
                                    ;   6. 最后再de=0xD700
                                    ;
                                    ; Input:
                                    ;    a: 待存入de的值
                                    ;   de: 待保存a值的地址
                                    ; Output:
                                    ;   de: 当输入的de的e为0xFF时，输出的de重置为0xD700
    jr      .wrRestData2Fls
; ---------------------------------------------------------------------------

.wrRestDataRle:
    ld      a, [byte_C023]	        ; 0x85B中:
                                    ;   用于存放在游戏卡SRAM中出现次数最少的Byte值。
                                    ;   当有多个Byte值出现次数都最少且相等，则存放的是这些Byte值中最小的一个。
                                    ;   比如假设0x66和0x77的出现次数都是0次，则只会保存0x66这个值。
                                    ; 0x8DB中:
                                    ;   使用0x85B调用后留下的该值，作为RLE压缩数据的指令字，
                                    ;   对于要RLE压缩的数据:
                                    ;	第1个字节就是这个[0xC023]的值，第2个字节是循环次数，第3字节是被重复的数据
                                    ; 0xA25中:
                                    ;   用于存放从对应ID的第一个FlashROM的Sector的第1个字节的值。
                                    ;   作为RLE解压指令字的字节。
                                    ;   后续遇到读取到的字节和这个指令字相等的情况，就意味着要继续读2个字节来作RLE解压。
                                    ;   如果后续读取到的不是这个指令字，就当作直接数据。
    call    fillSectr_wrFls	        ; 用途：
                                    ;   每次调用时将输入的字节先写入Sector写缓存中，
                                    ;   当写缓存凑满28SF040的Sector大小256字节时，就进行实际写FlashROM操作，
                                    ;   并重置写缓存的光标到0xD700的起始位置，等待下次的写入。
                                    ;
                                    ; 详细逻辑:
                                    ;   输入参数a
                                    ;   1. 将a写入缓存[de]。(实际使用中，de是0xD700-0xD7FF的256字节)
                                    ;   2. 然后e++
                                    ;	 如果e未变成0，就直接返回。(z此时是clr即nz状态)
                                    ;	 如果e变成了0，就表示已经凑满了一个Sector大小(0x100即256字节)，就继续第3步写FlashROM
                                    ;   3. 在Sector/File	Allocation Table(0xD800-0xDFFF)找到空余空间
                                    ;	 如果没有找到，则返回(z此时是clr即nz状态)
                                    ;	 如果找到了，就继续第4步
                                    ;   4. 将在Sector/File Allocation Table(0xD800-0xDFFF)找到的第一个空余格子值标记为
                                    ;	 当前存档总数量(即拿总数量作为下一个存档序号计数，该序号为>=1的数)
                                    ;   5. 然后把0xD700开始的0x100字节内容写入de对应的28SF040新扇区中(A18..A14已经提前锁存)。
                                    ;   6. 最后再de=0xD700
                                    ;
                                    ; Input:
                                    ;    a: 待存入de的值
                                    ;   de: 待保存a值的地址
                                    ; Output:
                                    ;   de: 当输入的de的e为0xFF时，输出的de重置为0xD700

.writeRleCount_2:				    ; 本地变量:
    ld      a, [byte_C024]	        ;   0x8DB中可能是用来存放当前某个字节值出现了多少次重复，以便作为RLE压缩数据写入FlashROM,
                                    ;   如果某个字节值重复次数超过0xFF次，那么会先以0xFF作为RLE压缩的参数，写入FlashROM，
                                    ;   然后后续的重复，重新开始记次。
    call    fillSectr_wrFls	        ; 用途：
                                    ;   每次调用时将输入的字节先写入Sector写缓存中，
                                    ;   当写缓存凑满28SF040的Sector大小256字节时，就进行实际写FlashROM操作，
                                    ;   并重置写缓存的光标到0xD700的起始位置，等待下次的写入。
                                    ;
                                    ; 详细逻辑:
                                    ;   输入参数a
                                    ;   1. 将a写入缓存[de]。(实际使用中，de是0xD700-0xD7FF的256字节)
                                    ;   2. 然后e++
                                    ;	 如果e未变成0，就直接返回。(z此时是clr即nz状态)
                                    ;	 如果e变成了0，就表示已经凑满了一个Sector大小(0x100即256字节)，就继续第3步写FlashROM
                                    ;   3. 在Sector/File	Allocation Table(0xD800-0xDFFF)找到空余空间
                                    ;	 如果没有找到，则返回(z此时是clr即nz状态)
                                    ;	 如果找到了，就继续第4步
                                    ;   4. 将在Sector/File Allocation Table(0xD800-0xDFFF)找到的第一个空余格子值标记为
                                    ;	 当前存档总数量(即拿总数量作为下一个存档序号计数，该序号为>=1的数)
                                    ;   5. 然后把0xD700开始的0x100字节内容写入de对应的28SF040新扇区中(A18..A14已经提前锁存)。
                                    ;   6. 最后再de=0xD700
                                    ;
                                    ; Input:
                                    ;    a: 待存入de的值
                                    ;   de: 待保存a值的地址
                                    ; Output:
                                    ;   de: 当输入的de的e为0xFF时，输出的de重置为0xD700

.writeRleByte_2:					    ; 本地变量：
    ld      a, [byte_C025]	        ;   0x8DB函数中用来保存参考字节，即上一次读到的非重复的字节。
                                    ;   用于和之后的字节进行比较来决定是否对后续数据进行RLE压缩。
    call    fillSectr_wrFls	        ; 用途：
                                    ;   每次调用时将输入的字节先写入Sector写缓存中，
                                    ;   当写缓存凑满28SF040的Sector大小256字节时，就进行实际写FlashROM操作，
                                    ;   并重置写缓存的光标到0xD700的起始位置，等待下次的写入。
                                    ;
                                    ; 详细逻辑:
                                    ;   输入参数a
                                    ;   1. 将a写入缓存[de]。(实际使用中，de是0xD700-0xD7FF的256字节)
                                    ;   2. 然后e++
                                    ;	 如果e未变成0，就直接返回。(z此时是clr即nz状态)
                                    ;	 如果e变成了0，就表示已经凑满了一个Sector大小(0x100即256字节)，就继续第3步写FlashROM
                                    ;   3. 在Sector/File	Allocation Table(0xD800-0xDFFF)找到空余空间
                                    ;	 如果没有找到，则返回(z此时是clr即nz状态)
                                    ;	 如果找到了，就继续第4步
                                    ;   4. 将在Sector/File Allocation Table(0xD800-0xDFFF)找到的第一个空余格子值标记为
                                    ;	 当前存档总数量(即拿总数量作为下一个存档序号计数，该序号为>=1的数)
                                    ;   5. 然后把0xD700开始的0x100字节内容写入de对应的28SF040新扇区中(A18..A14已经提前锁存)。
                                    ;   6. 最后再de=0xD700
                                    ;
                                    ; Input:
                                    ;    a: 待存入de的值
                                    ;   de: 待保存a值的地址
                                    ; Output:
                                    ;   de: 当输入的de的e为0xFF时，输出的de重置为0xD700
; ------------------------------------------------------------------
; 最后收尾的数据不一定正好凑足一个Sector写入FlashROM，这里作检查，并找到空余Sector写入。

.wrRestData2Fls:
    ld      a, e
    or      a
    ret     z			            ; 如果de正好递进到e为0的情况，即递进到一个新的Sector的开始，则直接返回。
                                    ;   因为所有老数据正好凑满一个Sector，都已经被写进了28SF040的FlashROM。

.getFreeSector:					    ; return a=0 if found	0x00 within [0xD800-0xDFFF]
    call    findUnusedInFAT	        ;   hl = 0xD800+de is	the absolute address that the value is 0x00.
                                    ;   de is the	relative address(relative to 0xD800) that the value == [0xC02B].
                                    ;
                                    ; return a=1 if not found any	0x00 within [0xD800-0xDFFF].
                                    ;   hl = 0xE000
                                    ;   de = 0x800
                                    ;   [0xC027] = 1
                                    ;
                                    ; 在0xD800-0xDFFF的文件分配表(Sector分配表)范围内搜索无存档数据的对应格
                                    ;   *	如果找到了，就返回a=0。
                                    ;	且此时hl就是文件分配表(Sector分配表)中的对应的绝对地址,其值从0xD800-0xDFFF。
                                    ;	de是相对地址(相对于0xD800)，其值从0x0-0x7FFF。
                                    ;	可以用该地址去进一步定位FlashROM中的Sector。
                                    ;   *	如果没有找到就返回a=1
                                    ;	且[0xC027]=1，这个变量会用于调用方后续判断本次搜索是否找到对应的空闲格
    or      a
    ret     nz			            ; return if not found	0x00 in	previous function call.

.foundFreeSector:				    ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。
    ld      a, [byte_C763]
    ld      [hl], a		            ; 在Sector/File Allocation Table中设定该Sector对应格子的备份文件Id。

.wrLastDataSectr:
    ld      hl, $D700
    ld      bc, $100
    call    SF040_Cp2Sector         ; 复制指定长度数据到28SF040指定的Sector(复制数据的总字节数必须为256的整数倍)
                                    ; Parameters:
                                    ;   hl: 待复制到28SF040目标扇区的数据的起始地址
                                    ;   de: A18..A8 of target sector(Sector Address, SA) of 28SF040
                                    ;   bc: 总共复制多少字节
                                    ;	  实际c被忽略，所以这个参数必须是0x100(256)的整数倍
                                    ;	  即28SF040的Sector大小的整数倍
    ret
; ---------------------------------------------------------------------------

.incGameSramBank:
    ld      [byte_C040], a	        ; 局部变量:
                                    ;   0x85B和0xA25函数中:
                                    ;	表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;	表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    ld      a, $C0 ; '�'
    ld      [IOPort_0x2000], a	    ; 0x2000 I/O Port:
                                    ;   Write
                                    ;	bit7..bit6:
                                    ;	  $C0:
                                    ;	    Switch to Game Cart	Mode. All access to address like $4000 passed to Game Cart.
                                    ;	  $80:
                                    ;	    Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;	bit5:
                                    ;	  Unknown. Always be 0.
                                    ;	bit4..bit0:
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would  be latched and out put to A18..A14 of 28SF040.
    ld      a, [byte_C040]	        ; 局部变量:
                                    ;   0x85B和0xA25函数中:
                                    ;	表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;	表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    ld      [byte_4000], a

.sw2MemBakMode_1:
    ld      a, $80 ; '�'
    ld      [IOPort_0x2000], a	    ; 0x2000 I/O Port:
                                    ;   Write
                                    ;	bit7..bit6:
                                    ;	  $C0:
                                    ;	    Switch to Game Cart	Mode. All access to address like $4000 passed to Game Cart.
                                    ;	  $80:
                                    ;	    Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;	bit5:
                                    ;	  Unknown. Always be 0.
                                    ;	bit4..bit0:
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would  be latched and out put to A18..A14 of 28SF040.
    ld      hl, $A000

.readNextByte:
    ld      a, [byte_C025]	        ; 本地变量：
                                    ;   0x8DB函数中用来保存参考字节，即上一次读到的非重复的字节。
                                    ;   用于和之后的字节进行比较来决定是否对后续数据进行RLE压缩。
    ld      b, a
    ldi     a, [hl]
    ld      [byte_C026], a	        ; 本地变量：
                                    ;   0x8DB函数中用来保存每次循环读取到的最新1个字节。
    cp      b
    jr      nz, .notEquRefByte

.equRefByte:					    ; 本地变量:
    ld      a, [byte_C024]	        ;   0x8DB中可能是用来存放当前某个字节值出现了多少次重复，以便作为RLE压缩数据写入FlashROM,
                                    ;   如果某个字节值重复次数超过0xFF次，那么会先以0xFF作为RLE压缩的参数，写入FlashROM，
                                    ;   然后后续的重复，重新开始记次。
    inc     a
    jr      z, .writeRleContent

.updateRleCount:					; 本地变量:
    ld      [byte_C024], a	        ;   0x8DB中可能是用来存放当前某个字节值出现了多少次重复，以便作为RLE压缩数据写入FlashROM,
                                    ;   如果某个字节值重复次数超过0xFF次，那么会先以0xFF作为RLE压缩的参数，写入FlashROM，
                                    ;   然后后续的重复，重新开始记次。
    jp      .chkEndOfBakData
; ---------------------------------------------------------------------------

.notEquRefByte:
    ld      a, [byte_C024]	        ; 本地变量:
                                    ;   0x8DB中可能是用来存放当前某个字节值出现了多少次重复，以便作为RLE压缩数据写入FlashROM,
                                    ;   如果某个字节值重复次数超过0xFF次，那么会先以0xFF作为RLE压缩的参数，写入FlashROM，
                                    ;   然后后续的重复，重新开始记次。
    cp      2
    jr      nc, .writeRleContent	; 如果重复次数已经>=2，就跳到写入RLE数据的逻辑去
                                    ; 如果重复次数是小于2的，用RLE写入就不值得，就不跳
; ------------------------------------------------------------------
; 检查之前重复次数小于2次的字节数据是否和Rle指令字相同
;   如果相同，就依然按照RLE压缩逻辑去处理。
;   如果不相同，就走非RLE压缩数据的写入逻辑。

.cmpRleCmdByte:					    ; 0x85B中:
    ld      a, [byte_C023]	        ;   用于存放在游戏卡SRAM中出现次数最少的Byte值。
                                    ;   当有多个Byte值出现次数都最少且相等，则存放的是这些Byte值中最小的一个。
                                    ;   比如假设0x66和0x77的出现次数都是0次，则只会保存0x66这个值。
                                    ; 0x8DB中:
                                    ;   使用0x85B调用后留下的该值，作为RLE压缩数据的指令字，
                                    ;   对于要RLE压缩的数据:
                                    ;	第1个字节就是这个[0xC023]的值，第2个字节是循环次数，第3字节是被重复的数据
                                    ; 0xA25中:
                                    ;   用于存放从对应ID的第一个FlashROM的Sector的第1个字节的值。
                                    ;   作为RLE解压指令字的字节。
                                    ;   后续遇到读取到的字节和这个指令字相等的情况，就意味着要继续读2个字节来作RLE解压。
                                    ;   如果后续读取到的不是这个指令字，就当作直接数据。
    ld      b, a
    ld      a, [byte_C025]	        ; 本地变量：
                                    ;   0x8DB函数中用来保存参考字节，即上一次读到的非重复的字节。
                                    ;   用于和之后的字节进行比较来决定是否对后续数据进行RLE压缩。
    cp      b
    jr      z, .writeRleContent
; ------------------------------------------------------------------
; 逻辑走到这里，[0xC024]中的值只可能是0或者1。
;   0表示[0xC025]中的值之前没有重复
;   1表示[0xC025]中的值之前重复了1次

.noneRleContent:					    ; 本地变量:
    ld      a, [byte_C024]	        ;   0x8DB中可能是用来存放当前某个字节值出现了多少次重复，以便作为RLE压缩数据写入FlashROM,
                                    ;   如果某个字节值重复次数超过0xFF次，那么会先以0xFF作为RLE压缩的参数，写入FlashROM，
                                    ;   然后后续的重复，重新开始记次。
    or      a
    ld      a, 0
    ld      [byte_C024], a	        ; 本地变量:
                                    ;   0x8DB中可能是用来存放当前某个字节值出现了多少次重复，以便作为RLE压缩数据写入FlashROM,
                                    ;   如果某个字节值重复次数超过0xFF次，那么会先以0xFF作为RLE压缩的参数，写入FlashROM，
                                    ;   然后后续的重复，重新开始记次。
    jr      nz, .writeRefByte_x2

.writeRefByte_x1:
    push    hl
    ld      a, [byte_C025]	        ; 本地变量：
                                    ;   0x8DB函数中用来保存参考字节，即上一次读到的非重复的字节。
                                    ;   用于和之后的字节进行比较来决定是否对后续数据进行RLE压缩。
    call    fillSectr_wrFls	        ; 用途：
                                    ;   每次调用时将输入的字节先写入Sector写缓存中，
                                    ;   当写缓存凑满28SF040的Sector大小256字节时，就进行实际写FlashROM操作，
                                    ;   并重置写缓存的光标到0xD700的起始位置，等待下次的写入。
                                    ;
                                    ; 详细逻辑:
                                    ;   输入参数a
                                    ;   1. 将a写入缓存[de]。(实际使用中，de是0xD700-0xD7FF的256字节)
                                    ;   2. 然后e++
                                    ;	 如果e未变成0，就直接返回。(z此时是clr即nz状态)
                                    ;	 如果e变成了0，就表示已经凑满了一个Sector大小(0x100即256字节)，就继续第3步写FlashROM
                                    ;   3. 在Sector/File	Allocation Table(0xD800-0xDFFF)找到空余空间
                                    ;	 如果没有找到，则返回(z此时是clr即nz状态)
                                    ;	 如果找到了，就继续第4步
                                    ;   4. 将在Sector/File Allocation Table(0xD800-0xDFFF)找到的第一个空余格子值标记为
                                    ;	 当前存档总数量(即拿总数量作为下一个存档序号计数，该序号为>=1的数)
                                    ;   5. 然后把0xD700开始的0x100字节内容写入de对应的28SF040新扇区中(A18..A14已经提前锁存)。
                                    ;   6. 最后再de=0xD700
                                    ;
                                    ; Input:
                                    ;    a: 待存入de的值
                                    ;   de: 待保存a值的地址
                                    ; Output:
                                    ;   de: 当输入的de的e为0xFF时，输出的de重置为0xD700
    pop     hl

.updateRefByte_3:				    ; 本地变量：
    ld      a, [byte_C026]	        ;   0x8DB函数中用来保存每次循环读取到的最新1个字节。
    ld      [byte_C025], a	        ; 本地变量：
                                    ;   0x8DB函数中用来保存参考字节，即上一次读到的非重复的字节。
                                    ;   用于和之后的字节进行比较来决定是否对后续数据进行RLE压缩。
    jp      .chkEndOfBakData
; ---------------------------------------------------------------------------

.writeRefByte_x2:
    push    hl
    ld      a, [byte_C025]	        ; 本地变量：
                                    ;   0x8DB函数中用来保存参考字节，即上一次读到的非重复的字节。
                                    ;   用于和之后的字节进行比较来决定是否对后续数据进行RLE压缩。
    call    fillSectr_wrFls	        ; 用途：
                                    ;   每次调用时将输入的字节先写入Sector写缓存中，
                                    ;   当写缓存凑满28SF040的Sector大小256字节时，就进行实际写FlashROM操作，
                                    ;   并重置写缓存的光标到0xD700的起始位置，等待下次的写入。
                                    ;
                                    ; 详细逻辑:
                                    ;   输入参数a
                                    ;   1. 将a写入缓存[de]。(实际使用中，de是0xD700-0xD7FF的256字节)
                                    ;   2. 然后e++
                                    ;	 如果e未变成0，就直接返回。(z此时是clr即nz状态)
                                    ;	 如果e变成了0，就表示已经凑满了一个Sector大小(0x100即256字节)，就继续第3步写FlashROM
                                    ;   3. 在Sector/File	Allocation Table(0xD800-0xDFFF)找到空余空间
                                    ;	 如果没有找到，则返回(z此时是clr即nz状态)
                                    ;	 如果找到了，就继续第4步
                                    ;   4. 将在Sector/File Allocation Table(0xD800-0xDFFF)找到的第一个空余格子值标记为
                                    ;	 当前存档总数量(即拿总数量作为下一个存档序号计数，该序号为>=1的数)
                                    ;   5. 然后把0xD700开始的0x100字节内容写入de对应的28SF040新扇区中(A18..A14已经提前锁存)。
                                    ;   6. 最后再de=0xD700
                                    ;
                                    ; Input:
                                    ;    a: 待存入de的值
                                    ;   de: 待保存a值的地址
                                    ; Output:
                                    ;   de: 当输入的de的e为0xFF时，输出的de重置为0xD700
    ld      a, [byte_C025]	        ; 本地变量：
                                    ;   0x8DB函数中用来保存参考字节，即上一次读到的非重复的字节。
                                    ;   用于和之后的字节进行比较来决定是否对后续数据进行RLE压缩。
    call    fillSectr_wrFls	        ; 用途：
                                    ;   每次调用时将输入的字节先写入Sector写缓存中，
                                    ;   当写缓存凑满28SF040的Sector大小256字节时，就进行实际写FlashROM操作，
                                    ;   并重置写缓存的光标到0xD700的起始位置，等待下次的写入。
                                    ;
                                    ; 详细逻辑:
                                    ;   输入参数a
                                    ;   1. 将a写入缓存[de]。(实际使用中，de是0xD700-0xD7FF的256字节)
                                    ;   2. 然后e++
                                    ;	 如果e未变成0，就直接返回。(z此时是clr即nz状态)
                                    ;	 如果e变成了0，就表示已经凑满了一个Sector大小(0x100即256字节)，就继续第3步写FlashROM
                                    ;   3. 在Sector/File	Allocation Table(0xD800-0xDFFF)找到空余空间
                                    ;	 如果没有找到，则返回(z此时是clr即nz状态)
                                    ;	 如果找到了，就继续第4步
                                    ;   4. 将在Sector/File Allocation Table(0xD800-0xDFFF)找到的第一个空余格子值标记为
                                    ;	 当前存档总数量(即拿总数量作为下一个存档序号计数，该序号为>=1的数)
                                    ;   5. 然后把0xD700开始的0x100字节内容写入de对应的28SF040新扇区中(A18..A14已经提前锁存)。
                                    ;   6. 最后再de=0xD700
                                    ;
                                    ; Input:
                                    ;    a: 待存入de的值
                                    ;   de: 待保存a值的地址
                                    ; Output:
                                    ;   de: 当输入的de的e为0xFF时，输出的de重置为0xD700
    pop     hl

.updateRefByte_2:				    ; 本地变量：
    ld      a, [byte_C026]	        ;   0x8DB函数中用来保存每次循环读取到的最新1个字节。
    ld      [byte_C025], a	        ; 本地变量：
                                    ;   0x8DB函数中用来保存参考字节，即上一次读到的非重复的字节。
                                    ;   用于和之后的字节进行比较来决定是否对后续数据进行RLE压缩。
    jp      .chkEndOfBakData
; ---------------------------------------------------------------------------

.writeRleContent:
    push    hl			            ; popped at 0x9D0

.writeRleCmdByte:				    ; 0x85B中:
    ld      a, [byte_C023]	        ;   用于存放在游戏卡SRAM中出现次数最少的Byte值。
                                    ;   当有多个Byte值出现次数都最少且相等，则存放的是这些Byte值中最小的一个。
                                    ;   比如假设0x66和0x77的出现次数都是0次，则只会保存0x66这个值。
                                    ; 0x8DB中:
                                    ;   使用0x85B调用后留下的该值，作为RLE压缩数据的指令字，
                                    ;   对于要RLE压缩的数据:
                                    ;	第1个字节就是这个[0xC023]的值，第2个字节是循环次数，第3字节是被重复的数据
                                    ; 0xA25中:
                                    ;   用于存放从对应ID的第一个FlashROM的Sector的第1个字节的值。
                                    ;   作为RLE解压指令字的字节。
                                    ;   后续遇到读取到的字节和这个指令字相等的情况，就意味着要继续读2个字节来作RLE解压。
                                    ;   如果后续读取到的不是这个指令字，就当作直接数据。
    call    fillSectr_wrFls	        ; 用途：
                                    ;   每次调用时将输入的字节先写入Sector写缓存中，
                                    ;   当写缓存凑满28SF040的Sector大小256字节时，就进行实际写FlashROM操作，
                                    ;   并重置写缓存的光标到0xD700的起始位置，等待下次的写入。
                                    ;
                                    ; 详细逻辑:
                                    ;   输入参数a
                                    ;   1. 将a写入缓存[de]。(实际使用中，de是0xD700-0xD7FF的256字节)
                                    ;   2. 然后e++
                                    ;	 如果e未变成0，就直接返回。(z此时是clr即nz状态)
                                    ;	 如果e变成了0，就表示已经凑满了一个Sector大小(0x100即256字节)，就继续第3步写FlashROM
                                    ;   3. 在Sector/File	Allocation Table(0xD800-0xDFFF)找到空余空间
                                    ;	 如果没有找到，则返回(z此时是clr即nz状态)
                                    ;	 如果找到了，就继续第4步
                                    ;   4. 将在Sector/File Allocation Table(0xD800-0xDFFF)找到的第一个空余格子值标记为
                                    ;	 当前存档总数量(即拿总数量作为下一个存档序号计数，该序号为>=1的数)
                                    ;   5. 然后把0xD700开始的0x100字节内容写入de对应的28SF040新扇区中(A18..A14已经提前锁存)。
                                    ;   6. 最后再de=0xD700
                                    ;
                                    ; Input:
                                    ;    a: 待存入de的值
                                    ;   de: 待保存a值的地址
                                    ; Output:
                                    ;   de: 当输入的de的e为0xFF时，输出的de重置为0xD700

.writeRleCount_1:				    ; 本地变量:
    ld      a, [byte_C024]	        ;   0x8DB中可能是用来存放当前某个字节值出现了多少次重复，以便作为RLE压缩数据写入FlashROM,
                                    ;   如果某个字节值重复次数超过0xFF次，那么会先以0xFF作为RLE压缩的参数，写入FlashROM，
                                    ;   然后后续的重复，重新开始记次。
    call    fillSectr_wrFls	        ; 用途：
                                    ;   每次调用时将输入的字节先写入Sector写缓存中，
                                    ;   当写缓存凑满28SF040的Sector大小256字节时，就进行实际写FlashROM操作，
                                    ;   并重置写缓存的光标到0xD700的起始位置，等待下次的写入。
                                    ;
                                    ; 详细逻辑:
                                    ;   输入参数a
                                    ;   1. 将a写入缓存[de]。(实际使用中，de是0xD700-0xD7FF的256字节)
                                    ;   2. 然后e++
                                    ;	 如果e未变成0，就直接返回。(z此时是clr即nz状态)
                                    ;	 如果e变成了0，就表示已经凑满了一个Sector大小(0x100即256字节)，就继续第3步写FlashROM
                                    ;   3. 在Sector/File	Allocation Table(0xD800-0xDFFF)找到空余空间
                                    ;	 如果没有找到，则返回(z此时是clr即nz状态)
                                    ;	 如果找到了，就继续第4步
                                    ;   4. 将在Sector/File Allocation Table(0xD800-0xDFFF)找到的第一个空余格子值标记为
                                    ;	 当前存档总数量(即拿总数量作为下一个存档序号计数，该序号为>=1的数)
                                    ;   5. 然后把0xD700开始的0x100字节内容写入de对应的28SF040新扇区中(A18..A14已经提前锁存)。
                                    ;   6. 最后再de=0xD700
                                    ;
                                    ; Input:
                                    ;    a: 待存入de的值
                                    ;   de: 待保存a值的地址
                                    ; Output:
                                    ;   de: 当输入的de的e为0xFF时，输出的de重置为0xD700

.writeRleByte_1:					    ; 本地变量：
    ld      a, [byte_C025]	        ;   0x8DB函数中用来保存参考字节，即上一次读到的非重复的字节。
                                    ;   用于和之后的字节进行比较来决定是否对后续数据进行RLE压缩。
    call    fillSectr_wrFls	        ; 用途：
                                    ;   每次调用时将输入的字节先写入Sector写缓存中，
                                    ;   当写缓存凑满28SF040的Sector大小256字节时，就进行实际写FlashROM操作，
                                    ;   并重置写缓存的光标到0xD700的起始位置，等待下次的写入。
                                    ;
                                    ; 详细逻辑:
                                    ;   输入参数a
                                    ;   1. 将a写入缓存[de]。(实际使用中，de是0xD700-0xD7FF的256字节)
                                    ;   2. 然后e++
                                    ;	 如果e未变成0，就直接返回。(z此时是clr即nz状态)
                                    ;	 如果e变成了0，就表示已经凑满了一个Sector大小(0x100即256字节)，就继续第3步写FlashROM
                                    ;   3. 在Sector/File	Allocation Table(0xD800-0xDFFF)找到空余空间
                                    ;	 如果没有找到，则返回(z此时是clr即nz状态)
                                    ;	 如果找到了，就继续第4步
                                    ;   4. 将在Sector/File Allocation Table(0xD800-0xDFFF)找到的第一个空余格子值标记为
                                    ;	 当前存档总数量(即拿总数量作为下一个存档序号计数，该序号为>=1的数)
                                    ;   5. 然后把0xD700开始的0x100字节内容写入de对应的28SF040新扇区中(A18..A14已经提前锁存)。
                                    ;   6. 最后再de=0xD700
                                    ;
                                    ; Input:
                                    ;    a: 待存入de的值
                                    ;   de: 待保存a值的地址
                                    ; Output:
                                    ;   de: 当输入的de的e为0xFF时，输出的de重置为0xD700
    pop     hl			            ; pushed at 0x9BD

.resetRleCount:
    xor      a
    ld      [byte_C024], a	        ; 本地变量:
                                    ;   0x8DB中可能是用来存放当前某个字节值出现了多少次重复，以便作为RLE压缩数据写入FlashROM,
                                    ;   如果某个字节值重复次数超过0xFF次，那么会先以0xFF作为RLE压缩的参数，写入FlashROM，
                                    ;   然后后续的重复，重新开始记次。

.updateRefByte_1:				    ; 本地变量：
    ld      a, [byte_C026]	        ;   0x8DB函数中用来保存每次循环读取到的最新1个字节。
    ld      [byte_C025], a	        ; 本地变量：
                                    ;   0x8DB函数中用来保存参考字节，即上一次读到的非重复的字节。
                                    ;   用于和之后的字节进行比较来决定是否对后续数据进行RLE压缩。
    jp      .chkEndOfBakData
; End of function core_Sram2Bak

; ---------------------------------------------------------------------------
unusedStr_01:	    db "RAM banks = %d",$A,0
unusedStr_02:	    db "token = %2",$A,0

; =============== S U B R O U T I N E =======================================

; return a=0 if	found [0xC02B] value within [0xD800-0xDFFF]
;   hl = 0xD800+de is the absolute address that	the value == [0xC02B].
;   de is the relative address(relative	to 0xD800) that	the value == [0xC02B].
;
; return a=1 if	not found matched value	within [0xD800-0xDFFF].
;   hl = 0xE000
;   de = 0x800
;   [0xC027] = 1
;
; 在0xD800-0xDFFF的文件分配表(Sector分配表)范围内搜索指定的存档序号(由[0xC02B]中存的值指定)
;   * 如果找到了，就返回a=0。
;     且此时hl就是文件分配表(Sector分配表)中的对应该值的绝对地址,其值从0xD800-0xDFFF。
;     de是相对地址(相对于0xD800)，其值从0x0-0x7FFF。
;     可以用该地址去进一步定位FlashROM中的数据。
;   * 如果没有找到就返回a=1
;     且[0xC027]=1，这个变量会用于调用方后续判断本次搜索是否找到对应的存档

findFileInFAT:
    ld      hl, $D800
    ld      de, 0

.checkNextByte:
    ld      a, [byte_C02B]	        ; 在Restore或Delete界面，按A键时，光标选中的备份文件的顺序号+1
                                    ; (不是当前屏幕内的序号，是在所有备份中的顺序号)
                                    ; 这个序号>=1，且<=([0xC763])
    cp      [hl]
    ld      a, 0
    ret     z			            ; return if [0xC02B]==[0xD800]

.valueNotEqual:
    inc     de
    inc     hl
    ld      a, h
    cp      $E0
    jr      c, .checkNextByte
    ld      a, 1
    ld      [byte_C027], a	        ; 在多个存档备份、恢复、搜索函数中用来标志本次执行结果是否失败，以便后续逻辑可以取出作为参考值。
                                    ;   比如:
                                    ;	用在了搜索某个Id的存档备份FlashROM Sector时
                                    ;	用在了搜索空闲的FlashROM Sector时
                                    ; 值为1是表示之前的执行结果是失败或者错误。
    ret
; End of function findFileInFAT


; =============== S U B R O U T I N E =======================================

; 功能概括:
;   从de所存的FlashROM地址中(0x4000-0x7FFF)，读取1个字节到a。按需递进28SF040的下一个可读扇区的Bank等参数设定。
; 功能详细:
;   从de所存的地址，读取1个字节到a。
;     如果de中的e不是0xFF，则直接返回。
;     如果de中的e是0xFF，即该扇区的最后一个字节，则：
;	1. 搜索在缓存的Sector/文件分配表中搜索下一个属于目标文件编号的格子。
;	   如果找不到，则直接返回。
;	   如果找到则继续第2步。
;	2. 将搜索到的缓存Sector/文件分配表中的这个格子的值置为0，避免之后再搜索到。
;	3. 将28SF040的A18..A14锁存为搜索到的格子对应的Sector的A18..A14
;	   de值设置为搜索到的格子对应的Sector的A13..A8
;	   以便后续可以继续对28SF040进行读写。
; 重点：
; Input:
;   de:	Source address for reading.
; Output:
;   a:	[de]的值;
;   de:	下一个可直接读取的FlashROM地址范围内的地址
; 其他:
;   hl值在本函数中会被影响，调用本函数前调用方得记得保存hl值

rdNxtByte_rdFls:
    ld      a, [de]
    inc     e
    ret     nz			            ; return if e+1 is not 0x00. (Won't return if e==0xFF)

.deIsEndOfSector:				    ; popped at 0xA23
    push    af
    call    findFileInFAT	        ; return a=0 if found	[0xC02B] value within [0xD800-0xDFFF]
                                    ;   hl = 0xD800+de is	the absolute address that the value == [0xC02B].
                                    ;   de is the	relative address(relative to 0xD800) that the value == [0xC02B].
                                    ;
                                    ; return a=1 if not found matched value within [0xD800-0xDFFF].
                                    ;   hl = 0xE000
                                    ;   de = 0x800
                                    ;   [0xC027] = 1
                                    ;
                                    ; 在0xD800-0xDFFF的文件分配表(Sector分配表)范围内搜索指定的存档序号(由[0xC02B]中存的值指定)
                                    ;   *	如果找到了，就返回a=0。
                                    ;	且此时hl就是文件分配表(Sector分配表)中的对应该值的绝对地址,其值从0xD800-0xDFFF。
                                    ;	de是相对地址(相对于0xD800)，其值从0x0-0x7FFF。
                                    ;	可以用该地址去进一步定位FlashROM中的数据。
                                    ;   *	如果没有找到就返回a=1
                                    ;	且[0xC027]=1，这个变量会用于调用方后续判断本次搜索是否找到对应的存档
    or      a
    jr      nz, .endOfFunc	        ; Jump if not	found the specific backup file by the id specified in [0xC02B].

.setReadMark_FAT:
    xor      a
    ld      [hl], a		            ; 将缓存中的Sector/File Allocation Table表中搜索到属于目标文件的接下来要读取的格子的对应文件编号设置为0。
                                    ; （以便本次读取文件过程中，后续的读取逻辑可以在Sector/File Allocation	Table中查找到该文件未读到的部分。）

.goNxtFileSector:				    ; 将输入的de理解为28SF040的A18..A16和A15..A8地址线对应的bit位。
    call    gotoSF040_Bank	        ; d左移2bit空出低2bit，然后将e的高2bit移入。
                                    ; 保留d的低5bit值，高3bit清0，以此作为要74HC373锁存的页地址。
                                    ; 将这个要74HC373锁存的页地址存入0xC022(很可能是74HC373的锁存I/O地址)。
                                    ;
                                    ; 然后切换到MemoryBackupCart模式。
                                    ; 取出e中代表用于A13..A8的值放入d，然后把d的bit6置为1。
                                    ; e清0。
                                    ; 最后返回。
                                    ; (这时返回值，实际上是de输入值所指定28SF040的Bank页内的起始地址，但是注意d.bit6被强制置为了1)
                                    ;
                                    ; (主要的处理是因为Flash芯片擦除时需要的A18..A8地址中的A18..A14在这个卡中是通过74373锁存的，
                                    ; 所以要特别处理，只有A13..A8是直接传送。)
                                    ;
                                    ; 输入参数：
                                    ;   de:
                                    ;	扇区擦除时Flash的SectorAddress地址中的A18..A8
                                    ;	其中d的低3bit存放A18..A16，e中存放A15..A8
                                    ; 输出参数
                                    ;   de
                                    ;	d寄存器，bit7为0，bit6为1，bit5..bit0存放A13..A8
                                    ;	e寄存器A7..A0 = 0x0
                                    ;   即输出的de拼上74373输出的高5位，形成了一个低8bit为0的28SF040的SectorAddress。
                                    ;
                                    ; 举例：
                                    ;   要访问0x7FF00-0x7FFFF这个Sector(28SF040的最后一个sector)
                                    ;   把A18..A8的0x7FF存入de，即d=0x07,e=0xFF, de=0x7FF=0111 1111	1111
                                    ;   把这个de拆成高5bit和低6bit，即0111 11和11 1111
                                    ;   最终de的高5bit被前面补0为0001 1111写入[0xC022]
                                    ;   返回值是de的低6bit，高位设成01，然后低位补0。即01 111111 00000000
                                    ;   *	高位设成01，是为了让FlashROM的地址范围0x0000-0x3FFF被映射到GB地址范围的0x4000-0x7FFF
                                    ;	这样在调用这个函数后，就可以直接用de作为地址对FlashROM进行直接的读写访问
                                    ;
                                    ; 伪代码:
                                    ;   a	= (de >> 6) & 0x1F;
                                    ;   [0xC022] = a;
                                    ;   [IO_0x2000] = (0x80 | a);
                                    ;   de = ((e & 0x3F) | 0x40) << 8;
                                    ;   return de;

.endOfFunc:
    pop     af			            ; pushed at 0xA17
    ret
; End of function rdNxtByte_rdFls


; =============== S U B R O U T I N E =======================================

; 这个函数根据0xC02B中指定的备份文件的ID，对备份数据进行读取和RLE解压，
; 将数据写入到卡带的SRAM中，完成备份文件恢复到卡带的底层数据写入逻辑。

core_Bak2Sram:
    ld      a, $C0 ; '�'
    ld      [IOPort_0x2000], a	    ; 0x2000 I/O Port:
                                    ;   Write
                                    ;	bit7..bit6:
                                    ;	  $C0:
                                    ;	    Switch to Game Cart	Mode. All access to address like $4000 passed to Game Cart.
                                    ;	  $80:
                                    ;	    Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;	bit5:
                                    ;	  Unknown. Always be 0.
                                    ;	bit4..bit0:
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would  be latched and out put to A18..A14 of 28SF040.

.setGmSramBank0:
    xor      a
    ld      [byte_4000], a

.initOtherVars:					    ; 局部变量:
    ld      [byte_C040], a	        ;   0x85B和0xA25函数中:
                                    ;	表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;	表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    ld      [byte_C027], a	        ; 在多个存档备份、恢复、搜索函数中用来标志本次执行结果是否失败，以便后续逻辑可以取出作为参考值。
                                    ;   比如:
                                    ;	用在了搜索某个Id的存档备份FlashROM Sector时
                                    ;	用在了搜索空闲的FlashROM Sector时
                                    ; 值为1是表示之前的执行结果是失败或者错误。

.sw2MemBakMode_0:
    ld      a, $80 ; '�'
    ld      [IOPort_0x2000], a	    ; 0x2000 I/O Port:
                                    ;   Write
                                    ;	bit7..bit6:
                                    ;	  $C0:
                                    ;	    Switch to Game Cart	Mode. All access to address like $4000 passed to Game Cart.
                                    ;	  $80:
                                    ;	    Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;	bit5:
                                    ;	  Unknown. Always be 0.
                                    ;	bit4..bit0:
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would  be latched and out put to A18..A14 of 28SF040.
    call    findFileInFAT	        ; return a=0 if found	[0xC02B] value within [0xD800-0xDFFF]
                                    ;   hl = 0xD800+de is	the absolute address that the value == [0xC02B].
                                    ;   de is the	relative address(relative to 0xD800) that the value == [0xC02B].
                                    ;
                                    ; return a=1 if not found matched value within [0xD800-0xDFFF].
                                    ;   hl = 0xE000
                                    ;   de = 0x800
                                    ;   [0xC027] = 1
                                    ;
                                    ; 在0xD800-0xDFFF的文件分配表(Sector分配表)范围内搜索指定的存档序号(由[0xC02B]中存的值指定)
                                    ;   *	如果找到了，就返回a=0。
                                    ;	且此时hl就是文件分配表(Sector分配表)中的对应该值的绝对地址,其值从0xD800-0xDFFF。
                                    ;	de是相对地址(相对于0xD800)，其值从0x0-0x7FFF。
                                    ;	可以用该地址去进一步定位FlashROM中的数据。
                                    ;   *	如果没有找到就返回a=1
                                    ;	且[0xC027]=1，这个变量会用于调用方后续判断本次搜索是否找到对应的存档

.startRdSector:
    xor      a
    ld      [hl], a		            ; 将要读取的Sector对应在Sector/文件分配表中的格子标注为0。
                                    ; * 这里有一种特殊情况要考虑，就是要搜索的文件id，在Sector/文件分配表中没有找到的情况。
                                    ;   这种情况下hl=0xE000, de=0x800:
                                    ;   *	按hl=0xE000去对[hl]赋值0，会导致[0xE000]=0，镜像映射，实际会是[0xC000]=0，
                                    ;	影响的是按键读取结果。
                                    ;   *	按de=0x800去调用0x7A7(gotoSF040_Bank)函数，会导致28SF040的A18..A14变为全0，
                                    ;	也就是切换到0x00000的地址去
                                    ; * 可能原作者没有考虑以上情况，假设了在文件系统正常的情况下选中的要恢复的文件序号必然能在Sector/文件分配表中找到。
    call    gotoSF040_Bank	        ; 将输入的de理解为28SF040的A18..A16和A15..A8地址线对应的bit位。
                                    ; d左移2bit空出低2bit，然后将e的高2bit移入。
                                    ; 保留d的低5bit值，高3bit清0，以此作为要74HC373锁存的页地址。
                                    ; 将这个要74HC373锁存的页地址存入0xC022(很可能是74HC373的锁存I/O地址)。
                                    ;
                                    ; 然后切换到MemoryBackupCart模式。
                                    ; 取出e中代表用于A13..A8的值放入d，然后把d的bit6置为1。
                                    ; e清0。
                                    ; 最后返回。
                                    ; (这时返回值，实际上是de输入值所指定28SF040的Bank页内的起始地址，但是注意d.bit6被强制置为了1)
                                    ;
                                    ; (主要的处理是因为Flash芯片擦除时需要的A18..A8地址中的A18..A14在这个卡中是通过74373锁存的，
                                    ; 所以要特别处理，只有A13..A8是直接传送。)
                                    ;
                                    ; 输入参数：
                                    ;   de:
                                    ;	扇区擦除时Flash的SectorAddress地址中的A18..A8
                                    ;	其中d的低3bit存放A18..A16，e中存放A15..A8
                                    ; 输出参数
                                    ;   de
                                    ;	d寄存器，bit7为0，bit6为1，bit5..bit0存放A13..A8
                                    ;	e寄存器A7..A0 = 0x0
                                    ;   即输出的de拼上74373输出的高5位，形成了一个低8bit为0的28SF040的SectorAddress。
                                    ;
                                    ; 举例：
                                    ;   要访问0x7FF00-0x7FFFF这个Sector(28SF040的最后一个sector)
                                    ;   把A18..A8的0x7FF存入de，即d=0x07,e=0xFF, de=0x7FF=0111 1111	1111
                                    ;   把这个de拆成高5bit和低6bit，即0111 11和11 1111
                                    ;   最终de的高5bit被前面补0为0001 1111写入[0xC022]
                                    ;   返回值是de的低6bit，高位设成01，然后低位补0。即01 111111 00000000
                                    ;   *	高位设成01，是为了让FlashROM的地址范围0x0000-0x3FFF被映射到GB地址范围的0x4000-0x7FFF
                                    ;	这样在调用这个函数后，就可以直接用de作为地址对FlashROM进行直接的读写访问
                                    ;
                                    ; 伪代码:
                                    ;   a	= (de >> 6) & 0x1F;
                                    ;   [0xC022] = a;
                                    ;   [IO_0x2000] = (0x80 | a);
                                    ;   de = ((e & 0x3F) | 0x40) << 8;
                                    ;   return de;

.rd1stByteFrmFls:
    ld      a, [de]
    inc     e
    ld      [byte_C023], a	        ; 0x85B中:
                                    ;   用于存放在游戏卡SRAM中出现次数最少的Byte值。
                                    ;   当有多个Byte值出现次数都最少且相等，则存放的是这些Byte值中最小的一个。
                                    ;   比如假设0x66和0x77的出现次数都是0次，则只会保存0x66这个值。
                                    ; 0x8DB中:
                                    ;   使用0x85B调用后留下的该值，作为RLE压缩数据的指令字，
                                    ;   对于要RLE压缩的数据:
                                    ;	第1个字节就是这个[0xC023]的值，第2个字节是循环次数，第3字节是被重复的数据
                                    ; 0xA25中:
                                    ;   用于存放从对应ID的第一个FlashROM的Sector的第1个字节的值。
                                    ;   作为RLE解压指令字的字节。
                                    ;   后续遇到读取到的字节和这个指令字相等的情况，就意味着要继续读2个字节来作RLE解压。
                                    ;   如果后续读取到的不是这个指令字，就当作直接数据。
    ld      hl, $A000

.chkEndOfBakData:
    ld      a, [byte_C027]	        ; 在多个存档备份、恢复、搜索函数中用来标志本次执行结果是否失败，以便后续逻辑可以取出作为参考值。
                                    ;   比如:
                                    ;	用在了搜索某个Id的存档备份FlashROM Sector时
                                    ;	用在了搜索空闲的FlashROM Sector时
                                    ; 值为1是表示之前的执行结果是失败或者错误。
    or      a
    ret     nz			            ; 如果之前的读取过程中，触发了递进到下一个Sector的行为，
                                    ; 但是已经找不到更多属于目标备份档的Sector，就会在这里直接返回，
                                    ; 结束整个备份档恢复到卡带的逻辑流程。

.continueRdData:					    ; 0x85B中:
    ld      a, [byte_C023]	        ;   用于存放在游戏卡SRAM中出现次数最少的Byte值。
                                    ;   当有多个Byte值出现次数都最少且相等，则存放的是这些Byte值中最小的一个。
                                    ;   比如假设0x66和0x77的出现次数都是0次，则只会保存0x66这个值。
                                    ; 0x8DB中:
                                    ;   使用0x85B调用后留下的该值，作为RLE压缩数据的指令字，
                                    ;   对于要RLE压缩的数据:
                                    ;	第1个字节就是这个[0xC023]的值，第2个字节是循环次数，第3字节是被重复的数据
                                    ; 0xA25中:
                                    ;   用于存放从对应ID的第一个FlashROM的Sector的第1个字节的值。
                                    ;   作为RLE解压指令字的字节。
                                    ;   后续遇到读取到的字节和这个指令字相等的情况，就意味着要继续读2个字节来作RLE解压。
                                    ;   如果后续读取到的不是这个指令字，就当作直接数据。
    ld      b, a
    push    hl
    call    rdNxtByte_rdFls	        ; 功能概括:
                                    ;   从de所存的FlashROM地址中(0x4000-0x7FFF)，读取1个字节到a。按需递进28SF040的下一个可读扇区的Bank等参数设定。
                                    ; 功能详细:
                                    ;   从de所存的地址，读取1个字节到a。
                                    ;	如果de中的e不是0xFF，则直接返回。
                                    ;	如果de中的e是0xFF，即该扇区的最后一个字节，则：
                                    ;	  1. 搜索在缓存的Sector/文件分配表中搜索下一个属于目标文件编号的格子。
                                    ;	     如果找不到，则直接返回。
                                    ;	     如果找到则继续第2步。
                                    ;	  2. 将搜索到的缓存Sector/文件分配表中的这个格子的值置为0，避免之后再搜索到。
                                    ;	  3. 将28SF040的A18..A14锁存为搜索到的格子对应的Sector的A18..A14
                                    ;	     de值设置为搜索到的格子对应的Sector的A13..A8
                                    ;	     以便后续可以继续对28SF040进行读写。
                                    ; 重点：
                                    ; Input:
                                    ;   de: Source address for reading.
                                    ; Output:
                                    ;   a:  [de]的值;
                                    ;   de: 下一个可直接读取的FlashROM地址范围内的地址
                                    ; 其他:
                                    ;   hl值在本函数中会被影响，调用本函数前调用方得记得保存hl值
    pop     hl
    cp      b
    jr      z, .isRleCmd		    ; 如果刚才读出的1个字节数据，和[0xC023]中的值相等，则跳。
                                    ; [0xC023]的值，实际是FlashROM中该存档记录的第1个数据扇区的第1个字节。
                                    ; 具体见0xA41逻辑。

.isDirectData:
    ld      b, a
    xor      a
    ld      c, a
    jr      .wr_b_toGameSram
; ---------------------------------------------------------------------------
; ------------------------------------------------------------
; 这里处理读取的字节和整个备份存档第1个字节相等的情况。
; 遇到这种情况时，相当于表示要进行一次RLE解压操作，步骤是:
;   再次读取一个字节作为c，用于作为重复写入的循环次数
;   再次读取一个字节作为b，用于作为实际写入的值
; 之后逻辑就是将b的内容，重复c次，写入游戏SRAM

.isRleCmd:
    push    hl
    call    rdNxtByte_rdFls	        ; 功能概括:
                                    ;   从de所存的FlashROM地址中(0x4000-0x7FFF)，读取1个字节到a。按需递进28SF040的下一个可读扇区的Bank等参数设定。
                                    ; 功能详细:
                                    ;   从de所存的地址，读取1个字节到a。
                                    ;	如果de中的e不是0xFF，则直接返回。
                                    ;	如果de中的e是0xFF，即该扇区的最后一个字节，则：
                                    ;	  1. 搜索在缓存的Sector/文件分配表中搜索下一个属于目标文件编号的格子。
                                    ;	     如果找不到，则直接返回。
                                    ;	     如果找到则继续第2步。
                                    ;	  2. 将搜索到的缓存Sector/文件分配表中的这个格子的值置为0，避免之后再搜索到。
                                    ;	  3. 将28SF040的A18..A14锁存为搜索到的格子对应的Sector的A18..A14
                                    ;	     de值设置为搜索到的格子对应的Sector的A13..A8
                                    ;	     以便后续可以继续对28SF040进行读写。
                                    ; 重点：
                                    ; Input:
                                    ;   de: Source address for reading.
                                    ; Output:
                                    ;   a:  [de]的值;
                                    ;   de: 下一个可直接读取的FlashROM地址范围内的地址
                                    ; 其他:
                                    ;   hl值在本函数中会被影响，调用本函数前调用方得记得保存hl值
    ld      c, a
    call    rdNxtByte_rdFls	        ; 功能概括:
                                    ;   从de所存的FlashROM地址中(0x4000-0x7FFF)，读取1个字节到a。按需递进28SF040的下一个可读扇区的Bank等参数设定。
                                    ; 功能详细:
                                    ;   从de所存的地址，读取1个字节到a。
                                    ;	如果de中的e不是0xFF，则直接返回。
                                    ;	如果de中的e是0xFF，即该扇区的最后一个字节，则：
                                    ;	  1. 搜索在缓存的Sector/文件分配表中搜索下一个属于目标文件编号的格子。
                                    ;	     如果找不到，则直接返回。
                                    ;	     如果找到则继续第2步。
                                    ;	  2. 将搜索到的缓存Sector/文件分配表中的这个格子的值置为0，避免之后再搜索到。
                                    ;	  3. 将28SF040的A18..A14锁存为搜索到的格子对应的Sector的A18..A14
                                    ;	     de值设置为搜索到的格子对应的Sector的A13..A8
                                    ;	     以便后续可以继续对28SF040进行读写。
                                    ; 重点：
                                    ; Input:
                                    ;   de: Source address for reading.
                                    ; Output:
                                    ;   a:  [de]的值;
                                    ;   de: 下一个可直接读取的FlashROM地址范围内的地址
                                    ; 其他:
                                    ;   hl值在本函数中会被影响，调用本函数前调用方得记得保存hl值
    ld      b, a
    pop     hl
; ------------------------------------------------------------
; 这里会基于c的值，循环将b的值写入hl所指的FlashROM地址多次
; 从写入次数为c+1。即：
;   当c为0时，将b写入[hl+0]一次。
;   当c为3时，将b依次写入[hl+0][hl+1][hl+2][hl+3]。

.wr_b_toGameSram:
    ld      a, [byte_C022]	        ; 写入这个地址值的低5bit是上次被74HC373锁存的值的低5bit，是作为28SF040的A18..A14的值。
                                    ; 保存在这里，以便之后可以渠道上次对28SF040进行的Bank切换。
                                    ; (28SF040因此被分成2^5 = 32个Bank)
    or      $C0	; '�'               ; 混合0xC0和待74373锁存的值。
    ld      [IOPort_0x2000], a	    ; 0x2000 I/O Port:
                                    ;   Write
                                    ;	bit7..bit6:
                                    ;	  $C0:
                                    ;	    Switch to Game Cart	Mode. All access to address like $4000 passed to Game Cart.
                                    ;	  $80:
                                    ;	    Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;	bit5:
                                    ;	  Unknown. Always be 0.
                                    ;	bit4..bit0:
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would  be latched and out put to A18..A14 of 28SF040.
    ld      a, b
    ldi     [hl], a

.sw2MemBakMode_1:				    ; 写入这个地址值的低5bit是上次被74HC373锁存的值的低5bit，是作为28SF040的A18..A14的值。
    ld      a, [byte_C022]	        ; 保存在这里，以便之后可以渠道上次对28SF040进行的Bank切换。
                                    ; (28SF040因此被分成2^5 = 32个Bank)
    or      $80	; '�'               ; 混合0x80和待74373锁存的值。
    ld      [IOPort_0x2000], a	    ; 0x2000 I/O Port:
                                    ;   Write
                                    ;	bit7..bit6:
                                    ;	  $C0:
                                    ;	    Switch to Game Cart	Mode. All access to address like $4000 passed to Game Cart.
                                    ;	  $80:
                                    ;	    Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;	bit5:
                                    ;	  Unknown. Always be 0.
                                    ;	bit4..bit0:
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would  be latched and out put to A18..A14 of 28SF040.
    ld      a, h
    cp      $C0	; '�'
    jr      c, .check_c_value	    ; Jump if not	need to	increase game SRAM Bank

.sw2GameMode_2:					    ; 写入这个地址值的低5bit是上次被74HC373锁存的值的低5bit，是作为28SF040的A18..A14的值。
    ld      a, [byte_C022]	        ; 保存在这里，以便之后可以渠道上次对28SF040进行的Bank切换。
                                    ; (28SF040因此被分成2^5 = 32个Bank)
    or      $C0	; '�'               ; 混合0xC0和待74373锁存的值。
    ld      [IOPort_0x2000], a	    ; 0x2000 I/O Port:
                                    ;   Write
                                    ;	bit7..bit6:
                                    ;	  $C0:
                                    ;	    Switch to Game Cart	Mode. All access to address like $4000 passed to Game Cart.
                                    ;	  $80:
                                    ;	    Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;	bit5:
                                    ;	  Unknown. Always be 0.
                                    ;	bit4..bit0:
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would  be latched and out put to A18..A14 of 28SF040.
    ld      a, [byte_C038]	        ; 存放的是当前插的游戏卡带，最后可用的SRAM的Bank编号数量(最后一个可用Bank的序号+1)
    ld      h, a
    ld      a, [byte_C040]	        ; 局部变量:
                                    ;   0x85B和0xA25函数中:
                                    ;	表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;	表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    inc     a
    cp      h
    ret     nc			            ; 如果当前设置给游戏卡的SRAM Bank ID已经是这个卡的最后一个SRAM Bank，
                                    ; 则不再继续直接返回，结束整个备份档恢复到卡带的逻辑流程。

.incGameSramBank:				    ; 局部变量:
    ld      [byte_C040], a	        ;   0x85B和0xA25函数中:
                                    ;	表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;	表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    ld      [byte_4000], a

.sw2MemBakMode_2:				    ; 写入这个地址值的低5bit是上次被74HC373锁存的值的低5bit，是作为28SF040的A18..A14的值。
    ld      a, [byte_C022]	        ; 保存在这里，以便之后可以渠道上次对28SF040进行的Bank切换。
                                    ; (28SF040因此被分成2^5 = 32个Bank)
    or      $80	; '�'               ; 混合0x80和待74373锁存的值。
    ld      [IOPort_0x2000], a	    ; 0x2000 I/O Port:
                                    ;   Write
                                    ;	bit7..bit6:
                                    ;	  $C0:
                                    ;	    Switch to Game Cart	Mode. All access to address like $4000 passed to Game Cart.
                                    ;	  $80:
                                    ;	    Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;	bit5:
                                    ;	  Unknown. Always be 0.
                                    ;	bit4..bit0:
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would  be latched and out put to A18..A14 of 28SF040.
    ld      hl, $A000

.check_c_value:
    ld      a, c
    sub     1
    ld      c, a
    jr      nc, .wr_b_toGameSram

.c_Is_Zero:
    jr      .chkEndOfBakData
; End of function core_Bak2Sram


; =============== S U B R O U T I N E =======================================

; 选好要恢复的备份，按A后，出现的Working等待处理中界面

dispRstoWorking:
    call    waitVBlankOccur	        ; 循环halt等待中断发生，直到VBlank发生1次后，本函数返回。
    call    clearOAM		        ; Fill [0xFE00-0xFE9F] OAM address range with	data 0x00.

.clrBgMap:
    ld      hl, $9800
    ld      bc, $400
    ld      a, $20 ; ' '
    call    vramFillWith_A	        ; 赋值源a中数据到目标地址中(目标地址可以是VRAM)，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; 为了确保目标地址是VRAM地址(8000h-9FFFh)或OAM(FE00h-FE9Fh)地址时也可写入，本函数会确保在LCD的HVBlank发生期间操作。
                                    ;
                                    ; Parameters:
                                    ;   LengthOfByte to fill: bc
                                    ;   Value: a
                                    ;   DestAddr:[hl]
                                    ;

.printString_Ln7:
    ld      a, 0
    ld      [byte_C00E], a	        ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      a, 6
    ld      [byte_C00F], a	        ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      hl, strWorking
    call    strPrintf		        ; 打印字符串到屏幕
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
    call    delay1second	        ; 这个函数设置Timer为4096Hz，然后设置Timer计数器初始为0x80(即还剩0x80计数到0xFF、发生中断)
                                    ; 然后发生0x20次中断后返回，即0x80 * 0x20	= 0x1000即4096计数，即延时1秒后函数返回。
    call    core_Bak2Sram	        ; 这个函数根据0xC02B中指定的备份文件的ID，对备份数据进行读取和RLE解压，
                                    ; 将数据写入到卡带的SRAM中，完成备份文件恢复到卡带的底层数据写入逻辑。

.reloadFAT:					        ; Copy 28SF040:[0x7F000-0x7F7FF] to WRAM-1:[0xD800-0xDFFF]
    call    rdSF040_0x7F000	        ; 从28SF040加载Sector/文件分配表到缓存[0xD800-0xDFFF]
    call    delay1second	        ; 这个函数设置Timer为4096Hz，然后设置Timer计数器初始为0x80(即还剩0x80计数到0xFF、发生中断)
                                    ; 然后发生0x20次中断后返回，即0x80 * 0x20	= 0x1000即4096计数，即延时1秒后函数返回。
    ret
; End of function dispRstoWorking

; ---------------------------------------------------------------------------
unusedStr_03:	    db "run_error=%d",$A,0
