; ---------------------------------------------------------------------------
arrCapacityPtr:
    db  LOW(tilemapCpct_0), HIGH(tilemapCpct_0)     ; db $87,$5 => 0x587
    db  LOW(tilemapCpct_1), HIGH(tilemapCpct_1)     ; db $93,$5 => 0x593
    db  LOW(tilemapCpct_2), HIGH(tilemapCpct_2)     ; db $9F,$5 => 0x59F
    db  LOW(tilemapCpct_3), HIGH(tilemapCpct_3)     ; db $AB,$5 => 0x5AB
    db  LOW(tilemapCpct_4), HIGH(tilemapCpct_4)     ; db $B7,$5 => 0x5B7
    db  LOW(tilemapCpct_5), HIGH(tilemapCpct_5)     ; db $C3,$5 => 0x5C3
    db  LOW(tilemapCpct_6), HIGH(tilemapCpct_6)     ; db $CF,$5 => 0x5CF
    db  LOW(tilemapCpct_7), HIGH(tilemapCpct_7)     ; db $DB,$5 => 0x5DB
    db  LOW(tilemapCpct_8), HIGH(tilemapCpct_8)     ; db $E7,$5 => 0x5E7

tilemapCpct_0:        db $B,"%c",1,1,1,1,1,1,1,$C,0
tilemapCpct_1:        db $B,9,"%c",1,1,1,1,1,1,$C,0
tilemapCpct_2:        db $B,9,9,"%c",1,1,1,1,1,$C,0
tilemapCpct_3:        db $B,9,9,9,"%c",1,1,1,1,$C,0
tilemapCpct_4:        db $B,9,9,9,9,"%c",1,1,1,$C,0
tilemapCpct_5:        db $B,9,9,9,9,9,"%c",1,1,$C,0
tilemapCpct_6:        db $B,9,9,9,9,9,9,"%c",1,$C,0
tilemapCpct_7:        db $B,9,9,9,9,9,9,9,"%c",$C,0
tilemapCpct_8:        db $B,9,9,9,9,9,9,9,9,$C,0
