#include "control.h"
#include "gpio.h"
#include "rs485.h"
/*#include "dmaPrintf.h"*/


uint8_t ctlCmd = CMD_OPEN_CONTROL; //����ģʽ
uint8_t motorId = 1; //���ID
int64_t ctlValue = 0; //����ֵ
uint32_t commandSendCount = 0; //����ʹ���
uint32_t commandUnreceivedCount = 0; //����δ�յ�����

int8_t motorTemperature = 0;  //����¶�
int16_t motorPowerOrTorque = 0; //������ʻ�Ť��
int16_t motorSpeed = 0; //����ٶ�
uint16_t motorEncoder = 0;  //���������

/*********************************************************************************
* @brief	��������
* @param	None
* @retval	None
*********************************************************************************/
void control_Run(void)
{
    switch (ctlCmd)
    {
        case CMD_OPEN_CONTROL:
            break;
    }
}

/*********************************************************************************
 * @brief	������͵�����
 * @param	None
 * @retval	None
 *********************************************************************************/
void control_PackCmd(uint8_t *buffer, uint8_t cmd, uint8_t id, uint8_t size, uint8_t *data)
{
    uint8_t i = 0;

    buffer[0] = CMD_HEAD;
    buffer[1] = cmd;
    buffer[2] = id;
    buffer[3] = size;
    buffer[4] = 0; // ��Ҫ����0
    for (i = 0; i < 4; i++)
        buffer[4] += buffer[i];

    if (size != 0)
    {
        buffer[LEAST_FRAME_SIZE + size] = 0; // ��Ҫ����0
        for (i = 0; i < size; i++)           // �������ݲ�����У��ֵ
        {
            buffer[LEAST_FRAME_SIZE + i] = data[i];
            buffer[LEAST_FRAME_SIZE + size] += buffer[LEAST_FRAME_SIZE + i];
        }
        _485TX_BUFFER_SIZE = i + LEAST_FRAME_SIZE + 1; // ��Ҫ���͵������ܳ���
    }
    else
        _485TX_BUFFER_SIZE = LEAST_FRAME_SIZE; // ��Ҫ���͵������ܳ���
}

/*********************************************************************************
 * @brief	���Ϳ�������
 * @param	None
 * @retval	None
 *********************************************************************************/
void control_Send(uint8_t cmd, uint8_t id, int64_t value)
{
    uint8_t dataSize = 0;      // �������ݳ���
    int16_t openCtlData = 0;   // ������������
    int16_t torqueCtlData = 0; // ���ػ���������
    int16_t speedCtlData = 0;  // �ٶȻ���������
    int16_t angleCtlData = 0;  // λ�û���������

    if (cmd == CMD_OPEN_CONTROL)
    {
        openCtlData = value;
        dataSize = 2;

        control_PackCmd(uart1TxBuffer, cmd, id, dataSize, (uint8_t *)&openCtlData);
        HAL_UART_Transmit_DMA(&huart1, uart1TxBuffer, _485TX_BUFFER_SIZE);
        _485RX_BUFFER_SIZE = LEAST_FRAME_SIZE + 7 + 1;
    }
    else if (cmd == CMD_TORQUE_CONTROL)
    {
        torqueCtlData = value;
        dataSize = 2;

        control_PackCmd(uart1TxBuffer, cmd, id, dataSize, (uint8_t *)&torqueCtlData);
        HAL_UART_Transmit_DMA(&huart1, uart1TxBuffer, _485TX_BUFFER_SIZE);
        _485RX_BUFFER_SIZE = LEAST_FRAME_SIZE + 7 + 1;
    }
    else if (cmd == CMD_SPEED_CONTROL)
    {
        speedCtlData = value;
        dataSize = 4;

        control_PackCmd(uart1TxBuffer, cmd, id, dataSize, (uint8_t *)&speedCtlData);
        HAL_UART_Transmit_DMA(&huart1, uart1TxBuffer, _485TX_BUFFER_SIZE);
        _485RX_BUFFER_SIZE = LEAST_FRAME_SIZE + 7 + 1;
    }
    else if (cmd == CMD_ANGLE_CONTROL1)
    {
        angleCtlData = value;
        dataSize = 8;

        control_PackCmd(uart1TxBuffer, cmd, id, dataSize, (uint8_t *)&angleCtlData);
        HAL_UART_Transmit_DMA(&huart1, uart1TxBuffer, _485TX_BUFFER_SIZE);
        _485RX_BUFFER_SIZE = LEAST_FRAME_SIZE + 7 + 1;
    }
}

/*********************************************************************************
 * @brief	���յ���ظ�����
 * @param	None
 * @retval	None
 *********************************************************************************/
void control_Receive(void)
{
    RS485_ReceiveData(received_data, 0);
}

/*********************************************************************************
 * @brief	��������
 * @param	None
 * @retval	None
 *********************************************************************************/
void command_Run(void)
{
    // ���ڼ���
    static uint16_t tick = 0;
    // �յ��ظ���־
    uint8_t receivedFlag = 0;

    // ��tickС��50ʱִ�����´���
    if (tick < 50)
    {
        // ��tickΪ0ʱ�����Ϳ��������������
        if (tick == 0)
        {
            control_Send(ctlCmd, motorId, ctlValue);
            control_Receive();
        }
        // ��tickΪ20ʱ������Ƿ��յ����ݣ������·��ͺ�δ���ռ���
        else if (tick == 20)
        {
            receivedFlag = control_CheckReceivedData();

            commandSendCount++;
            // ���δ�յ��ظ���δ���ռ�����һ
            if (receivedFlag == 0)
            {
                commandUnreceivedCount++;
            }
        }

        // tick����
        tick++;
    }
    // ��tick�ﵽ50ʱ������tickΪ0��ʵ��ѭ��
    else
    {
        tick = 0;
    }
}

/*********************************************************************************
 * @brief	״̬����
 * @param	None
 * @retval	None
 *********************************************************************************/
void status_Run(void)
{
    static uint16_t tick = 0;

    if (tick < 1000)
    {
        tick++;

        if (tick == 500)
        {
            LED1_TOGGLE();

            printf ("motorTemperature = %d, motorPowerOrTorque = %d, motorSpeed = %d, motorEncoder = %d\n", motorTemperature, motorPowerOrTorque, motorSpeed, motorEncoder);
        }
        else if (tick == 1000)
        {
            LED1_TOGGLE();

            printf ("commandSendCount = %d, unreceivedCount = %d\n", commandSendCount, commandUnreceivedCount);
        }
    }
    else
    {

        tick = 0;
    }
}
