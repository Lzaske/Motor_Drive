#ifndef _RS485_DRV_H_
#define _RS485_DRV_H_

#include "usart.h"


/* User Config */
#define RS485_USART   huart1
#define RS485_EN_GPIO_Port GPIOD
#define RS485_EN_Pin       GPIO_PIN_15
#define _485RX_BUFFER_SIZE 256
#define _485TX_BUFFER_SIZE 256
extern uint8_t _485rx_buffer[_485RX_BUFFER_SIZE];
extern uint16_t rx_index;
extern uint8_t received_data[_485RX_BUFFER_SIZE];
extern uint8_t _485tx_buffer[_485TX_BUFFER_SIZE];
extern uint8_t _485tx_data[_485TX_BUFFER_SIZE];
/* RS485 TX/RX Control */
#define RS485_TX_ENABLE()   HAL_Delay(1);\
                            HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_SET);\
                            HAL_Delay(1);

#define RS485_RX_ENABLE()   HAL_Delay(1);\
                            HAL_GPIO_WritePin(RS485_EN_GPIO_Port, RS485_EN_Pin, GPIO_PIN_RESET);\
                            HAL_Delay(1);

void RS485_Init(void);
int RS485_Transmit(uint8_t *data, uint16_t len);
int RS485_Receive(uint8_t *data, uint16_t len);
uint16_t RS485_ReceiveData(uint8_t *data, uint16_t length);

#endif /* _RS485_DRV_H_ */
