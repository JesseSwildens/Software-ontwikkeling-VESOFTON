#include "CHAL.h"

CHAL_UART_HandleTypeDef CHAL_UART2;
CHAL_DMA_handler CHAL_DMA2_Stream5;
CHAL_DMA_Stream_TypeDef stream5;

void CHAL_init_uart(void)
{
    ll_GPIO_UART_init();
    if (ll_uart_init(115200) != CHAL_OK)
        __NOP(); // add error handler?
}

CHAL_StatusTypeDef ll_GPIO_UART_init(void)
{

    SET_BIT(RCC->AHB1ENR, RCC_APB1ENR_USART2EN);
    // could add a temp check to check if valid

    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);
    // could add a temp check to check if valid

    SET_BIT(GPIOA->MODER, 0x20); // 8.4.1 reference manual alternate function PA2
    SET_BIT(GPIOA->MODER, 0x80); // 8.4.1 reference manual alternate function PA3

    // adress offset 0x20
    SET_BIT(GPIOA->AFR[0], 0x0000700); //(7 << 8)
    SET_BIT(GPIOA->AFR[0], 0x0007000); //(7 << 12)

    return CHAL_OK;
}

/*todo:
1. check bit 12 of CR1 for start en stop bits
2. check of BRR register is correct
*/
CHAL_StatusTypeDef ll_uart_init(uint32_t BaudRate)
{
    SET_BIT(USART2->CR1, 0x00000); // clear all bits
    SET_BIT(USART2->CR1, 0x2000); //(1<<13)

#ifndef APBPrescTable
    const uint8_t APBPrescTable[8] = { 0, 0, 0, 0, 1, 2, 3, 4 };
#endif
    // Not sure if the below works
    uint32_t pclk = (SystemCoreClock >> APBPrescTable[(RCC->CFGR & RCC_CFGR_PPRE1) >> 10U]); // RCC_CFGR_PPRE1_Pos
    uint32_t tempreg = CHAL_UART_BRR_SAMPLING16(pclk, BaudRate); // check the 16 times oversampling
    USART2->BRR = tempreg;

    SET_BIT(USART2->CR1, 0x4); // enable RE
    SET_BIT(USART2->CR1, 0x8); // enable TE

    return CHAL_OK;
}

// a very dangerous and stupid (but working) way of receiving
uint8_t CHAL_UART2_get_char(void)
{
    uint8_t Temp;

    while (!(USART2->SR & (1 << 5)))
    {
    }
    Temp = USART2->DR; // Read the data.
    return Temp;
}

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

uint8_t CHAL_init_DMA_timers()
{

    // maybe disable the DMA first
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA1EN);
    SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN);

    CHAL_set_priority_NVIC(DMA1_Stream5_IRQn, 0, 0);
    NVIC_EnableIRQ(DMA1_Stream5_IRQn);

    CHAL_set_priority_NVIC(DMA2_Stream5_IRQn, 0, 0);
    NVIC_EnableIRQ(DMA2_Stream5_IRQn);

    return CHAL_OK;
}

uint8_t CHAL_set_priority_NVIC(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority)
{
    uint32_t prioritygroup = 0x00U;
    prioritygroup = NVIC_GetPriorityGrouping();
    NVIC_SetPriority(IRQn, NVIC_EncodePriority(prioritygroup, PreemptPriority, SubPriority));

    return CHAL_OK;
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

void DMA1_Stream5_IRQHandler(void)
{
    CHAL_DMA2_Stream5.lock = false;
}

// DMA_CalcBaseAndBitshift