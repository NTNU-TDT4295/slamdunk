void setup_uart();
void set_recv_callback(void (*f)(char));
void test_callback();

void uartSetup(void);
void cmuSetup(void);
void uartPutData(uint8_t uart_channel, uint8_t * dataPtr, uint32_t dataLen);
uint32_t uartGetData(uint8_t * dataPtr, uint32_t dataLen);
void uartPutChar(uint8_t uart_channel, uint8_t ch);
uint8_t uartGetChar(void);
