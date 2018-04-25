
.syntax unified

.global addroundkey
.type   addroundkey, %function

addroundkey:

	stmfd sp!,{r4-r11,lr}

	mov r4, #0xFFFFFFFF // Mask for transforming keybit into word when multiplied
	
	ldm r1, {r1, r2, r3} // Fetch the key
	push {r3}
	push {r2}

	// In the following, the relevant key (half)word is always in r1.
	// The first halfword is in the lower half. Our code works on the top half, so let's shift.

	lsl r2, r1, #16
	mov r1, #0
	mov r1, r2
	mov r2, #4
	bl keyword
	
	// Just take the entire word in this case.
	pop {r1}
	mov r2, #8
	bl keyword
	
	// This word contains the relevant data and the extra state bits that aren't used, so we
	// only do 4 loop iterations here too.
	pop {r1}
	mov r2, #4
	bl keyword

	ldmfd sp!,{r4-r11,pc}


.global keyword
.type   keyword, %function
	
// r1 always contains the remaining keybits
keyword:

	stmfd sp!,{r4-r11,lr}

	mov r14, r2			//r14 is used as lr in the function call, and all other registers are in use, so we gotta move it over
loop: 	ldm r0, {r9-r12}
	ubfx r3, r1, #31, #1		// Get topmost keybit in word
	lsl r2, r1, #1			// Get rid of it, slide the next in place by shifting
	mov r1, #0					// security: overwrite original, non-shifted key
	mov r1, r2					// then write the modified key back
    	mov r2, #0x55555555         // HW 16
    	mul r2, r3, r4              // now HW 0 or 32. So either way, distance of 16.
    	eor r5, r2, r9				// apply to data

	ubfx r3, r1, #31, #1
	lsl r2, r1, #1
	mov r1, #0
	mov r1, r2
    	mov r2, #0x55555555
    	mul r2, r3, r4
    	eor r6, r2, r10

	ubfx r3, r1, #31, #1
	lsl r2, r1, #1
	mov r1, #0
	mov r1, r2
    	mov r2, #0x55555555
    	mul r2, r3, r4
    	eor r7, r2, r11

	ubfx r3, r1, #31, #1
	lsl r2, r1, #1
	mov r1, #0
	mov r1, r2
    	mov r2, #0x55555555
    	mul r2, r3, r4
    	eor r8, r2, r12

    	stm r0!, {r5-r8}            // Write back to same position, update after for new reads
    	subs r14, #1
    	bne loop
	
	ldmfd sp!,{r4-r11,pc}


