#include "control.h"
#include "gpio.h"
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
