
/**
 * @file CHAL.cpp
 * @brief UART and DMA initialization functions.
 */

#include "CHAL.h"
#include "string.h"
#include <cstdio>
#include <queue>
#include <string>
#include <vector>

// #define ECHO_INCOMMING

using namespace std;
std::queue<std::string> incoming_commands_q;

// both temp for video streaming
uint8_t tempMainBuffer[2048];
int offset = 0;

/**
 * @brief Initializes the UART and GPIO interface by calling the GPIO and UART init functions.
 *
 * This function initializes the GPIO pins for UART functionality and
 * enables RCC clock for USART.
 *
 * It calls the low-level initialization functions `CHAL_GPIO_UART_init`
 * and `CHAL_uart_init` to configure the hardware.
 *
 * @return void
 */
void CHAL_init_uart(void)
{
    CHAL_GPIO_UART_init();
    CHAL_uart_config(115200);
}

/**
 * @brief Initializes the GPIO for UART (Sets P2 and P3 for alternate functionality).
 *
 * @return CHAL_StatusTypeDef returns 0
 */
CHAL_StatusTypeDef CHAL_GPIO_UART_init(void)
{
    // Enables Peripheral
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    // Enables Peripheral
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
CHAL_StatusTypeDef CHAL_uart_config(uint32_t BaudRate)
{

    NVIC_InitTypeDef NVIC_InitStructure;

    // reset Control register
    USART2->CR1 = 0x00;

    // usart enable
    USART2->CR1 |= (1 << 13);

    // Program the M bit in USART_CR1 to define the word length.
    USART2->CR1 &= ~(1 << 12);

    USART2->CR1 |= (1 << 4); // enable IDLE line detection in UART
    USART2->CR3 |= (1 << 6); // enable DMA Receiver

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
    while (!(USART2->SR & (1 << 5)))
    {
    }
    // Wait for RXNE to SET.. This indicates that the data has been Received
    return USART2->DR;
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
    USART2->DR = c; // Load the Data
    while (!(USART2->SR & (1 << 6)))
    {
    } // Wait for TC to SET.. This indicates that the data has been transmitted
}

/**
 * @brief Sends a string over UART in blocking mode
 *
 *
 * @note This is a blocking function and needs to be avoided
 *
 * @param string
 * @param length length of string
 *
 * @return void
 *
 * @todo Create a non-blocking version
 */
void CHAL_UART2_SendString(char* string, uint16_t length)
{
    // Check for null pointer
    if (string == NULL)
    {
        return;
    }

    // Send each character up to the specified length
    for (uint16_t i = 0; i < length; ++i)
    {
        CHAL_UART2_SendChar(string[i]);
    }
}

void CHAL_UART2_SendString(std::string str)
{
    CHAL_UART2_SendString((char*)str.c_str(), str.size());
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
    /*
     *  Bit 25-27: channel select (channel 4)
     *  Bit 23-24: Memory burst transfer configuration (16 beats)
     *  Bit 21-22: Peripheral burst transfer configuration (16 beats)
     *  Bit 20: Reserved
     *  Bit 19: current target (M0AR)
     *  Bit 18: DBM (no switching)
     *  Bit 17-16: Priority level (Very high)
     *  Bit 15: Peripheral inc (fixed)
     *  Bit 14-13: Memory data size (Byte)
     *  Bit 11-12: Peripheral data size (Byte)
     *  Bit 10: Memory increment mode (increment enabled)
     *  Bit 9: Peripheral increment mode (increment disabled)
     *  Bit 8: Circular buffer (enabled)
     *  Bit 7-6: data transfer direction (Peripheral-to-memory)
     *  Bit 5: Flow controller (DMA)
     *  Bit 4-1: interrupts (all off) DMA is triggered by UART
     */

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

    // 2. Set the peripheral address in PAR Register
    DMA1_Stream5->PAR = srcAdd;

    // 3. Set the Memory address in MAR Register
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
    tmpreg = USART2->DR; // data register;
    (void)tmpreg;
}

/**
 * @brief A new polled function to move the data from the DMA receive buffer to a queue.
 *
 * The data is send to a larger array to form a queue that can be used by the following layers.
 *
 * The function can handle one line at a time (until LF)
 *
 * @return void This function does not return a value.
 */
void CHAL_push_to_q(uint8_t* rx_buff, uint16_t bufferlength)
{
    string s = string((char*)rx_buff);
    size_t line_feed_pos = s.find_first_of('\n');
    size_t old_line_feed_pos = 0;
    while (old_line_feed_pos != std::string::npos)
    {
        line_feed_pos = s.find_first_of('\n', old_line_feed_pos + 1);
        std::string currentString = s.substr(old_line_feed_pos, line_feed_pos - old_line_feed_pos);
        incoming_commands_q.push(currentString);
        old_line_feed_pos = line_feed_pos + 1;
        if (line_feed_pos == std::string::npos)
            break;
    }

#ifdef ECHO_INCOMMING
    const std::string& back_command = incoming_commands_q.back();
    const char* back_command_cstr = back_command.c_str();
    CHAL_UART2_SendString((char*)back_command_cstr);
#endif
    CHAL_disable_DMA(DMA1_Stream5); // to change the NDTR register the DMA NEEDS to be disabled first.
    DMA1_Stream5->NDTR = bufferlength; // reset RX-buff pointer to start
    memset(rx_buff, 0, bufferlength); // reset rx_buff for new reception
    CHAL_enable_DMA(DMA1_Stream5); // restart the DMA for UART reception
}
