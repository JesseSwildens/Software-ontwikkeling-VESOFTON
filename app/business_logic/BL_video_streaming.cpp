#include "BL_video_streaming.h"
#include "BL_callbacks.h"
#include "CHAL.h"

int BL_hyperdrive()
{
    log_message("hyperdrive works");
    CHAL_uart_config(115200);
    return 0;
}