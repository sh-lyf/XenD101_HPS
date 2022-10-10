/**
  ******************************************************************************
  * @file           : dataprocess.c
  * @author         : iclm team
  * @brief          : data process module
  ******************************************************************************
  */
#include <stdio.h>
#include <string.h>
#include "global_conf.h"
#include "utilities.h"
#include "dataprocess.h"
#include "radar.h"
#include "cmdprocess.h"
#include "system.h"
#ifdef STM32_PLATFORM
#include "rtos.h"
#include "cmsis_os.h"
#include "usb_transfer.h"
#endif
#if defined(SUPPORT_MOTION_TARGET_TRACKING) || defined(SUPPORT_DYNAMIC_SYS_MODE)
#include "mtt_task.h"
#endif
#if defined(SUPPORT_VITAL_SIGN) || defined(SUPPORT_DYNAMIC_SYS_MODE)
#include "vs_config.h"
#endif
#if defined(SUPPORT_GESTURE_RECOGNITION) || defined(SUPPORT_DYNAMIC_SYS_MODE)
#include "gr_task.h"
#endif
#ifdef SUPPORT_RVA_CALC
#include "rva.h"
#endif
#ifdef SUPPORT_ABD
#include "abd.h"
#endif

static RADAR_DATA_PARSE_T RadarDataParse[CHANNEL_MAX];
RADAR_PARA_T RadarPara;
void SystemInit (void);

static void CheckChirpIndex(uint8_t channel, uint8_t chirpIndex)
{
    static uint8_t curIndex[CHANNEL_MAX] = {0};
    static uint8_t oldCurIndex[CHANNEL_MAX] = {0};
    static uint8_t skipNum = SKIP_NUM;

    if (channel >= CHANNEL_MAX)
    {
        return;
    }
        
	if (skipNum) 
    {
		skipNum--;
		curIndex[channel] = oldCurIndex[channel] = chirpIndex % RadarPara.chirpNum;
	} 
    else 
    {
		curIndex[channel] = chirpIndex % RadarPara.chirpNum;
		if (curIndex[channel] != ((oldCurIndex[channel] + 1) % RadarPara.chirpNum))
        {
			Indicator_RadarDataIndexError();
		}
		oldCurIndex[channel] = curIndex[channel];
	}
}

static void CheckFrameCnt(uint8_t channel, uint16_t frameCnt)
{
    static uint16_t oldFrameCnt[CHANNEL_MAX] = {0};
    static uint8_t skipNum = SKIP_NUM;

    if (channel >= CHANNEL_MAX)
    {
        return;
    }
    
	if (skipNum) 
    {
		skipNum--;
		oldFrameCnt[channel] = frameCnt;
	} 
    else if (frameCnt != oldFrameCnt[channel] + 1)
    {
        Indicator_RadarDataIndexError();
    }
    oldFrameCnt[channel] = frameCnt;
}

static void DataStateIdParse(uint8_t data, uint8_t channel)
{
    uint8_t flag = 0;
    
    switch (RadarPara.dataType)
    {
        case DATA_TYPE_FFT:
            if ((data & ID_MASK) == FFT0_ID || (data & ID_MASK) == FFT1_ID)
            {
				if(channel > 1)
				{
					data += 0x20;
				}
                RadarDataParse[channel].chirpIndex = (data & CHIRP_INDEX_MASK) << CHIRP_INDEX_POS0;
                flag = 1;
            }
            break;
        case DATA_TYPE_DSRAW:
            if ((data & ID_MASK) == DSRAW0_ID || (data & ID_MASK) == DSRAW1_ID)
            {
				if(channel > 1)
				{
					data += 0x20;
				}
                RadarDataParse[channel].chirpIndex = (data & CHIRP_INDEX_MASK) << CHIRP_INDEX_POS0;
                flag = 1;
            }
            break;
        case DATA_TYPE_DFFT:
			if ((data & ID_MASK) == DFFT0_ID || (data & ID_MASK) == DFFT1_ID)
            {
                flag = 1;
			}
            break;
        
        case DATA_TYPE_DFFT_PEAK:
            if ((data & ID_MASK) == DFFT_PEAK_ID)
            {
                flag = 1;
			}
            break;
        
        default:
            break;
    }

    if (flag)
    {
        RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = data;
        RadarDataParse[channel].state = DATA_STATE_INDEX1;
    }
    else
    {
        RadarDataParse[channel].state = DATA_STATE_HEAD;
    }
}

static void DataStateIndex1Parse(uint8_t data, uint8_t channel)
{
    switch (RadarPara.dataType)
    {
		case DATA_TYPE_DSRAW:
        case DATA_TYPE_FFT:
            RadarDataParse[channel].chirpIndex += data >> CHIRP_INDEX_POS1;
            break;
        
        case DATA_TYPE_DFFT:
            RadarDataParse[channel].frameCnt = data << FRAME_CNT_POS;
            break;
        
        case DATA_TYPE_DFFT_PEAK:
            break;
        
        default:
            break;
    }

    RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = data;
    RadarDataParse[channel].state = DATA_STATE_INDEX2;
}

static void DataStateIndex2Parse(uint8_t data, uint8_t channel)
{
    switch (RadarPara.dataType)
    {
        case DATA_TYPE_FFT:
            break;
        
        case DATA_TYPE_DFFT:
            RadarDataParse[channel].frameCnt += data;
            break;
        
        case DATA_TYPE_DFFT_PEAK:
            break;
        
        default:
            break;
    }
    
    RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = data;
    RadarDataParse[channel].state = DATA_STATE_DATA;
}

#ifdef PY32_PLATFORM
void CmdProc_AdcReset(void)
{
    uint16_t val = 0;
                  
    Delay(2);
    I2C_Write(I2C_ADDR_RADAR_Chip0, 0x67, 0x1E00);
	Delay(2);
    I2C_Read(I2C_ADDR_RADAR_Chip0, 0x67, &val);
	Delay(2);
    I2C_Write(I2C_ADDR_RADAR_Chip0, 0x67, 0x1E40);    

}
#endif

static uint8_t DataStateTail3Parse(uint8_t data, uint8_t channel)
{
    RadarDataParse[channel].state = DATA_STATE_HEAD;
    if (data != DATA_TAIL) 
    {
        return 0;
    }
    
    switch (RadarPara.dataType)
    {
        case DATA_TYPE_FFT:
            if ((RadarDataParse[channel].chirpIndex == RadarPara.chirpNum - 1) && (channel == CHANNEL_MAX - 1))
            {
                CmdProc_AdcReset();
            }
            break;
        
        case DATA_TYPE_DFFT:             
            if (channel == CHANNEL_MAX - 1)
            {
                CmdProc_AdcReset();
            }
            break;
        
        case DATA_TYPE_DFFT_PEAK:
            break;
        
        default:
            break;
    }
    
    RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = data;
    
    return 1;
}

static void DataCopy(uint8_t* buf, uint16_t len, uint8_t channel, uint16_t *i)
{
    uint16_t copyLen = 0;

    if (NULL == buf || NULL == i)
    {
        return;
    }
    
	copyLen = (RadarDataParse[channel].needCopyLen > (len - *i))?
            (len - *i) : RadarDataParse[channel].needCopyLen;
    memcpy(&RadarDataParse[channel].buf[RadarDataParse[channel].curIndex], &buf[*i], copyLen);

    RadarDataParse[channel].curIndex += copyLen;
    *i += (copyLen - 1);
    RadarDataParse[channel].needCopyLen -= copyLen;

    if (!RadarDataParse[channel].needCopyLen)
    {
    	RadarDataParse[channel].state = DATA_STATE_TAIL0;
        RadarDataParse[channel].needCopyLen = RadarPara.dataLen - SPI_FRAME_HLEN - SPI_FRAME_TLEN;
    }
}

static uint8_t DataParse(uint8_t* buf, uint16_t len, uint8_t channel)
{
    uint16_t i = 0;
    uint8_t parseFinish = 0;

    if (NULL == buf || 0 == len || channel >= CHANNEL_MAX)
    {
        return 0;
    }

	for (i = 0; i < len; i++) 
    {
		switch(RadarDataParse[channel].state)
		{                    
			case DATA_STATE_HEAD:
				if (buf[i] == DATA_HEAD) 
                {
                    RadarDataParse[channel].curIndex = 0;
                    RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = buf[i];
					RadarDataParse[channel].state = DATA_STATE_ID;
				}
				break;
                
			case DATA_STATE_ID:
                DataStateIdParse(buf[i], channel);
				break;
                
			case DATA_STATE_INDEX1:
                DataStateIndex1Parse(buf[i], channel);
				break;
                
			case DATA_STATE_INDEX2:
                DataStateIndex2Parse(buf[i], channel);
				break;
            
			case DATA_STATE_DATA:				
                DataCopy(buf, len, channel, &i);
				break;

            case DATA_STATE_TAIL0:
                RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = buf[i];
				RadarDataParse[channel].state = DATA_STATE_TAIL1;
				break;

            case DATA_STATE_TAIL1:
                RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = buf[i];
				RadarDataParse[channel].state = DATA_STATE_TAIL2;
				break;

            case DATA_STATE_TAIL2:
                RadarDataParse[channel].buf[RadarDataParse[channel].curIndex++] = buf[i];
				RadarDataParse[channel].state = DATA_STATE_TAIL3;
				break;
                
   		    case DATA_STATE_TAIL3:
                parseFinish = DataStateTail3Parse(buf[i], channel);
				break;
			
			default:
				RadarDataParse[channel].state = DATA_STATE_HEAD;
				break;

		}
	}

    return parseFinish;
}

#if defined(SUPPORT_DATA_PASSTHROUGH) || defined(SUPPORT_DYNAMIC_SYS_MODE)
static uint16_t nRecvCnt[CHANNEL_MAX] = {0};
void DataProc_ResetRecvCnt(void)
{
    memset(nRecvCnt, 0, sizeof(nRecvCnt));
}

static void StartDataTransfer(uint8_t* frameBuf, uint16_t bufLen, uint8_t channel, uint16_t index)
{
    uint8_t noCheckIndex = 1;
    uint16_t uploadSampleRate = 0;
    uint8_t chipCount = g_TxCount;
    
    uploadSampleRate = System_GetUploadSampleRate();
    if (DATA_TYPE_DFFT_PEAK == RadarPara.dataType)
    {
        uploadSampleRate = 1;
    }
    
	if(DATA_TYPE_FFT == RadarPara.dataType || DATA_TYPE_DSRAW == RadarPara.dataType)
	{
		noCheckIndex = 0;
	}

    if ((noCheckIndex || (nRecvCnt[channel] != 0 || index == 0)) &&
        (nRecvCnt[channel]++ % (uploadSampleRate * chipCount) < chipCount))	
    {   
#ifdef STM32_PLATFORM
        UsbTransfer_Send(frameBuf, bufLen); 
#else
        UART_Send2Buf(frameBuf, bufLen);        
#endif
	}

#ifdef XenD101
    if (DATA_TYPE_DFFT_PEAK == RadarPara.dataType || 
        ((DATA_TYPE_FFT == RadarPara.dataType) && (index == RadarPara.chirpNum - 3))) /*send data at this time will not lose data*/
    {
        g_uartSendTime = 1;
    }
#endif
}
#endif

void StartAlgorithm(uint8_t* dataBuf, uint16_t dataLen, uint8_t channel, uint16_t index)
{
    /*do algo here*/
#ifdef SUPPORT_RVA_CALC
    RVA_CalcRva(dataBuf, dataLen, channel, index, RadarPara.dataType);
#endif

#if defined(SUPPORT_RV_OUTPUT)
    ABD_DetectByPeakData(dataBuf, dataLen);
#elif defined(XenD101_HPS01)
    ABD_HumanPresenceSensing(dataBuf, dataLen);
#elif defined(SUPPORT_ABD)
    ABD_PrintStatus();
#endif

#ifdef SUPPORT_LOW_POWER
    Radar_EnterPDMode();
#ifdef MCU_LOW_POWER
    PWR_EnterSTOPMode(PWR_Regulator_LowPower, PWR_STOPEntry_WFI);
    SystemInit();
    Radar_EnterNormalMode();
#endif
#endif
}

static void DataDispatch(uint8_t* frameBuf, uint16_t bufLen, uint8_t channel, uint16_t index)
{
    uint8_t* dataBuf = NULL;
    uint16_t dataLen = 0;

    if (NULL == frameBuf || 0 == bufLen)
    {
        return;
    }
	
#ifdef SUPPORT_DATA_PASSTHROUGH
    if (System_GetSysMode() == SYS_MODE_PASSTHROUGH)
    {
        StartDataTransfer(frameBuf, bufLen, channel, index);
    }
    else
#endif
    {
        dataBuf = frameBuf + SPI_FRAME_HLEN;
        dataLen = bufLen - SPI_FRAME_HLEN - SPI_FRAME_TLEN;
        StartAlgorithm(dataBuf, dataLen, channel, index);
    }
}

static void DataProcess(uint8_t channel, uint8_t dmaFlag, uint8_t *recvBuf, uint16_t bufLen)
{
    uint8_t parseFinish = 0;
    uint16_t index = 0;
    uint16_t threshold = INDICATOR_RECV_THRESHOLD;

#if defined(MM32_PLATFORM) || defined(PY32_PLATFORM)
#ifndef MCU_LOW_POWER
    IWDG_Reload();
#endif
#endif
    if (channel >= CHANNEL_MAX || dmaFlag >= DMA_RECV_FLAG_MAX || NULL == recvBuf)
    {
        printf("Error para!\r\n");
        return;
    }
#ifndef PY32_PLATFORM	
    if (CmdProc_InCmdMode())
    {
        return;
    }
#endif
#ifdef STM32_PLATFORM
    CACHE_InvalDCache((uint32_t*)recvBuf, bufLen);
#endif
    parseFinish = DataParse(recvBuf, bufLen, channel);
    g_dataRecvFlag[channel][dmaFlag] = 0;
    if (!parseFinish)
    {
        return;
    }
    switch (RadarPara.dataType)
    {
		case DATA_TYPE_DSRAW:
        case DATA_TYPE_FFT:
            CheckChirpIndex(channel, RadarDataParse[channel].chirpIndex);
            index = RadarDataParse[channel].chirpIndex;
#ifdef XenD101
            threshold >>= INDICATOR_RECV_THD_FFT_SHIFT;
#endif
            break;
        
        case DATA_TYPE_DFFT:
            CheckFrameCnt(channel, RadarDataParse[channel].frameCnt);	
            index = RadarDataParse[channel].frameCnt;
            threshold >>= INDICATOR_RECV_THD_DFFT_SHIFT;
            break;
        
        case DATA_TYPE_DFFT_PEAK:
            threshold >>= INDICATOR_RECV_THD_DPEAK_SHIFT;
            break;
        
        default:
            break;
    }
    
    Indicator_RadarDataReceived(threshold);
    DataDispatch(RadarDataParse[channel].buf, RadarPara.dataLen, channel, index);
}

static int8_t GetRadarPara(RADAR_PARA_T *radarPara)
{
#if defined(DEVMB) || defined (EVBSN01)
    uint8_t dfftDataNum = 0;
    uint8_t dfftChirpNum = 0;
#endif

    if (NULL == radarPara)
    {
        return FAIL;
    }

    radarPara->dataType = Radar_GetDataType();
#if defined(CONFIG_DEBUG)
    printf("dataType: %d\r\n", radarPara->dataType);
#endif
    switch (radarPara->dataType)
    {
		case DATA_TYPE_DSRAW:
            radarPara->dataLen = Radar_GetRawPoint() * 2 * 2 + SPI_FRAME_HLEN + SPI_FRAME_TLEN;  /*16 bit, IQ*/
            break;
        
        case DATA_TYPE_FFT:
            radarPara->dataLen = Radar_GetFftPoint() * 2 * 2 + SPI_FRAME_HLEN + SPI_FRAME_TLEN;  /*16 bit, IQ*/
            break;
        
#if defined(DEVMB) || defined (EVBSN01)
        case DATA_TYPE_DFFT:
            dfftDataNum = Radar_GetDfftDataNum();
            dfftChirpNum = Radar_GetDfftChirpNum();
#if defined(CONFIG_DEBUG)
            printf("dfftDataNum: %d, dfftChirpNum: %d\r\n", dfftDataNum, dfftChirpNum);
#endif
            radarPara->dataLen = dfftDataNum * 2 * 2 * dfftChirpNum + SPI_FRAME_HLEN + SPI_FRAME_TLEN;  /*16 bit, IQ*/
            break;
#endif
        
        case DATA_TYPE_DFFT_PEAK:
            radarPara->dataLen = Radar_GetDfftPeakSize() + SPI_FRAME_HLEN + SPI_FRAME_TLEN;
            break;
        
        default:
            printf("Error: unsupport dataType\r\n");
            return FAIL;
    }

#if defined(CONFIG_DEBUG)
    printf("dataLen: %d\r\n", radarPara->dataLen);
#endif
    if (radarPara->dataLen > SPI_FRAME_LEN_MAX)
    {
        printf("Error: dataLen is too long\r\n");
        return FAIL;
    }

    radarPara->chirpNum = Radar_GetOneFrameChirpNum();
#if defined(CONFIG_DEBUG)
    printf("chirpNum: %d\r\n", radarPara->chirpNum);
#endif

    return OK;
}

void DataProc_Init(void)
{
    uint8_t channel = 0;
    
    memset(&RadarDataParse, 0 ,sizeof(RadarDataParse));
    memset(&RadarPara, 0 ,sizeof(RadarPara));

    if (FAIL == GetRadarPara(&RadarPara))
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }

    for (channel = 0; channel < CHANNEL_MAX; channel++)
    {
        RadarDataParse[channel].needCopyLen = RadarPara.dataLen - SPI_FRAME_HLEN - SPI_FRAME_TLEN;
    }

#ifdef STM32_PLATFORM
    if (DATA_TYPE_DFFT_PEAK == RadarPara.dataType)
    {
        UsbTransfer_DecUsbTxLen();
    }
#endif

    SPI_Init(RadarPara.dataLen * 2); /*radar data received by spi dma, ping-pang buffer*/
}

uint8_t DataProc_NeedReconfig(void)
{    
    uint8_t channel = 0;
    uint8_t needReconfig = 0;
    RADAR_PARA_T radarParaTmp = {0};
    
    if (FAIL == GetRadarPara(&radarParaTmp))
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }

    RadarPara.chirpNum = radarParaTmp.chirpNum;
    if ((radarParaTmp.dataType != RadarPara.dataType) || (radarParaTmp.dataLen != RadarPara.dataLen))
    {
        needReconfig = 1;
        RadarPara.dataType = radarParaTmp.dataType;
        RadarPara.dataLen = radarParaTmp.dataLen;
        memset(&RadarDataParse, 0 ,sizeof(RadarDataParse));
        for (channel = 0; channel < CHANNEL_MAX; channel++)
        {
            RadarDataParse[channel].needCopyLen = RadarPara.dataLen - SPI_FRAME_HLEN - SPI_FRAME_TLEN;
        }
    }

    return needReconfig;
}

uint8_t DataProc_GetRadarDataType(void)
{    
    return RadarPara.dataType;
}

uint16_t DataProc_GetRadarDataLen(void)
{    
    return RadarPara.dataLen;
}

#ifdef STM32_PLATFORM
osThreadId dataProcTaskHandle;
QueueHandle_t radarDataQueue[CHANNEL_MAX];

void DataProc_Send2RadarDataQueue(uint8_t channel, void *radarData)
{
	BaseType_t xHigherPriorityTaskWoken = 0;

    if (channel >= CHANNEL_MAX || NULL == radarData)
    {
        return;
    }
    
	if (xQueueSendFromISR(radarDataQueue[channel], radarData, &xHigherPriorityTaskWoken) != pdPASS) 
    {
		Indicator_RadarDataRecvOverFlow();
	}

	portYIELD_FROM_ISR(xHigherPriorityTaskWoken); 
}


void DataProcTask(void const * argument)
{
	RADAR_DATA_T radarData = {0};
    uint16_t loop = 0;
        
    while(1)
    { 
        for (loop = 0; loop < g_ChannelCount; loop++)
        {
            if (DATA_TYPE_DFFT_PEAK == RadarPara.dataType)
            {
                if (CHANNEL_1 == loop)
                {
                    continue;
                }
            }
            
            if (xQueueReceive(radarDataQueue[loop], &radarData, portMAX_DELAY))
            {
                DataProcess(loop, radarData.dmaFlag, radarData.buf, radarData.len);
            }
        }
    } 
}

void DataProc_TaskInit(void)
{
    uint16_t loop = 0;

    for (loop = 0; loop < CHANNEL_MAX; loop++)
    {
        radarDataQueue[loop] = xQueueCreate(RADAR_DATA_QUEUE_SIZE, sizeof(RADAR_DATA_T));
    }

    osThreadDef(dataProcTask, DataProcTask, osPriorityHigh, 0, DATA_PROC_STACK_SIZE);
    dataProcTaskHandle = osThreadCreate(osThread(dataProcTask), NULL);
    if (NULL == dataProcTaskHandle)
    {
        RunFailed((uint8_t *)__FILE__, __LINE__);
    }
}
#else
void DataProc_Recv(void)
{
    uint8_t channel = 0;
    uint8_t dmaFlag = 0;
    uint16_t dataPos = 0;

    for (dmaFlag = 0; dmaFlag < DMA_RECV_FLAG_MAX; dmaFlag++)
    {
        for (channel = 0; channel < g_ChannelCount; channel++)
        {
            while (!g_dataRecvFlag[channel][dmaFlag]);

            if (DMA_RECV_FLAG_MEM_0 == dmaFlag)
            {
                dataPos = 0;
            }
            else
            {
                dataPos = RadarPara.dataLen;
            }
            DataProcess(channel, dmaFlag, &g_dataRecvBuf[channel][dataPos], RadarPara.dataLen);
        }
    }
}

void DataProc_Send(void)
{    
#if defined(SUPPORT_DATA_PASSTHROUGH) || defined(SUPPORT_DYNAMIC_SYS_MODE)
    UART_Send();
#endif
}

#endif

