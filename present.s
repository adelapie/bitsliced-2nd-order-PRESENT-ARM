
.syntax unified

.data

	.balign 4
	state01: .skip 256

	.balign 4
	state02: .skip 256

	.balign 4
	state03: .skip 256
	
	.balign 4
	state11: .skip 256

	.balign 4
	state12: .skip 256

	.balign 4
	state13: .skip 256
	
	.balign 4
	readfrom: .skip 12
	
	.balign 4
	writeto: .skip 12
	
	.balign 4
	roundctr: .skip 4

	.balign 4
	rngptr: .skip 4

	.balign 4
	keyptr: .skip 4

	.balign 4
	dataptr: .skip 4
	
.text

.global present
.type   present, %function

present:

	stmfd sp!,{r4-r11,lr}
	
	// populate pointers
	ldr r4, =dataptr
	str r0, [r4]
	ldr r4, =keyptr
	str r1, [r4]
	//ldr r4, =rngptr
	//str r2, [r4]
	
	// populate writeto
	ldr r4, =state11
	ldr r5, =state12
	ldr r6, =state13
	ldr r7, =writeto
	stm r7, {r4-r7}

    // initialize round counter
    ldr r4, =roundctr
    mov r5, #1
    str r5, [r4]

    mov r1, r2
    ldr r2, =state01
    ldr r3, =state02
    ldr r4, =state03
    ldr r5, =readfrom
    stm r5, {r2-r4}

	bl splitdata
	
	bl tons_of_nops

rounds: ldr r0, =readfrom
    ldr r0, [r0]
    ldr r1, =keyptr
	ldr r1, [r1]
    bl addroundkey
    
    ldr r0, =keyptr
	ldr r0, [r0]
    ldr r1, =roundctr
    ldr r1, [r1]
	bl updatekey

    ldr r0, =readfrom
    ldm r0, {r0-r2}
    ldr r3, =writeto
    ldm r3, {r3-r5}
    ldr r6, =rngptr
	ldr r6, [r6]

	bl sbox_present
	
	// Switch read and write pointers
	ldr r0, =readfrom
	ldm r0, {r1-r3}
    ldr r4, =writeto
    ldm r4, {r5-r7}
    stm r0, {r5-r7}
    stm r4, {r1-r3}
    
    // update round counter, check and branch
	ldr r4, =roundctr
	ldr r5, [r4]
	add r5, #1
	str r5, [r4]            // not actually required if not looping
	teq r5, #32
	bne rounds

    ldr r0, =readfrom
    ldr r0, [r0]
    ldr r1, =keyptr
	ldr r1, [r1]
	bl addroundkey
	
	bl tons_of_nops

	ldr r0, =dataptr
	ldr r0, [r0]
	ldr r1, =readfrom
	ldm r1, {r1-r3}
	bl unsplitdata
	
	ldmfd sp!,{r4-r11,pc}

.global tons_of_nops
.type   tons_of_nops, %function
tons_of_nops:
stmfd sp!,{r0-r12,lr}
mov r0,#0x2700
orr r0,#0x0010
top:
nop
subs r0,#1
bne top
ldmfd sp!,{r0-r12,pc}
