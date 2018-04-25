
.syntax unified

.data

	.balign 4
	rng: 		.skip 4

.text

.global splitdata
.type   splitdata, %function

splitdata:

		stmfd sp!,{r4-r11,lr}

//-----------loop1---------------

   		bl wrap_fun_tester2

		movw r7,0xEA60
		movt r7,0x2000 //r11
		ldr r7, [r7] //0

		//#define ADDR_RNG_OUT 0x2000EA6A
		movw r12,0xEA6A
		movt r12,0x2000
		str r7,[r12]
		add r12,r12,#4

    	ldrd r5, r6, [r0], #8		// d1, d2


		bl wrap_fun_tester2
    	movw r8,0xEA60
		movt r8,0x2000 //r11
		ldr r8, [r8] //0// r12

		str r8,[r12]
		add r12,r12,#4

    	eor r11, r5, r7			// d1 + r11

    	 bl wrap_fun_tester2
    	movw r9,0xEA60
		movt r9,0x2000 //r11
		ldr r9, [r9]			// r21

		str r9,[r12]
		add r12,r12,#4

    	eor r5, r6, r9			// d2 + r21

    	 bl wrap_fun_tester2
    	movw r10,0xEA60
		movt r10,0x2000 //r11
		ldr r10, [r10]		// r22

		str r10,[r12]
		add r12,r12,#4

    	eor r6, r11, r8			// (d1 + r11) + r12
    	eor r11, r5, r10		// (d2 + r21) + r22
    	strd r6, r11, [r2], #8		// (d1 + r11) + r12, (d2 + r21) + r22
    	strd r7, r9,  [r3], #8		// r11, r21
    	strd r8, r10, [r4], #8		// r12, r22

    	//-------------------loop2----------------------

    	   bl wrap_fun_tester2

		movw r7,0xEA60
		movt r7,0x2000 //r11
		ldr r7, [r7] //0

		str r7,[r12]
		add r12,r12,#4

    	ldrd r5, r6, [r0], #8		// d1, d2


		bl wrap_fun_tester2
    	movw r8,0xEA60
		movt r8,0x2000 //r11
		ldr r8, [r8] //0// r12

		str r8,[r12]
		add r12,r12,#4

    	eor r11, r5, r7			// d1 + r11

    	 bl wrap_fun_tester2
    	movw r9,0xEA60
		movt r9,0x2000 //r11
		ldr r9, [r9]			// r21

		str r9,[r12]
		add r12,r12,#4

    	eor r5, r6, r9			// d2 + r21

    	 bl wrap_fun_tester2
    	movw r10,0xEA60
		movt r10,0x2000 //r11

		ldr r10, [r10]		// r22

		str r10,[r12]


    	eor r6, r11, r8			// (d1 + r11) + r12
    	eor r11, r5, r10		// (d2 + r21) + r22
    	strd r6, r11, [r2], #8		// (d1 + r11) + r12, (d2 + r21) + r22
    	strd r7, r9,  [r3], #8		// r11, r21
    	strd r8, r10, [r4], #8		// r12, r22


    	//-----------------rest 30 loops








    	mov r12, #30			// Each loop iteration splits 2 words, so we loop 32 times.


//2 LOOPS RNG
loop:
        bl wrap_fun_tester2

		movw r7,0xEA60
		movt r7,0x2000 //r11
		ldr r7, [r7] //0

    	ldrd r5, r6, [r0], #8		// d1, d2


		bl wrap_fun_tester2
    	movw r8,0xEA60
		movt r8,0x2000 //r11
		ldr r8, [r8] //0// r12

    	eor r11, r5, r7			// d1 + r11

    	 bl wrap_fun_tester2
    	movw r9,0xEA60
		movt r9,0x2000 //r11
		ldr r9, [r9]			// r21
    	eor r5, r6, r9			// d2 + r21

    	 bl wrap_fun_tester2
    	movw r10,0xEA60
		movt r10,0x2000 //r11
		ldr r10, [r10]		// r22

    	eor r6, r11, r8			// (d1 + r11) + r12
    	eor r11, r5, r10		// (d2 + r21) + r22
    	strd r6, r11, [r2], #8		// (d1 + r11) + r12, (d2 + r21) + r22
    	strd r7, r9,  [r3], #8		// r11, r21
    	strd r8, r10, [r4], #8		// r12, r22
    	subs r12, #1
    	bne loop

		ldmfd sp!,{r4-r11,pc}

.global wrap_fun_tester2
.type   wrap_fun_tester2, %function
wrap_fun_tester2:
stmfd sp!,{r0-r12,lr}
bl updateRNG_only_once
ldmfd sp!,{r0-r12,pc}

