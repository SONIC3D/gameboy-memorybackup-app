; =============== S U B	R O U T	I N E =======================================

; 等待28SF040的ToggleBit不再变化。
; （即写入/扇区擦除/全片擦除操作完成）

waitToggleBit:
    ld      a, [byte_4000]
    and	    $40

.loopCmpTogleBit:
    ld      c, a
    ld      a, [byte_4000]
    and	    $40
    cp	    c
    jr	    nz,	.loopCmpTogleBit
    ret
; End of function waitToggleBit


; =============== S U B	R O U T	I N E =======================================

; 切换到MemoryBackupCart模式，并解锁28SF040
; 注意: 有类似函数$743(负责加锁28SF040)

SF040_Unprotect:
    ld      a, $80
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
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would	be latched and out put to A18..A14 of 28SF040.

.unprtct_28SF040:				    ; Check SST 28SF040A doc:
    ld      a, [unk_5823]	        ; Software Data Unprotect
    ld      a, [unk_5820]
    ld      a, [unk_5822]
    ld      a, [unk_4418]
    ld      a, [unk_441B]
    ld      a, [unk_4419]
    ld      a, [unk_441A]
    ret
; End of function SF040_Unprotect


; =============== S U B	R O U T	I N E =======================================

; 切换到MemoryBackupCart模式，并加锁28SF040
; 注意: 有类似函数$728(负责解锁28SF040)

SF040_Protect:
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
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would	be latched and out put to A18..A14 of 28SF040.

.protect_28SF040:				    ; Check SST 28SF040A doc:
    ld      a, [unk_5823]	        ; Software Data protect
    ld      a, [unk_5820]
    ld      a, [unk_5822]
    ld      a, [unk_4418]
    ld      a, [unk_441B]
    ld      a, [unk_4419]
    ld      a, [unk_440A]
		    ret
; End of function SF040_Protect


; =============== S U B	R O U T	I N E =======================================


SF040_ChipErase:
    push    af
    ld      a, [byte_FFFF]	        ; FFFF - IE -	Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt	Enable	(INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt	Enable	(INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt	Enable	(INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt	Enable	(INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt	Enable	(INT 60h)  (1=Enable)
    ld      [byte_C039], a	        ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt	Enable (R/W)
    pop	    af
    call    waitToggleBit	        ; 等待28SF040的ToggleBit不再变化。
                                    ; （即写入/扇区擦除/全片擦除操作完成）
    call    SF040_Unprotect	        ; 切换到MemoryBackupCart模式，并解锁28SF040
                                    ; 注意: 有类似函数$743(负责加锁28SF040)
    ld      a, $30 ; '0'
    ld      [byte_4000], a
    ld      a, $30 ; '0'
    ld      [byte_4000], a	        ; 28SF040 $30	$30 command: Chip Erase
    call    waitToggleBit	        ; 等待28SF040的ToggleBit不再变化。
                                    ; （即写入/扇区擦除/全片擦除操作完成）
    call    SF040_Protect	        ; 切换到MemoryBackupCart模式，并加锁28SF040
                                    ; 注意: 有类似函数$728(负责解锁28SF040)
    push    af
    ld      a, [byte_C039]	        ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt	Enable (R/W)
    ld      [byte_FFFF], a	        ; FFFF - IE -	Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt	Enable	(INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt	Enable	(INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt	Enable	(INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt	Enable	(INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt	Enable	(INT 60h)  (1=Enable)
    pop	    af
    ret
; End of function SF040_ChipErase


; =============== S U B	R O U T	I N E =======================================

; 读取存储芯片(28SF040)的厂商/设备ID
; 返回值
;   h: 28SF040的Device	Code,应当为$04
;   l: 28SF040的厂商ID, 应当$BF

SF040_ReadId:
    push    af
    ld      a, [byte_FFFF]	        ; FFFF - IE -	Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt	Enable	(INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt	Enable	(INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt	Enable	(INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt	Enable	(INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt	Enable	(INT 60h)  (1=Enable)
    ld      [byte_C039], a	        ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt	Enable (R/W)
    pop	    af
    call    waitToggleBit	        ; 等待28SF040的ToggleBit不再变化。
                                    ; （即写入/扇区擦除/全片擦除操作完成）

.SF040_Cmd_DevId:
    ld      a, $90 ; '�'
    ld      [byte_4000], a	        ; 28SF040 $90	command: Read ID
    ld      a, [byte_4000]
    ld      l, a		            ; l =	Manufacturer Code of 28SF040   // Should be 0xBF
    ld      a, [byte_4001]
    ld      h, a		            ; h =	Device Code of 28SF040	 // Should be 0x04

.SF040_Cmd_Reset:
    ld      a, $FF
    ld      [byte_4000], a	        ; 28SF040 $FF	command: Reset
    push    af
    ld      a, [byte_C039]	        ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt	Enable (R/W)
    ld      [byte_FFFF], a	        ; FFFF - IE -	Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt	Enable	(INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt	Enable	(INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt	Enable	(INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt	Enable	(INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt	Enable	(INT 60h)  (1=Enable)
    pop	    af
    ret
; End of function SF040_ReadId


; =============== S U B	R O U T	I N E =======================================

; 将输入的de理解为28SF040的A18..A16和A15..A8地址线对应的bit位。
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
;     扇区擦除时Flash的SectorAddress地址中的A18..A8
;     其中d的低3bit存放A18..A16，e中存放A15..A8
; 输出参数
;   de
;     d寄存器，bit7为0，bit6为1，bit5..bit0存放A13..A8
;     e寄存器A7..A0 = 0x0
;   即输出的de拼上74373输出的高5位，形成了一个低8bit为0的28SF040的SectorAddress。
;
; 举例：
;   要访问0x7FF00-0x7FFFF这个Sector(28SF040的最后一个sector)
;   把A18..A8的0x7FF存入de，即d=0x07,e=0xFF, de=0x7FF=0111 1111 1111
;   把这个de拆成高5bit和低6bit，即0111 11和11 1111
;   最终de的高5bit被前面补0为0001 1111写入[0xC022]
;   返回值是de的低6bit，高位设成01，然后低位补0。即01 111111	00000000
;   * 高位设成01，是为了让FlashROM的地址范围0x0000-0x3FFF被映射到GB地址范围的0x4000-0x7FFF
;     这样在调用这个函数后，就可以直接用de作为地址对FlashROM进行直接的读写访问
;
; 伪代码:
;   a =	(de >> 6) & 0x1F;
;   [0xC022] = a;
;   [IO_0x2000]	= (0x80	| a);
;   de = ((e & 0x3F) | 0x40) <<	8;
;   return de;

gotoSF040_Bank:
    push    de
    rl	    e
    rl	    d
    rl	    e
    rl	    d			            ; d的最低2bit来自于e的最高2bit
                                    ; d =	d.b5/4/3/2/1/0/e.b7/6
    ld      a, d
    and	    $1F			            ; 取d的低5bit，即进入函数时，d的低3bit和e的高2bit
                                    ; 即A18..A14
    ld      [byte_C022], a	        ; 写入这个地址值的低5bit是上次被74HC373锁存的值的低5bit，是作为28SF040的A18..A14的值。
                                    ; 保存在这里，以便之后可以渠道上次对28SF040进行的Bank切换。
                                    ; (28SF040因此被分成2^5 = 32个Bank)
    or	    $80	; '�'               ; 混合0x80和待74373锁存的值。
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
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would	be latched and out put to A18..A14 of 28SF040.
    pop	    de
    ld      a, e
    and	    $3F	; '?'
    or	    $40	; '@'               ; a = e的bit7清零，bit6置1，保留e的低6bit原来值
    ld      d, a
    ld      e, 0
    ret
; End of function gotoSF040_Bank


; =============== S U B	R O U T	I N E =======================================

; 复制指定长度数据到28SF040指定的Sector(复制数据的总字节数必须为256的整数倍)
; Parameters:
;   hl:	待复制到28SF040目标扇区的数据的起始地址
;   de:	A18..A8	of target sector(Sector	Address, SA) of	28SF040
;   bc:	总共复制多少字节
;	实际c被忽略，所以这个参数必须是0x100(256)的整数倍
;	即28SF040的Sector大小的整数倍

SF040_Cp2Sector:
    push    af
    ld      a, [byte_FFFF]	        ; FFFF - IE -	Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt	Enable	(INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt	Enable	(INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt	Enable	(INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt	Enable	(INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt	Enable	(INT 60h)  (1=Enable)
    ld      [byte_C039], a	        ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt	Enable (R/W)
    pop	    af
    call    waitToggleBit	        ; 等待28SF040的ToggleBit不再变化。
                                    ; （即写入/扇区擦除/全片擦除操作完成）
    call    SF040_Unprotect	        ; 切换到MemoryBackupCart模式，并解锁28SF040
                                    ; 注意: 有类似函数$743(负责加锁28SF040)
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

.startSectorErs:
    ld      a, $20 ; ' '
    ld      [byte_4000], a	        ; 28SF040 $20	command: Sector	Erase
    ld      a, $D0 ; '�'            ; Write 0xD0 to SA(Sector Address)
    ld      [de], a

.chkToggleBit:
    ld      a, [byte_4000]	        ; 28SF040: Read Toggle bit
    and	    $40	; '@'

.loopCmpTogleBit:
    ld      c, a
    ld      a, [byte_4000]	        ; 28SF040: Read Toggle bit
    and	    $40	; '@'
    cp	    c
    jr	    nz,	.loopCmpTogleBit

.startBytePrg:
    ld      a, $10
    ld      [de], a		            ; 28SF040 $10	command: Byte program
    ldi	    a, [hl]
    ld      [de], a		            ; write byte content to target PA(Program Address)
    inc	    de
    xor	    a
    or	    e
    jr	    nz,	.chkToggleBit

.chkEndOfBank:
    ld      a, d
    cp	    $80	; '�'
    jr	    c, .nxtSector2Write

.switch2NextBank:
    ld      d, $40 ; '@'
    ld      a, [byte_C022]	        ; 写入这个地址值的低5bit是上次被74HC373锁存的值的低5bit，是作为28SF040的A18..A14的值。
                                    ; 保存在这里，以便之后可以渠道上次对28SF040进行的Bank切换。
                                    ; (28SF040因此被分成2^5 = 32个Bank)
    inc	    a
    ld      [byte_C022], a	        ; [0xC022] = [0xC022]	+ 1;
    or	    $80	; '�'               ; 混合0x80和待74373锁存的值。
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
                                    ;	  bit4..bit0 of	input value to the 74HC373. Which would	be latched and out put to A18..A14 of 28SF040.

.nxtSector2Write:
    call    waitToggleBit	        ; 等待28SF040的ToggleBit不再变化。
                                    ; （即写入/扇区擦除/全片擦除操作完成）
    dec	    b
    jr	    nz,	.startSectorErs

.allSectrWritten:				    ; 切换到MemoryBackupCart模式，并加锁28SF040
    call    SF040_Protect	        ; 注意: 有类似函数$728(负责解锁28SF040)
    push    af
    ld      a, [byte_C039]	        ; 用来存放0xFFFF(GB的中断启用状态)的值  // FFFF - IE - Interrupt	Enable (R/W)
    ld      [byte_FFFF], a	        ; FFFF - IE -	Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt	Enable	(INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt	Enable	(INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt	Enable	(INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt	Enable	(INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt	Enable	(INT 60h)  (1=Enable)
    pop	    af
    ret
; End of function SF040_Cp2Sector
