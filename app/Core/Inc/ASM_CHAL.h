#ifndef ASM_CHAL_H
#define ASM_CHAL_H

typedef enum
{
    ASM_CHAL_OK = 0,
    ASM_CHAL_ERROR
} ASMCHAL_StatusTypeDef;

extern ASMCHAL_StatusTypeDef ll_GPIO_UART_init_ASM(void);
extern char ASMCHAL_event_call_back(unsigned char* rx_buff, unsigned int bufferlength);

// if not work add "C" after extern

#endif // ASM_CHAL_H