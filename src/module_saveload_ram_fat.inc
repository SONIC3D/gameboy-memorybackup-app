; =============== S U B R O U T I N E =======================================

; Copy 28SF040:[0x7F000-0x7F7FF] to WRAM-1:[0xD800-0xDFFF]
; 从28SF040加载Sector/文件分配表到缓存[0xD800-0xDFFF]

rdSF040_0x7F000:
    ld      de, $7F0                ; $7F0 = 111 1111 0000   // map to 28SF040[A18..A8]: 111 11110000 xxxxxxxx = 0x7F0xx
                                    ; 实际就是为了访问(0x7F0 << 8)对应的地址。
    call    gotoSF040_Bank          ; 将输入的de理解为28SF040的A18..A16和A15..A8地址线对应的bit位。
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
                                    ;    扇区擦除时Flash的SectorAddress地址中的A18..A8
                                    ;    其中d的低3bit存放A18..A16，e中存放A15..A8
                                    ; 输出参数
                                    ;   de
                                    ;    d寄存器，bit7为0，bit6为1，bit5..bit0存放A13..A8
                                    ;    e寄存器A7..A0 = 0x0
                                    ;   即输出的de拼上74373输出的高5位，形成了一个低8bit为0的28SF040的SectorAddress。
                                    ;
                                    ; 举例：
                                    ;   要访问0x7FF00-0x7FFFF这个Sector(28SF040的最后一个sector)
                                    ;   把A18..A8的0x7FF存入de，即d=0x07,e=0xFF, de=0x7FF=0111 1111    1111
                                    ;   把这个de拆成高5bit和低6bit，即0111 11和11 1111
                                    ;   最终de的高5bit被前面补0为0001 1111写入[0xC022]
                                    ;   返回值是de的低6bit，高位设成01，然后低位补0。即01 111111 00000000
                                    ;   *    高位设成01，是为了让FlashROM的地址范围0x0000-0x3FFF被映射到GB地址范围的0x4000-0x7FFF
                                    ;    这样在调用这个函数后，就可以直接用de作为地址对FlashROM进行直接的读写访问
                                    ;
                                    ; 伪代码:
                                    ;   a    = (de >> 6) & 0x1F;
                                    ;   [0xC022] = a;
                                    ;   [IO_0x2000] = (0x80 | a);
                                    ;   de = ((e & 0x3F) | 0x40) << 8;
                                    ;   return de;
    push    de
    pop     hl                      ; Copy [0x7F000-0x7F7FF] to [0xD800-0xDFFF]
    ld      de, $D800
    ld      bc, $800
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]
    ret
; End of function rdSF040_0x7F000


; =============== S U B R O U T I N E =======================================

; Copy WRAM:1:[0xD800-0xDFFF] to 28SF040:[0x7F000-0x7F7FF].
; 保存Sector分配表到FlashROM

wrSF040_0x7F000:
    ld      hl, $D800
    ld      de, $7F0
    ld      bc, $800
    call    SF040_Cp2Sector         ; 复制指定长度数据到28SF040指定的Sector(复制数据的总字节数必须为256的整数倍)
                                    ; Parameters:
                                    ;   hl: 待复制到28SF040目标扇区的数据的起始地址
                                    ;   de: A18..A8 of target sector(Sector Address, SA) of 28SF040
                                    ;   bc: 总共复制多少字节
                                    ;      实际c被忽略，所以这个参数必须是0x100(256)的整数倍
                                    ;      即28SF040的Sector大小的整数倍
    ret
; End of function wrSF040_0x7F000


; =============== S U B R O U T I N E =======================================

; Copy 28SF040:[0x7F800-0x7FEFF] to WRAM-0:[0xC063-0xC762]

rdSF040_0x7F800:
    ld      de, $7F8
    call    gotoSF040_Bank          ; 将输入的de理解为28SF040的A18..A16和A15..A8地址线对应的bit位。
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
                                    ;    扇区擦除时Flash的SectorAddress地址中的A18..A8
                                    ;    其中d的低3bit存放A18..A16，e中存放A15..A8
                                    ; 输出参数
                                    ;   de
                                    ;    d寄存器，bit7为0，bit6为1，bit5..bit0存放A13..A8
                                    ;    e寄存器A7..A0 = 0x0
                                    ;   即输出的de拼上74373输出的高5位，形成了一个低8bit为0的28SF040的SectorAddress。
                                    ;
                                    ; 举例：
                                    ;   要访问0x7FF00-0x7FFFF这个Sector(28SF040的最后一个sector)
                                    ;   把A18..A8的0x7FF存入de，即d=0x07,e=0xFF, de=0x7FF=0111 1111    1111
                                    ;   把这个de拆成高5bit和低6bit，即0111 11和11 1111
                                    ;   最终de的高5bit被前面补0为0001 1111写入[0xC022]
                                    ;   返回值是de的低6bit，高位设成01，然后低位补0。即01 111111 00000000
                                    ;   *    高位设成01，是为了让FlashROM的地址范围0x0000-0x3FFF被映射到GB地址范围的0x4000-0x7FFF
                                    ;    这样在调用这个函数后，就可以直接用de作为地址对FlashROM进行直接的读写访问
                                    ;
                                    ; 伪代码:
                                    ;   a    = (de >> 6) & 0x1F;
                                    ;   [0xC022] = a;
                                    ;   [IO_0x2000] = (0x80 | a);
                                    ;   de = ((e & 0x3F) | 0x40) << 8;
                                    ;   return de;
    push    de
    pop     hl
    ld      de, C063_fnameCache     ; [0xC063-0xC762]:
                                    ;   每$10个字节一组，存放N组卡带中已经备份数据的文件名，总数量在$C763中。
                                    ;   总共$70组，即112组，但是实际好像只用了100组也就是1600字节(0x640字节)
                                    ;   用$700只是为了写入28SF040时，对齐256字节的扇区
    ld      bc, $700
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]
    ret
; End of function rdSF040_0x7F800


; =============== S U B R O U T I N E =======================================

; Copy WRAM:0:[0xC063-0xC762] to 28SF040:[0x7F800-0x7FEFF].

wrSF040_0x7F800:
    ld      hl, $C063
    ld      de, $7F8
    ld      bc, $700
    call    SF040_Cp2Sector         ; 复制指定长度数据到28SF040指定的Sector(复制数据的总字节数必须为256的整数倍)
                                    ; Parameters:
                                    ;   hl: 待复制到28SF040目标扇区的数据的起始地址
                                    ;   de: A18..A8 of target sector(Sector Address, SA) of 28SF040
                                    ;   bc: 总共复制多少字节
                                    ;      实际c被忽略，所以这个参数必须是0x100(256)的整数倍
                                    ;      即28SF040的Sector大小的整数倍
    ret
; End of function wrSF040_0x7F800


; =============== S U B R O U T I N E =======================================

; Copy 28SF040:[0x7FF00-0x7FFFF] to WRAM-0:[0xC763-0xC862]

rdSF040_0x7FF00:
    ld      de, $7FF
    call    gotoSF040_Bank          ; 将输入的de理解为28SF040的A18..A16和A15..A8地址线对应的bit位。
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
                                    ;    扇区擦除时Flash的SectorAddress地址中的A18..A8
                                    ;    其中d的低3bit存放A18..A16，e中存放A15..A8
                                    ; 输出参数
                                    ;   de
                                    ;    d寄存器，bit7为0，bit6为1，bit5..bit0存放A13..A8
                                    ;    e寄存器A7..A0 = 0x0
                                    ;   即输出的de拼上74373输出的高5位，形成了一个低8bit为0的28SF040的SectorAddress。
                                    ;
                                    ; 举例：
                                    ;   要访问0x7FF00-0x7FFFF这个Sector(28SF040的最后一个sector)
                                    ;   把A18..A8的0x7FF存入de，即d=0x07,e=0xFF, de=0x7FF=0111 1111    1111
                                    ;   把这个de拆成高5bit和低6bit，即0111 11和11 1111
                                    ;   最终de的高5bit被前面补0为0001 1111写入[0xC022]
                                    ;   返回值是de的低6bit，高位设成01，然后低位补0。即01 111111 00000000
                                    ;   *    高位设成01，是为了让FlashROM的地址范围0x0000-0x3FFF被映射到GB地址范围的0x4000-0x7FFF
                                    ;    这样在调用这个函数后，就可以直接用de作为地址对FlashROM进行直接的读写访问
                                    ;
                                    ; 伪代码:
                                    ;   a    = (de >> 6) & 0x1F;
                                    ;   [0xC022] = a;
                                    ;   [IO_0x2000] = (0x80 | a);
                                    ;   de = ((e & 0x3F) | 0x40) << 8;
                                    ;   return de;
    push    de
    pop     hl
    ld      de, byte_C763           ; 0xC763是缓存在内存的编目数据中，用来表示当前备份存档总计个数，长度1个字节。
    ld      bc, $100
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]
    ret
; End of function rdSF040_0x7FF00


; =============== S U B R O U T I N E =======================================

; 计算[0xC763]..[0xC861]共0xFE个字节的校验和，并加上初始校验和值0xAE
; 返回时，c和a中都是checksum最终值
; Make checksum    by adding [0xC763]..[0xC861] to    0xAE.
;
; Pseudo code:
;   byte* addr=0xC763;
;   byte  chksum = 0xAE;
;   for    (b=0xFF,b>0;b--) {
;     chksum +=    (*addr);
;     addr += 1;
;   }

makeChksum_C763:
    ld      hl, byte_C763           ; Checksum start address.
    ld      b, $FF                  ; Length of bytes to sum.
    ld      c, $AE                  ; Init checksum = 0xAE.

.loopSum:
    ldi     a, [hl]
    add     a, c
    ld      c, a
    dec     b
    jr      nz, .loopSum
    ret
; End of function makeChksum_C763


; =============== S U B R O U T I N E =======================================

; 检查0xC862中的checksum是否和0xAE + [0xC763]..[0xC861]的值相等
; 返回值
;   Z Flag:
;     set(z):  Checksum相等
;     clr(nz): Checksum不相等

verifyChksum:
    ld      a, [byte_C862]          ; 存放0xAE + [0xC763]..[0xC861]共0xFE个字节的校验和
    ld      d, a
    call    makeChksum_C763         ; 计算[0xC763]..[0xC861]共0xFE个字节的校验和，并加上初始校验和值0xAE
                                    ; 返回时，c和a中都是checksum最终值
                                    ; Make checksum by adding [0xC763]..[0xC861] to 0xAE.
                                    ;
                                    ; Pseudo code:
                                    ;   byte* addr=0xC763;
                                    ;   byte  chksum = 0xAE;
                                    ;   for (b=0xFF,b>0;b--) {
                                    ;    chksum += (*addr);
                                    ;    addr +=    1;
                                    ;   }
    cp      d
    ret
; End of function verifyChksum


; =============== S U B R O U T I N E =======================================

; Set [0xDFF0-0xDFFF] to 0xFF.
; Create checksum for [0xC763-0xC861] and store    the checksum at    [0xC862].
; Copy WRAM-0:[0xC763-0xC862] to 28SF040:[0x7FF00-0x7FFFF].

wrSF040_0x7FF00:
    ld      hl, $DFF0               ; Set [0xDFF0-0xDFFF] to 0xFF.
    ld      bc, $10
    ld      a, $FF
    call    memFillWith_A           ; 赋值源a中数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to fill: bc
                                    ;   Value: a
                                    ;   DestAddr:[hl]
; ---------------------------------------------------------------------------
; Create checksum for [0xC763-0xC861] and store the checksum at [0xC862].
    call    makeChksum_C763      ; 计算[0xC763]..[0xC861]共0xFE个字节的校验和，并加上初始校验和值0xAE
                                    ; 返回时，c和a中都是checksum最终值
                                    ; Make checksum by adding [0xC763]..[0xC861] to 0xAE.
                                    ;
                                    ; Pseudo code:
                                    ;   byte* addr=0xC763;
                                    ;   byte  chksum = 0xAE;
                                    ;   for (b=0xFF,b>0;b--) {
                                    ;    chksum += (*addr);
                                    ;    addr +=    1;
                                    ;   }
    ld      [byte_C862], a          ; 存放0xAE + [0xC763]..[0xC861]共0xFE个字节的校验和
; ---------------------------------------------------------------------------
; Copy WRAM-0:[0xC763-0xC862] to 28SF040:[0x7FF00-0x7FFFF].
    ld      hl, $C763
    ld      de, $7FF
    ld      bc, $100
    call    SF040_Cp2Sector         ; 复制指定长度数据到28SF040指定的Sector(复制数据的总字节数必须为256的整数倍)
                                    ; Parameters:
                                    ;   hl: 待复制到28SF040目标扇区的数据的起始地址
                                    ;   de: A18..A8 of target sector(Sector Address, SA) of 28SF040
                                    ;   bc: 总共复制多少字节
                                    ;      实际c被忽略，所以这个参数必须是0x100(256)的整数倍
                                    ;      即28SF040的Sector大小的整数倍
    ret
; End of function wrSF040_0x7FF00
