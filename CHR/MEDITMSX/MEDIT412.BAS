10 '    SAVE"MEDIT412.BAS",A
20 SOUND8,0:SOUND9,0:SOUND10,0:CLEAR 100,&HC000:SCREEN0,2,0:COLOR15,0,1:KEY 6,"color 15,0,1"+CHR$(13)
30 DEFINT A-Z:CLS:PRINT " NEO MAP-EDITOR Ver 4.12":LOCATE 20,1:PRINT "(C) 1991/2025 m@3":PRINT "SPC = SET PATTERN":PRINT "(SHIFT = CURSOR MOVE IN THE SCREEN)":PRINT "RETURN = MENU"
40 '
50 PRINT"(HIT ANY KEY!) ";:LOCATE,,1:PRINTCHR$(255):LOCATE,,0
60 IFINKEY$<>""ORSTRIG(1)THENSCREEN5,2:VDP(9)=VDP(9) OR 32:OPEN"GRP:"AS#1:DRAW"BM0,2":PRINT#1,"NEO MAPEDITOR V4.12":DRAW"BM120,10":PRINT#1,"(C)1991/2025 m@3"ELSE60
70 CLEAR100,&HB000: BLOAD"MEDIT411.BIN"
80 OPEN"GRP:"AS#1:DEFINTA-Z
90 DEFUSR2=&HB600:DEFUSR3=&HB603:DEFUSR4=&HB606:DEFUSR5=&HB609:DEFUSR6=&HB60C:DEFUSR7=&HB60F:DEFUSR8=&HB612:POKE&HD202,0
100 SETPAGE,1:CLS:COPY(0,0)-(255,44),1TO(0,211),1,PSET:GOSUB620:SETPAGE,0
110 X=0:Y=0:CX=0:CY=0:Q=0
120 ONSTOPGOSUB590:STOPON:WX=60:WY=24:WM=200:WN=172:WK=&HC300:GOSUB670
130 COLOR:COLOR 3:RESTORE 190:READ A$:PRESET (88,40):PRINT #1,A$:COLOR15
140 FOR I=0 TO 10:READ A$(I):PRESET (88,I*10+50):PRINT #1,A$(I):NEXT
150 '
160 GOSUB610:FOR I=0 TO 1:S=STICK(0)ORSTICK(1):QQ=Q:Q=(Q+(S=1)-(S=5)+11)MOD11:IF QQ<>Q THEN PSET (72,QQ*10+50):COLOR 15:PRINT #1,"  ";A$(QQ)
170 PSET (72,Q*10+50),13:COLOR 13:PRINT #1,"> ";:COLOR 8:PRINT #1,A$(Q)
180 I=-STRIG(0)-STRIG(1)-(INKEY$=CHR$(13)):NEXT:POKE WK+2,CX*16+CY
190 DATA Menu Select ,ϯ�ߴ�ި��,��׸�-����,��׸�-�-��,ϯ���-��,ϯ�߾-��,��׸�-��,ϯ�߸ر-,��׸�-��ި�-,��ޓý�,�������,������
200 ON Q+1 GOSUB220,250,280,300,320,340,490,520,530,550,590:GOTO 120
210 RETURN
220 POKEWK,0:POKEWK+1,2:POKEWK+2,CX*16+CY:SETPAGE,2:FORI=0TO7:LINE(I*16+128,32)-STEP(15,15),0,BF:LINE-STEP(-15,-15),15,B:NEXT:SETPAGE,0
230 WX=11:WY=11:WM=165:WN=165:GOSUB670:COPY(0 ,0 )-(255,211),0TO(0 ,0 ),1:POKEWK+5,X:POKEWK+6,Y:U=USR5(0):X=PEEK(WK+5):Y=PEEK(WK+6):POKEWK+1,0:SETPAGE,2:LINE(128,32)-STEP(255,15),0,BF,PSET:SETPAGE0,0:IFPEEK(&HFBEB)AND1THENRETURNELSEGOSUB250:GOTO220'EDIT
240 'CHR SELECT
250 U=USR4(0):SETPAGE1,1:A$=CHR$(255)+STRING$(14,&H80)+STRING$(2,255)+STRING$(14,1)+CHR$(255):SPRITE$(0)=A$:GOSUB610
260 S=STICK(0)ORSTICK(1):CX=CX+((S=7 AND CX>0)-(S=3 AND CX<15)):CY=CY+((S=1 AND CY>0)-(S=5 AND CY<5)):PUTSPRITE 0,(CX*16,CY*16-1),15,0:IF STRIG(0)ORSTRIG(1)ORSTRIG(3)ORINKEY$=CHR$(13)THENGOSUB620:SETPAGE0,0:RETURNELSE260
270 'CHRLOAD
280 KT$=".SC5":GOSUB700:SETPAGE 2,2:BLOADN$+KT$,S:SETPAGE0,0:RETURN
290 'LOAD
300 KT$=".MDT":GOSUB700:SETPAGE1,1:BLOAD N$+KT$,S:U=USR6(1):SETPAGE0,0:RETURN
310 'SAVE
320 KT$=".MDT":GOSUB700:SETPAGE1,1:U=USR6(0):BSAVE N$+KT$,0,&H3D7F,S:SETPAGE0,0:RETURN
330 'CHANGE
340 SETPAGE ,0:SPRITE$(0)=CHR$(255)+STRING$(6,&H81)+CHR$(255):SPRITE$(1)=CHR$(255)+STRING$(14,&H80)+STRING$(2,255)+STRING$(14,1)+CHR$(255):U=USR4(0):COPY(0,0)-(255,211),1TO(0,0),0:A=0:B=0:OX=127:OY=96:AD=0:COLOR=(0,0,3,2)
350 RESTORE350:FORI=0TO3:READFX(I),FY(I):NEXT:DATA8,0,-8,8,8,0,-8,-8
360 FORI=0TO1:S=STICK(0)ORSTICK(1):YY=(S=1ORS=2ORS=8)-(S>3ANDS<7):XX=(S>5)-(S<5ANDS>1):GC=CG:CG=2+((PEEK(&HFBEB)AND1)=0):A$=INKEY$:IFA$=CHR$(24)THENSETPAGE3:FORJ=0TO1:J=-(INKEY$=CHR$(24)):NEXT:SETPAGE0:I=0:NEXT
370 I=-(STRIG(0)ORSTRIG(1))-(A$=CHR$(13)ORSTRIG(3))*2
380 IF CG=2 GOTO 400 ELSE AQ=AX+XX*16:BQ=BX+YY*16:AF=0:BF=0:FR=0:IF AQ>-1 AND AQ<255 THEN AX=AQ
390 IF BQ>-1AND BQ<96 THEN BX=BQ
400 XO=OX:YO=OY:OX=(1-(AX\128))*126+1:OY=(1-(BX\104))*95+1:OX=1:IFOX=XOTHENIFOY=YOTHENIFCG=GCTHEN420
410 COPY(XO-1,YO-1)-(XO+128,YO+112),1 TO(XO-1,YO-1),0,PSET
420 IF CG=2 THEN AW=A:BW=B:A=(A+XX*8+128)MOD128:B=(B+YY*8+112)MOD112:LINE(OX-1,OY-1)-STEP(129,113),15,B:COPY(0,0)-(127,111),CGTO(OX,OY),0,PSET
430 CC=RND(1)*14+2:PUTSPRITE1,(AX,BX-1),CC:PUTSPRITE2,(AX+AF,BX+BF-1),17-CC,0:IFCG=2THENPUTSPRITE0,(A+OX,B-1+OY),CCELSEPUTSPRITE0,(0,217)
440 NEXT:COLOR=(4,7,7,7):COLOR=(4,0,0,7):BEEP:IFI=3THENCOLOR=(0,0,0,0):GOSUB620:RETURNELSEGOSUB470:AF=AF+FX(FR):BF=BF+FY(FR)
450 IF FR=3 THEN AX=AX+16:IF AX>255 THEN AX=0:BX=(BX+16)MOD208
460 FR=(FR+1)MOD4:GOTO 360
470 COPY(A,B)-(A+7,B+7),2TO(AF+AX,BF+BX),1:COPY(A,B)-(A+7,B+7),2TO(AF+AX,BF+BX),0:POKEWK+2,A\8+(B\8)*16:AD=(AX+BX*16)\4:U=USR8(AD+FR):RETURN
480 'CLEAR
490 WX=36:WY=85:WM=228:WN=117:GOSUB670:DRAW"BM46,97":COLOR5:PRINT#1,"ϯ�߆ �ر- �?";:COLOR15:PRINT#1,"(Y/N)";:GOSUB640:IFA=2THENRETURN
500 U=USR3(0):RETURN
510 'EDITOR
520 WX=76:WY=84:WM=182-8:WN=109:GOSUB670:DRAW"BM86,93":COLOR5:PRINT#1,"OK?";:COLOR15:PRINT#1,"(Y/N) ";:COLOR15:GOSUB640:IFA=1THENGOSUB600:CLEAR100,&HCB00:RUN"NEWV3-3.BAS"ELSERETURN
530 POKEWK,1:GOTO 230'TEST
540 'LOOK MAP
550 WX=58:WY=47:WM=WX+137:WN=WY+129:GOSUB670:U=USR2(0):SPRITE$(0)=CHR$(255)+STRING$(6,&H81)+CHR$(255)
560 FORI=0TO1:PUTSPRITE0,(X\2+63,Y\2+51),RND(1)*16,0:S=STICK(0)ORSTICK(1):X=(X+(S>5)*8-(S<5ANDS>1)*8+237)MOD237:Y=(Y-(S<7ANDS>3)*8+((S=1ORS=2ORS=8))*8+224)MOD224:I=-STRIG(0)-STRIG(1)-STRIG(3)-(INKEY$=CHR$(13)):NEXTI
570 '
580 GOSUB620:RETURN
590 SETPAGE0,0:WX=20:WY=85:WM=228:WN=117:GOSUB670:DRAW"BM30,97":COLOR15:PRINT#1,"WAKU.SC5��-�ޜ?";:COLOR15:PRINT#1,"(Y/N)";:GOSUB650:IFA=1THENSETPAGE,1:BLOAD"WAKU.SC5",S
595 GOSUB600:SETBEEP1,4:BEEP:DEFUSR=&H3E:U=USR(0):KEY7,"files"+CHR$(13):KEY10,"load"+CHR$(34)+CHR$(13):KEY8,"run"+CHR$(34)+"newv3.bas"+CHR$(13):SCREEN0:COLOR15,0,0:PRINTPEEK(WK+5);PEEK(WK+6):CLEAR100,&HCB00:END
600 DEFUSR2=&HCB00:DEFUSR3=&HCB03:          DEFUSR4=&HCB06:DEFUSR5=&HCB09:          DEFUSR6=&HCB0C:DEFUSR7=&HCB0F:RETURN
610 IFINKEY$<>""ORSTRIG(0)ORSTRIG(1)THEN610ELSERETURN
620 FORI=0TO31:PUTSPRITEI,(0,217):NEXT:RETURN
630 'GET KEY SUB
640 GOSUB610
650 A$=INKEY$:B$="YyYNn"+CHR$(13):A=-((INSTR(B$,A$)+2)\3)*(A$<>"")-STRIG(1)-STRIG(3)*2:IF A THEN PRINT#1,MID$(B$,A*3-2,1):RETURN ELSE 650
660 'WINDOW
670 IFPEEK(&HFCAB)<>0THENFOR I=0 TO 4:LINE (WX+I,WY+I)-(WM-I,WN-I),VAL("&H"+MID$("54F45",I+1,1)),B:NEXT:LINE(WX+I,WY+I)-(WM-I,WN-I),0,BF:RETURN
680 FOR I=0 TO 5:LINE (WX+I,WY+I)-(WM-I,WN-I),VAL("&H"+MID$("54F450",I+1,1)),B,PSET:NEXT:LINE(WX+I,WY+I)-(WM-I,WN-I),0,BF,PSET:RETURN
690 'GET NAME
700 WX=48:WY=58+40:WM=216:WN=96+40:GOSUB670:DRAW "BM58,108":COLOR 10:PRINT #1,"INPUT NAME(8 WORDS)":DRAW "BM58,120":COLOR 8:PRINT #1,"________";:COLOR15:PRINT#1,KT$:N$="":DRAW "BM58,120":COLOR 15:GOSUB 610:FOR I=1 TO 8
710 A=ASC(INPUT$(1)):IFA=13THENIFI=1THENRETURN120ELSEI=8ELSEIFA=8ANDI<>1THENI=I-2:PSET(58+I*8,120),0:COLOR 8:PRINT #1,"_":COLOR 15:PRESET(58+I*8,120),1:N$=LEFT$(N$,I)ELSEIFA<48OR(A<65ANDA>57)ORA=127THENI=I-1ELSEN$=N$+CHR$(A):PRINT#1,CHR$(A);
720 NEXT:FORI=0TO500:NEXT:WX=76:WY=84+40:WM=182:WN=109+40:GOSUB670:DRAW "BM86,133":COLOR 5:PRINT #1,"OK? ";:COLOR 15:PRINT #1,"(Y/N) ";:COLOR 15
730 GOSUB640:IFA=1THENRETURNELSE700
740 A$=INKEY$:IF A$="N" OR A$="n" OR STRIG(3) THEN COLOR 8:PRINT #1,"N":GOTO 700 ELSE IF A$="Y" OR A$="y" OR STRIG(1) THEN COLOR 15:PRINT #1,"Y" ELSE 740
750 RETURN
