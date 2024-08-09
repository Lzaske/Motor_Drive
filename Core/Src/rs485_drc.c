#include  <rs485_drc.h>
#include <string.h>
/**
* @breif   ��ʼ��RS485�շ���Ϊ����ģʽ
* @param   ��
* @retval  ��
* @note    �ڵ��ô˺���֮ǰ��ȷ���Ѿ���ʼ��ͨ�Ŵ��ںͿ���GPIO
*/

uint8_t _485rx_buffer[_485RX_BUFFER_SIZE];
uint16_t rx_index;
uint8_t rx_data[rx_data_size];
uint8_t received_data[_485RX_BUFFER_SIZE];
/**
* @breif   ����RS485�շ�����������
* @param   data Ҫ���͵�����
* @param   len  Ҫ�������ݵĳ���
* @retval  0 - ���ͳɹ���-1 = ����ʧ��
*/


/** ��ʼ�� **/
//void RS485_Init()
//{
//    
//    /* ʹ�ܴ��ڽ����ж� */
//    HAL_UART_Receive_IT(&RS485_USART, _485rx_buffer, _485RX_BUFFER_SIZE);
//   // HAL_UART_Receive_IT(&RS485_USART, rx_data, rx_data_size);
//    
//    RS485_RX_ENABLE();
//}

/**
 * @brief   �����жϻص�����
 * @note    �˺������������庯��������ʵ�֣�HAL����Զ�����
*/
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
//{
//    /* �ж����ĸ����ڴ������ж� */
//    if(huart ->Instance == UART4)
//    {

//        // ���֮ǰ�Ļ�����
//        memset(_485rx_buffer, 0, _485RX_BUFFER_SIZE);
//        rx_index = 0;
//        // �����յ������ݴ洢��������
//        for (int i = 0; i < 5; i++) {
//            _485rx_buffer[rx_index++] = rx_data[i];
//            if (rx_index >= _485RX_BUFFER_SIZE) {
//                rx_index = 0; // ��ֹ���������
//            }
//        }
//                
//        for (int j = 0 ; j < 5 ;j++)
//        {
//        //�����յ������ݴ�ӡ
//        printf("RX Data:0x%02x\r\n", rx_data[j]);
//        }
//        //����ʹ�ܴ��ڽ����ж�
//        HAL_UART_Receive_IT(&RS485_USART, rx_data, rx_data_size);
//    }
//}



//void RS485_ReceiveData(uint8_t *data, uint16_t length) {
//    //memset(_485rx_buffer, 0, _485RX_BUFFER_SIZE);
//    if (length == 0) {
//        length = rx_index;  // ���û���Զ��峤�ȣ����Զ�������յ������ݳ���
//    }

//    for (uint16_t i = 0; i < length; i++) {
//        data[i] = _485rx_buffer[i];
//    }

//    // ��ӡ���յ�������
//    for (uint16_t i = 0; i < length; i++) {
//        printf("%02X ", data[i]);
//    }
//    printf("\n");

//    // ���ý�������
//    rx_index = 0;

//    // ���������жϽ���
//    HAL_UART_Receive_IT(&RS485_USART, _485rx_buffer, _485RX_BUFFER_SIZE);
//}
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
//    if (huart == &RS485_USART) {
//        // ���ӽ�������
//        if (rx_index < _485RX_BUFFER_SIZE) {
//            rx_index++;
//        }

//        // ����������һ���ֽ�
//        HAL_UART_Receive_IT(&RS485_USART, &_485rx_buffer[rx_index], 1);
//    }
//}
void RS485_Init()
{
    rx_index = 0 ;
    HAL_UART_Receive_IT(&RS485_USART, &_485rx_buffer[rx_index], 1);  // ��ʼ���յ�һ���ֽ�
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
        // ���������δ��
        if (rx_index < _485RX_BUFFER_SIZE) {
            // ���ӽ�������
            _485rx_buffer[rx_index++] = (uint8_t)huart->Instance->DR;

            // ����������һ���ֽ�
            HAL_UART_Receive_IT(&RS485_USART, &_485rx_buffer[rx_index], 1);
        }
    }
}

uint16_t RS485_ReceiveData(uint8_t *data, uint16_t length) {
    if (length == 0) {
        length = rx_index;  // ���û���Զ��峤�ȣ����Զ�������յ������ݳ���
    }

    // ȷ����������ݳ��Ȳ������������ݵ�ʵ�ʳ���
    if (length > rx_index) {
        length = rx_index;
    }

    // �����յ������ݸ��Ƶ��û��ṩ�Ļ�����
    for (uint16_t i = 0; i < length; i++) {
        data[i] = _485rx_buffer[i];
    }

    // ��¼ʵ�ʽ��յ������ݳ���
    uint16_t received_length = length;

    // ���ý�������
    rx_index = 0;

    return received_length;  // ���ؽ��յ������ݳ���
}


