/**
  ******************************************************************************
  * @file           : abd.c
  * @author         : iclm team
  * @brief          : algorithm for area body detect
  ******************************************************************************
  */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "global_conf.h"
#include "platform.h"
#include "abd.h"
#include "utilities.h"

static uint16_t targetRangeBuf[RANGE_BUF_SIZE] = {0};
static DfftPeakData_t dfftPeakData = {0};
static uint8_t targetOffBuf[TAR_OFF_BUF_SIZE];

static uint16_t sig_sec[ROI_MAX + 1] = {0};
static uint16_t sig_buffer[SLIDE_WINDOW_SIZE][ROI_MAX + 1] = {0};
static uint16_t sig_sensing[ROI_MAX + 1] = {0};

static ABD_PARA_T abdPara __ALIGN(4) =
{
    ROI_MAX,
    0,
    VALID_RANGE,
    EXIT_THD,
    HOLD_TICKS
};

static uint16_t sensitivityMap[SENSITIVITY_LEVEL_MAX] = 
{
    0,
    SENSITIVITY_LEVEL_1_THD,
    SENSITIVITY_LEVEL_2_THD,
    SENSITIVITY_LEVEL_3_THD
};

static const uint16_t MovingTargetTh[MOV_TAR_TH_NUM] = 
{
#if defined (FFT_ZERO_FILL_10CM)
    100,/* idx 0 */
    100,/* idx 1 */
    100,/* idx 2 */
    100,/* idx 3 */
    100,/* idx 4 */
    100,/* idx 5 */
    100,/* idx 6 */
    100,/* idx 7 */
    100,/* idx 8 */
    100,/* idx 9 */
    100,/* idx 10 */
    50,/* idx 11 */
    50,/* idx 12 */
    50,/* idx 13 */
    50,/* idx 14 */
    0,  /* idx 15 */
    0,  /* idx 16 */
    0,  /* idx 17 */
    0,  /* idx 18 */
    0,  /* idx 19 */
    0,  /* idx 20 */
    0,  /* idx 21 */
    0,  /* idx 22 */
    0,  /* idx 23 */
    0,  /* idx 24 */
    0,  /* idx 25 */
    0,  /* idx 26 */
    0,  /* idx 27 */
    0,  /* idx 28 */
    0,  /* idx 29 */
    0,  /* idx 30 */
    0   /* idx 31 */  
#elif defined (FFT_ZERO_FILL_20CM)
    200,/* idx 0 */
    200,/* idx 1 */
    200,/* idx 2 */
    200,/* idx 3 */
    200,/* idx 4 */
    200,/* idx 5 */
    200,/* idx 6 */
    200,/* idx 7 */
    200,/* idx 8 */
    200,/* idx 9 */
    200,/* idx 10 */
    200,/* idx 11 */
    200,/* idx 12 */
    200,/* idx 13 */
    200,/* idx 14 */
    100,  /* idx 15 */
    100,  /* idx 16 */
    100,  /* idx 17 */
    100,  /* idx 18 */
    100,  /* idx 19 */
    100,  /* idx 20 */
    100,  /* idx 21 */
    100,  /* idx 22 */
    100,  /* idx 23 */
    100,  /* idx 24 */
    100,  /* idx 25 */
    0,  /* idx 26 */
    0,  /* idx 27 */
    0,  /* idx 28 */
    0,  /* idx 29 */
    0,  /* idx 30 */
    0   /* idx 31 */
#else
    2000,/* idx 0 */
    2000,/* idx 1 */
    2000,/* idx 2 */
    1500,/* idx 3 */
    1300,/* idx 4 */
    1200,/* idx 5 */
    1200,/* idx 6 */
    1600,/* idx 7 */
    1600,/* idx 8 */
    1600,/* idx 9 */
    1300,/* idx 10 */
    500,/* idx 11 */
    400,/* idx 12 */
    350,/* idx 13 */
    350,/* idx 14 */
    730,/* idx 15 */ 
    730,/* idx 16 */ 
    730,/* idx 17 */ 
    730,/* idx 18 */ 
    730,/* idx 19 */ 
    730,/* idx 20 */ 
    730,/* idx 21 */ 
    730,/* idx 22 */ 
    730,/* idx 23 */ 
    730,/* idx 24 */ 
    730,/* idx 25 */ 
    730,/* idx 26 */ 
    730,/* idx 27 */ 
    730,/* idx 28 */ 
    730,/* idx 29 */ 
    730,/* idx 30 */ 
    730 /* idx 31 */  
#endif
 };

void rangeGearParaUpdate(int32_t val)
{
    switch (val)  
    {		
       case RANGE_GEAR_1_5_M:
            abdPara.roiMax = ROI_MAX_1_5_M;
            abdPara.validRange = VALID_RANGE_1_5_M;
            break;
            
       case RANGE_GEAR_1_0_M:
            abdPara.roiMax = ROI_MAX_1_0_M;
            abdPara.validRange = VALID_RANGE_1_0_M;
            break;
            
       case RANGE_GEAR_0_6_M:
            abdPara.roiMax = ROI_MAX_0_6_M;
            abdPara.validRange = VALID_RANGE_0_6_M;
            break;

        default:
            break;
    }  
}

void* ABD_GetParaAddr(void)
{
    return (void*)&abdPara;
}

uint32_t ABD_GetParaLen(void)
{
    return sizeof(abdPara);
}

int8_t ABD_ParaUpdate(uint16_t type, int32_t val)
{    
    switch (type)  
    {		
       case ABD_RANGE_GEAR:
            rangeGearParaUpdate(val);
            break;
       
       case ABD_SENSITIVITY_LEVEL:
            if (val < SENSITIVITY_LEVEL_MAX && val > 0)
            {
                abdPara.exitThd = sensitivityMap[val];
            }
            break;
            
       case ABD_LAG_TIME:
            abdPara.holdTicks = val * FRAME_FREQ;
            break;
       
        default:
            return -1;
    }

    return 0;
}

static void GetDfftPeakData(uint8_t *data_buf, DfftPeakData_t *pData)
{
    if (NULL == data_buf || NULL == pData)
    {
        return;
    }
    
    for(uint16_t i = 0; i < DFFT_PEAK_OUTPUT_NUM; i++)
    {
       pData->v_idx[i] = data_buf[12 * i + 0]; 
       pData->r_idx[i] = data_buf[12 * i + 1];
       pData->mag[i] = (uint32_t)((data_buf[12 * i + 4] << 24) | 
                                  (data_buf[12 * i + 5] << 16) |
                                  (data_buf[12 * i + 6] << 8) |
                                  (data_buf[12 * i + 7]));
    }
}

static TargetStatus_t GetTargetStatus(DfftPeakData_t *pData)
{    
    if (NULL == pData)
    {
        return TAR_OFF;
    }

    if(pData->mag[0] > MovingTargetTh[pData->r_idx[0]]) /* 能量值很高，很有可能是运动目标 */
    {
        return TAR_MOVING;
    }

    /* 能量值不高，但是有目标，此时目标可能处于静止状态 */
	#ifdef PY32_PLATFORM
    if(HAL_GPIO_ReadPin(TSPI_IO_PORT, TSPI_IO_PIN))
	#else
    if(GPIO_ReadInputDataBit(TSPI_IO_PORT, TSPI_IO_PIN))
    #endif
    {
        return TAR_STOP;
    }
    
    /* SOC无法检测到目标，判断为无人 */
    return TAR_OFF;
 }

void PrintTargetInfo(DfftPeakData_t *pData)
{   
    if (NULL == pData)
    {
        return;
    }

    printf("r:%u v:%u val:%u\r\n", pData->r_idx[0], pData->v_idx[0], pData->mag[0]);  
 }

uint16_t GetTargetRange(DfftPeakData_t *pData, uint8_t *lastRidx)
{      
    if (NULL == lastRidx || NULL == pData)
    {
        return 0;
    }

#ifdef CENTER_GRAVITY
    int32_t left_level = 0;
    int32_t right_level = 0;
    int32_t level = 0;
  
    //uint8_t v_idx = 0;
    uint8_t  r_idx = 0;
    uint8_t idx_tmp = 0;
     
    uint8_t flag_tmp = 0;
    float   distance = 0;
    float   d = 0;
    
    //v_idx = pData->v_idx[0];//速度索引
    r_idx = pData->r_idx[0];//距离索引
    level = (int32_t)(pData->mag[0]);//能量值
    
	
	for(uint8_t i = 1; i < DFFT_PEAK_OUTPUT_NUM; i++)
    {
        idx_tmp = pData->r_idx[i];    
        if( idx_tmp == r_idx - 1 )//取左侧距离门的能量值
        { 
           flag_tmp |= 0x01;
           left_level = (int32_t)(pData->mag[i]);//能量值  
           break;
        }
    }
		
	for(uint8_t i = 1; i < DFFT_PEAK_OUTPUT_NUM; i++)
    {
        idx_tmp = pData->r_idx[i];
     
        if(idx_tmp == r_idx + 1)//取右侧距离门的能量值
        { 
           flag_tmp |= 0x02;
           right_level = (int32_t)(pData->mag[i]);//能量值  
           break;
        }
    }
    
    if((flag_tmp & 0x01) == 0x00)
    {
        left_level = 0;
    }
    
    if((flag_tmp & 0x02) == 0x00)
    {
        right_level = 0;
    }
    
    d =     (float)(right_level - left_level) 
          / (float)(2 * (2 * level - left_level - right_level));
    distance = (((float)r_idx + d) * (float)RANGE_RESOLUTION);
    	
    return distance;
#else
    static uint8_t ignoreCnt = 0;

    if (*lastRidx != 0 && abs(pData->r_idx[0] - *lastRidx) > IGNORE_RANGE_DIFF)
    {
        if (ignoreCnt > IGNORE_NUM)
        {
            ignoreCnt = 0;
        }
        else
        {
            //printf("ignore ridx:%u\r\n", pData->r_idx[0]);
            ignoreCnt++;
            pData->r_idx[0] = *lastRidx;
        }
    }
    else
    {
        ignoreCnt = 0;
    }
    *lastRidx = pData->r_idx[0];

    return ((pData->r_idx[0]) * RANGE_RESOLUTION );
#endif
}

static uint8_t IsConditonMeet(uint8_t *contentBuf, uint16_t bufSize, uint16_t condition)
{
    uint16_t i = 0;
    uint16_t sum = 0;
    
    if (NULL == contentBuf)
    {
        return 0;
    }

    for (i = 0; i < bufSize; i++)
    {
        if (contentBuf[i])
        {
            sum++;
        }
    }

    if (sum >= condition)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}


void ABD_DetectByPeakData(uint8_t* dataBuf, uint16_t dataLen)
{
    static uint8_t k = 0;
    static uint8_t pos = 0;
    static uint8_t tOffIdx = 0;
    static uint8_t lastRidx = 0;
    static uint16_t lastRange = 0;
    uint8_t tarOff = 0;
    uint16_t thisRange = 0;
    TargetStatus_t targetStatus = TAR_OFF;

    if (NULL == dataBuf)
    {
        return;
    }
    
    GetDfftPeakData(dataBuf, &dfftPeakData);
    
#if 0
    PrintTargetInfo(&dfftPeakData);    
    return;
#endif

    targetStatus = GetTargetStatus(&dfftPeakData);
    if (targetStatus == TAR_OFF)
    {
        targetOffBuf[tOffIdx] = 1;
    }
    else
    {
        targetOffBuf[tOffIdx] = 0;
    }
    tOffIdx = (++tOffIdx) % TAR_OFF_BUF_SIZE;

    if(TAR_MOVING == targetStatus)
    {
        memset(targetOffBuf, 0, sizeof(targetOffBuf));
    }
    
    tarOff = IsConditonMeet(targetOffBuf, TAR_OFF_BUF_SIZE, TAR_OFF_TH);
    if (tarOff)
    {
        printf("---OFF---\r\n");
        k = 0;
        pos = 0;
        lastRange = 0;
        lastRidx = 0;
        memset(targetRangeBuf, 0, sizeof(targetRangeBuf));

        return; 
    }
    
    if(TAR_MOVING == targetStatus)
    {        
        targetRangeBuf[k] = GetTargetRange(&dfftPeakData, &lastRidx);
        k = (++k) % RANGE_BUF_SIZE;
                       
        pos = (pos >= RANGE_BUF_SIZE ? RANGE_BUF_SIZE : ++pos);
        for(uint8_t i = 0; i < pos; i++)
        {
            thisRange += targetRangeBuf[i];
        }  
        thisRange /= pos;
        
        lastRange = thisRange;     
    }
    else
    {
        thisRange = lastRange ; 
    }
    
    if (thisRange == 0)
    {
        printf("---ON---\r\n");
    }
    else
    {
        printf("---ON---(R = %d)\r\n", thisRange);
    }
 }

void ABD_Init(void)
{
    IoOutInit();
    IoOutOff();
    TspiIoInit();
    //ExtixInit();
}

void ABD_PrintStatus(void)
{
    static uint16_t tickCnt = 0;
    bool status = 0;
	
	#ifdef PY32_PLATFORM
    status = HAL_GPIO_ReadPin(TSPI_IO_PORT, TSPI_IO_PIN);
	#else
    status = GPIO_ReadInputDataBit(TSPI_IO_PORT, TSPI_IO_PIN);
    #endif

    if (status)
    {
        IoOutOn();
    }
    else
    {
        IoOutOff();
    }

    tickCnt++;
    if (tickCnt % ABD_CHECK_INTERVAL == 0)
    {
        if (status)
        {
            printf("ON\r\n");
        }
        else
        {
            printf("OFF\r\n");
        }
    }
}

uint32_t lsp(uint32_t n)
{
	uint32_t l;
    if (n & 0x8000)
        l = 120 + ((n & 0x7000) >> 12);
    else if(n & 0x4000)
        l = 112 + ((n & 0x3800) >> 11);
    else if( n & 0x2000)
        l = 104 + ((n & 0x1C00) >> 10);
    else if( n & 0x1000)
        l = 96 + ((n & 0x0E00) >> 9);
    else if( n & 0x0800)
        l = 88 + ((n & 0x0700) >> 8);
    else if( n & 0x0400)
        l = 80 + ((n & 0x0380) >> 7);
    else if( n & 0x0200)
        l = 72 + ((n & 0x01C0) >> 6);
    else if( n & 0x0100)
        l = 64 + ((n & 0x00E0) >> 5);
    else if( n & 0x0080)
        l = 56 + ((n & 0x0070) >> 4);
    else if( n & 0x0040)
        l = 48 + ((n & 0x0038) >> 3);
    else if( n & 0x0020)
        l = 40 + ((n & 0x001C) >> 2);
    else if( n & 0x0010)
        l = 32 + ((n & 0x000E) >> 1);
    else if( n & 0x0008)
        l = 24 + ((n & 0x0007) >> 0);
    else if( n & 0x0004)
        l = 16 + ((n & 0x0003) << 1);
    else if( n & 0x0002)
        l = 8 + ((n & 0x0001) << 2);
    else
        l = 0;
    return l;
}

uint16_t GetRangeByCenterGravity(DfftPeakData_t *pData)
{
    int32_t left_level = 0;
    int32_t right_level = 0;
    int32_t level = 0;
  
    //uint8_t v_idx = 0;
    uint8_t  r_idx = 0;
    uint8_t idx_tmp = 0;
     
    uint8_t flag_tmp = 0;
    float   d = 0;
    uint16_t distance = 0;
    
    //v_idx = pData->v_idx[0];//速度索引
    r_idx = pData->r_idx[0];//距离索引
    level = (int32_t)(pData->mag[0]);//能量值
    if (level == 0)
    {
        return 0;
    }
    
	
	for(uint8_t i = 1; i < DFFT_PEAK_OUTPUT_NUM; i++)
    {
        idx_tmp = pData->r_idx[i];    
        if( idx_tmp == r_idx - 1 )//取左侧距离门的能量值
        { 
           flag_tmp |= 0x01;
           left_level = (int32_t)(pData->mag[i]);//能量值  
           break;
        }
    }
		
	for(uint8_t i = 1; i < DFFT_PEAK_OUTPUT_NUM; i++)
    {
        idx_tmp = pData->r_idx[i];
     
        if(idx_tmp == r_idx + 1)//取右侧距离门的能量值
        { 
           flag_tmp |= 0x02;
           right_level = (int32_t)(pData->mag[i]);//能量值  
           break;
        }
    }
    
    if((flag_tmp & 0x01) == 0x00)
    {
        left_level = 0;
    }
    
    if((flag_tmp & 0x02) == 0x00)
    {
        right_level = 0;
    }
    
    d =     (float)(right_level - left_level) 
          / (float)(2 * (2 * level - left_level - right_level));

    distance = (uint16_t)(((float)r_idx + d) * (float)RANGE_RESOLUTION);
    	
    return distance;
}
void ABD_HumanPresenceSensing(uint8_t* dataBuf, uint16_t dataLen)
{
    static uint8_t fcnt = 0;
    static uint8_t bufpt = 0;
    static uint8_t on_off_status = 0;
    static uint16_t sigMax = 0;
    static uint16_t on_cnt = 0;

    uint8_t i = 0;
    uint8_t motion_status = 0;
    uint8_t motionless_status = 0;
    uint16_t range = 0;

	GetDfftPeakData(dataBuf, &dfftPeakData);

    fcnt = (++fcnt) % FRAME_ACC_NUM;
    if (fcnt == 0)
    {
        memset(sig_sec, 0, sizeof(sig_sec));
    }

    range = GetRangeByCenterGravity(&dfftPeakData);
#ifdef ABD_DEBUG
	printf("range:%d\r\n", range);
    printf("r:%d v:%d mag:%d\r\n", dfftPeakData.r_idx[0], dfftPeakData.v_idx[0], dfftPeakData.mag[0]);
    //printf("roi:%d vR:%d eT:%d HT:%d\r\n", abdPara.roiMax, abdPara.validRange, abdPara.exitThd, abdPara.holdTicks);
#endif
    if (range < abdPara.validRange)
    {
        if ((dfftPeakData.mag[0] > ENTER_THD) && (dfftPeakData.r_idx[0] <= abdPara.roiMax))
        {
            dfftPeakData.mag[0] = ENTER_THD;
            motion_status = 1;
        }
        else
        {
            dfftPeakData.mag[0]++;
        }
		
		if((dfftPeakData.v_idx[0] == V_STATIC) && (dfftPeakData.r_idx[0] <= abdPara.roiMax))
		{
			sig_sec[dfftPeakData.r_idx[0]] += lsp(dfftPeakData.mag[0]);
		}
    }
   
	if(fcnt == (FRAME_ACC_NUM - 1))
	{		
		for(i = 0; i <= abdPara.roiMax; i++)
		{
			sig_sensing[i] -= sig_buffer[bufpt][i];
			sig_sensing[i] += sig_sec[i];
			sig_buffer[bufpt][i] = sig_sec[i];
		}

		bufpt = (++bufpt) % SLIDE_WINDOW_SIZE;	

        for(i = 0; i <= abdPara.roiMax; i++)
		{
            if (sig_sensing[i] > sigMax)
            {
                sigMax = sig_sensing[i];
            }
		}
        sigMax >>= 3;
          
	}	
#ifdef ABD_DEBUG
    printf("sigMax:%d\r\n", sigMax);
#endif

    if (sigMax > abdPara.exitThd)
    {
        motionless_status = 1;
    }

    if (motion_status || motionless_status)
    {
        on_off_status = 1;
        on_cnt = 0;
    }
    else
    {
        if (on_off_status)
        {
            if (on_cnt < abdPara.holdTicks)
            {
                on_cnt++;
            }
            else
            {
                on_off_status = 0;
            }
        }
    }

    if (on_off_status)
    {
        IoOutOn();
        printf("ON\r\n");
    }
    else
    {
        IoOutOff();
        printf("OFF\r\n");
    }
}


