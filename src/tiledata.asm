SECTION "Tile data", ROM0[$16C4]

fontTiles_16C4::
    db  $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF; $0      ; Copied to Tiles RAM: 0x8000, 0x8010, 0x8020...0x8920
    db  $FF, $FF, $FF,   0, $FF,   0, $FF,   0, $FF,   0, $FF,   0, $FF,   0, $FF, $FF; $10
    db  $FF, $FF, $7F, $80, $7F, $80, $7F, $80, $7F, $80, $7F, $80, $7F, $80, $FF, $FF; $20
    db  $FF, $FF, $3F, $C0, $3F, $C0, $3F, $C0, $3F, $C0, $3F, $C0, $3F, $C0, $FF, $FF; $30
    db  $FF, $FF, $1F, $E0, $1F, $E0, $1F, $E0, $1F, $E0, $1F, $E0, $1F, $E0, $FF, $FF; $40
    db  $FF, $FF,  $F, $F0,  $F, $F0,  $F, $F0,  $F, $F0,  $F, $F0,  $F, $F0, $FF, $FF; $50
    db  $FF, $FF,   7, $F8,   7, $F8,   7, $F8,   7, $F8,   7, $F8,   7, $F8, $FF, $FF; $60
    db  $FF, $FF,   3, $FC,   3, $FC,   3, $FC,   3, $FC,   3, $FC,   3, $FC, $FF, $FF; $70
    db  $FF, $FF,   1, $FE,   1, $FE,   1, $FE,   1, $FE,   1, $FE,   1, $FE, $FF, $FF; $80
    db  $FF, $FF,   0, $FF,   0, $FF,   0, $FF,   0, $FF,   0, $FF,   0, $FF, $FF, $FF; $90
    db  $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF, $FF; $A0
    db    1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1,   1; $B0
    db  $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80, $80; $C0
    db  $FF, $FF, $81, $81, $81, $81, $81, $81, $81, $81, $81, $81, $81, $81, $FF, $FF; $D0
    db  $FF, $FF, $81, $C3, $81, $A5, $81, $99, $81, $99, $81, $A5, $81, $C3, $FF, $FF; $E0
    db  $99, $99, $5A, $5A, $3C, $3C, $E7, $E7, $E7, $E7, $3C, $3C, $5A, $5A, $99, $99; $F0
    db  $80, $80, $E0, $E0, $F8, $F8, $FE, $FE, $F8, $F8, $E0, $E0, $80, $80,   0,   0; $100
    db    2,   2,  $E,  $E, $3E, $3E, $FE, $FE, $3E, $3E,  $E,  $E,   2,   2,   0,   0; $110
    db  $18, $18, $3C, $3C, $7E, $7E, $18, $18, $18, $18, $7E, $7E, $3C, $3C, $18, $18; $120
    db  $66, $66, $66, $66, $66, $66, $66, $66, $66, $66,   0,   0, $66, $66,   0,   0; $130
    db  $7F, $7F, $DB, $DB, $DB, $DB, $7B, $7B, $1B, $1B, $1B, $1B, $1B, $1B,   0,   0; $140
    db  $3E, $3E, $63, $63, $38, $38, $6C, $6C, $6C, $6C, $38, $38, $CC, $CC, $78, $78; $150
    db    0,   0,   0,   0,   0,   0,   0,   0, $7E, $7E, $7E, $7E, $7E, $7E,   0,   0; $160
    db  $18, $18, $3C, $3C, $7E, $7E, $18, $18, $7E, $7E, $3C, $3C, $18, $18, $FF, $FF; $170
    db  $18, $18, $3C, $3C, $7E, $7E, $18, $18, $18, $18, $18, $18, $18, $18,   0,   0; $180
    db  $18, $18, $18, $18, $18, $18, $18, $18, $7E, $7E, $3C, $3C, $18, $18,   0,   0; $190
    db    0,   0, $18, $18,  $C,  $C, $FE, $FE,  $C,  $C, $18, $18,   0,   0,   0,   0; $1A0
    db    0,   0, $30, $30, $60, $60, $FE, $FE, $60, $60, $30, $30,   0,   0,   0,   0; $1B0
    db    0,   0, $F4, $F4, $86, $86, $F5, $F5, $84, $84, $F4, $F4,   0,   0,   0,   0; $1C0
    db    0,   0, $5C, $5C, $52, $52, $52, $52, $D2, $D2, $5C, $5C,   0,   0,   0,   0; $1D0
    db    0,   0, $18, $18, $3C, $3C, $7E, $7E, $FF, $FF, $FF, $FF,   0,   0,   0,   0; $1E0
    db    0,   0, $FF, $FF, $FF, $FF, $7E, $7E, $3C, $3C, $18, $18,   0,   0,   0,   0; $1F0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $200
    db  $30, $30, $30, $30, $30, $30, $30, $30, $30, $30,   0,   0, $30, $30,   0,   0; $210
    db  $6C, $6C, $6C, $6C, $6C, $6C,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $220
    db  $6C, $6C, $6C, $6C, $FE, $FE, $6C, $6C, $FE, $FE, $6C, $6C, $6C, $6C,   0,   0; $230
    db  $30, $30, $7C, $7C, $C0, $C0, $78, $78,  $C,  $C, $F8, $F8, $30, $30,   0,   0; $240
    db    0,   0, $C6, $C6, $CC, $CC, $18, $18, $30, $30, $66, $66, $C6, $C6,   0,   0; $250
    db  $38, $38, $6C, $6C, $38, $38, $76, $76, $DC, $DC, $CC, $CC, $76, $76,   0,   0; $260
    db  $60, $60, $60, $60, $C0, $C0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $270
    db  $18, $18, $30, $30, $60, $60, $60, $60, $60, $60, $30, $30, $18, $18,   0,   0; $280
    db  $60, $60, $30, $30, $18, $18, $18, $18, $18, $18, $30, $30, $60, $60,   0,   0; $290
    db    0,   0, $66, $66, $3C, $3C, $FF, $FF, $3C, $3C, $66, $66,   0,   0,   0,   0; $2A0
    db    0,   0, $30, $30, $30, $30, $FC, $FC, $30, $30, $30, $30,   0,   0,   0,   0; $2B0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $30, $30, $30, $30, $60, $60; $2C0
    db    0,   0,   0,   0,   0,   0, $FC, $FC,   0,   0,   0,   0,   0,   0,   0,   0; $2D0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $30, $30, $30, $30,   0,   0; $2E0
    db    6,   6,  $C,  $C, $18, $18, $30, $30, $60, $60, $C0, $C0, $80, $80,   0,   0; $2F0
    db  $7C, $7C, $C6, $C6, $CE, $CE, $DE, $DE, $F6, $F6, $E6, $E6, $7C, $7C,   0,   0; $300
    db  $30, $30, $70, $70, $30, $30, $30, $30, $30, $30, $30, $30, $FC, $FC,   0,   0; $310
    db  $78, $78, $CC, $CC,  $C,  $C, $38, $38, $60, $60, $CC, $CC, $FC, $FC,   0,   0; $320
    db  $78, $78, $CC, $CC,  $C,  $C, $38, $38,  $C,  $C, $CC, $CC, $78, $78,   0,   0; $330
    db  $1C, $1C, $3C, $3C, $6C, $6C, $CC, $CC, $FE, $FE,  $C,  $C, $1E, $1E,   0,   0; $340
    db  $FC, $FC, $C0, $C0, $F8, $F8,  $C,  $C,  $C,  $C, $CC, $CC, $78, $78,   0,   0; $350
    db  $38, $38, $60, $60, $C0, $C0, $F8, $F8, $CC, $CC, $CC, $CC, $78, $78,   0,   0; $360
    db  $FC, $FC, $CC, $CC,  $C,  $C, $18, $18, $30, $30, $30, $30, $30, $30,   0,   0; $370
    db  $78, $78, $CC, $CC, $CC, $CC, $78, $78, $CC, $CC, $CC, $CC, $78, $78,   0,   0; $380
    db  $78, $78, $CC, $CC, $CC, $CC, $7C, $7C,  $C,  $C, $18, $18, $70, $70,   0,   0; $390
    db    0,   0, $30, $30, $30, $30,   0,   0,   0,   0, $30, $30, $30, $30,   0,   0; $3A0
    db    0,   0, $30, $30, $30, $30,   0,   0,   0,   0, $30, $30, $30, $30, $60, $60; $3B0
    db  $18, $18, $30, $30, $60, $60, $C0, $C0, $60, $60, $30, $30, $18, $18,   0,   0; $3C0
    db    0,   0,   0,   0, $FC, $FC,   0,   0,   0,   0, $FC, $FC,   0,   0,   0,   0; $3D0
    db  $60, $60, $30, $30, $18, $18,  $C,  $C, $18, $18, $30, $30, $60, $60,   0,   0; $3E0
    db  $78, $78, $CC, $CC,  $C,  $C, $18, $18, $30, $30,   0,   0, $30, $30,   0,   0; $3F0
    db  $7C, $7C, $C6, $C6, $DE, $DE, $DE, $DE, $DE, $DE, $C0, $C0, $78, $78,   0,   0; $400
    db  $30, $30, $78, $78, $CC, $CC, $CC, $CC, $FC, $FC, $CC, $CC, $CC, $CC,   0,   0; $410
    db  $FC, $FC, $66, $66, $66, $66, $7C, $7C, $66, $66, $66, $66, $FC, $FC,   0,   0; $420
    db  $3C, $3C, $66, $66, $C0, $C0, $C0, $C0, $C0, $C0, $66, $66, $3C, $3C,   0,   0; $430
    db  $F8, $F8, $6C, $6C, $66, $66, $66, $66, $66, $66, $6C, $6C, $F8, $F8,   0,   0; $440
    db  $7E, $7E, $60, $60, $60, $60, $78, $78, $60, $60, $60, $60, $7E, $7E,   0,   0; $450
    db  $7E, $7E, $60, $60, $60, $60, $78, $78, $60, $60, $60, $60, $60, $60,   0,   0; $460
    db  $3C, $3C, $66, $66, $C0, $C0, $C0, $C0, $CE, $CE, $66, $66, $3E, $3E,   0,   0; $470
    db  $CC, $CC, $CC, $CC, $CC, $CC, $FC, $FC, $CC, $CC, $CC, $CC, $CC, $CC,   0,   0; $480
    db  $78, $78, $30, $30, $30, $30, $30, $30, $30, $30, $30, $30, $78, $78,   0,   0; $490
    db  $1E, $1E,  $C,  $C,  $C,  $C,  $C,  $C, $CC, $CC, $CC, $CC, $78, $78,   0,   0; $4A0
    db  $E6, $E6, $66, $66, $6C, $6C, $78, $78, $6C, $6C, $66, $66, $E6, $E6,   0,   0; $4B0
    db  $60, $60, $60, $60, $60, $60, $60, $60, $60, $60, $60, $60, $7E, $7E,   0,   0; $4C0
    db  $C6, $C6, $EE, $EE, $FE, $FE, $FE, $FE, $D6, $D6, $C6, $C6, $C6, $C6,   0,   0; $4D0
    db  $C6, $C6, $E6, $E6, $F6, $F6, $DE, $DE, $CE, $CE, $C6, $C6, $C6, $C6,   0,   0; $4E0
    db  $38, $38, $6C, $6C, $C6, $C6, $C6, $C6, $C6, $C6, $6C, $6C, $38, $38,   0,   0; $4F0
    db  $FC, $FC, $66, $66, $66, $66, $7C, $7C, $60, $60, $60, $60, $F0, $F0,   0,   0; $500
    db  $78, $78, $CC, $CC, $CC, $CC, $CC, $CC, $DC, $DC, $78, $78, $1C, $1C,   0,   0; $510
    db  $FC, $FC, $66, $66, $66, $66, $7C, $7C, $6C, $6C, $66, $66, $E6, $E6,   0,   0; $520
    db  $78, $78, $CC, $CC, $E0, $E0, $78, $78, $1C, $1C, $CC, $CC, $78, $78,   0,   0; $530
    db  $FC, $FC, $30, $30, $30, $30, $30, $30, $30, $30, $30, $30, $30, $30,   0,   0; $540
    db  $CC, $CC, $CC, $CC, $CC, $CC, $CC, $CC, $CC, $CC, $CC, $CC, $FC, $FC,   0,   0; $550
    db  $CC, $CC, $CC, $CC, $CC, $CC, $CC, $CC, $CC, $CC, $78, $78, $30, $30,   0,   0; $560
    db  $C6, $C6, $C6, $C6, $C6, $C6, $D6, $D6, $FE, $FE, $EE, $EE, $C6, $C6,   0,   0; $570
    db  $C6, $C6, $C6, $C6, $6C, $6C, $38, $38, $38, $38, $6C, $6C, $C6, $C6,   0,   0; $580
    db  $CC, $CC, $CC, $CC, $CC, $CC, $78, $78, $30, $30, $30, $30, $78, $78,   0,   0; $590
    db  $FE, $FE,   6,   6,  $C,  $C, $18, $18, $30, $30, $60, $60, $FE, $FE,   0,   0; $5A0
    db  $78, $78, $60, $60, $60, $60, $60, $60, $60, $60, $60, $60, $78, $78,   0,   0; $5B0
    db  $C0, $C0, $60, $60, $30, $30, $18, $18,  $C,  $C,   6,   6,   2,   2,   0,   0; $5C0
    db  $78, $78, $18, $18, $18, $18, $18, $18, $18, $18, $18, $18, $78, $78,   0,   0; $5D0
    db  $10, $10, $38, $38, $6C, $6C, $C6, $C6,   0,   0,   0,   0,   0,   0,   0,   0; $5E0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $FF, $FF; $5F0
    db  $30, $30, $30, $30, $18, $18,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $600
    db    0,   0,   0,   0, $78, $78,  $C,  $C, $7C, $7C, $CC, $CC, $76, $76,   0,   0; $610
    db  $E0, $E0, $60, $60, $60, $60, $7C, $7C, $66, $66, $66, $66, $DC, $DC,   0,   0; $620
    db    0,   0,   0,   0, $78, $78, $CC, $CC, $C0, $C0, $CC, $CC, $78, $78,   0,   0; $630
    db  $1C, $1C,  $C,  $C,  $C,  $C, $7C, $7C, $CC, $CC, $CC, $CC, $76, $76,   0,   0; $640
    db    0,   0,   0,   0, $78, $78, $CC, $CC, $FC, $FC, $C0, $C0, $78, $78,   0,   0; $650
    db  $38, $38, $6C, $6C, $60, $60, $F0, $F0, $60, $60, $60, $60, $F0, $F0,   0,   0; $660
    db    0,   0,   0,   0, $76, $76, $CC, $CC, $CC, $CC, $7C, $7C,  $C,  $C, $F8, $F8; $670
    db  $E0, $E0, $60, $60, $6C, $6C, $76, $76, $66, $66, $66, $66, $E6, $E6,   0,   0; $680
    db  $30, $30,   0,   0, $70, $70, $30, $30, $30, $30, $30, $30, $78, $78,   0,   0; $690
    db   $C,  $C,   0,   0,  $C,  $C,  $C,  $C,  $C,  $C, $CC, $CC, $CC, $CC, $78, $78; $6A0
    db  $E0, $E0, $60, $60, $66, $66, $6C, $6C, $78, $78, $6C, $6C, $E6, $E6,   0,   0; $6B0
    db  $70, $70, $30, $30, $30, $30, $30, $30, $30, $30, $30, $30, $78, $78,   0,   0; $6C0
    db    0,   0,   0,   0, $CC, $CC, $FE, $FE, $FE, $FE, $D6, $D6, $C6, $C6,   0,   0; $6D0
    db    0,   0,   0,   0, $F8, $F8, $CC, $CC, $CC, $CC, $CC, $CC, $CC, $CC,   0,   0; $6E0
    db    0,   0,   0,   0, $78, $78, $CC, $CC, $CC, $CC, $CC, $CC, $78, $78,   0,   0; $6F0
    db    0,   0,   0,   0, $DC, $DC, $66, $66, $66, $66, $7C, $7C, $60, $60, $F0, $F0; $700
    db    0,   0,   0,   0, $76, $76, $CC, $CC, $CC, $CC, $7C, $7C,  $C,  $C, $1E, $1E; $710
    db    0,   0,   0,   0, $DC, $DC, $76, $76, $66, $66, $60, $60, $F0, $F0,   0,   0; $720
    db    0,   0,   0,   0, $7C, $7C, $C0, $C0, $78, $78,  $C,  $C, $F8, $F8,   0,   0; $730
    db  $10, $10, $30, $30, $7C, $7C, $30, $30, $30, $30, $34, $34, $18, $18,   0,   0; $740
    db    0,   0,   0,   0, $CC, $CC, $CC, $CC, $CC, $CC, $CC, $CC, $76, $76,   0,   0; $750
    db    0,   0,   0,   0, $CC, $CC, $CC, $CC, $CC, $CC, $78, $78, $30, $30,   0,   0; $760
    db    0,   0,   0,   0, $C6, $C6, $D6, $D6, $FE, $FE, $FE, $FE, $6C, $6C,   0,   0; $770
    db    0,   0,   0,   0, $C6, $C6, $6C, $6C, $38, $38, $6C, $6C, $C6, $C6,   0,   0; $780
    db    0,   0,   0,   0, $CC, $CC, $CC, $CC, $CC, $CC, $7C, $7C,  $C,  $C, $F8, $F8; $790
    db    0,   0,   0,   0, $FC, $FC, $98, $98, $30, $30, $64, $64, $FC, $FC,   0,   0; $7A0
    db  $1C, $1C, $30, $30, $30, $30, $E0, $E0, $30, $30, $30, $30, $1C, $1C,   0,   0; $7B0
    db  $18, $18, $18, $18, $18, $18,   0,   0, $18, $18, $18, $18, $18, $18,   0,   0; $7C0
    db  $E0, $E0, $30, $30, $30, $30, $1C, $1C, $30, $30, $30, $30, $E0, $E0,   0,   0; $7D0
    db  $76, $76, $DC, $DC,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $7E0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $7F0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $800
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $10, $38, $6C, $44,   0, $C6; $810
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $820
    db    1,   0,   0,   1,   3,   1,   1,   2,   6,   2,   2,   4,  $C,   4,   4,   8; $830
    db  $93, $82, $BA, $11, $39, $39, $39, $38, $38, $38, $38, $38, $10, $38, $38, $10; $840
    db    0,   0,   0,   0, $80,   0,   0, $80, $C0, $80, $80, $40, $60, $40, $40, $20; $850
    db  $18,   8, $18, $10, $20, $10, $30, $20, $20, $20, $30, $20, $3F, $1F,  $F,   0; $860
    db  $10, $10, $10,   0, $38, $10, $38, $38, $38, $10,   0,   0, $FF, $FF, $FF,   0; $870
    db  $30, $20, $30, $10,   8, $10, $18,   8,  $C,   8, $1C,   8, $FC, $F0, $F8,   0; $880
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  $F,  $E, $3B, $3F, $6B, $4F; $890
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $F0, $70, $DC, $FC, $D6, $F2; $8A0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $7F, $7F, $7F, $7F, $6F, $7F; $8B0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $F8, $F8, $DC, $FC, $EE, $DC; $8C0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  $F,  $E, $3B, $3F, $6B, $4F; $8D0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $F0, $70, $DC, $FC, $D6, $F2; $8E0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $7F, $7F, $7F, $7F, $6F, $7F; $8F0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $F8, $F8, $DC, $FC, $EE, $DC; $900
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $910
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $920
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   7,   7,   $F, $F, $1A, $1D; $930
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $80, $80, $C0, $C0; 0       ; Copied to Tiles RAM: 0x8940, 0x8950, ... 0x8FF0.
    db  $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F; $10
    db  $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2; $20
    db  $70, $7F, $7F, $7F, $60, $7F, $3F, $60, $3F, $60, $3F, $60, $3F, $60, $3F, $60; $30
    db  $1E, $FC, $FE, $FC,  $E, $FC, $FA,  $C, $FA,  $C, $FA,  $C, $FA,  $C, $FA,  $C; $40
    db  $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F; $50
    db  $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2; $60
    db  $70, $7F, $7F, $7F, $60, $7F, $3F, $60, $3F, $60, $3F, $60, $3F, $60, $3F, $60; $70
    db  $1E, $FC, $FE, $FC,  $E, $FC, $FA,  $C, $FA,  $C, $FA,  $C, $FA,  $C, $FA,  $C; $80
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $90
    db    0,   0,   0,   0,   0,   0,   1,   1,   3,   3,   6,   7,   3,   3,   4,   7; $A0
    db  $34, $3B, $69, $77, $D3, $EF, $A7, $DF, $4F, $BE, $1F, $7C, $BE, $F8, $FC, $F0; $B0
    db  $E0, $C0, $E0, $C0, $E0, $80, $C0,   0, $80,   0,   0,   0,   0,   0,   0,   0; $C0
    db  $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F; $D0
    db  $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2; $E0
    db  $30, $60, $3C, $7F, $3E, $7F, $3F,   0,   0,   0,   7,   7,   7,   7,   7,   7; $F0
    db  $1A,  $C, $7A, $FC, $FA, $FC, $FE,   0,   0,   0, $80, $80, $C0, $80, $C0, $80; $100
    db  $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F; $110
    db  $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2; $120
    db  $30, $60, $3C, $7F, $3E, $7F, $3F,   0,   0,   0,   1,   1,   3,   3,   7,   7; $130
    db  $1A,  $C, $7A, $FC, $FA, $FC, $FE,   0,   0,   0, $80, $80, $C0, $C0, $E0, $E0; $140
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   2,   3,   6,   4; $150
    db   $A,  $D, $15, $1B, $2B, $36, $57, $6C, $AE, $D8, $5C, $B0, $B8, $60, $70, $C0; $160
    db  $F8, $E0, $F0, $40, $A0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $170
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $180
    db  $3B, $3F, $1F,  $F,   7,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $190
    db  $DF, $FC, $FE, $F0, $F8,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $1A0
    db  $3F, $3F, $1F, $1F,  $F,  $F,   7,   7,   3,   3,   1,   0,   0,   0,   0,   0; $1B0
    db  $F0, $F0, $F8, $E0, $F0, $C0, $E0, $80, $C0,   0, $80,   0,   0,   0,   0,   0; $1C0
    db  $3B, $3F, $1F,  $F,   7,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $1D0
    db  $DF, $FC, $FE, $F0, $F8,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $1E0
    db   $F,  $F, $1F, $1F,  $F,   3,   3,   3,   3,   3,   1,   0,   0,   0,   0,   0; $1F0
    db  $F0, $F0, $F8, $F8, $FC, $C0, $E0, $C0, $E0, $C0, $E0,   0,   0,   0,   0,   0; $200
    db    5,   4,   3,   3,   1,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $210
    db  $E0, $80, $C0,   0, $80,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $220
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $230
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $240
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $250
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1,   3,   3,   6,   7; $260
    db    0,   0,   0,   0, $38, $38, $7C, $7C, $D6, $EE, $A7, $DE, $4F, $BE, $9F, $7C; $270
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $280
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  $F,  $E, $3B, $3F, $6B, $4F; $290
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $F0, $70, $DC, $FC, $D6, $F2; $2A0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   3,   3; $2B0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $C0, $C0; $2C0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $2D0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $2E0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   7,   7; $2F0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $80, $80; $300
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   1,   1; $310
    db   $D,  $E, $1A, $1D, $30, $3B, $1D, $1F, $27, $3F, $57, $6F, $AF, $DA, $5D, $B0; $320
    db  $3E, $F8, $7C, $F0, $F8, $E0, $F0, $C0, $E0, $80, $C0,   0, $9E, $1E, $33, $33; $330
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $340
    db  $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F; $350
    db  $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2; $360
    db    6,   6, $7F, $7F, $4C, $70, $7F, $7F, $3B, $3E, $2E, $3A, $2E, $3A, $2E, $3A; $370
    db  $60, $60, $FE, $FE, $66, $1E, $FE, $FE, $BE, $FC, $EE, $BC, $EE, $BC, $EE, $BC; $380
    db  $7F, $7F, $7F, $7F, $6F, $7F, $70, $7F, $7F, $7F, $60, $7F, $3F, $60, $3F, $60; $390
    db  $F8, $F8, $DC, $FC, $EE, $DC, $1E, $FC, $FE, $FC,  $E, $FC, $FA,  $C, $FA,  $C; $3A0
    db   $C,  $C, $FF, $FF, $98, $E0, $FF, $FF, $77, $7D, $5D, $75, $5D, $75, $5D, $75; $3B0
    db  $C0, $C0, $FC, $FC, $CE, $3C, $FE, $FC, $7E, $F8, $DC, $78, $DC, $78, $DC, $78; $3C0
    db    2,   3,   5,   6,  $A,  $D, $15, $1B, $33, $26, $2F, $24, $1E, $18,  $C,   0; $3D0
    db  $BB, $63, $72, $C3, $E3, $83, $C1,   1, $81,   1,   1,   1,   1,   1,   1,   1; $3E0
    db  $FF, $FF, $63, $80, $FF, $FF, $DD, $F7, $77, $D5, $77, $D5, $77, $D5, $77, $D5; $3F0
    db  $F0, $F0, $38, $F0, $F8, $F0, $F8, $E0, $70, $E0, $70, $E0, $70, $E0, $70, $E0; $400
    db  $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F; $410
    db  $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2, $DF, $FC, $D6, $F2; $420
    db  $2E, $3A, $2E, $3A, $2E, $3A, $2E, $3A, $3E, $3A, $1F, $1F,  $F,   0,   0,   0; $430
    db  $EE, $BC, $EE, $BC, $EE, $BC, $EE, $BC, $EE, $BC, $FE, $F8, $FC,   0,   0,   0; $440
    db  $3F, $60, $3F, $60, $3F, $60, $30, $60, $3C, $7F, $3E, $7F, $3F,   0,   0,   0; $450
    db  $FA,  $C, $FA,  $C, $FA,  $C, $1A,  $C, $7A, $FC, $FA, $FC, $FE,   0,   0,   0; $460
    db  $5D, $75, $5D, $75, $5D, $75, $5D, $75, $7D, $75, $3F, $3F, $1F,   0,   0,   0; $470
    db  $DC, $78, $DC, $78, $DC, $78, $DC, $78, $DC, $78, $FC, $F0, $F8,   0,   0,   0; $480
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $490
    db    1,   1,   1,   1,   1,   1,   1,   1,   0,   0,   0,   0,   0,   0,   0,   0; $4A0
    db  $77, $D5, $77, $D5, $77, $D5, $F7, $D5, $FF, $FF, $7F,   0,   0,   0,   0,   0; $4B0
    db  $70, $E0, $70, $E0, $70, $E0, $70, $E0, $F0, $C0, $E0,   0,   0,   0,   0,   0; $4C0
    db  $3B, $3F, $1F,  $F,   7,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $4D0
    db  $DF, $FC, $FE, $F0, $F8,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $4E0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $4F0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $500
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $510
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $520
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $530
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $540
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $550
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $560
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $570
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $580
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $590
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $5A0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $5B0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $5C0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $5D0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $5E0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $5F0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $600
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $610
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $620
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $630
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $640
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $650
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $660
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $670
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $680
    db  $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F, $3B, $3F, $6B, $4F; $690
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0, $F0, $70, $DC, $FC, $D6, $F2; $6A0
    db    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0; $6B0
fontTiles_16C4_End::
