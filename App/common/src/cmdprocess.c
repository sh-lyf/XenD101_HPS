/**
  ******************************************************************************
  * @file           : cmdprocess.c
  * @author         : iclm team
  * @brief          : command process module
  ******************************************************************************
  */
#include <stdio.h>
#include <string.h>
#include "global_conf.h"
#include "platform.h"
#include "cmdprocess.h"
#include "radar.h"
#include "config.h"
#include "utilities.h"
#include "system.h"
#ifdef STM32_PLATFORM
#include "I2CStressTest.h"
#include "usbd_cdc_if.h"
#include "usb_transfer.h"
#include "rtos.h"
#include "cmsis_os.h"
#endif
#ifdef SUPPORT_ABD
#include "abd.h"
#endif

static uint8_t enableResetAdc = 0;
static uint16_t resetDelayMs = RESET_DELAY_TIME;

static CMD_PARSE_T CmdDataParse;
static uint8_t CmdModeFlag = 0;

static const uint8_t CmdHead[CMD_HEAD_MAX] = {0xFD, 0xFC, 0xFB, 0xFA};
static const uint8_t CmdTail[CMD_TAIL_MAX] = {0x04, 0x03, 0x02, 0x01};
static uint8_t CmdAck[CMD_LEN_MAX] __ALIGN(4) = {0};

#ifdef STM32_PLATFORM
static uint8_t* pI2CTestData = NULL;
static uint16_t I2CTestDataCurIndex = 0;
static uint8_t stressTestReady = 1;
#endif
void CmdProc_AdcReset(void)
{
    uint16_t val = 0;
    
    if (enableResetAdc && CmdModeFlag == 0)
    {                       
        Delay(resetDelayMs);
        I2C_Write(I2C_ADDR_RADAR_Chip0, 0x67, 0x1E00);   
        I2C_Read(I2C_ADDR_RADAR_Chip0, 0x67, &val);    
        I2C_Write(I2C_ADDR_RADAR_Chip0, 0x67, 0x1E40);    
    }
}

static uint16_t FillCmdAck(uint16_t *data, uint16_t dataLen, uint16_t cmdType, uint16_t status)
{
    uint16_t index = 0;

    if (dataLen * sizeof(uint16_t) > (CMD_LEN_MAX - CMD_OVERHEAD_LEN))
    {
        return 0;
    }
    
	memcpy(&CmdAck[index], CmdHead, sizeof(CmdHead));
    index += sizeof(CmdHead);

    if (data == NULL)
    {
        *((uint16_t*)(&CmdAck[index])) = CMD_TYPE_LEN + CMD_STATUS_LEN;
    }
    else
    {
        *((uint16_t*)(&CmdAck[index])) = CMD_TYPE_LEN + (dataLen * sizeof(uint16_t));
    }
    index += sizeof(uint16_t);

    *((uint16_t*)(&CmdAck[index])) = cmdType | CMD_ACK_TYPE;
    index += sizeof(uint16_t);

    if (data == NULL)
    {
        *((uint16_t*)(&CmdAck[index])) = status;
        index += sizeof(uint16_t);
    }
    else
    {
        memcpy(&CmdAck[index], data, dataLen*sizeof(uint16_t));
        index += dataLen * sizeof(uint16_t);   
    }

	memcpy(&CmdAck[index], CmdTail, sizeof(CmdTail));
    index += sizeof(CmdTail);
    
	return index;
}

static uint16_t DoAdcReset(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;

    if (NULL == cmd || 0 == cmdLen)
    {
        return 0;
    }
        
    if (CmdModeFlag == 1)
    {
		enableResetAdc = (uint8_t)cmd->cmdData[0];
		resetDelayMs = cmd->cmdData[1];
#if defined(CONFIG_DEBUG) && defined(STM32_PLATFORM)
        printf("enableResetAdc:%d resetDelayMs:%d\r\n", enableResetAdc, resetDelayMs);
#endif
	} 
    else 
    {
        status = ACK_FAIL;
	}
    
    ackLen = FillCmdAck(NULL, 0, cmd->cmdType, status);

    return ackLen;
}

static uint16_t DoWriteReg(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;
    uint16_t regNum = 0;
    uint16_t loop = 0;
    uint16_t regVal = 0;
	uint16_t devAddress = I2C_ADDR_RADAR_Chip0;
    CMD_REG_T *cmdReg = NULL;

    if (NULL == cmd || 0 == cmdLen)
    {
        return 0;
    }
        
	if (CmdModeFlag) 
    {
		devAddress = cmd->cmdData[0] << 1;
		regNum = (cmdLen - CMD_TYPE_LEN - CMD_DEV_ADDR_LEN) / sizeof(CMD_REG_T);
		for (loop = 0; loop < regNum; loop++) 
        {
            cmdReg = (CMD_REG_T*)((uint8_t*)(cmd->cmdData) + CMD_DEV_ADDR_LEN + (loop * sizeof(CMD_REG_T)));
            I2C_Write(devAddress, (uint8_t)(cmdReg->addr), cmdReg->val);
			if(devAddress == I2C_ADDR_RADAR_Chip0)
			{//update master register
				Radar_UpdateReg(cmdReg->addr, cmdReg->val);
			}
#ifdef STM32_PLATFORM
			HAL_Delay(10); 
#endif
			
			I2C_Read(devAddress, (uint8_t)(cmdReg->addr), &regVal);
			if (regVal != cmdReg->val) 
            {
				status = ACK_FAIL;
			}
		}
	} 
    else 
    {
		status = ACK_FAIL;
	}
    
    ackLen = FillCmdAck(NULL, 0, cmd->cmdType, status);
    
    return ackLen;
}

static uint16_t DoReadReg(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;
    uint16_t regNum = 0;
    uint16_t loop = 0;
    uint16_t *readBuf = NULL;
	uint16_t devAddress = I2C_ADDR_RADAR_Chip0;
	
    if (NULL == cmd || 0 == cmdLen)
    {
        return 0;
    }
        
    if (CmdModeFlag) 
    {
		devAddress = cmd->cmdData[0] << 1;
        regNum = (cmdLen - CMD_TYPE_LEN - CMD_DEV_ADDR_LEN) / CMD_REG_ADDR_LEN;
        
        if (regNum > CMD_REG_MAX)
        {
            status = ACK_FAIL;
        }
        else
        {
            readBuf = (uint16_t*)(&CmdAck[CMD_DATA_POS]);
            for (loop = 0; loop < regNum; loop++) 
            {
				uint16_t regVal = 0;
                I2C_Read(devAddress, (uint8_t)(cmd->cmdData[loop + 1]), &regVal);
				readBuf[loop] = regVal;
            }
        }   
    } 
    else 
    {
        status = ACK_FAIL;
    }
    
    if (status == ACK_OK)
    {
        ackLen = FillCmdAck(readBuf, regNum, cmd->cmdType, ACK_OK);
    }
    else
    {
        ackLen = FillCmdAck(NULL, 0, cmd->cmdType, ACK_FAIL);
    }
    
    return ackLen;
}

__weak int8_t MTT_ParaUpdate(uint16_t type, int32_t val)
{
    return -1;
}

static uint16_t DoWritePara(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;
    uint16_t paraNum = 0;
    uint16_t loop = 0;
    CMD_PARA_T *cmdPara = NULL;
    int8_t ret = 0;

    if (NULL == cmd || 0 == cmdLen)
    {
        return 0;
    }
        
	if (CmdModeFlag) 
    {
		paraNum = (cmdLen - CMD_TYPE_LEN) / sizeof(CMD_PARA_T);
		for (loop = 0; loop < paraNum; loop++) 
        {
            cmdPara = (CMD_PARA_T*)((uint8_t*)(cmd->cmdData) + (loop * sizeof(CMD_PARA_T)));
            switch (cmd->cmdType)
            {
                case WRITE_MTT_CMD: 
                    ret = MTT_ParaUpdate(cmdPara->type, cmdPara->val);
                    Config_NeedFlashWrite();
                    break;

                case WRITE_SYS_CMD: 
                    ret = System_ParaUpdate(cmdPara->type, cmdPara->val);
                    Config_NeedFlashWrite();
                    break;
#ifdef SUPPORT_DFFT_PEAK_CONFIG
                case WRITE_PEAK_CMD: 
                    ret = PEAK_ParaUpdate(cmdPara->type, cmdPara->val);
                    Config_NeedFlashWrite();
                    break;
#endif
#ifdef SUPPORT_ABD
                case WRITE_ABD_CMD: 
                    ret = ABD_ParaUpdate(cmdPara->type, cmdPara->val);
                    Config_NeedFlashWrite();
                    break;
#endif                
                default:
                    ret = -1;
                    break;
            }
            if (ret)
            {
                status = ACK_FAIL;
            }
		}
	} 
    else 
    {
		status = ACK_FAIL;
	}
    
    ackLen = FillCmdAck(NULL, 0, cmd->cmdType, status);
    
    return ackLen;
}

__weak int32_t MTT_ParaRead(uint16_t type)
{
    return 0x7fffffff; /*invalid value*/
}

static uint16_t DoReadPara(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;
    uint16_t paraNum = 0;
    uint16_t loop = 0;
    uint32_t *readBuf = NULL;

    if (NULL == cmd || 0 == cmdLen)
    {
        return 0;
    }
        
    if (CmdModeFlag) 
    {
        paraNum = (cmdLen - CMD_TYPE_LEN) / CMD_PARA_NAME_LEN;
        
        if (paraNum > CMD_PARA_MAX)
        {
            status = ACK_FAIL;
        }
        else
        {
            readBuf = (uint32_t*)(&CmdAck[CMD_DATA_POS]);
            for (loop = 0; loop < paraNum; loop++) 
            {
                switch (cmd->cmdType)
                {
                    case READ_MTT_CMD:
                        readBuf[loop] = MTT_ParaRead(cmd->cmdData[loop]);
                        break;
                    
                    case READ_SYS_CMD:
                        readBuf[loop] = System_ParaRead(cmd->cmdData[loop]);
                        break;
                    
#ifdef SUPPORT_DFFT_PEAK_CONFIG                   
                    case READ_PEAK_CMD:
                        readBuf[loop] = PEAK_ParaRead(cmd->cmdData[loop]);
                        break;
#endif                    
                    
                    default:
                        break;
                }
            }
        }   
    } 
    else 
    {
        status = ACK_FAIL;
    }
    
    if (status == ACK_OK)
    {
        ackLen = FillCmdAck((uint16_t*)readBuf, paraNum*2, cmd->cmdType, ACK_OK);
    }
    else
    {
        ackLen = FillCmdAck(NULL, 0, cmd->cmdType, ACK_FAIL);
    }
    
    return ackLen;
}

static uint16_t DoReadSn(CMD_T *cmd)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;
    uint16_t sn[SN_LEN] = {0};

    if (NULL == cmd)
    {
        return 0;
    }
        
    if (CmdModeFlag) 
    {   
        sn[0] = RADAR_DEV_MODEL_TYPE;
        *(uint32_t*)(&sn[1]) = Config_GetSN();      
    } 
    else 
    {
        status = ACK_FAIL;
    }
    
    if (status == ACK_OK)
    {
        ackLen = FillCmdAck(sn, sizeof(sn), cmd->cmdType, ACK_OK);
    }
    else
    {
        ackLen = FillCmdAck(NULL, 0, cmd->cmdType, ACK_FAIL);
    }
    
    return ackLen;
}

#ifndef XenD101
static uint16_t DoCascadingMode(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;

    if (NULL == cmd || 0 == cmdLen)
    {
        return 0;
    }
#ifdef STM32_PLATFORM      
    if (CmdModeFlag == 1)
    {
		//uint8_t cascadingMode = (uint8_t)cmd->cmdData[0];
		uint8_t tRMode = cmd->cmdData[1]; 
		g_ChannelCount = MIN(tRMode, g_maxChannelCount);
		g_TxCount = (g_ChannelCount + 1) / 2;
		if((cmdLen - CMD_TYPE_LEN) == 6)
		{
			g_TxCount = cmd->cmdData[2];
		}
	} 
    else 
    {
        status = ACK_FAIL;
	}
#endif  
    ackLen = FillCmdAck(NULL, 0, cmd->cmdType, status);

    return ackLen;
}

#ifdef STM32_PLATFORM 
void ResetI2CTestDataBuf()
{
	if(pI2CTestData != NULL)
	{
		free(pI2CTestData);
		pI2CTestData = NULL;
	}
}
#endif 

static uint16_t StartI2CTest(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    uint16_t status = ACK_OK;
	
    if (NULL == cmd || 0 == cmdLen)
    {
        return 0;
    }
#ifdef STM32_PLATFORM 	
	uint16_t dataLen = 0;
	uint16_t packIndex = 0;
    dataLen = cmd->cmdData[0];
	packIndex = cmd->cmdData[1];
	
	if(packIndex == 0)
	{
		if(I2CStressTest_IsStressTestProcessRunning())
		{
			stressTestReady = (I2CStressTest_ForceStopI2CStressTestProcess() != 0) ? 1 : 0;
		}
		if(stressTestReady)
		{
			ResetI2CTestDataBuf();
			pI2CTestData = malloc(dataLen);
			I2CTestDataCurIndex = 0;	
		}
	}
	if(stressTestReady && pI2CTestData != NULL)
	{
		memcpy(pI2CTestData + I2CTestDataCurIndex, (uint8_t*)(cmd->cmdData) + 4, cmdLen - CMD_TYPE_LEN - 4);
		I2CTestDataCurIndex += cmdLen - CMD_TYPE_LEN - 4;
			 
		if(I2CTestDataCurIndex == dataLen)
		{
			I2CStressTest_ResetRunningState();
			I2CTEST_DATA_T I2CTestData = {0};
			I2CTestData.buf = pI2CTestData;
			I2CTestData.len = I2CTestDataCurIndex;
			I2CStressTest_Send2I2CTestDataQueue(&I2CTestData);
		}
	}
	
	if(!stressTestReady)
	{
		status = ACK_FAIL;
	}
#endif     
    ackLen = FillCmdAck(NULL, 0, cmd->cmdType, status);
    return ackLen;
}

static uint16_t StopI2CTest(CMD_T *cmd, uint32_t cmdLen)
{
	uint16_t ackLen = 0;
    uint16_t status = ACK_OK;
#ifdef STM32_PLATFORM 
	if(I2CStressTest_IsStressTestProcessRunning())
	{
		status = (I2CStressTest_ForceStopI2CStressTestProcess() != 0) ? ACK_OK : ACK_FAIL;
	}
#endif   
    ackLen = FillCmdAck(NULL, 0, cmd->cmdType, status);
	return ackLen;	
}

static uint16_t GetI2CTestResult(CMD_T *cmd, uint32_t cmdLen)
{
	uint16_t ackLen = 0;
	uint16_t status = ACK_OK;
	uint8_t buf[10] = {0};
#ifdef STM32_PLATFORM
	I2CTEST_RESULT_T testResult = {0};
	testResult = I2CStressTest_GetI2CTestResult();
	
	*((uint16_t*)buf) = testResult.running;
	*((uint32_t*)(buf + 2)) = testResult.curTestCount;
	*((uint32_t*)(buf + 6)) = testResult.curTestErrCount;
	
	//printf("GetI2CTestResult curTestCount=%d  curTestCount=%d\r\n", testResult.curTestCount, testResult.curTestCount);
#endif 
	ackLen = FillCmdAck((uint16_t*)buf, 5, cmd->cmdType, status);
	return ackLen;
}
#endif

static void AckSend(uint16_t ackLen)
{
#ifdef STM32_PLATFORM
    USB_DATA_T usbDataQ = {0};
#endif
#if defined(CONFIG_DEBUG) && defined(STM32_PLATFORM)
    uint16_t i = 0;
#endif

    if (0 == ackLen)
    {
        return;
    }

#if defined(CONFIG_DEBUG) && defined(STM32_PLATFORM)
    printf("ackLen:%d\r\n", ackLen);
    for (i = 0; i < ackLen; i++)
    {
        printf("0x%02x ", CmdAck[i]);
    }
    printf("\r\n");
#endif

#if defined(STM32_PLATFORM)
    //CDC_Transmit_FS(CmdAck, ackLen);
    usbDataQ.buf = CmdAck;
    usbDataQ.len = ackLen;
    UsbTransfer_Send2UsbDataQueue(&usbDataQ);
#elif defined(MM32_PLATFORM)
    UART_DirectSend(CmdAck, ackLen);
#else
    UART0_DmaSend(CmdAck, ackLen);
#endif
}

static void CmdExec(CMD_T *cmd, uint32_t cmdLen)
{
    uint16_t ackLen = 0;
    //uint16_t devVersion[] = {RADAR_DEV_MODEL_TYPE, FIRMWARE_TYPE, RADAR_DEV_VER_MAJAR, RADAR_DEV_VER_MINOR, RADAR_DEV_VER_PATCH};
    uint16_t devVersion[] = {RADAR_DEV_VER_MAJAR, RADAR_DEV_VER_MINOR, RADAR_DEV_VER_PATCH};

    if (NULL == cmd || 0 == cmdLen)
    {
        return;
    }

	switch(cmd->cmdType)
    {
		case START_CFG_CMD:
			CmdModeFlag = 1;
			ackLen = FillCmdAck(NULL, 0, cmd->cmdType, ACK_OK);
		    break;
        
		case FINISH_CFG_CMD:
            Config_SavePara2Flash();
            System_Reconfig();
			ackLen = FillCmdAck(NULL, 0, cmd->cmdType, ACK_OK);
            AckSend(ackLen);
            CmdModeFlag = 0;
#ifdef XenD101
            Delay(SYS_RESET_DELAY);
            NVIC_SystemReset();
#endif
		    return;

        case ADC_RESET_CMD:         
            ackLen = DoAdcReset(cmd, cmdLen);
            break;
            
		case WRITE_REG_CMD:
            ackLen = DoWriteReg(cmd, cmdLen);
		    break;
            
		case READ_REG_CMD:
            ackLen = DoReadReg(cmd, cmdLen);
		    break;
            
		case READ_VER_CMD:
            ackLen = FillCmdAck(devVersion, ARRAY_SIZE(devVersion), cmd->cmdType, ACK_OK);
            break;
        
		case WRITE_MTT_CMD:
        case WRITE_SYS_CMD:
        case WRITE_PEAK_CMD:
        case WRITE_ABD_CMD:
            ackLen = DoWritePara(cmd, cmdLen);
		    break;

        case READ_MTT_CMD:
        case READ_SYS_CMD:
        case READ_PEAK_CMD:
        case READ_ABD_CMD:
            ackLen = DoReadPara(cmd, cmdLen);
		    break;
        
        case READ_SN_CMD:
            ackLen = DoReadSn(cmd);
		    break;
#ifndef XenD101
		case CASCADING_MODE_CMD:
			ackLen = DoCascadingMode(cmd, cmdLen);
            break;
		
		case START_I2C_TEST_CMD:
			ackLen = StartI2CTest(cmd, cmdLen);
            break;
		
		case STOP_I2C_TEST_CMD:
			ackLen = StopI2CTest(cmd, cmdLen);
            break;
        
		case GET_I2C_TEST_CMD:
			ackLen = GetI2CTestResult(cmd, cmdLen);
            break;
#endif
		default:
		    break;
	}
	
    AckSend(ackLen);
}

uint8_t CmdProc_InCmdMode(void)
{
    return CmdModeFlag;
}

static void CmdProcess(uint8_t* buf, uint16_t len)
{
    uint16_t loop = 0;
#if defined(CONFIG_DEBUG) && defined(STM32_PLATFORM)
    uint16_t i = 0;
#endif

    if (NULL == buf || 0 == len)
    {
        return;
    }
    
#if defined(CONFIG_DEBUG) && defined(STM32_PLATFORM)
    printf("recvLen:%d\r\n", len);
    for (i = 0; i < len; i++)
    {
        printf("0x%02x ", buf[i]);
    }
    printf("\r\n");
#endif

	for (loop = 0; loop < len; loop++) 
    {
		switch(CmdDataParse.state)
		{                    
			case CMD_STATE_HEAD0:
				if (buf[loop] == CmdHead[CMD_HEAD_0]) 
                {
                    CmdDataParse.curIndex = 0;
					CmdDataParse.state = CMD_STATE_HEAD1;
				}
				break;
                
			case CMD_STATE_HEAD1:
				if (buf[loop] == CmdHead[CMD_HEAD_1])
                {
                    CmdDataParse.state = CMD_STATE_HEAD2;
				}
                else
                {
                    CmdDataParse.state = CMD_STATE_HEAD0;
                }
				break;

            case CMD_STATE_HEAD2:
				if (buf[loop] == CmdHead[CMD_HEAD_2])
                {
                    CmdDataParse.state = CMD_STATE_HEAD3;
				}
                else
                {
                    CmdDataParse.state = CMD_STATE_HEAD0;
                }
				break;

            case CMD_STATE_HEAD3:
				if (buf[loop] == CmdHead[CMD_HEAD_3])
                {
                    CmdDataParse.state = CMD_STATE_LEN0;
				}
                else
                {
                    CmdDataParse.state = CMD_STATE_HEAD0;
                }
				break;

            case CMD_STATE_LEN0:
                CmdDataParse.len = buf[loop];
                CmdDataParse.state = CMD_STATE_LEN1;
				break;

            case CMD_STATE_LEN1:
                CmdDataParse.len += buf[loop] << CMD_LEN_HIGH_POS;
                if (CmdDataParse.len <= CMD_BUF_LEN)
                {
                    CmdDataParse.state = CMD_STATE_DATA;
                }
                else
                {
                    CmdDataParse.state = CMD_STATE_HEAD0;
                }
				break;
                
            case CMD_STATE_DATA:
                CmdDataParse.buf[CmdDataParse.curIndex++] = buf[loop];
				if (CmdDataParse.curIndex == CmdDataParse.len)
                {
					CmdDataParse.state = CMD_STATE_TAIL0;
				}
				break;
                
            case CMD_STATE_TAIL0:
				if (buf[loop] == CmdTail[CMD_TAIL_0])
                {
                    CmdDataParse.state = CMD_STATE_TAIL1;
				}
                else
                {
                    CmdDataParse.state = CMD_STATE_HEAD0;
                }
				break;

            case CMD_STATE_TAIL1:
				if (buf[loop] == CmdTail[CMD_TAIL_1])
                {
                    CmdDataParse.state = CMD_STATE_TAIL2;
				}
                else
                {
                    CmdDataParse.state = CMD_STATE_HEAD0;
                }
				break;
                
            case CMD_STATE_TAIL2:
				if (buf[loop] == CmdTail[CMD_TAIL_2])
                {
                    CmdDataParse.state = CMD_STATE_TAIL3;
				}
                else
                {
                    CmdDataParse.state = CMD_STATE_HEAD0;
                }
				break;

            case CMD_STATE_TAIL3:
				if (buf[loop] == CmdTail[CMD_TAIL_3])
                {
                    CmdExec((CMD_T*)(CmdDataParse.buf), CmdDataParse.len);   
				}
                CmdDataParse.state = CMD_STATE_HEAD0;
				break; 
			
			default:
				CmdDataParse.state = CMD_STATE_HEAD0;
				break;

		}
	}

}

#if defined(GD32_PLATFORM) || defined(MM32_PLATFORM)
void CmdProc_Recv(void)
{  
    if (!g_cmdRecv.cmdReady)
    {
        return;
    }

    CmdProcess(g_cmdRecv.buf[g_cmdRecv.bufProc], g_cmdRecv.bufLen);

    g_cmdRecv.cmdReady = 0;
}
#endif

void CmdProc_Init(void)
{
    memset(&g_cmdRecv, 0, sizeof(g_cmdRecv));
}

#ifdef STM32_PLATFORM
osThreadId cmdProcTaskHandle;
QueueHandle_t cmdDataQueue;

void CmdProc_Send2CmdDataQueue(void *cmdData)
{
	BaseType_t xHigherPriorityTaskWoken = 0;

    if (NULL == cmdData)
    {
        return;
    }
    
	if (xQueueSendFromISR(cmdDataQueue, cmdData, &xHigherPriorityTaskWoken) != pdPASS) 
    {
        Indicator_CmdDataRecvOverFlow();
	}

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); 
}


void CmdProcTask(void const * argument)
{
	CMD_DATA_T cmdData = {0};
        
    while(1)
    { 
        if (xQueueReceive(cmdDataQueue, &cmdData, portMAX_DELAY))
        {
            CmdProcess(cmdData.buf, cmdData.len);
        }
    } 
}

void CmdProc_TaskInit(void)
{
    cmdDataQueue = xQueueCreate(CMD_DATA_QUEUE_SIZE, sizeof(CMD_DATA_T));
    
    osThreadDef(cmdProcTask, CmdProcTask, osPriorityHigh, 0, CMD_PROC_STACK_SIZE);
    cmdProcTaskHandle = osThreadCreate(osThread(cmdProcTask), NULL);
    if (NULL == cmdProcTaskHandle)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}
#endif



