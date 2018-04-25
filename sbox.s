
.syntax unified

.data
	.balign 4
	sbox_presentarray: 	.skip 48

	.balign 4
	playerctr: 	.skip 4

	.balign 4
	writeto: 	.skip 12

	.balign 4
	rng: 		.skip 4

.text

.global sbox_present
.type   sbox_present, %function

sbox_present:

	// 4 'blocks' of 3 words each. We need them to be consecutive so we define them as one array and we'll use offsets in code
	// These are used to contain values in RAM when registers are needed for computation

    	// After setup, this is the scheme:
	// b1: r0-r2
	// b2: r3-r5
	// b3: r6-r8
	// b4: r9-r11
	//r12 and r14 are scratch
	//A-D are locations in RAM.
	//[Yx] is the list of ouput values also in RAM


    // r0-r2 contain the readfrom pointers.
    // r3-r5 contain the writeto pointers.
    // r6 contains the rng pointer

	stmfd sp!,{r4-r11,lr}


    // Need to save the source pointers but no registers after reading, so stack
    push {r0-r2}
    
    // populate the source destination pointers
    ldr r7, =writeto
    stm r7, {r3-r5}

    // prepare counter. 60 (15 * 4) ensures that the first addition puts pointer right
    mov r7, #60
    ldr r8, =playerctr
    str r7, [r8]

    // Fetch blocks from shares and update pointers
loop: pop {r11, r12, r14}
    ldm r11!, {r0, r3, r6, r9}
    ldm r12!, {r1, r4, r7, r10}
	push {r11, r12}
    ldm r14!, {r2, r5, r8, r11}
	push {r14}		// Very inelegant but no more registers.
	// Note that 
	

	//b1 = X0, b2 = X1, b3 = X2, b4 = X3           A = null, B = null, C = null, D = null         RND1, RND2, RND3, RND4
	  
	//  T1 = X2 ^ X1//

	eor r6, r3
	eor r7, r4
	eor r8, r5

	//b1 = X0, b2 = X1, b3 = T1, b4 = X3           A = null, B = null, C = null, D = null         RND1, RND2, RND3, RND4

	//  T2 = X1 & T1//

	//[WriteToMem X0, X3]
	ldr r14, =sbox_presentarray
	stm r14, {r0, r1, r2, r9, r10, r11}

	//[ReadFromMem RND1]
	//call RNG
	//bl updateRNG


	movw r14,0xEA60
	movt r14,0x2000
	ldr r9, [r14] //0
    ldr r10, [r14, #4] //1
    ldr r11, [r14, #8] //2

	 //b1 = X0, b2 = X1, b3 = T1, b4 = RND1           A = X0, B = X3, C = null, D = null         RND2, RND3, RND4

	////c1 = a1b1 XOR (RND12 XOR RND13)
	and r0, r3, r6
	eor r12, r9, r10
	eor r0, r12

	////c2 = a2b2 XOR (((RND12 XOR a2b1) XOR a1b2) XOR RND23)
	and r1, r4, r7
	and r12, r4, r6
	eor r12, r9
	and r14, r3, r7
	eor r12, r14
	eor r1, r12
	eor r1, r11

	////c3 = a3b3 XOR (((RND13 XOR a3b1) XOR a1b3) XOR ((RND23 XOR a3b2) XOR a2b3))
	and r2, r5, r8
	and r12, r5, r6
	eor r12, r10
	and r14, r3, r8
	eor r12, r14
	eor r2, r12
	and r12, r5, r7
	eor r12, r11
	and r14, r4, r8
	eor r12, r14
	eor r2, r12

	 //b1 = T2, b2 = X1, b3 = T1, b4 = RND1           A = X0, B = X3, C = null, D = null         RND2, RND3, RND4

	//  T3 = X0 ^ T2//

	//[ReadFromMem X0]
	ldr r14, =sbox_presentarray
	ldm r14, {r9, r10, r11}

	 //b1 = T2, b2 = X1, b3 = T1, b4 = X0           A = X0, B = X3, C = null, D = null         RND2, RND3, RND4

	eor r9, r0
	eor r10, r1
	eor r11, r2

	 //b1 = T2, b2 = X1, b3 = T1, b4 = T3           A = X0, B = X3, C = null, D = null         RND2, RND3, RND4
	  
	//  Y3 = X3 ^ T3//

	//[ReadFromMem X3]
	ldr r14, =sbox_presentarray + 12
	ldm r14, {r0, r1, r2}

	 //b1 = X3, b2 = X1, b3 = T1, b4 = T3           A = X0, B = X3, C = null, D = null         RND2, RND3, RND4

	eor r0, r9
	eor r1, r10
	eor r2, r11

	 //b1 = Y3, b2 = X1, b3 = T1, b4 = T3           A = X0, B = X3, C = null, D = null         RND2, RND3, RND4

	//  T2 = T1 & T3//

	//[WriteToMem X1]
	ldr r14, =sbox_presentarray
	stm r14, {r3, r4, r5}

	//[WriteToMem Y3]
	push {r0, r1, r2}       // Will need it a little later
	ldr r12, =playerctr
	ldr r14, [r12]
	add r14, #132
	str r14, [r12]
	ldr r12, =writeto
	ldm r12, {r3, r4, r5}   // Use the fact that these will be overwritten below here
	str r0, [r3, r14]
	str r1, [r4, r14]
    str r2, [r5, r14]

	 //b1 = Y3, b2 = X1, b3 = T1, b4 = T3           A = X1, B = X3, C = null, D = null         RND2, RND3, RND4 [Y3]
	  
	//[ReadFromMem RND2]


	movw r14,0xEA60
	movt r14,0x2000
	ldr r3, [r14, #12] //0
    ldr r4, [r14, #16] //1
    ldr r5, [r14, #20] //2


	 //b1 = Y3, b2 = RND2, b3 = T1, b4 = T3           A = X1, B = X3, C = null, D = null       RND3, RND4 [Y3]

	//c1 = a1b1 XOR (RND12 XOR RND13)
	and r0, r6, r9
	eor r12, r3, r4
	eor r0, r12

	//c2 = a2b2 XOR (((RND12 XOR a2b1) XOR a1b2) XOR RND23)
	and r1, r7, r10
	and r12, r7, r9
	eor r12, r3
	and r14, r6, r10
	eor r12, r14
	eor r1, r12
	eor r1, r5

	//c3 = a3b3 XOR (((RND13 XOR a3b1) XOR a1b3) XOR ((RND23 XOR a3b2) XOR a2b3))
	and r2, r8, r11
	and r12, r8, r9
	eor r12, r4
	and r14, r6, r11
	eor r12, r14
	eor r2, r12
	and r12, r8, r10
	eor r12, r5
	and r14, r7, r11
	eor r12, r14
	eor r2, r12

	//  T1 ^= Y3//

	//[ReadFromMem Y3]
	pop {r3, r4, r5}

	 //b1 = T2, b2 = Y3, b3 = T1, b4 = T3           A = X1, B = X3, C = null, D = null       RND3, RND4 [Y3]

	eor r6, r3
	eor r7, r4
	eor r8, r5

	 //b1 = T2, b2 = Y3, b3 = T1 (updated), b4 = T3           A = X1, B = X3, C = null, D = null       RND3, RND4 [Y3]

	//  T2 ^= X1//

	//[WriteToMem T1, T3]
	ldr r14, =sbox_presentarray + 24
	stm r14, {r6, r7, r8, r9, r10, r11}

	//[ReadFromMem X1, X3]
	ldr r14, =sbox_presentarray
	ldm r14, {r6, r7, r8, r9, r10, r11}

	 //b1 = T2, b2 = Y3, b3 = X1, b4 = X3           A = X1, B = X3, C = T1, D = T3       RND3, RND4 [Y3]

	eor r0, r6
	eor r1, r7
	eor r2, r8

	 //b1 = T2 (updated), b2 = Y3, b3 = X1, b4 = X3           A = X1, B = X3, C = T1, D = T3       RND3, RND4 [Y3]

	//  T4 = X3 | T2//
	  
	//[ReadFromMem RND3]


	movw r14,0xEA60
	movt r14,0x2000
	ldr r3, [r14, #24] //0
    ldr r4, [r14, #28] //1
    ldr r5, [r14, #32] //2


	 //b1 = T2, b2 = RND3, b3 = T4, b4 = X3           A = X1, B = X3, C = T1, D = T3       RND4 [Y3]

	mvn r0, r0
	mvn r9, r9
	// do the and

	//c1 = a1b1 XOR (RND12 XOR RND13)
	and r6, r0, r9
	eor r12, r3, r4
	eor r6, r12

	//c2 = a2b2 XOR (((RND12 XOR a2b1) XOR a1b2) XOR RND23)
	and r7, r1, r10
	and r12, r1, r9
	eor r12, r3
	and r14, r0, r10
	eor r12, r14
	eor r7, r12
	eor r7, r5

	//c3 = a3b3 XOR (((RND13 XOR a3b1) XOR a1b3) XOR ((RND23 XOR a3b2) XOR a2b3))
	and r8, r2, r11
	and r12, r2, r9
	eor r12, r4
	and r14, r0, r11
	eor r12, r14
	eor r8, r12
	and r12, r2, r10
	eor r12, r5
	and r14, r1, r11
	eor r12, r14
	eor r8, r12

	mvn r0, r0
	mvn r6, r6

	 //b1 = T2, b2 = RND3, b3 = T4, b4 = ~X3           A = X1, B = X3, C = T1, D = T3       RND4 [Y3]

	//  Y2 = T1 ^ T4//

	//[ReadFromMem T1]
	ldr r14, =sbox_presentarray + 24
	ldm r14, {r3, r4, r5}

	 //b1 = T2, b2 = T1, b3 = T4, b4 = ~X3           A = X1, B = X3, C = T1, D = T3       RND4 [Y3]

	eor r6, r3
	eor r7, r4
	eor r8, r5

	//[WriteToMem Y2]    // not as elegant as Y3 due to register pressure
	ldr r12, =playerctr
	ldr r14, [r12]
	sub r14, #64
	str r14, [r12]
	ldr r12, =writeto
	ldr r12, [r12]
	str r6, [r12, r14]
    ldr r12, =writeto + 4
	ldr r12, [r12]
	str r7, [r12, r14]
	ldr r12, =writeto + 8
	ldr r12, [r12]
    str r8, [r12, r14]

	 //b1 = T2, b2 = T1, b3 = Y2, b4 = ~X3           A = X1, B = X3, C = T1, D = T3       RND4 [Y2, Y3]

	//  T2 ^= (~X3)//

	 //b1 = T2, b2 = T1, b3 = Y2, b4 = ~X3           A = X1, B = X3, C = T1, D = T3       RND4 [Y2, Y3]

	eor r0, r9
	eor r1, r10
	eor r2, r11

	 //b1 = T2 (updated), b2 = T1, b3 = Y2, b4 = ~X3           A = X1, B = X3, C = T1, D = T3       RND4 [Y2, Y3]

	//  Y0 = (Y2) ^ T2//

	eor r9, r6, r0
	eor r10, r7, r1
	eor r11, r8, r2

	//[WriteToMem Y0]
	ldr r12, =playerctr
	ldr r14, [r12]
	sub r14, #128
	str r14, [r12]
	ldr r12, =writeto
	ldm r12, {r6, r7, r8}   // Use the fact that these will be overwritten in ReadFromMem RND4
	str r9, [r6, r14]
	str r10, [r7, r14]
    str r11, [r8, r14]

	 //b1 = T2 , b2 = T1, b3 = Y2, b4 = Y0                       A = X1, B = X3, C = T1, D = T3       RND4 [Y0, Y2, Y3]
	  
	//  T2 |= T1//
	  
	//[ReadFromMem RND4]

	//bl wrap_fun_call
	movw r14,0xEA60
	movt r14,0x2000
	ldr r6, [r14, #36] //0
    ldr r7, [r14, #40] //1
    ldr r8, [r14, #44] //2


	 //b1 = T2 , b2 = T1, b3 = RND4, b4 = T2 (updated)           A = X1, B = X3, C = T1, D = T3        [Y0, Y2, Y3]

	mvn r0, r0
	mvn r3, r3
	// do the and

	//c1 = a1b1 XOR (RND12 XOR RND13)
	and r9, r0, r3
	eor r12, r6, r7
	eor r9, r12

	//c2 = a2b2 XOR (((RND12 XOR a2b1) XOR a1b2) XOR RND23)
	and r10, r1, r4
	and r12, r1, r3
	eor r12, r6
	and r14, r0, r4
	eor r12, r14
	eor r10, r12
	eor r10, r8

	//c3 = a3b3 XOR (((RND13 XOR a3b1) XOR a1b3) XOR ((RND23 XOR a3b2) XOR a2b3))
	and r11, r2, r5
	and r12, r2, r3
	eor r12, r7
	and r14, r0, r5
	eor r12, r14
	eor r11, r12
	and r12, r2, r4
	eor r12, r8
	and r14, r1, r5
	eor r12, r14
	eor r11, r12

	mvn r3, r3
	mvn r9, r9
	  
	//  Y1 = T3 ^ T2//

	//[ReadFromMem T3]
	ldr r14, =sbox_presentarray + 36
	ldm r14, {r0, r1, r2}

	 //b1 = T3 , b2 = T1, b3 = RND4, b4 = T2                     A = X1, B = X3, C = T1, D = T3        [Y0, Y2, Y3]

	eor r6, r0, r9
	eor r7, r1, r10
	eor r8, r2, r11

	//[WriteToMem Y1]
	ldr r12, =playerctr
	ldr r14, [r12]
	add r14, #64
	str r14, [r12]
	ldr r12, =writeto
	ldm r12, {r3, r4, r5}   // At the end of the loop, don't care about contents
	str r6, [r3, r14]
	str r7, [r4, r14]
    str r8, [r5, r14]

	 //b1 = T3, b2 = T1, b3 = Y1, b4 = T2                        A = X1, B = X3, C = T1, D = T3        [Y0, Y1, Y2, Y3] 
	
	ldr r14, =playerctr
	ldr r12, [r14]
	teq r12, #124 			// offset 124 (4 * 31) is the last word to be written. If this is present we've had the complete state
	bne loop
	
	pop {r11, r12, r14}		// The read pointers are still on the stack

	ldmfd sp!,{r4-r11,pc}

.global wrap_fun_call
.type   wrap_fun_call, %function
wrap_fun_call:
stmfd sp!,{r0-r12,lr}
bl updateRNG
ldmfd sp!,{r0-r12,pc}

addr_sbox_presentarray:   .word sbox_presentarray
addr_playerctr:   .word playerctr
addr_writeto: 	  .word writeto
addr_rng:	  .word rng


