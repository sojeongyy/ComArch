init	lw	0	1	num	; reg1 = 3
        lw 	0 	2 	neg     ; reg2 = -1
	lw 	0 	3	back    ; reg3 store label "back" address
loop    add 	1 	2 	1      	; reg1--
        beq 	1 	0 	done    ; if reg1 == 0 then jump to label "done"
        jalr 	3 	4       	; jump to loop, store PC + 1 in reg4
        noop          
done    halt           
num     .fill 	3
neg     .fill 	-1
back    .fill 	loop     
