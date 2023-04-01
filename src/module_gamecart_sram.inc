;SECTION "Module GameCart SRAM Operation", ROM0[$015C]

; =============== S U B R O U T I N E =======================================

; Write [0xA000]=([0xA000] xor 0xFF) and re-read to test if it is writable.
; Return
;   z:
;     set(z):  A000 is Read/Write supported.
;     clr(nz): A000 failed in Read/Write test.
tst_0xA000_RW:
    ld      a, [A000_CartSramSt]
    cpl
    ld      [A000_CartSramSt], a
    ld      c, a                    ; c = ([0xA000] ^ $FF)
                                    ; [0xA000] = c
    ld      a, [A000_CartSramSt]
    cp      c
    ret     nz                      ; return if (c != [0xA000])
    cpl
    ld      [A000_CartSramSt], a    ; [0xA000] = ([0xA000] ^ $FF)
    xor     a
    ret
; End of function tst_0xA000_RW

; =============== S U B R O U T I N E =======================================

; 似乎是为了检测有多少个SRAM    Bank可用
;
; 整个逻辑是:
;   切到GameCart模式
;   测试第0个SRAM Bank是否可读写
;     不可读写就直接返回，Z Flag清空，为nz
;   备份[0xA000]到[0xC053]
;   写入测试字符串(16个字符)到第0个Bank的[0xA000]
;   循环赋值[0x4000], 从0x01到0x0F(注意0x4000是MBC1/3/5)，所以不支持MBC6)
;     测试SRAM Bank是否可读写，不可读写直接走结束流程
;     循环读取[0xA000]开始到[0xA00F]，比对其中的所有字节是否和测试字符串相符合
;    相符则走到结束流程，不相符则继续测试下一个Bank
;   循环结束
;   结束流程:
;     从[0xC053]恢复备份[0xA000]
;     切到MemoryBackupCart模式
;     把最后一轮的b寄存器值保存到[0xC038]
;
detectBankCnt:
    ld      a, $C0
    ld      [IOPort_0x2000], a      ; 0x2000 I/O Port:
                                    ;   Write
                                    ;    bit7..bit6:
                                    ;      $C0:
                                    ;        Switch to Game Cart    Mode. All access to address like $4000 passed to Game Cart.
                                    ;      $80:
                                    ;        Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;    bit5:
                                    ;      Unknown. Always be 0.
                                    ;    bit4..bit0:
                                    ;      bit4..bit0 of    input value to the 74HC373. Which would    be latched and out put to A18..A14 of 28SF040.
    xor     a
    ld      [byte_4000], a          ; [0x2000] = $C0, [0x4000] = 0; Switch to GameCart SRAM Bank 0
    call    tst_0xA000_RW           ; Write [0xA000]=([0xA000] xor 0xFF) and re-read to test if it is writable.
                                    ; Return
                                    ;   z:
                                    ;    set(z):     A000 is Read/Write supported.
                                    ;    clr(nz): A000 failed in    Read/Write test.
    ld      a, 0
    ld      [byte_C038], a          ; [0xC038] = 0
    ret     nz                      ; Return if 0xA000 is    failed in R/W test.
    ld      hl, A000_CartSramSt
    ld      de, byteArr_C053
    ld      bc, $10                 ; 从卡带SRAM范围[0xA000]备份复制一个字节到[0xC053]
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]
    ld      hl, strSramWriteTst     ; "Mushroom/Kingpin"
    ld      de, A000_CartSramSt
    ld      bc, $10                 ; 从测试字符串复制一个字节到卡带SRAM范围[0xA000]
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]
    ld      b, 1
.nextSramBank:
    ld      a, b
    cp      $10
    jr      nc, .endOfDetect        ; Jump if (b >= $10)
    ld      a, b
    ld      [byte_4000], a          ; [0x2000] = $C0, [0x4000] = b;    //    b=1,2,3,4,5,6,7,8,9,A,B,C,D,E,F
    call    tst_0xA000_RW           ; Write [0xA000]=([0xA000] xor 0xFF) and re-read to test if it is writable.
                                    ; Return
                                    ;   z:
                                    ;    set(z):     A000 is Read/Write supported.
                                    ;    clr(nz): A000 failed in    Read/Write test.
    jr      nz, .endOfDetect         ; Jump if 0xA000 is failed in    R/W test.
    ld      hl, A000_CartSramSt
    ld      de, strSramWriteTst     ; "Mushroom/Kingpin"
.loopCmpTstStr:
    ldi     a, [hl]
    ld      c, a                    ; Read a byte    from [0xA000]
    ld      a, [de]
    inc     de
    cp      c
    jr      nz, .incrSramBank        ; Jump if loop read compare failed
    ld      a, l
    cp      $10
    jr      c, .loopCmpTstStr        ; Jump if (l < $10)
.endOfDetect:
    push    bc                      ; b中是当前正在检查可用性的卡带RAM的编号
                                    ; c无意义(有可能是最后一个检查完毕的字符的值，如果可写入性检测失败，也有可能是其他值)
                                    ; 在0x1D1 pop到af
    xor     a
    ld      [byte_4000], a          ; [0x2000] = $C0, [0x4000] = 0; Switch to GameCart SRAM Bank 0
    ld      hl, byteArr_C053
    ld      de, A000_CartSramSt
    ld      bc, $10                 ; 从[0xC053]恢复备份复制一个字节到卡带SRAM范围[0xA000]
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]
    ld      a, $80 ; '�'
    ld      [IOPort_0x2000], a      ; 0x2000 I/O Port:
                                    ;   Write
                                    ;    bit7..bit6:
                                    ;      $C0:
                                    ;        Switch to Game Cart    Mode. All access to address like $4000 passed to Game Cart.
                                    ;      $80:
                                    ;        Switch to MemoryBackup Cart. $4000 is mapped to $0000 of FlashROM 28SF040.
                                    ;    bit5:
                                    ;      Unknown. Always be 0.
                                    ;    bit4..bit0:
                                    ;      bit4..bit0 of    input value to the 74HC373. Which would    be latched and out put to A18..A14 of 28SF040.
    pop     af                      ; Data pushed    at 0x1BB
    ld      [byte_C038], a          ; 存放的是当前插的游戏卡带，最后可用的SRAM的Bank编号数量(最后一个可用Bank的序号+1)
    ret
    ; ---------------------------------------------------------------------------
.incrSramBank:
    inc        b
    jr        .nextSramBank
; End of function detectBankCnt
