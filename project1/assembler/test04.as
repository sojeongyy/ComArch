        lw      0       1       neg       ; check negative value load
        lw      0       2       5         ; check numeric address 
	sw	0	3	6	  ; check store instruction
done    halt
neg     .fill   -9999
five	.fill   5
six	.fill	6
