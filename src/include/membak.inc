;*
;* Gameboy MemoryBackup hardware definitions
;*

; 28SF040 cmd address
DEF unk_440A        EQU $440A
DEF unk_4418        EQU $4418
DEF unk_4419        EQU $4419
DEF unk_441A        EQU $441A
DEF unk_441B        EQU $441B
DEF unk_5820        EQU $5820
DEF unk_5822        EQU $5822
DEF unk_5823        EQU $5823

; Gameboy I/O
DEF IOPort_0x0000   EQU $0000
DEF IOPort_0x2000   EQU $2000
DEF IOPort_0x6000   EQU $6000
DEF byte_4000       EQU $4000
DEF byte_4001       EQU $4001
DEF A000_CartSramSt EQU $A000


DEF byte_FE00       EQU $FE00
DEF byte_FE01       EQU $FE01
DEF byte_FE02       EQU $FE02

DEF FF00_P1_Joypad  EQU $FF00
DEF FF05_TimCounter EQU $FF05
DEF FF06_TimModulo  EQU $FF06
DEF FF07_TimCtrl    EQU $FF07
DEF FF0F_IntrptFlag EQU $FF0F       ; FF0F - IF - Interrupt Flag (R/W)
                                    ;   Bit 0: V-Blank  Interrupt Request (INT 40h)  (1=Request)
                                    ;   Bit 1: LCD STAT Interrupt Request (INT 48h)  (1=Request)
                                    ;   Bit 2: Timer    Interrupt Request (INT 50h)  (1=Request)
                                    ;   Bit 3: Serial   Interrupt Request (INT 58h)  (1=Request)
                                    ;   Bit 4: Joypad   Interrupt Request (INT 60h)  (1=Request)

DEF byte_FF10       EQU $FF10       ; FF10 - NR10 - Channel 1 Sweep register (R/W)
DEF byte_FF11       EQU $FF11       ; FF11 - NR11 - Channel 1 Sound length/Wave pattern duty (R/W)
DEF byte_FF12       EQU $FF12       ; FF12 - NR12 - Channel 1 Volume Envelope (R/W)
DEF byte_FF13       EQU $FF13       ; FF13 - NR13 - Channel 1 Frequency lo (Write Only)
DEF byte_FF14       EQU $FF14       ; FF14 - NR14 - Channel 1 Frequency hi (R/W)
DEF byte_FF19       EQU $FF19       ; FF19 - NR24 - Channel 2 Frequency hi data (R/W)
DEF byte_FF1E       EQU $FF1E       ; FF1E - NR34 - Channel 3 Frequency's higher data (R/W)
DEF byte_FF23       EQU $FF23       ; FF23 - NR44 - Channel 4 Counter/consecutive; Inital (R/W)
DEF byte_FF24       EQU $FF24       ; FF24 - NR50 - Channel control / ON-OFF / Volume (R/W)
DEF byte_FF25       EQU $FF25       ; FF25 - NR51 - Selection of Sound output terminal (R/W)
DEF byte_FF26       EQU $FF26       ; FF26 - NR52 - Sound on/off

DEF byte_FF40       EQU $FF40       ; FF40 - LCDC - LCD Control (R/W)
                                    ;   Bit 7 - LCD Display Enable              (0=Off, 1=On)
                                    ;   Bit 6 - Window Tile Map Display Select  (0=9800-9BFF, 1=9C00-9FFF)
                                    ;   Bit 5 - Window Display Enable           (0=Off, 1=On)
                                    ;   Bit 4 - BG & Window Tile Data Select    (0=8800-97FF, 1=8000-8FFF)
                                    ;   Bit 3 - BG Tile Map Display Select      (0=9800-9BFF, 1=9C00-9FFF)
                                    ;   Bit 2 - OBJ (Sprite) Size               (0=8x8, 1=8x16)
                                    ;   Bit 1 - OBJ (Sprite) Display Enable     (0=Off, 1=On)
                                    ;   Bit 0 - BG Display (for CGB see below)  (0=Off, 1=On)
DEF byte_FF41       EQU $FF41       ; FF41 - STAT - LCDC Status (R/W)
                                    ;   Bit 6 - LYC=LY Coincidence Interrupt    (1=Enable) (Read/Write)
                                    ;   Bit 5 - Mode 2 OAM Interrupt            (1=Enable) (Read/Write)
                                    ;   Bit 4 - Mode 1 V-Blank Interrupt        (1=Enable) (Read/Write)
                                    ;   Bit 3 - Mode 0 H-Blank Interrupt        (1=Enable) (Read/Write)
                                    ;   Bit 2 - Coincidence Flag                (0:LYC<>LY, 1:LYC=LY) (Read Only)
                                    ;   Bit 1-0 - Mode Flag (Mode 0-3, see below) (Read Only)
                                    ;        0: During H-Blank
                                    ;        1: During V-Blank
                                    ;        2: During Searching OAM-RAM
                                    ;        3: During Transfering Data to LCD Driver
DEF byte_FF44       EQU $FF44       ; FF44 - LY - LCDC Y-Coordinate (R)
                                    ;   The LY indicates the vertical line to which the present data is transferred to the LCD Driver.
                                    ;   The LY can take on any value between 0 through 153.
                                    ;   The values between 144 and 153 indicate the V-Blank period.
                                    ;   Writing will reset the counter.
DEF byte_FF47       EQU $FF47       ; FF47 - BGP - BG Palette Data (R/W) - Non CGB Mode Only
                                    ; This register assigns gray shades to the color numbers of the BG and Window tiles.
                                    ;   Bit 7-6 - Shade for Color Number 3
                                    ;   Bit 5-4 - Shade for Color Number 2
                                    ;   Bit 3-2 - Shade for Color Number 1
                                    ;   Bit 1-0 - Shade for Color Number 0
                                    ; The four possible gray shades are:
                                    ;   0 White
                                    ;   1 Light gray
                                    ;   2 Dark gray
                                    ;   3 Black
                                    ; In CGB Mode the Color Palettes are taken from CGB Palette Memory instead.
DEF byte_FF48       EQU $FF48       ; FF48 - OBP0 - Object Palette 0 Data (R/W) - Non CGB Mode Only
                                    ;   This register assigns gray shades for sprite palette 0.
                                    ;   It works exactly as BGP (FF47), except that the lower two bits aren't used because sprite data 00 is transparent.
DEF byte_FFFF       EQU $FFFF       ; FFFF - IE - Interrupt Enable (R/W)
                                    ;   Bit 0: V-Blank  Interrupt Enable (INT 40h)  (1=Enable)
                                    ;   Bit 1: LCD STAT Interrupt Enable (INT 48h)  (1=Enable)
                                    ;   Bit 2: Timer    Interrupt Enable (INT 50h)  (1=Enable)
                                    ;   Bit 3: Serial   Interrupt Enable (INT 58h)  (1=Enable)
                                    ;   Bit 4: Joypad   Interrupt Enable (INT 60h)  (1=Enable)
