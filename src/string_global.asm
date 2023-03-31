SECTION "String data $26C4", ROM0[$26C4]

strMegaMemTitle::   db "   GB MegaMem v1.0",0
strFactoryTest::    db "Factory test...",$A,0
strRamBanks::       db "RAM banks = %d",$A,0
strFlashId::        db "Flash ID = %2%2",$A,0
strCredits::
                    db "     Design:",$A
                    db "       D.Barwin",$A
                    db "       M.Connors",$A
                    db $A
                    db "     Hardware:",$A
                    db "       R.Harding",$A
                    db "       D.Barwin",$A
                    db $A
                    db "     Code:",$A
                    db "       A.Edge",0
strDatelCopyRt::    db "  (C)Datel D&D 1999",0
strYesNoConfirm::
                    db $80,$81,$25,$73,$81,$80,$A
		            db $83,$84,$85,"      Yes     ",$83,$84,$85,$A
		            db $86,$87,$88,"      No      ",$86,$87,$88,0
strWorking::
                    db $80,$81,"                ",$81,$80, $A
                    db $83,$84,$85,"  Working...  ",$83,$84,$85,$A
                    db $86,$87,$88, "              ", $86,$87,$88,0
                    db "              ",0
strEraseGameSv::    db "Erase game save ",0
strReset::          db "      Reset     ",0
strDelete_2::       db "     Delete     ",0
strSramWriteTst::   db "Mushroom/Kingpin",0

strData2_End::
