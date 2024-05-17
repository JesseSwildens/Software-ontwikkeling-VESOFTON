
/**
 * @file uart_init.c
 * @brief UART initialization functions.
 * @author @Mirkoet
 */

#include "CHAL.h"
// #include "ASM_CHAL.h"
#include "string.h"
// #include <cstdio>
// #include <queue>
// #include <string>
// #include <vector>

// using namespace std;
// std::queue<std::string> incoming_commands_q;

// both temp until cpp is fixed
uint8_t tempMainBuffer[2048];
int offset = 0;

CHAL_UART_HandleTypeDef CHAL_UART2;
CHAL_DMA_handler CHAL_DMA2_Stream5;
CHAL_DMA_Stream_TypeDef stream5;

/**
 * @brief Initializes the UART and GPIO interface by calling the GPIO and UART init functions.
 *
 * This function initializes the GPIO pins for UART functionality and
 * enables RCC clock for USART.
 *
 * It calls the low-level initialization functions `ll_GPIO_UART_init`
 * and `ll_uart_init` to configure the hardware.
 *
 * @return void
 */
void CHAL_init_uart(void)
{
    ll_GPIO_UART_init();
    ll_uart_config(115200);
}

/**
 * @brief Initializes the GPIO for UART (Sets PA2 and PA3 for alternate functionality).
 *
 *
 * @note The baud rate is hardcoded to 115200 in this function.
 *       For different baud rates, modify the function accordingly.
 *
 * @return void
 */
CHAL_StatusTypeDef ll_GPIO_UART_init(void)
{
    // // could add a temp check to check if valid
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    // could add a temp check to check if valid
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOA->MODER |= (2 << 4); // 8.4.1 reference manual alternate function PA2
    GPIOA->MODER |= (2 << 6); // 8.4.1 reference manual alternate function PA3

    // adress offset 0x20
    GPIOA->AFR[0] |= (7 << 8);
    GPIOA->AFR[0] |= (7 << 12);

    return CHAL_OK;
}

/**
 * @brief Initializes the UART interface with the desired settings and sets IRQ settings of USART2 in the NVIC.
 *
 * @param BaudRate Desired baudrate of the uart interface
 *
 * @return uint8_t This function returns a status (0 = OK, 1 = ERROR)
 *
 * @todo add error handling
 * @todo check bit 12 of CR1 for start en stop bits
 * @todo check of BRR register is correct
 */
CHAL_StatusTypeDef ll_uart_config(uint32_t BaudRate)
{

    NVIC_InitTypeDef NVIC_InitStructure;

    USART2->CR1 = 0x00;
    USART2->CR1 |= (1 << 13);

    // Program the M bit in USART_CR1 to define the word length.
    USART2->CR1 &= ~(1 << 12);

    USART2->CR1 |= (1 << 4); // enable IDLE line detection in UART
    USART2->CR3 |= (1 << 6); // enable DMA Receiver

    // Not sure if the below works
    RCC_ClocksTypeDef clocks;
    RCC_ClocksTypeDef* clocks_ptr = &clocks;
    RCC_GetClocksFreq(clocks_ptr);
    USART2->BRR = clocks.PCLK1_Frequency / BaudRate;

    USART2->CR1 |= (1 << 2);
    USART2->CR1 |= (1 << 3);

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    return CHAL_OK;
}

/**
 * @brief waits until a char is received by the UART.
 *
 *
 * @note This is a blocking function and needs to be avoided
 *
 * @return char returns the received char
 *
 * @todo Create a non-blocking version
 */
uint8_t CHAL_UART2_get_char(void)
{
    uint8_t Temp;

    while (!(USART2->SR & (1 << 5)))
        ; // Wait for RXNE to SET.. This indicates that the data has been Received
    Temp = USART2->DR; // Read the data.
    return Temp;
}

/**
 * @brief waits until a char can be transmitted by the UART and sends a char if possible.
 *
 *
 * @note This is a blocking function and needs to be avoided
 *
 * @return void
 *
 * @todo Create a non-blocking version
 */
void CHAL_UART2_SendChar(char c)
{
    USART2->DR = c; // LOad the Data
    while (!(USART2->SR & (1 << 6)))
        ; // Wait for TC to SET.. This indicates that the data has been transmitted
}

/**
 * @brief Sends a string over UART in blocking mode
 *
 *
 * @note This is a blocking function and needs to be avoided
 *
 * @return void
 *
 * @todo Create a non-blocking version
 */
void CHAL_UART2_SendString(char* string)
{
    while (*string)
        CHAL_UART2_SendChar(*string++);
}

/**
 * @brief Disables a specified DMA stream.
 *
 * This function disables the given DMA stream by clearing the relevant
 * enable bit in the DMA control register. It ensures that the DMA stream
 * is properly disabled before any further configuration or usage.
 *
 * @note This function needs to be called if ANY of the registers of the DMA controller is set or cleared manually.
 *
 * @param stream A pointer to the DMA stream to be disabled.
 *               This should be a valid pointer to a DMA_Stream_TypeDef structure.
 *
 * @return void This function does not return a value.
 */
void CHAL_disable_DMA(DMA_Stream_TypeDef* stream)
{
    while ((stream->CR & 0x1) == 1)
        stream->CR &= ~(1 << 0);
}

/**
 * @brief Enables a specified DMA stream.
 *
 * This function enables the given DMA stream by setting the relevant
 * enable bit in the DMA control register. It ensures that the DMA stream
 * is properly disabled before any further configuration or usage.
 *
 *
 * @param stream A pointer to the DMA stream to be disabled.
 *               This should be a valid pointer to a DMA_Stream_TypeDef structure.
 *
 * @return void This function does not return a value.
 */
void CHAL_enable_DMA(DMA_Stream_TypeDef* stream)
{
    stream->CR |= (1 << 0);
}

/**
 * @brief Sets the requires bits of the DMA to enable DMA with UART in circular mode.
 *
 * @todo Add the settings in the doxygen
 *
 * @return uint8_t This function returns a status (0 = OK, 1 = ERROR)
 */
uint8_t CHAL_DMA_Init(void)
{

    // 1. Enable DMA1 Clock
    CHAL_init_DMA_timers();

    // if DMA is on, turn it off and wait till it is sure it is turned off
    CHAL_disable_DMA(DMA1_Stream5);

    DMA_DeInit(DMA1_Stream5);

    // stap 1 of stream configuration procedure
    // Clear bits 25-23, 21-19, 18-16, 15, 13-11, 10-8, 6, and 5
    DMA1_Stream5->CR &= ~(MASK_25_23 | MASK_21_19 | MASK_18_16 | MASK_15 | MASK_13_11 | MASK_10_8 | MASK_6 | MASK_5 | MASK_4_1);

    // Set bits 25-23, 21-19, 18-16, 15, 13-11, 10-8, 6, and 5 to desired values
    DMA1_Stream5->CR |= (4 << 25) | (3 << 23) | (3 << 21) | (0 << 19) | (3 << 16) | (1 << 15) | (1 << 10) | (1 << 8);

    // Enable DMA Interrupts
    DMA1_Stream5->CR |= CHAL_DMA_IT_TC; // | CHAL_DMA_IT_TE | CHAL_DMA_IT_DME;
    // DMA1_Stream5->CR |= CHAL_DMA_IT_HT;

    return CHAL_OK; // MBURST? 16 beats; same for PBURST;
}

/**
 * @brief Sets the requires bits of the DMA to enable DMA with UART in circular mode.
 *
 * @todo Add the settings in the doxygen
 *
 * @return uint8_t This function returns a status (0 = OK, 1 = ERROR)
 */
void CHAL_DMA_config(uint32_t srcAdd, uint32_t destAdd, uint16_t datasize)
{
    // 1. Set the data size in CNDTR Register
    DMA1_Stream5->NDTR = datasize;

    // 2. Set the  peripheral address in PAR Register
    DMA1_Stream5->PAR = srcAdd;

    // 3. Set the  Memory address in MAR Register
    DMA1_Stream5->M0AR = destAdd;

    // 4. Enable the DMA1
    DMA1_Stream5->CR |= 1 << 0;
}

/**
 * @brief Clears the status register of the DMA
 *
 * This is needed because the status register need to be empty in order to set the enable bit after the settings are written.
 *
 * @note DMA1 -> stream 5 -> HIFCR
 *
 * @return uint8_t This function returns a status (0 = OK, 1 = ERROR)
 */
uint8_t CHAL_clear_status_regs()
{
    DMA1->HIFCR |= 0xBEF0BEF;
    DMA1->LIFCR |= 0xBEF0BEF;

    while ((DMA1->LISR != 0x0) | (DMA1->HISR != 0x0))
    {
    }

    return CHAL_OK;
}

/**
 * @brief Sets DMA priority and enables DMA1 peripheral clock
 *
 * @return uint8_t This function returns a status (0 = OK, 1 = ERROR)
 */
uint8_t CHAL_init_DMA_timers()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    NVIC_SetPriority(DMA1_Stream5_IRQn, 5); // priority needs to be higherr than the DMA2 Stream 5 interupt to minimize the visual tears
    NVIC_EnableIRQ(DMA1_Stream5_IRQn);

    NVIC_SetPriority(DMA2_Stream5_IRQn, 0);
    NVIC_EnableIRQ(DMA2_Stream5_IRQn);

    return CHAL_OK;
}

/**
 * @brief Clears the idle line detection of the UART
 *
 * Clearing the IT flags in the SR register can be done by reading the SR and DR registers
 *
 * @return void This function does not return a value.
 */
void CHAL_clear_idledetect()
{
    volatile uint32_t tmpreg;
    tmpreg = USART2->SR;
    (void)tmpreg;
    tmpreg = USART2->DR;
    (void)tmpreg;
}

/**
 * @brief A polled function to move the data from the DMA receive buffer to a larger array.
 *
 * This should be replaced with CPP queue, but for now: The data is send to a larger array to form a queue that can be used by the following layers.
 * The DMA receive buffer can also just be larger, but it needs to be put into a queue in the future
 *
 * The function can handle one line at a time (until LF)
 *
 *
 * @return void This function does not return a value.
 */
void CHAL_event_call_back(uint8_t* rx_buff, uint16_t bufferlength)
{
    strcpy((char*)(tempMainBuffer) + offset, (char*)rx_buff); // copy uart/dma receive buffer into new (larger) buffer. Offset is used to prevent wrinting over previous data
    CHAL_disable_DMA(DMA1_Stream5); // to change the NDTR register the DMA NEEDS to be disabled first.
    offset += (bufferlength - DMA1_Stream5->NDTR) + 1;
    DMA1_Stream5->NDTR = bufferlength; // reset RX-buff pointer to start
    memset(rx_buff, 0, bufferlength); // reset rx_buff for new reception
    CHAL_enable_DMA(DMA1_Stream5); // restart the DMA for UART reception
}
