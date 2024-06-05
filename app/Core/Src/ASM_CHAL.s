.text
.syntax unified
.thumb
.align 4
.global ll_GPIO_UART_init_ASM
.global ASMCHAL_event_call_back

.global memset

// Define necessary constants
.equ RCC_APB1ENR, 0x40023840 // base address of RCC_APB1ENR
.equ RCC_AHB1ENR, 0x40023830 // base address of RCC_AHB1ENR
.equ GPIOA_MODER, 0x40020000 // base address of GPIOA_MODER
.equ GPIOA_AFR0, 0x40020020 // base address of GPIOA_AFR[0]

.equ RCC_APB1ENR_USART2EN, (1 << 17)
.equ RCC_AHB1ENR_GPIOAEN, (1 << 0)
.equ ASM_CHAL_OK, 0

.equ DMA1_stream5_BASE, 0x40026000 // Base address of DMA1 stream 5
.equ DMA_SxCR, 0x88 // Base address of DMA1 stream 5
.equ DMA_SxNDTR, 0x8C             // Offset of NDTR register within DMA stream

.equ CHAR_SIZE, 1                 // Size of a character (1 byte)

.equ DMA_DISABLE_BIT, (1 << 0)    // Bit position to disable DMA
.equ DMA_ENABLE_BIT,  (1 << 0)    // Bit position to enable DMA

ASMCHAL_GPIO_UART_init_ASM:
    // Enable USART2 clock
    ldr r0, =RCC_APB1ENR        // Load the address of RCC_APB1ENR
    ldr r1, [r0]                // Load the current value of RCC_APB1ENR
    orr r1, r1, #RCC_APB1ENR_USART2EN // Set the USART2EN bit
    str r1, [r0]                // Store the updated value back to RCC_APB1ENR

    // Enable GPIOA clock
    ldr r0, =RCC_AHB1ENR        // Load the address of RCC_AHB1ENR
    ldr r1, [r0]                // Load the current value of RCC_AHB1ENR
    orr r1, r1, #RCC_AHB1ENR_GPIOAEN // Set the GPIOAEN bit
    str r1, [r0]                // Store the updated value back to RCC_AHB1ENR

    // Configure PA2 and PA3 to alternate function mode
    ldr r0, =GPIOA_MODER        // Load the address of GPIOA_MODER
    ldr r1, [r0]                // Load the current value of GPIOA_MODER
    orr r1, r1, #(2 << 4)       // Set the bits for PA2 (MODER2[1:0] = 10)
    orr r1, r1, #(2 << 6)       // Set the bits for PA3 (MODER3[1:0] = 10)
    str r1, [r0]                // Store the updated value back to GPIOA_MODER

    // Set alternate function 7 for PA2 and PA3
    ldr r0, =GPIOA_AFR0         // Load the address of GPIOA_AFR[0]
    ldr r1, [r0]                // Load the current value of GPIOA_AFR[0]
    orr r1, r1, #(7 << 8)       // Set the bits for PA2 (AFR2[3:0] = 0111)
    orr r1, r1, #(7 << 12)      // Set the bits for PA3 (AFR3[3:0] = 0111)
    str r1, [r0]                // Store the updated value back to GPIOA_AFR[0]

    // Return CHAL_OK
    movs r0, #ASM_CHAL_OK           // Load CHAL_OK (0) into R0
    bx lr                       // Return from function





ASMCHAL_event_call_back:
    push {r4-r11, lr}
    // Copy data from rx_buff to tempMainBuffer
    mov r5, #16384                 // get buffer length
    ldr r0, =tempMainBuffer    // Load address of tempMainBuffer
    ldr r1, =offset            // Load address of offset
    ldr r1, [r1]               // Load offset value into R1
    adds r0, r0, r1             // Calculate destination address in tempMainBuffer

    ldr r1, =rx_buff           // Load address of rx_buff
    mov r6, r5

// optimize this so it stops when \n
copy_loop:
    cmp r6, #0                 // Check if buffer length is zero
    beq end_copy_loop          // Exit loop if buffer length is zero
    ldrb r3, [r1], #1          // Load byte from rx_buff and increment pointer
    strb r3, [r0], #1          // Store byte to tempMainBuffer and increment pointer
    subs r6, r6, #1            // Decrement buffer length counter
    bne copy_loop              // Loop until buffer length becomes zero
end_copy_loop:

    // Disable DMA1 Stream 5
    ldr r0, =DMA1_stream5_BASE // Load base address of DMA1 Stream 5
    ldr r1, [r0, #DMA_SxCR]     // Load DMA Stream CR register
    bic r1, r1, #DMA_DISABLE_BIT // Clear DMA enable bit
    str r1, [r0, #DMA_SxCR]     // Store back the modified value to CR register

    // Calculate new offset
    ldr r1, =offset            // Load address of offset
    ldr r4, [r1]               //load value of offset into r4
    mov r2, r5                 // Load buffer length
    ldr r3, =DMA1_stream5_BASE // Load base address of DMA1 Stream 5
    ldr r8, [r3, #DMA_SxNDTR]  // Load NDTR value of DMA1 Stream 5
    subs r9, r2, r8        // Calculate remaining bytes in DMA buffer
    adds r10, r9, #CHAR_SIZE    // Add one fot space in memory
    adds r4, r4, r10            // Add to offset
    str r4, [r1]               // Store new offset value

    // Reset DMA1 Stream 5 NDTR to bufferlength
    ldr r0, =DMA1_stream5_BASE      // Load base address of DMA1 Stream 5
    mov r1, r5                      // Load buffer length
    str r1, [r0, #DMA_SxNDTR]       // Store buffer length to NDTR

    // Clear rx_buff for new reception
    ldr r0, =rx_buff           // Load address of rx_buff
    mov r1, #0                 // Load zero value
    mov r2, r5                 // Load buffer length into R2
    bl memset           // Call custom_memset(rx_buff, 0, bufferlength

    // Enable DMA1 Stream 5
    ldr r0, =DMA1_stream5_BASE // Load base address of DMA1 Stream 5
    ldr r1, [r0, #DMA_SxCR]     // Load DMA Stream CR register
    orr r1, r1, #DMA_ENABLE_BIT // Set DMA enable bit
    str r1, [r0, #DMA_SxCR]     // Store back the modified value to CR register

    movs r0, #ASM_CHAL_OK           // Load CHAL_OK (0) into R0
    pop {r4-r11, pc}

custom_memset:
    push {r4, lr}            // Save R4 and the return address
    cmp r2, #0               // Check if length (len) is zero
    beq memset_done          // If length is zero, return immediately

    mov r4, r1               // Move the value to R4 (value to set)

memset_loop:
    strb r4, [r0], #1        // Store the byte value at dest and increment pointer
    subs r2, r2, #1          // Decrement length (len)
    bne memset_loop          // Repeat until length (len) becomes zero

memset_done:
    pop {r4, pc}             // Restore R4 and return








