#include "stm32f4xx.h"

#ifndef DMA1_Stream5
#define DMA1_Stream5 0x40026088 // periphiral base + AP1 adress + DMA base + stream 5 base
#endif

#define GPIO_PIN_PA2 ((uint16_t)0x0004) /* Pin 2 selected    */
#define GPIO_PIN_PA3 ((uint16_t)0x0008) /* Pin 3 selected    */

#define CHAL_UART_DIV_SAMPLING16(_PCLK_, _BAUD_) ((uint32_t)((((uint64_t)(_PCLK_)) * 25U) / (4U * ((uint64_t)(_BAUD_)))))
#define CHAL_UART_DIVMANT_SAMPLING16(_PCLK_, _BAUD_) (CHAL_UART_DIV_SAMPLING16((_PCLK_), (_BAUD_)) / 100U)
#define CHAL_UART_DIVFRAQ_SAMPLING16(_PCLK_, _BAUD_) ((((CHAL_UART_DIV_SAMPLING16((_PCLK_), (_BAUD_)) - (CHAL_UART_DIVMANT_SAMPLING16((_PCLK_), (_BAUD_)) * 100U)) * 16U) + 50U) / 100U)
#define CHAL_UART_BRR_SAMPLING16(_PCLK_, _BAUD_) ((CHAL_UART_DIVMANT_SAMPLING16((_PCLK_), (_BAUD_)) << 4U) + (CHAL_UART_DIVFRAQ_SAMPLING16((_PCLK_), (_BAUD_)) & 0xF0U) + (CHAL_UART_DIVFRAQ_SAMPLING16((_PCLK_), (_BAUD_)) & 0x0FU))

#define __CHAL_DMA_DISABLE(__HANDLE__) ((__HANDLE__)->CR &= ~CHAL_DMA_SxCR_EN)
#define __CHAL_DMA_ENABLE(__HANDLE__) ((__HANDLE__)->CR |= CHAL_DMA_SxCR_EN)

#define CHAL_DMA_SxCR_CHSEL 0x0E000000 /*!< 0x0E000000 */
#define CHAL_DMA_SxCR_MBURST 0x01800000
#define CHAL_DMA_SxCR_PBURST 0x00600000
#define CHAL_DMA_SxCR_PL 0x00030000
#define CHAL_DMA_SxCR_MSIZE 0x00006000
#define CHAL_DMA_SxCR_PSIZE 0x00001800
#define CHAL_DMA_SxCR_MINC 0x00000400
#define CHAL_DMA_SxCR_PINC 0x00000200
#define CHAL_DMA_SxCR_CIRC 0x00000100
#define CHAL_DMA_SxCR_DIR 0x000000C0
#define CHAL_DMA_SxCR_CT 0x00080000
#define CHAL_DMA_SxCR_DBM 0x00040000
#define CHAL_DMA_SxCR_EN 0x00000001

#define CHAL_DMA_CHANNEL_4 0x08000000U
#define CHAL_DMA_PERIPH_TO_MEMORY 0x00000000U
#define CHAL_DMA_PINC_DISABLE 0x00000000U
#define CHAL_DMA_MINC_DISABLE 0x00000400U
#define CHAL_DMA_PDATAALIGN_BYTE 0x00000000U
#define CHAL_DMA_MDATAALIGN_BYTE 0x00000000U
#define CHAL_DMA_CIRCULAR 0x00000100
#define CHAL_DMA_PRIORITY 0x00010000

#define CHAL_DMA_SxFCR_FTH 0x00000003
#define CHAL_DMA_SxFCR_DMDIS 0x00000004

#define CHAL_DMA_FIFOMODE_DISABLE 0x00000000U

#define CHAL_DMA_IT_TC 0x00000010
#define CHAL_DMA_IT_TE 0x00000004
#define CHAL_DMA_IT_DME 0x00000002
#define CHAL_DMA_IT_HT 0x00000008

static const uint32_t USART2_BASE_ADDRESS = 0x40004400UL;
static const uint32_t USART3_BASE_ADDRESS = 0x40004800UL;

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

CHAL_StatusTypeDef CHAL_init_reg_uart(CHAL_UART_HandleTypeDef* uart);
void CHAL_init_uart(void);

uint8_t CHAL_UART2_get_char(void);
CHAL_StatusTypeDef ll_uart_init(uint32_t BaudRate);
CHAL_StatusTypeDef ll_GPIO_UART_init(void);
uint8_t CHAL_NVIC_EnableIRQ(IRQn_Type IRQn);
uint8_t CHAL_set_priority_NVIC(IRQn_Type IRQn, uint32_t PreemptPriority, uint32_t SubPriority);
uint32_t CHAL_DMA_CalcBaseAndBitshift(CHAL_DMA_handler* dma);
void CHAL_DMA_SetConfig(CHAL_DMA_handler* dma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);
CHAL_StatusTypeDef CHAL_DMA_Start_IT(CHAL_DMA_handler* dma, uint32_t SrcAddress, uint32_t DstAddress, uint32_t DataLength);