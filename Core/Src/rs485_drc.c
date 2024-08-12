#include  <rs485_drc.h>
#include <string.h>
/**
* @breif   ��ʼ��RS485�շ���Ϊ����ģʽ
* @param   ��
* @retval  ��
* @note    �ڵ��ô˺���֮ǰ��ȷ���Ѿ���ʼ��ͨ�Ŵ��ںͿ���GPIO
*/

uint8_t _485rx_buffer[_485RX_BUFFER_SIZE]; //���ջ�����
uint16_t rx_index; // ���ջ���������
uint8_t received_data[_485RX_BUFFER_SIZE]; //��������
uint8_t _485tx_buffer[_485TX_BUFFER_SIZE]; //���ͻ�����
uint8_t _485txDataSize = 0;
/*
* @breif   RS485��ʼ��
* @param  
        - _485rx_buffer ���ջ�����
        - rx_index ���ջ���������
* @retval  ��
* @note    ��ʼ������Ϊ0 �����жϽ��� 485����ʹ��
*/
void RS485_Init()
{
    rx_index = 0 ;
    HAL_UART_Receive_IT(&RS485_USART, &_485rx_buffer[rx_index], 1);  // ��ʼ���յ�һ���ֽ�
    RS485_RX_ENABLE();
}

/**
* @breif   RS485��������
* @param  
        - data Ҫ���͵�����
        - len Ҫ���͵����ݳ���
* @retval  ��
* @note    ��������ǰ��ʹ��485����
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


