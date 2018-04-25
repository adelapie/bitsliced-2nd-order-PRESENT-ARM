
.syntax unified

.text

.global updatekey
.type   updatekey, %function

updatekey:

	stmfd sp!,{r4-r11,lr}

    ldm r0, {r2-r4}         // r2 contains F

    // Split the key in parts so they can be reassembled in their right order
    ubfx r5, r4, #0, #3     // A
    ubfx r6, r4, #3, #16    // B
    ubfx r7, r4, #19, #13   // C
    ubfx r8, r3, #0, #19    // D
    ubfx r9, r3, #19, #13   // E
    
    // Reconstitute the parts into a shifted key
    mov r10, r6
    mov r11, r9             // E
    orr r11, r11, r2, lsl #13    // F
    orr r11, r11, r5, lsl #29    // A
    mov r12, r7             // C
    orr r12, r12, r8, lsl #13    // D

    // get bits for sbox
    ubfx r6, r10, #15, #1   // X0
    ubfx r7, r10, #14, #1   // X1
    ubfx r8, r10, #13, #1   // X2
    ubfx r9, r10, #12, #1   // X3

    // Perform Boolean sbox (Y0 = r7, Y1 = r8, Y2 = r6, Y3 = r5)
    eor r2, r7, r8          // T1=X2^X1  retire X2
    and r3, r7, r2          // T2=X1&T1
    eor r4, r6, r3          // T3=X0^T2  retire X0
    eor r5, r9, r4          // Y3=X3^T3
    and r3, r2, r4          // T2=T1&T3
    eor r2, r5              // T1^=Y3
    eor r3, r7              // T2^=X1    retire X1
    orr r6, r9, r3          // T4=X3|T2
    eor r6, r2              // Y2=T1^T4  retire T4
    eor r9, #1              // X3=~X3
    eor r3, r9              // T2^=X3    retire X3
    eor r7, r6, r3          // Y0=Y2^T2
    orr r3, r2              // T2|=T1
    eor r8, r3, r4          // Y1=T3^T2

    // Apply sbox to key
    bfc r10, #12, #4


    eor r10, r10, r7, lsl #15
    eor r10, r10, r8, lsl #14
    eor r10, r10, r6, lsl #13
    eor r10, r10, r5, lsl #12

    // XOR counter with key bits 19-15
    eor r12, r12, r1, lsl #15

    // push registers back into memory
    stm r0, {r10-r12}

	ldmfd sp!,{r4-r11,pc}

