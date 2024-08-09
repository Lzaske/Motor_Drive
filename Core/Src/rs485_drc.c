#include  <rs485_drc.h>
#include <string.h>
/**
* @breif   初始化RS485收发器为接收模式
* @param   无
* @retval  无
* @note    在调用此函数之前，确保已经初始化通信串口和控制GPIO
*/

uint8_t _485rx_buffer[_485RX_BUFFER_SIZE];
uint16_t rx_index;
uint8_t rx_data[rx_data_size];
uint8_t received_data[_485RX_BUFFER_SIZE];
/**
* @breif   控制RS485收发器发送数据
* @param   data 要发送的数据
* @param   len  要发送数据的长度
* @retval  0 - 发送成功，-1 = 发送失败
*/


/** 初始化 **/
//void RS485_Init()
//{
//    
//    /* 使能串口接收中断 */
//    HAL_UART_Receive_IT(&RS485_USART, _485rx_buffer, _485RX_BUFFER_SIZE);
//   // HAL_UART_Receive_IT(&RS485_USART, rx_data, rx_data_size);
//    
//    RS485_RX_ENABLE();
//}

/**
 * @brief   串口中断回调函数
 * @note    此函数属于弱定义函数的重新实现，HAL库会自动调用
*/
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//    /* 判断是哪个串口触发的中断 */
//    if(huart ->Instance == UART4)
//    {

//        // 清空之前的缓冲区
//        memset(_485rx_buffer, 0, _485RX_BUFFER_SIZE);
//        rx_index = 0;
//        // 将接收到的数据存储到缓冲区
//        for (int i = 0; i < 5; i++) {
//            _485rx_buffer[rx_index++] = rx_data[i];
//            if (rx_index >= _485RX_BUFFER_SIZE) {
//                rx_index = 0; // 防止缓冲区溢出
//            }
//        }
//                
//        for (int j = 0 ; j < 5 ;j++)
//        {
//        //将接收到的数据打印
//        printf("RX Data:0x%02x\r\n", rx_data[j]);
//        }
//        //重新使能串口接收中断
//        HAL_UART_Receive_IT(&RS485_USART, rx_data, rx_data_size);
//    }
//}



//void RS485_ReceiveData(uint8_t *data, uint16_t length) {
//    //memset(_485rx_buffer, 0, _485RX_BUFFER_SIZE);
//    if (length == 0) {
//        length = rx_index;  // 如果没有自定义长度，则自动计算接收到的数据长度
//    }

//    for (uint16_t i = 0; i < length; i++) {
//        data[i] = _485rx_buffer[i];
//    }

//    // 打印接收到的数据
//    for (uint16_t i = 0; i < length; i++) {
//        printf("%02X ", data[i]);
//    }
//    printf("\n");

//    // 重置接收索引
//    rx_index = 0;

//    // 重新启动中断接收
//    HAL_UART_Receive_IT(&RS485_USART, _485rx_buffer, _485RX_BUFFER_SIZE);
//}
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//    if (huart == &RS485_USART) {
//        // 增加接收索引
//        if (rx_index < _485RX_BUFFER_SIZE) {
//            rx_index++;
//        }

//        // 继续接收下一个字节
//        HAL_UART_Receive_IT(&RS485_USART, &_485rx_buffer[rx_index], 1);
//    }
//}
void RS485_Init()
{
    rx_index = 0 ;
    HAL_UART_Receive_IT(&RS485_USART, &_485rx_buffer[rx_index], 1);  // 开始接收第一个字节
    RS485_RX_ENABLE();
}

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


