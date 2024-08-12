#include  <rs485_drc.h>
#include <string.h>
/**
* @breif   初始化RS485收发器为接收模式
* @param   无
* @retval  无
* @note    在调用此函数之前，确保已经初始化通信串口和控制GPIO
*/

uint8_t _485rx_buffer[_485RX_BUFFER_SIZE]; //接收缓冲区
uint16_t rx_index; // 接收缓冲区索引
uint8_t received_data[_485RX_BUFFER_SIZE]; //接收数据
uint8_t _485tx_buffer[_485TX_BUFFER_SIZE]; //发送缓冲区
uint8_t _485txDataSize = 0;
/*
* @breif   RS485初始化
* @param  
        - _485rx_buffer 接收缓冲区
        - rx_index 接收缓冲区索引
* @retval  无
* @note    初始化索引为0 开启中断接收 485发送使能
*/
void RS485_Init()
{
    rx_index = 0 ;
    HAL_UART_Receive_IT(&RS485_USART, &_485rx_buffer[rx_index], 1);  // 开始接收第一个字节
    RS485_RX_ENABLE();
}

/**
* @breif   RS485发送数据
* @param  
        - data 要发送的数据
        - len 要发送的数据长度
* @retval  无
* @note    发送数据前先使能485发送
*/
int RS485_Transmit(uint8_t *data, uint16_t len)
{
    HAL_StatusTypeDef status;
    
    RS485_TX_ENABLE();
    
    status = HAL_UART_Transmit(&RS485_USART, data, len, HAL_MAX_DELAY);
    
    HAL_Delay(1);
    
    RS485_RX_ENABLE();
    
    return status == HAL_OK ? 0 : -1;
}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart == &RS485_USART) {
        // 如果缓冲区未满
        if (rx_index < _485RX_BUFFER_SIZE) {
            // 增加接收索引
            _485rx_buffer[rx_index++] = (uint8_t)huart->Instance->DR;

            // 继续接收下一个字节
            HAL_UART_Receive_IT(&RS485_USART, &_485rx_buffer[rx_index], 1);
        }
    }
}

uint16_t RS485_ReceiveData(uint8_t *data, uint16_t length) {
    if (length == 0) {
        length = rx_index;  // 如果没有自定义长度，则自动计算接收到的数据长度
    }

    // 确保请求的数据长度不超过接收数据的实际长度
    if (length > rx_index) {
        length = rx_index;
    }

    // 将接收到的数据复制到用户提供的缓冲区
    for (uint16_t i = 0; i < length; i++) {
        data[i] = _485rx_buffer[i];
    }

    // 记录实际接收到的数据长度
    uint16_t received_length = length;

    // 重置接收索引
    rx_index = 0;

    return received_length;  // 返回接收到的数据长度
}


