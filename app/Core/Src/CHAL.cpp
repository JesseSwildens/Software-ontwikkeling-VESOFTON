#include "CHAL.h"
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

void CHAL_init_uart(void)
{
    ll_GPIO_UART_init();
    ll_uart_init(115200);
    // if ( != CHAL_OK)
    //     __NOP(); // add error handler?
}

CHAL_StatusTypeDef ll_GPIO_UART_init(void)
{

    // SET_BIT(RCC->AHB1ENR, RCC_APB1ENR_USART2EN);
    // // could add a temp check to check if valid
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
    // SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
    // could add a temp check to check if valid
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    // SET_BIT(GPIOA->MODER, 0x20); // 8.4.1 reference manual alternate function PA2
    // SET_BIT(GPIOA->MODER, 0x80); // 8.4.1 reference manual alternate function PA3
    GPIOA->MODER |= (2 << 4);
    GPIOA->MODER |= (2 << 6);

    // adress offset 0x20
    // SET_BIT(GPIOA->AFR[0], 0x0000700); //(7 << 8)
    // SET_BIT(GPIOA->AFR[0], 0x0007000); //(7 << 12)
    GPIOA->AFR[0] |= (7 << 8);
    GPIOA->AFR[0] |= (7 << 12);

    return CHAL_OK;
}

/*todo:
1. check bit 12 of CR1 for start en stop bits
2. check of BRR register is correct
*/
CHAL_StatusTypeDef ll_uart_init(uint32_t BaudRate)
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
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    return CHAL_OK;
}

// a very dangerous and stupid (but working) way of receiving
uint8_t CHAL_UART2_get_char(void)
{
    uint8_t Temp;

    while (!(USART2->SR & (1 << 5)))
        ; // Wait for RXNE to SET.. This indicates that the data has been Received
    Temp = USART2->DR; // Read the data.
    return Temp;
}

void CHAL_UART2_SendChar(char c)
{
    USART2->DR = c; // LOad the Data
    while (!(USART2->SR & (1 << 6)))
        ; // Wait for TC to SET.. This indicates that the data has been transmitted
}

void CHAL_UART2_SendString(char* string)
{
    while (*string)
        CHAL_UART2_SendChar(*string++);
}

void CHAL_disable_DMA(DMA_Stream_TypeDef* stream)
{
    while ((stream->CR & 0x1) == 1)
        stream->CR &= ~(1 << 0);
}

void CHAL_enable_DMA(DMA_Stream_TypeDef* stream)
{
    stream->CR |= (1 << 0);
}

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

uint8_t CHAL_clear_status_regs()
{
    DMA1->HIFCR |= 0xBEF0BEF;
    DMA1->LIFCR |= 0xBEF0BEF;

    while ((DMA1->LISR != 0x0) | (DMA1->HISR != 0x0))
    {
    }

    return CHAL_OK;
}

uint8_t CHAL_init_DMA_timers()
{

    // maybe disable the DMA first
    // RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    // RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    NVIC_SetPriority(DMA1_Stream5_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Stream5_IRQn);

    NVIC_SetPriority(DMA2_Stream5_IRQn, 0);
    NVIC_EnableIRQ(DMA2_Stream5_IRQn);

    return CHAL_OK;
}

// register CR2 has an iterupt for linebreak detection... maybe that is useable

void CHAL_clear_idledetect()
{
    volatile uint32_t tmpreg;
    tmpreg = USART2->SR;
    (void)tmpreg;
    tmpreg = USART2->DR;
    (void)tmpreg;
}

void CHAL_event_call_back(uint8_t* rx_buff, uint16_t bufferlength, uint8_t* flag)
{
    *flag = 0;
    for (uint16_t i = 0; i < bufferlength; i++)
    {
        if (rx_buff[i] == '\n')
        {
            strcpy((char*)(tempMainBuffer) + offset, (char*)rx_buff);
#ifdef ECHO_RMESSAGES
            std::cout
                << incoming_commands_q.back() << std::endl;
#endif
            CHAL_disable_DMA(DMA1_Stream5);
            DMA1_Stream5->NDTR = bufferlength;
            memset(rx_buff, 0, bufferlength);
            offset += i + 1;
            CHAL_enable_DMA(DMA1_Stream5);
            break;
        }

        if (tempMainBuffer[256] != 0)
        {
            i++;
        }
    }
}

// DMA_CalcBaseAndBitshift

// set CR register of the DMA
uint8_t CHAL_DMA_init(CHAL_DMA_handler* dma, CHAL_DMA_Stream_TypeDef* stream, uint32_t Direction) // CHAL_DMA_PERIPH_TO_MEMORY for stream 5
{
    CHAL_DMA_Base_Registers* regs;
    uint32_t tmp = 0U;

    dma->Instance = stream;
    dma->Direction = Direction;

    __CHAL_DMA_DISABLE(dma->Instance);

    tmp = dma->Instance->CR;

    tmp &= ((uint32_t) ~(CHAL_DMA_SxCR_CHSEL | CHAL_DMA_SxCR_MBURST | CHAL_DMA_SxCR_PBURST | CHAL_DMA_SxCR_PL | CHAL_DMA_SxCR_MSIZE | CHAL_DMA_SxCR_PSIZE | CHAL_DMA_SxCR_MINC | CHAL_DMA_SxCR_PINC | CHAL_DMA_SxCR_CIRC | CHAL_DMA_SxCR_DIR | CHAL_DMA_SxCR_CT | CHAL_DMA_SxCR_DBM));

    tmp |= CHAL_DMA_CHANNEL_4 | dma->Direction | CHAL_DMA_PINC_DISABLE | CHAL_DMA_MINC_DISABLE | CHAL_DMA_PDATAALIGN_BYTE | CHAL_DMA_MDATAALIGN_BYTE | CHAL_DMA_CIRCULAR | CHAL_DMA_PRIORITY;
    dma->Instance->CR = tmp;

    /* Get the FCR register value */
    tmp = dma->Instance->FCR;
    /* Clear Direct mode and FIFO threshold bits */
    tmp &= (uint32_t) ~(DMA_SxFCR_DMDIS | DMA_SxFCR_FTH);
    /* Prepare the DMA Stream FIFO configuration */
    tmp |= CHAL_DMA_FIFOMODE_DISABLE;
    dma->Instance->FCR = tmp;

    regs = (CHAL_DMA_Base_Registers*)CHAL_DMA_CalcBaseAndBitshift(&CHAL_DMA2_Stream5);

    regs->IFCR = 0x3FU << dma->StreamIndex;

    return CHAL_OK;
}

CHAL_StatusTypeDef CHAL_DMA_Start_IT(CHAL_DMA_handler* dma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
    if (dma->lock == true)
        return CHAL_BUSY;

    dma->lock = true;
    CHAL_DMA_Base_Registers* regs = (CHAL_DMA_Base_Registers*)dma->StreamBaseAddress;
    CHAL_DMA_SetConfig(dma, SrcAddress, DstAddress, DataLength);
    regs->IFCR = 0x3FU << dma->StreamIndex;
    dma->Instance->CR |= CHAL_DMA_IT_TC | CHAL_DMA_IT_TE | CHAL_DMA_IT_DME;
    dma->Instance->CR |= CHAL_DMA_IT_HT;
    __CHAL_DMA_ENABLE(dma->Instance);

    return CHAL_OK;
}

void CHAL_DMA_SetConfig(CHAL_DMA_handler* dma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength)
{
    /* Clear DBM bit */
    dma->Instance->CR &= (uint32_t)(~CHAL_DMA_SxCR_DBM);

    /* Configure DMA Stream data length */
    dma->Instance->NDTR = DataLength;

    /* Memory to Peripheral */
    if ((dma->Direction) == CHAL_DMA_PERIPH_TO_MEMORY)
    {
        /* Configure DMA Stream destination address */
        dma->Instance->PAR = DstAddress;

        /* Configure DMA Stream source address */
        dma->Instance->M0AR = SrcAddress;
    }
    /* Peripheral to Memory */
    else
    {
        /* Configure DMA Stream source address */
        dma->Instance->PAR = SrcAddress;

        /* Configure DMA Stream destination address */
        dma->Instance->M0AR = DstAddress;
    }
}

uint32_t CHAL_DMA_CalcBaseAndBitshift(CHAL_DMA_handler* dma)
{
    uint32_t stream_number = (((uint32_t)dma->Instance & 0xFFU) - 16U) / 24U;

    /* lookup table for necessary bitshift of flags within status registers */
    static const uint8_t flagBitshiftOffset[8U] = { 0U, 6U, 16U, 22U, 0U, 6U, 16U, 22U };
    dma->StreamIndex = flagBitshiftOffset[stream_number]; // i think this can just be 6U

    if (stream_number > 3U)
    {
        /* return pointer to HISR and HIFCR */
        dma->StreamBaseAddress = (((uint32_t)dma->Instance & (uint32_t)(~0x3FFU)) + 4U);
    }
    else
    {
        /* return pointer to LISR and LIFCR */
        dma->StreamBaseAddress = ((uint32_t)dma->Instance & (uint32_t)(~0x3FFU));
    }

    return dma->StreamBaseAddress;
}