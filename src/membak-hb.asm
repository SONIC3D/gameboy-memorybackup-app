INCLUDE "include/hardware.inc"
INCLUDE "include/membak.inc"

; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
SECTION "ServiceProgramCode", ROM0[$0]
UnUsed_0x0000:
    ret


; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
INCLUDE "module_joypad.inc"


; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
SECTION "Jump Vector of Interrupts", ROM0[$40]

VBlank:
    jp      VIntFunc
    ds      $48 - @, 0              ; Alignment
; ---------------------------------------------------------------------------
LCD_STAT:
    reti
    ds      $50 - @, 0              ; Alignment
; ---------------------------------------------------------------------------
Timer:
    reti
    ds      $58 - @, 0              ; Alignment
; ---------------------------------------------------------------------------
Serial:
    reti
    ds      $60 - @, 0              ; Alignment
; ---------------------------------------------------------------------------
Joypad:
    reti


; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
INCLUDE "module_ram_vram_op.inc"


; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
SECTION "UserProgramCode", ROM0[$100]
; =============== S U B R O U T I N E =======================================
GlobalEntry_100:
    nop
    jp      mainEntry
; End of function GlobalEntry_100

; ---------------------------------------------------------------------------
    ds $134 - @, 0                  ; Make room for logo and header info
; ---------------------------------------------------------------------------
SECTION "Cartridge Header", ROM0[$0134]
; https://problemkaputt.de/pandocs.htm#thecartridgeheader
    db "DEMO            "           ; $0134-0143 Game Title
    db 0,0                          ; $0144-0145 New Licensee Code
    db 0                            ; $0146 SGB Flag
    db $1B                          ; $0147 Cartridge Type
    db 0                            ; $0148 ROM Size
    db 3                            ; $0149 RAM Size
    db 1                            ; $014A Destination Code
    db 1                            ; $014B Old Licensee Code
    db 0                            ; $014C Mask ROM Version Number
    db $22                          ; $014D Header Checksum
    db $F6,$2D                      ; $014E-014F Global Checksum
;    ds 3                            ; Enable this line if checksum $014D-014F is empty

; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
INCLUDE "module_oam_op.inc"


; ---------------------------------------------------------------------------
; ---------------------------------------------------------------------------
INCLUDE "module_gamecart_sram.inc"


; =============== S U B R O U T I N E =======================================
mainEntry:
    di
    xor     a                       ; 清理中断请求、禁用所有中断、初始化栈顶地址
    ld      [FF0F_IntrptFlag], a    ; FF0F - IF -    Interrupt Flag (R/W)
                                    ;   Bit 0: V-Blank  Interrupt    Request    (INT 40h)  (1=Request)
                                    ;   Bit 1: LCD STAT Interrupt    Request    (INT 48h)  (1=Request)
                                    ;   Bit 2: Timer    Interrupt    Request    (INT 50h)  (1=Request)
                                    ;   Bit 3: Serial   Interrupt    Request    (INT 58h)  (1=Request)
                                    ;   Bit 4: Joypad   Interrupt    Request    (INT 60h)  (1=Request)
    ld      [byte_FFFF], a          ; FFFF - IE -    Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt    Enable    (INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt    Enable    (INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt    Enable    (INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt    Enable    (INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt    Enable    (INT 60h)  (1=Enable)
    ld      sp, $FFFE
.waitLY_eq_0x90:
    ld      a, [byte_FF44]          ; FF44 - LY -    LCDC Y-Coordinate (R)
                                    ;   The LY indicates the vertical line to which the present data is transferred to the LCD Driver.
                                    ;   The LY can take on any value between 0 through 153.
                                    ;   The values between 144 and 153 indicate the V-Blank period.
                                    ;   Writing will reset the counter.
    cp      $90    ; '�'
    jr      c, .waitLY_eq_0x90
.initDisplayMode:                   ; 0x11 = 0001 0001
    ld      a, $11                  ; Set    LCD
                                    ;   Display Off
                                    ;   Window TileMap Display [9800-9BFF]
                                    ;   Window Display         Off
                                    ;   BG & Window TileData:  [8000-8FFF]
                                    ;   BG TileMap Display:    [9800-9BFF]
                                    ;   Obj size             8x8
                                    ;   Obj display         Off
                                    ;   BG display         On
    ld      [byte_FF40], a          ; FF40 - LCDC    - LCD Control (R/W)
                                    ;   Bit 7 - LCD Display Enable         (0=Off, 1=On)
                                    ;   Bit 6 - Window Tile Map Display Select (0=9800-9BFF, 1=9C00-9FFF)
                                    ;   Bit 5 - Window Display Enable         (0=Off, 1=On)
                                    ;   Bit 4 - BG & Window Tile Data Select   (0=8800-97FF, 1=8000-8FFF)
                                    ;   Bit 3 - BG Tile Map Display Select     (0=9800-9BFF, 1=9C00-9FFF)
                                    ;   Bit 2 - OBJ (Sprite) Size             (0=8x8, 1=8x16)
                                    ;   Bit 1 - OBJ (Sprite) Display Enable    (0=Off, 1=On)
                                    ;   Bit 0 - BG Display (for CGB see below) (0=Off, 1=On)
.initGameCartRAM:
    ld      a, $C0 ; '�'
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
    ld      a, $A
    ld      [IOPort_0x0000], a
    ld      a, 1
    ld      [IOPort_0x6000], a
.chgToMemBakMode:
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
.initOAM:                           ; Fill [0xFE00-0xFE9F] OAM address range with    data 0x00.
    call    clearOAM
.initCopyTiles:
    ld      hl, $16C4
    ld      de, $8000
    ld      bc, $1000
    call    memcpyHL2DE             ; 赋值源地址数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to Copy: bc
                                    ;   SrcAddr: [hl]
                                    ;   DestAddr:[de]
.initBgMap:
    ld      hl, $9800
    ld      bc, $400
    ld      a, $20 ; ' '            ; 0x20 is the space graphic in tiles data.(0x18C4-0x18D3)
    call    memFillWith_A           ; 赋值源a中数据到目标地址中，总赋值字节数为bc中存放的16bit值，b是高8bit
                                    ; Parameters:
                                    ;   LengthOfByte to fill: bc
                                    ;   Value: a
                                    ;   DestAddr:[hl]
.initBgPalette:
    ld      a, $E4 ; '�'
    ld      [byte_FF47], a          ; FF47 - BGP - BG Palette Data (R/W) - Non CGB Mode Only
                                    ; This register assigns gray shades to the color numbers of the BG and Window    tiles.
                                    ;   Bit 7-6 -    Shade for Color    Number 3
                                    ;   Bit 5-4 -    Shade for Color    Number 2
                                    ;   Bit 3-2 -    Shade for Color    Number 1
                                    ;   Bit 1-0 -    Shade for Color    Number 0
                                    ; The    four possible gray shades are:
                                    ;   0     White
                                    ;   1     Light gray
                                    ;   2     Dark gray
                                    ;   3     Black
                                    ; In CGB Mode    the Color Palettes are taken from CGB Palette Memory instead.
.initObjPalette0:
    ld      a, $AA ; '�'
    ld      [byte_FF48], a          ; FF48 - OBP0    - Object Palette 0 Data    (R/W) -    Non CGB    Mode Only
                                    ;   This register assigns gray shades for sprite palette 0.
                                    ;   It works exactly as BGP (FF47), except that the lower two bits aren't used because sprite data 00 is transparent.
    ld      a, $93 ; '�'            ; 0x93 = 1001 0011
                                    ; Compare to 0x1E8, it turn on LCD and turn on Obj display.
    ld      [byte_FF40], a          ; FF40 - LCDC    - LCD Control (R/W)
                                    ;   Bit 7 - LCD Display Enable         (0=Off, 1=On)
                                    ;   Bit 6 - Window Tile Map Display Select (0=9800-9BFF, 1=9C00-9FFF)
                                    ;   Bit 5 - Window Display Enable         (0=Off, 1=On)
                                    ;   Bit 4 - BG & Window Tile Data Select   (0=8800-97FF, 1=8000-8FFF)
                                    ;   Bit 3 - BG Tile Map Display Select     (0=9800-9BFF, 1=9C00-9FFF)
                                    ;   Bit 2 - OBJ (Sprite) Size             (0=8x8, 1=8x16)
                                    ;   Bit 1 - OBJ (Sprite) Display Enable    (0=Off, 1=On)
                                    ;   Bit 0 - BG Display (for CGB see below) (0=Off, 1=On)
.initSoundReg:
    xor     a
    ld      [byte_FF14], a          ; FF14 - NR14    - Channel 1 Frequency hi (R/W)
    ld      [byte_FF19], a          ; FF19 - NR24    - Channel 2 Frequency hi data (R/W)
    ld      [byte_FF1E], a          ; FF1E - NR34    - Channel 3 Frequency's higher data (R/W)
    ld      [byte_FF23], a          ; FF23 - NR44    - Channel 4 Counter/consecutive; Inital    (R/W)
.initVIntJumpPtr:
    xor     a
    ld      [byte_C035], a          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
    ld      [byte_C036], a          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
.initInterrupt:                     ; Only enable    V-Blank    interrupt.
    ld      a, 1
    ld      [byte_FFFF], a          ; FFFF - IE -    Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt    Enable    (INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt    Enable    (INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt    Enable    (INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt    Enable    (INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt    Enable    (INT 60h)  (1=Enable)
.startMainLogic:
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
    call    enterSvcMode            ; 按住Start+Select+B+A，则进入Service Mode
                                    ; 打印Flash    ROM芯片信息和游戏卡带的SRAM Bank信息
    call    rdSF040_0x7F000         ; Copy 28SF040:[0x7F000-0x7F7FF] to WRAM-1:[0xD800-0xDFFF]
                                    ; 从28SF040加载Sector/文件分配表到缓存[0xD800-0xDFFF]
    call    rdSF040_0x7F800         ; Copy 28SF040:[0x7F800-0x7FEFF] to WRAM-0:[0xC063-0xC762]
    call    rdSF040_0x7FF00         ; Copy 28SF040:[0x7FF00-0x7FFFF] to WRAM-0:[0xC763-0xC862]
    call    verifyChksum            ; 检查0xC862中的checksum是否和0xAE + [0xC763]..[0xC861]的值相等
                                    ; 返回值
                                    ;   Z    Flag:
                                    ;    set(z):     Checksum相等
                                    ;    clr(nz): Checksum不相等
    call    nz, creditsFmtFlash     ; Display credits info and initial formating 28SF040.
    call    detectBankCnt           ; 似乎是为了检测有多少个SRAM Bank可用
                                    ;
                                    ; 整个逻辑是:
                                    ;   切到GameCart模式
                                    ;   测试第0个SRAM    Bank是否可读写
                                    ;    不可读写就直接返回，Z    Flag清空，为nz
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
    call    displayMainMenu
    ei
.waitVBlank:
    xor     a
    ld      [byte_C037], a          ; 每次VBlank函数发生时，该变量会被设成1
    halt
    nop
    ld      a, [byte_C037]          ; 每次VBlank函数发生时，该变量会被设成1
    or      a
    jr      z, .waitVBlank          ; 0xC037设为0，然后Halt后再次读取0xC037
                                    ;   如果发现没有变1，则继续循环等待
                                    ;   如果变1了，说明VBlank执行了，代码就执行下去

.vblankHappened:
    ld      a, [byte_C035]          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
    ld      l, a
    ld      a, [byte_C036]          ; [0xC036,0xC035]会在VInt发生后，在0x0264处被赋值给hl，然后只要hl不是0，就跳转到[hl]，最后再返回到$258等待下一个VInt发生。
    ld      h, a
    or      l
    jr      z, .waitVBlank          ; hl = [C036][C035]，如果取出的hl为0000,则循环跳回
    ld      de, .waitVBlank         ; TODO: 这里的$258可能应该改为waitVBlank这个Label地址
    push    de
    jp      hl                      ;jp      [hl]
; End of function mainEntry

INCLUDE "vblank_routines.inc"
INCLUDE "module_servicemode.inc"
INCLUDE "module_saveload_ram_fat.inc"

; File system and sound fx related
INCLUDE "module_filesys.inc"
INCLUDE "module_sfx.inc"
INCLUDE "tilemap_capacity_bar.inc"

INCLUDE "module_str_printf.inc"
INCLUDE "module_flashrom_28sf040.inc"
INCLUDE "module_flashrom_rw.inc"

INCLUDE "menusys/mainmenu.inc"


SECTION "Padding ROMX", ROMX[$4000]
    ds $4000, $FF
