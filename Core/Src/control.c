#include "control.h"
#include "gpio.h"
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
