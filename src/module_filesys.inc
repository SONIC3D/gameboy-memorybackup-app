; =============== S U B R O U T I N E =======================================

; 执行删除Backup的某个存档

doDeleteBackup:
    ld      hl, $D800
    ld      a, [byte_C02B]          ; 在Restore或Delete界面，按A键时，光标选中的备份文件的顺序号+1
                                    ; (不是当前屏幕内的序号，是在所有备份中的顺序号)
                                    ; 这个序号>=1，且<=([0xC763])
    ld      b, a                    ; b = 选中要要删除的存档的序号，序号从1开始计
; ==========================
; 这一段实质逻辑是对Sector分配表对应的存档文件编号，进行修改:
;   1.遍历0xD800-0xDFFF(Sector分配表)
;   2.对每个内容值与目标序号相等的地址，内容值置0
;   3.对每个内容值比目标序号大的地址，内容值减1
;   相当于是对一个文件分配表中，某个特定序号的文件作抽离后的处理
;
; 这一段实质逻辑对应的伪代码是:
;
; do {
;   if ([hl] > [0xC02B]) {
;     [hl] = [hl]-1;
;   } else if ([hl] == [0xC02B]) {
;     [hl] = 0;
;   }
;   hl++;
; } while (hl<0xE000)

.processNextHL:
    ld      a, [hl]
    cp      b
    jr      c, .updateHL            ; Jump if ([hl] < [0xC02B])

.hl_GE_0xC02B:
    jr      nz, .decA

.hl_Equ_0xC02B:
    ld      a, 1

.decA:
    dec     a

.updateHL:
    ldi     [hl], a
    ld      a, h
    cp      $E0
    jr      c, .processNextHL
; ----------------------------------
; 以下代码大致逻辑，是从文件名列表中抽掉被删除的文件名
;
; 0xC063到0xC762的内存范围，共0x700字节，每0x10一组代表一个文件名记录。
; 要删除一个文件名记录，就把这个文件记录开始的数据，用其后0x10偏移处开始的下一个文件记录数据进行覆盖
; 即从该文件记录的地址+0x10开始，直到0xC763-1，往前移动0x10个字节。
;
    ld      a, [byte_C02B]          ; 在Restore或Delete界面，按A键时，光标选中的备份文件的顺序号+1
                                    ; (不是当前屏幕内的序号，是在所有备份中的顺序号)
                                    ; 这个序号>=1，且<=([0xC763])
    dec     a                       ; a = [0xC02B]-1;  // 取以0为基的偏移量而非以1为基的序号
    ld      l, a
    ld      h, 0
    add     hl, hl
    add     hl, hl
    add     hl, hl
    add     hl, hl
    push    hl
    pop     de                      ; de = ([0xC02B] - 1) << 4
    ld      hl, C063_fnameCache     ; [0xC063-0xC762]:
                                    ;   每$10个字节一组，存放N组卡带中已经备份数据的文件名，总数量在$C763中。
                                    ;   总共$70组，即112组，但是实际好像只用了100组也就是1600字节(0x640字节)
                                    ;   用$700只是为了写入28SF040时，对齐256字节的扇区
    add     hl, de                  ; hl = 0xC063 + de = 0xC063 + (([0xC02B] - 1) << 4)
    push    hl                      ; pop at 0x45E
                                    ; 作为复制目标起始地址de给后面的memcpyHL2DE用
; ----------------------------------------
    ld      de, $10
    add     hl, de
    push    hl                      ; hl = hl + 0x10
                                    ; 作为复制源起始地址hl给后面的memcpyHL2DE用
; ----------------------------------------
    pop     de                      ; de = hl
    ld      bc, byte_C763           ; bc = 0xC763
    ld      a, c
    sub     e
    ld      c, a                    ; c = c-e
    ld      a, b
    sbc     a, d
    ld      b, a                    ; b = b-d-carryflag
                                    ; 即:
                                    ; bc = bc-de = 0xC763-hl
                                    ; 作为总复制字节数bc给后面的memcpyHL2DE用
; ----------------------------------------
    pop     de                      ; pushed at 0x44E
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]

.decBakFileCount:                   ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。
    ld      a, [byte_C763]
    dec     a
    ld      [byte_C763], a          ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。

.savToFlashROM:                     ; Copy WRAM:1:[0xD800-0xDFFF] to 28SF040:[0x7F000-0x7F7FF].
    call    wrSF040_0x7F000         ; 保存Sector分配表到FlashROM
    call    wrSF040_0x7F800         ; Copy WRAM:0:[0xC063-0xC762] to 28SF040:[0x7F800-0x7FEFF].
    call    wrSF040_0x7FF00         ; Set [0xDFF0-0xDFFF] to 0xFF.
                                    ; Create checksum for [0xC763-0xC861] and store the checksum at [0xC862].
                                    ; Copy WRAM-0:[0xC763-0xC862] to 28SF040:[0x7FF00-0x7FFFF].
    ret
; End of function doDeleteBackup


; =============== S U B R O U T I N E =======================================

; 找出现有存档中是否有同名备份。
; 具体实现是以0xC763中的存档备份总数作为循环次数，从0xC063开始循环
; 每次读取0x10个字节和用户输入在0xC042中的文件名作比较。
;   输入值:
;     [0xC042]: 其中存放用户输入的文件名内容。
;   返回值:
;     a=0，表示找到同名的，此时hl就是这个存档文件名的偏移地址(在0xC063开始的范围)。
;     a=1，表示没有找到同名的（也可能是0xC776这个用途不明的字节，内容值为0导致）
;
;
; sub_473() {
;   if (byte_C776_OptVal_UseUniqueName != 0) {
;     unsigned char byte_C040 = 0;
;     hl = 0xC063;
;
;     do {
;   c = 0x10;       // 字节数组比较长度
;   de = 0xC042;        // 字节数组比较的源地址
;   int currAddr = hl;  // 字节数组比较的目标地址
;   do {
;     b = [currAddr++];
;     a = [de++];
;
;     if (a!=b)
;       break;
;
;     c--;
;     if (c==0) {
;       a = 0;
;       // 返回a=0,表示hl开始的0x10个字节和$C042开始的10个字节全等
;       return a,hl;
;     }
;   } while (c>0)
;
;   // 字节数组比较的源地址递进$10字节，到下一条记录
;   hl += 0x10;
;   // 当前正在处理的存档备份编号递增1
;   byte_C040++;
;     } while(byte_C040 < byte_C763)  // byte_C763是内存编目中，存档备份的总个数
;   }
;   a=1;
;   return a;
; }

checkIfNameDupe:
    ld      a, [C776_OptVal04]      ; Option选项值，是否强制要求唯一文件名。为0时，表示允许同名文件。
                                    ; 初始化为0x0，即允许同名文件。
    or      a
    jr      z, .noDupeNameFound

.C776_isnot_0:
    xor     a
    ld      [byte_C040], a          ; 局部变量:
                                    ;   0x85B和0xA25函数中:
                                    ;    表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;    表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    ld      hl, C063_fnameCache     ; [0xC063-0xC762]:
                                    ;   每$10个字节一组，存放N组卡带中已经备份数据的文件名，总数量在$C763中。
                                    ;   总共$70组，即112组，但是实际好像只用了100组也就是1600字节(0x640字节)
                                    ;   用$700只是为了写入28SF040时，对齐256字节的扇区

.checkNextName:
    ld      c, $10
    ld      de, C042_bufFileNam     ; 共$10字节。用于存放备份功能时，用户输入的文件名字符串，以便用于和卡中已有存档名进行比较。
    push    hl

.compareNextByte:
    ldi     a, [hl]
    ld      b, a
    ld      a, [de]
    inc     de
    cp      b
    jr      z, .hl_value_equ_de     ; Jump if [hl]==[de]

.hl_value_neq_de:
    pop     hl
    ld      de, $10
    add     hl, de

.chkNameCntLimit:                   ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。
    ld      a, [byte_C763]
    ld      b, a
    ld      a, [byte_C040]          ; 局部变量:
                                    ;   0x85B和0xA25函数中:
                                    ;    表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;    表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    inc     a
    ld      [byte_C040], a          ; 局部变量:
                                    ;   0x85B和0xA25函数中:
                                    ;    表示当前设置给Game Cart的SRAM Bank ID
                                    ;   0x1034(listSavFilename)函数中:
                                    ;    表示当前正在处理的backup filename entry偏移(也可以理解为是已经处理好的entry数量)
    cp      b
    jr      c, .checkNextName

.noDupeNameFound:
    ld      a, 1
    ret
; ---------------------------------------------------------------------------

.hl_value_equ_de:
    dec     c
    jr      nz, .compareNextByte

.equalNameFound:
    pop     hl
    xor     a
    ret
; End of function checkIfNameDupe


; =============== S U B R O U T I N E =======================================

; 计算备份空间的占用情况，将计算值存入[0xC03C]，数值实际应该在[0-$40]内，即[0-64]。
; 计算[0xD800-0xD8FF]的范围，共有多少个字节非0，该计数存入bc。
; [0xC03C] = (int)((bc+8)/32);

calcSpaceUsage:
    ld      bc, 0
    ld      hl, $D800

.readNextBlock:
    ldi     a, [hl]
    or      a
    jr      z, .checkUpLimit

.incrCounter:
    inc     bc

.checkUpLimit:
    ld      a, h
    cp      $E0
    jr      c, .readNextBlock
    ld      hl, 8
    add     hl, bc                  ; hl = bc + 8

.hl_Div_32:
    srl     h
    rr      l
    srl     h
    rr      l
    srl     h
    rr      l
    srl     h
    rr      l                       ; hl = hl>>4    // hl = hl/16
    srl     l                       ; hl >> 1;      // hl累计除以32

.storeResult:
    ld      a, l
    ld      [byte_C03C], a          ; 备份空间的占用情况，数值实际应该在[0-$40]内，即[0-64]。
    ret
; End of function calcSpaceUsage


; =============== S U B R O U T I N E =======================================

; 根据0xC03C内的值，画出对应的存档空间占用比例的进度条。

drawCapacityBar:
    ld      a, [byte_C03C]          ; 备份空间的占用情况，数值实际应该在[0-$40]内，即[0-64]。
    cp      $41    ; 'A'
    jr      c, .getPrgrsBarType

.setClampValue:
    ld      a, $40 ; '@'

.getPrgrsBarType:
    ld      e, a
    srl     e
    srl     e
    srl     e                       ; (进度值/8)并取整，用来选择8种不同的进度条大类。
    sla     e                       ; e*2，用来作为一个(2字节*8个元素)的数组的单字节偏移值。
    ld      d, 0
    ld      hl, $575                ; 0x575是这个(2字节*8个元素)数组的起始
    add     hl, de                  ; hl = 最终进度条模版字符串指针值所在的地址

.getStrPtr2DE:
    ldi     a, [hl]
    ld      e, a
    ld      a, [hl]
    ld      d, a
    push    de                      ; popped at 0x51C, pop to hl

.readCharForFmt:                    ; 备份空间的占用情况，数值实际应该在[0-$40]内，即[0-64]。
    ld      a, [byte_C03C]
    and     7
    inc     a
    ld      d, a                    ; d = ([byte_C03C] & b'00000111)+1
                                    ; 这里是取空间占用数量内存[0xC03C]的最低3bit，得到了0-7的值，然后加1，得到1-8的值
                                    ; 这1-8的值正号对应FontTile中，一个小方格从1-8的充满度的图，用这个图作为进度条描画的最后一格

.setPrintPos:
    ld      a, 5
    ld      [byte_C00E], a          ; 当前输出内容的列偏移坐标(共32列，最后一列0x1F。其中最左侧20列可见,从左到右0-0x13，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf
    ld      a, $10
    ld      [byte_C00F], a          ; 当前输出内容的行偏移坐标(共32行，最后一行0x1F。屏幕上可见的共17行,从上到下0-0x11，根据卷轴状态，可能有偏移)。
                                    ; 具体参考函数:
                                    ;   0x691 strPrintf

.storeSP:                           ; if sp=0xFFFC, then [0xC00C]=0xFC, [0xC00D]=0xFF
    ld      [word_C00C], sp
    ld      sp, $C00C
    push    hl                      ; [0xC00A] = hl
    push    de                      ; [0xC008] = de
                                    ;    [0xC008] = e
                                    ;    [0xC009] = d = ([byte_C03C] & b'00000111)+1
    push    bc                      ; [0xC006] = bc
    push    af                      ; [0xC004] = af

.restoreSP:
    ld      a, [word_C00C]
    ld      l, a
    ld      a, [word_C00C+1]
    ld      h, a
    ld      sp, hl
    ld      hl, C010_PrintArgv1     ; 在0x691的strPrintf函数中，用来作为第1个格式化符号对应的参数
    ld      a, [byte_C009]
    ldi     [hl], a

.printFmtChar:
    pop     hl                      ; pushed at 0x4F0
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
; End of function drawCapacityBar
