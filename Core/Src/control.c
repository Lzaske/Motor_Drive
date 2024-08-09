#include "control.h"
#include "gpio.h"
#include "rs485.h"
/*#include "dmaPrintf.h"*/


uint8_t ctlCmd = CMD_OPEN_CONTROL; //控制模式
uint8_t motorId = 1; //电机ID
int64_t ctlValue = 0; //控制值
uint32_t commandSendCount = 0; //命令发送次数
uint32_t commandUnreceivedCount = 0; //命令未收到次数

int8_t motorTemperature = 0;  //电机温度
int16_t motorPowerOrTorque = 0; //电机功率或扭矩
int16_t motorSpeed = 0; //电机速度
uint16_t motorEncoder = 0;  //电机编码器

/*********************************************************************************
* @brief	控制运行
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
 * @brief	打包发送的数据
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
    buffer[4] = 0; // 需要先清0
    for (i = 0; i < 4; i++)
        buffer[4] += buffer[i];

    if (size != 0)
    {
        buffer[LEAST_FRAME_SIZE + size] = 0; // 需要先清0
        for (i = 0; i < size; i++)           // 复制数据并计算校验值
        {
            buffer[LEAST_FRAME_SIZE + i] = data[i];
            buffer[LEAST_FRAME_SIZE + size] += buffer[LEAST_FRAME_SIZE + i];
        }
        _485TX_BUFFER_SIZE = i + LEAST_FRAME_SIZE + 1; // 需要发送的数据总长度
    }
    else
        _485TX_BUFFER_SIZE = LEAST_FRAME_SIZE; // 需要发送的数据总长度
}

/*********************************************************************************
 * @brief	发送控制命令
 * @param	None
 * @retval	None
 *********************************************************************************/
void control_Send(uint8_t cmd, uint8_t id, int64_t value)
{
    uint8_t dataSize = 0;      // 命令数据长度
    int16_t openCtlData = 0;   // 开环控制数据
    int16_t torqueCtlData = 0; // 力矩环控制数据
    int16_t speedCtlData = 0;  // 速度环控制数据
    int16_t angleCtlData = 0;  // 位置环控制数据

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
 * @brief	接收电机回复数据
 * @param	None
 * @retval	None
 *********************************************************************************/
void control_Receive(void)
{
    RS485_ReceiveData(received_data, 0);
}

/*********************************************************************************
 * @brief	命令运行
 * @param	None
 * @retval	None
 *********************************************************************************/
void command_Run(void)
{
    // 用于计数
    static uint16_t tick = 0;
    // 收到回复标志
    uint8_t receivedFlag = 0;

    // 当tick小于50时执行以下代码
    if (tick < 50)
    {
        // 当tick为0时，发送控制命令并接收数据
        if (tick == 0)
        {
            control_Send(ctlCmd, motorId, ctlValue);
            control_Receive();
        }
        // 当tick为20时，检查是否收到数据，并更新发送和未接收计数
        else if (tick == 20)
        {
            receivedFlag = control_CheckReceivedData();

            commandSendCount++;
            // 如果未收到回复，未接收计数加一
            if (receivedFlag == 0)
            {
                commandUnreceivedCount++;
            }
        }

        // tick自增
        tick++;
    }
    // 当tick达到50时，重置tick为0，实现循环
    else
    {
        tick = 0;
    }
}

/*********************************************************************************
 * @brief	状态运行
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
