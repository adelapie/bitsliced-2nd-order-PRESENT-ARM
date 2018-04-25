
.syntax unified

.text

.global unsplitdata
.type   unsplitdata, %function

unsplitdata:

	stmfd sp!,{r4-r11,lr}
	
	mov r14, #21 		// Do 3 words per loop iteration. 6 words, so 21 loops plus 1 remainder
loop: 	ldm r1!, {r4-r6} 	// First get three words from all three shares
	ldm r2!, {r7-r9}
	ldm r3!, {r10-r12}

	eor r4, r7 		// Then simply add them. No security cares, this is already the ciphertext.
	eor r4, r10
	eor r5, r8
	eor r5, r11
	eor r6, r9
	eor r6, r12
	stm r0!, {r4-r6}	// Finally write to the datapointer
    	subs r14, #1
    	bne loop
	
	
	ldr r4, [r1]		// Only 1 word remaining 
	ldr r5, [r2]
	ldr r6, [r3]
	eor r4, r5
	eor r4, r6
	str r4, [r0]
	
	ldmfd sp!,{r4-r11,pc}
