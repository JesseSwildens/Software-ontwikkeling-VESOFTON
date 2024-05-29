#include "ASM_CHAL.h"
#include "misc.h"
#include "stm32_ub_vga_screen.h"
#include "stm32f4xx.h"
#include "stm32f4xx_dma.h"
#include "stm32f4xx_rcc.h"

#ifndef DMA1_Stream5
#define DMA1_Stream5 0x40026088 //  peripheral + AHB1 address + DMA1 base + stream 5 base
#endif

extern uint8_t in_inactive_region_flag;
extern VGA_t VGA;

#define TIMING_PADDING 10

#define DMA_Stream0_IT_MASK (uint32_t)(DMA_LISR_FEIF0 | DMA_LISR_DMEIF0 | DMA_LISR_TEIF0 | DMA_LISR_HTIF0 | DMA_LISR_TCIF0)
#define DMA_Stream1_IT_MASK (uint32_t)(DMA_Stream0_IT_MASK << 6)
#define DMA_Stream5_IT_MASK (uint32_t)(DMA_Stream1_IT_MASK | (uint32_t)0x20000000)

#define ARRAY_LEN(x) (sizeof(x) / sizeof((x)[0]))

#define CHAL_DMA_IT_TC 0x00000010
#define CHAL_DMA_IT_TE 0x00000004
#define CHAL_DMA_IT_DME 0x00000002
#define CHAL_DMA_IT_HT 0x00000008
#define USART2_BASE_ADDRESS 0x40004400UL
#define USART3_BASE_ADDRESS 0x40004800UL

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        volatile uint32_t CR; /*!< DMA stream x configuration register      */
        volatile uint32_t NDTR; /*!< DMA stream x number of data register     */
        volatile uint32_t PAR; /*!< DMA stream x peripheral address register */
        volatile uint32_t M0AR; /*!< DMA stream x memory 0 address register   */
        volatile uint32_t M1AR; /*!< DMA stream x memory 1 address register   */
        volatile uint32_t FCR; /*!< DMA stream x FIFO control register       */
    } CHAL_DMA_Stream_TypeDef;

    typedef struct
    {
        CHAL_DMA_Stream_TypeDef* Instance;
        uint32_t StreamIndex;
        uint32_t StreamBaseAddress;
        uint32_t Direction;
        uint8_t lock;
    } CHAL_DMA_handler;

    // Define the UART initialization struct
    typedef struct
    {
        uint32_t BaudRate;
        uint32_t WordLength;
        uint32_t StopBits;
        uint32_t Parity;
        uint32_t Mode;
        uint32_t HwFlowCtl;
        uint32_t OverSampling;
    } CHAL_UART_InitTypeDef;

    typedef struct
    {
        USART_TypeDef* Instance;

        CHAL_UART_InitTypeDef init;

    } CHAL_UART_HandleTypeDef;

    typedef enum
    {
        CHAL_OK = 0x00U,
        CHAL_ERROR = 0x01U,
        CHAL_BUSY = 0x02U,
        CHAL_TIMEOUT = 0x03U
    } CHAL_StatusTypeDef;

    typedef struct
    {
        uint32_t Pin;
        uint32_t Mode;
        uint32_t Pull;
        uint32_t Speed;
        uint32_t Alternate;
    } CHAL_GPIO_InitTypeDef;

    typedef struct
    {
        volatile uint32_t ISR; /*!< DMA interrupt status register */
        volatile uint32_t Reserved0;
        volatile uint32_t IFCR; /*!< DMA interrupt flag clear register */
    } CHAL_DMA_Base_Registers;

    void CHAL_init_uart(void);
    CHAL_StatusTypeDef CHAL_GPIO_UART_init(void);
    CHAL_StatusTypeDef CHAL_uart_config(uint32_t BaudRate);
    uint8_t CHAL_UART2_get_char(void);
    void CHAL_UART2_SendChar(char c);
    void CHAL_UART2_SendString(char* string, uint16_t length);
    void CHAL_disable_DMA(DMA_Stream_TypeDef* stream);
    void CHAL_enable_DMA(DMA_Stream_TypeDef* stream);
    uint8_t CHAL_DMA_Init(void);
    void CHAL_DMA_config(uint32_t srcAdd, uint32_t destAdd, uint16_t datasize);
    uint8_t CHAL_clear_status_regs();
    uint8_t CHAL_init_DMA_timers();
    void CHAL_clear_idledetect();
    void CHAL_event_call_back(uint8_t* rx_buff, uint16_t bufferlength);
    void CHAL_push_to_q(uint8_t* rx_buff, uint16_t bufferlength);
#ifdef __cplusplus
}
#endif
