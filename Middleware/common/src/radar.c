/**
  ******************************************************************************
  * @file           : radar.c
  * @author         : iclm team
  * @brief          : radar driver
  ******************************************************************************
  */
#include <stdio.h>
#include "platform.h"
#include "radar.h"
#include "config.h"
#include "radar_para.c"
#include "utilities.h"

uint8_t g_ChannelCount = CHANNEL_MAX;
uint8_t g_maxChannelCount = CHANNEL_MAX;
uint8_t g_TxCount = 1;

static const uint16_t rawPointMap[RAW_MAP_NUM] = 
{
	RAW_POINT_64,
    RAW_POINT_128,
    RAW_POINT_256,
    RAW_POINT_512,
    RAW_POINT_1024
};

#ifndef XenD101_1815_ULP_BOARD
static const RADAR_REG_T RegListPD[] =
{
    {0x70, 0x1020},
    {0x6C, 0x8880},
    {0x6D, 0x8800},
    {0x72, 0x0650},
    {0x67, 0x0000},
    {0x66, 0xF0F0},
    {0x6E, 0x03FC},    
    {0x41, 0xC864},	 
    {0x00, 0x0000} /*must be last, do not delete!!!*/
};
#endif

static const RADAR_REG_T RegListNormal[] =
{
#if 1 //old
    {0x41, 0xC874}, 
    {0x72, 0x0613},
    {0x6C, 0x9990},
    {0x6D, 0x99C0},
    {0x70, 0x2EA0},
    {0x6E, 0x8BFC},
    {0x66, 0xF8F0},
    {0x67, 0x1040},  
    {0x69, 0x0006},
    {0x00, 0x0000} /*must be last, do not delete!!!*/
#else //new from zhoushijun
    {0x41, 0xc864},
    {0x72, 0x0653},
    {0x6C, 0x9990},
    {0x6D, 0x9940},
    {0x70, 0x32a0},
    {0x6E, 0xabFC},
    {0x66, 0x0a00},
    {0x67, 0x1840},
    {0x00, 0x0000} /*must be last, do not delete!!!*/
#endif
};

#ifdef SUPPORT_DFFT_PEAK_CONFIG
static RADAR_REG_T RegListDfftPeak[DFFT_PEAK_REG_NUM + 1];
static PEAK_PARA_T peakPara __ALIGN(4) = 
{
    MIN_RANGE_IDX,
    MAX_RANGE_IDX,
    ACTIVE_FRAME_NUM,
    INACTIVE_FRAME_NUM,
    {
#if 0 //挂壁南京办公室十二楼楼道
        0x0150,/*0x20*/    
        0x0150,/*0x21*/
        0x0150,/*0x22*/
        0x0148,/*0x23*/
        0x0145,/*0x24*/
        0x0145,/*0x25*/
        0x0143,/*0x26*/
        0x0143,/*0x27*/
        0x0135,/*0x28*/    //6?
        0x0130,/*0x29*/
        0x0130,/*0x2A*/    //ok
        0x0124,/*0x2B*/    //ok
        0x0120,/*0x2C*/
        0x0120,/*0x2D*/
        0x0120,/*0x2E*/
        0x00F0,/*0x2F*/
#endif

#if 0 //挂壁南京办公室一楼大厅
        0x0150,/*0x20*/    
        0x0150,/*0x21*/
        0x0150,/*0x22*/
        0x0148,/*0x23*/
        0x0145,/*0x24*/
        0x0145-15,/*0x25*/
        0x0143-35,/*0x26*/
        0x0143-40,/*0x27*/
        0x0135-45,/*0x28*/    //6?
        0x0130-45,/*0x29*/
        0x0130-45,/*0x2A*/    //ok
        0x0124-40,/*0x2B*/    //ok
        0x0120-40,/*0x2C*/
        0x0120-40,/*0x2D*/
        0x0120-40,/*0x2E*/
        0x00F0-90,/*0x2F*/
#endif

#if 0 //挂壁南京办公室二楼大会议室 (由于15m左右有大音响，导致2F寄存器无法调出最优值)
        0x012C + 300,/*0x20*/		//600
        0x012C + 300,/*0x21*/		//600
        0x012C + 300,/*0x22*/		//600
        0x012C + 300,/*0x23*/		//600
        0x00FA + 300,/*0x24*/		//550
        0x00FA + 300 - 10,/*0x25*/		//550
        0x00FA + 300 - 30,/*0x26*/		//550
        0x00FA + 200 - 10,/*0x27*/		//450
        0x00C8 + 200 - 10,/*0x28*/		//400
        0x00C8 + 200 - 20,/*0x29*/		//400
        0x00C8 + 200 - 30,/*0x2A*/		//400
        0x00C8 + 200 - 40,/*0x2B*/		//400
        0x00C8 + 200 - 40,/*0x2C*/		//400
        0x00BE + 200 - 40,/*0x2D*/		//390
        0x00BE + 200 - 40,/*0x2E*/		//390
        0x00BE + 100 - 10,/*0x2F*/		//290
#endif

#if 0 //挂顶南京办公室十二楼楼道
        0x0150,/*0x20*/    
        0x0150,/*0x21*/
        0x0150,/*0x22*/
        0x0148,/*0x23*/
        0x0145,/*0x24*/
        0x0145-15,/*0x25*/
        0x0143-35,/*0x26*/
        0x0143-40,/*0x27*/
        0x0135-220,/*0x28*/    
        0x0130-220,/*0x29*/
        0x0130-220,/*0x2A*/   // 70~80
        0x0124-220,/*0x2B*/   
        0x0120-220,/*0x2C*/
        0x0120-220,/*0x2D*/
        0x0120-220,/*0x2E*/
        0x00F0,/*0x2F*/ //-90
#endif

#if 0 //挂顶南京紫光展锐二楼托育中心
        0x0150,/*0x20*/    
        0x0150,/*0x21*/
        0x0150,/*0x22*/
        0x0148,/*0x23*/
        0x0145,/*0x24*/
        0x0145-15,/*0x25*/
        0x0143-35,/*0x26*/
        0x0143-40,/*0x27*/
        0x0135-50,/*0x28*/    
        0x0130-50,/*0x29*/
        0x0130-50,/*0x2A*/
        0x0124-70,/*0x2B*/   
        0x0120-70,/*0x2C*/
        0x0120-70,/*0x2D*/
        0x0120-70,/*0x2E*/
        0x00F0 + 1000,/*0x2F*/ //-90
#endif

#if 0 //挂顶南京空旷厂房  
        0x0150,/*0x20*/    
        0x0150,/*0x21*/
        0x0150,/*0x22*/
        0x0148,/*0x23*/
        0x0145,/*0x24*/
        0x0145-15,/*0x25*/
        0x0143-35,/*0x26*/
        0x0143-40,/*0x27*/
        0x0135-90,/*0x28*/    
        0x0130-90,/*0x29*/
        0x0130-90,/*0x2A*/  //100 ON
        0x0124-100,/*0x2B*/   
        0x0120-100,/*0x2C*/
        0x0120+ 1000,/*0x2D*/
        0x0120+ 1000,/*0x2E*/
        0x00F0 + 1000,/*0x2F*/ 
#endif

#if 0 //挂顶南京办公室巨人会议室
        0x0150,/*0x20*/    
        0x0150,/*0x21*/
        0x0150,/*0x22*/
        0x0148+500,/*0x23*/
        0x0145+500,/*0x24*/
        0x0145+500,/*0x25*/
        0x0143+500,/*0x26*/
        0x0143+500,/*0x27*/
        0x0135-50,/*0x28*/    
        0x0130-70,/*0x29*/      
        0x0130+300,/*0x2A*/     
        0x0124+300,/*0x2B*/   
        0x0120+300,/*0x2C*/     
        0x0120+300,/*0x2D*/
        0x0120+300,/*0x2E*/
        0x00F0 +300,/*0x2F*/ 
#endif

#if 1 // default
#ifdef FFT_ZERO_FILL_10CM
#ifdef COVER_PANEL //HUAWEI 裸板+玻璃
        30000*RATIO,   //rangeGate 0    *1000
        10000*RATIO,    //rangeGate 1   *1000
        30000*RATIO,    //rangeGate 2
        10000*RATIO,    //rangeGate 3
        1700*RATIO,    //rangeGate 4
        550*RATIO,    //rangeGate 5
        400*RATIO,    //rangeGate 6
        400*RATIO,    //rangeGate 7
        400*RATIO,    //rangeGate 8
        300*RATIO,    //rangeGate 9
        250*RATIO,    //rangeGate 10
        200*RATIO1,    //rangeGate 11
        200*RATIO1,    //rangeGate 12
        200*RATIO1,    //rangeGate 13
        200*RATIO1,    //rangeGate 14
        250*RATIO2     //rangeGate 15
#else  //HUAWEI 裸板
        30000,   //rangeGate 0    *1000
        10000,    //rangeGate 1   *1000
        30000,    //rangeGate 2
        10000,    //rangeGate 3
        1700,    //rangeGate 4
        550,    //rangeGate 5
        400,    //rangeGate 6
        400,    //rangeGate 7
        400,    //rangeGate 8
        300,    //rangeGate 9
        250,    //rangeGate 10
        200,    //rangeGate 11
        200,    //rangeGate 12
        200,    //rangeGate 13
        200,    //rangeGate 14
        250     //rangeGate 15
#endif
#elif defined (FFT_ZERO_FILL_20CM)
        30000,   //rangeGate 0   *1000
        1000,    //rangeGate 1   *1000
        500,     //rangeGate 2   *1000
        8000,    //rangeGate 3
        5000,    //rangeGate 4
        600,    //rangeGate 5
        500,    //rangeGate 6
        500,    //rangeGate 7
        500,    //rangeGate 8
        500,    //rangeGate 9
        400,    //rangeGate 10
        400,    //rangeGate 11
        400,    //rangeGate 12
        400,    //rangeGate 13
        400,    //rangeGate 14
        400     //rangeGate 15
#elif defined (CENTER_GRAVITY)
        12000,   //rangeGate 0    *4
        1200,    //rangeGate 1    *4
        1200,    //rangeGate 2
        1200,    //rangeGate 3
        1200,    //rangeGate 4
        1200,    //rangeGate 5
        1200,    //rangeGate 6
        450,    //rangeGate 7
        400,    //rangeGate 8
        400,    //rangeGate 9
        400,    //rangeGate 10
        400,    //rangeGate 11
        400,    //rangeGate 12
        390,    //rangeGate 13
        390,    //rangeGate 14
        290     //rangeGate 15
#elif defined (XenD101_HPS01)
#ifdef XenD101_HPS01_1G		
        30000,   //rangeGate 0   *1000
        1950,    //rangeGate 1   *1000
        400,     //rangeGate 2   *1000
        10500,    //rangeGate 3
        4000,    //rangeGate 4
        480,    //rangeGate 5
        420,    //rangeGate 6
        420,    //rangeGate 7
        420,    //rangeGate 8
        420,    //rangeGate 9
        280,    //rangeGate 10
        280,    //rangeGate 11
        280,    //rangeGate 12
        280,    //rangeGate 13
        280,    //rangeGate 14
        280     //rangeGate 15
#else
        30000,   //rangeGate 0   *1000
        10,    //rangeGate 1   *1000
        1,     //rangeGate 2   *1000
        600,    //rangeGate 3
        600,    //rangeGate 4
        600,    //rangeGate 5
        600,    //rangeGate 6
        600,    //rangeGate 7
        600,    //rangeGate 8
        600,    //rangeGate 9
        600,    //rangeGate 10
        600,    //rangeGate 11
        600,    //rangeGate 12
        600,    //rangeGate 13
        600,    //rangeGate 14
        600     //rangeGate 15
#endif
#else
        1200,   //rangeGate 0    *4
        750,    //rangeGate 1    *4
        600,    //rangeGate 2
        600,    //rangeGate 3
        550,    //rangeGate 4
        550,    //rangeGate 5
        550,    //rangeGate 6
        450,    //rangeGate 7
        400,    //rangeGate 8
        400,    //rangeGate 9
        400,    //rangeGate 10
        400,    //rangeGate 11
        400,    //rangeGate 12
        390,    //rangeGate 13
        390,    //rangeGate 14
        290     //rangeGate 15
#endif
#endif
    }
};
#endif

uint16_t Radar_GetFftPoint(void)
{
	uint16_t regVal = 0;
    I2C_Read(I2C_ADDR_RADAR_Chip0, RADAR_DIG_FFT_NUM, &regVal);
	return regVal;
}

uint16_t Radar_GetRawPoint(void)
{
    uint16_t val = 0;
    uint16_t rawVal = 0;
    
    I2C_Read(I2C_ADDR_RADAR_Chip0, RADAR_DIG_RAW_PEAK_NUM, &val);
    rawVal = (val >> RADAR_RAW_POS) & RADAR_RAW_MASK;

    if (rawVal < RAW_MAP_NUM)
    {
        return rawPointMap[rawVal];
    }
    else
    {
        return RAW_POINT_64;
    }
}

uint16_t Radar_GetOneFrameChirpNum(void)
{
	uint16_t val = 0;
    I2C_Read(I2C_ADDR_RADAR_Chip0, RADAR_PAT_CHIRP_NUM, &val);
	return val;
}

uint16_t Radar_GetDfftDataNum(void)
{
    uint16_t val = 0;
    
    I2C_Read(I2C_ADDR_RADAR_Chip0, RADAR_DIG_DFFT_DATA_NUM, &val);

    return (val >> RADAR_DFFT_DATA_NUM_POS);
}

uint16_t Radar_GetDfftPeakSize(void)
{
    uint16_t val = 0;
    
    I2C_Read(I2C_ADDR_RADAR_Chip0, RADAR_DIG_RAW_PEAK_NUM, &val);

    return ((val & RADAR_PEAK_MASK) * 4); /*4--word length*/
}

uint16_t Radar_GetDfftChirpNum(void)
{
    uint16_t val = 0;
    
    I2C_Read(I2C_ADDR_RADAR_Chip0, RADAR_DIG_DFFT_CHIRP_NUM, &val);

    return (val >> RADAR_DFFT_CHIRP_NUM_POS);
}

uint8_t Radar_GetDataType(void)
{
    uint8_t dataType = 0;
    uint16_t val = 0;
    
    I2C_Read(I2C_ADDR_RADAR_Chip0, RADAR_DIG_FUN_SWITCH, &val);

    if (val & RADAR_DFFT_DATA)
    {
        dataType = DATA_TYPE_DFFT;
    }
    else if (val & RADAR_FFT_DATA)
    {
        dataType = DATA_TYPE_FFT;
    }
    else if (val & RADAR_DFFT_PEAK_DATA)
    {
        dataType = DATA_TYPE_DFFT_PEAK;
    }
	else if (val & RADAR_DSRAW_DATA)
    {
        dataType = DATA_TYPE_DSRAW;
    }
    else
    {
        dataType = DATA_TYPE_MAX;
    } 

    return dataType;
}

#ifdef DEVMB
static void ConfigRadarChip1(void)
{
    uint16_t loop = 0;
    uint16_t nErrCount = 0;

    while(InitRegList[loop].addr) 
    {
        uint8_t regAddr = (uint8_t)(InitRegList[loop].addr); 
        uint16_t regVal = InitRegList[loop].val;
        
        if (loop > REG_FIX_NUM)
        {
            if (InitRegList[loop].addr == 0x5E)
            {
                regVal = 0x5500;
            }
            else if (InitRegList[loop].addr == 0x41)
            {
                regVal = 0xE874;
            }
            else if (InitRegList[loop].addr == 0x72)
            {
                regVal = 0x0653;
            }
        }   
        if(I2C_Write(I2C_ADDR_RADAR_Chip1, regAddr, regVal) == HAL_ERROR)
        {
            nErrCount ++;
        }      
        if(nErrCount > 3 && loop < 5)
        {
            g_maxChannelCount = CHANNEL_2;
            g_ChannelCount = g_maxChannelCount;
            break;
        }
        
        loop++;
    } 
}
#endif

#ifdef SUPPORT_DFFT_PEAK_CONFIG
void ConfigDfftPeakPara(void)
{
    uint16_t loop = 0;
    uint16_t k = 0;  
    uint16_t shiftVal = 0;
    uint16_t thTmp = 0;

    RegListDfftPeak[loop].addr = RADAR_2DFFT_PEAK_RANGE_IDX;
    RegListDfftPeak[loop].val = (peakPara.roiMax << 8) | peakPara.roiMin;
    loop++;

    RegListDfftPeak[loop].addr = RADAR_2DFFT_PEAK_VELOCITY_IDX;
    RegListDfftPeak[loop].val = (MAX_VELOCITY_IDX << 8) | MIN_VELOCITY_IDX;
    loop++;

    RegListDfftPeak[loop].addr = RADAR_2DFFT_PEAK_TAR_FRAME_NUM;
    RegListDfftPeak[loop].val = (peakPara.activeFrameNum << 8) | peakPara.inactiveFrameNum;
    loop++;
    
    for (k = 0; k < RANGE_THRESHOLD_REG_NUM; k++)
    {
        RegListDfftPeak[loop].addr =  RANGE_THRESHOLD_BASE + k;
        shiftVal = 0;
        thTmp = peakPara.threshold[k];
        while (thTmp > RANGE_THRESHOLD_MANTISSA)
        {
            thTmp >>= 1;
            shiftVal++;
        }
        
        if (k == RANGE_GATE_0 || k == RANGE_GATE_1
#if defined (FFT_ZERO_FILL_20CM) || defined (XenD101_HPS01)
            || k == RANGE_GATE_2
#endif
           )
        {
            shiftVal += RANGE_GATE_ADJ_VAL;
        }
        
        RegListDfftPeak[loop].val = thTmp | (shiftVal << RANGE_THRESHOLD_SHIFT);
        loop++;
    }

    RegListDfftPeak[loop].addr = 0;
    RegListDfftPeak[loop].val = 0;

    loop = 0;
    while(RegListDfftPeak[loop].addr) 
    {
        I2C_Write(I2C_ADDR_RADAR_Chip0, (uint8_t)(RegListDfftPeak[loop].addr), RegListDfftPeak[loop].val);
        loop++;
    }
}

void* PEAK_GetSysParaAddr(void)
{
    return (void*)&peakPara;
}

uint32_t PEAK_GetSysParaLen(void)
{
    return sizeof(peakPara);
}

int8_t PEAK_ParaUpdate(uint16_t type, int32_t val)
{
    PEAK_THRES_T *peakThres = NULL;
    
    switch (type)  
    {		
       case PEAK_ROI_MIN:
            peakPara.roiMin = (uint8_t)val;
            break;
       case PEAK_ROI_MAX:
            peakPara.roiMax = (uint8_t)val;
            break;
       case PEAK_ACT_FNUM:
            peakPara.activeFrameNum = (uint8_t)val;
            break;
       case PEAK_INACT_FNUM:
            peakPara.inactiveFrameNum = (uint8_t)val;
            break;
       case PEAK_THRESHOLD:
            peakThres = (PEAK_THRES_T*)&val;
            if (peakThres->rangeGate < PEAK_TH_NUM)
            {
                peakPara.threshold[peakThres->rangeGate] = peakThres->thVal;
            }
            break;
        default:
            return -1;
    }

    return 0;
}

int32_t PEAK_ParaRead(uint16_t type)
{
    switch (type)  
    {
        case PEAK_ROI_MIN:
             return peakPara.roiMin;
        case PEAK_ROI_MAX:
             return peakPara.roiMax;
        case PEAK_ACT_FNUM:
             return peakPara.activeFrameNum;
        case PEAK_INACT_FNUM:
             return peakPara.inactiveFrameNum;
        default:
            return 0x7fffffff; /*invalid value*/
    }
}
#endif

#ifdef CONFIG_DEBUG
static void DumpFlashRegValue(void)
{
    uint16_t loop = 0;
    
    printf("radar flash value:\r\n");
    while(InitRegList[loop].addr) 
    {
        printf("%02X=%04X\r\n", InitRegList[loop].addr, InitRegList[loop].val);
        loop++;
    } 
}

static void DumpChipRegValue(void)
{
    uint16_t loop = 0;
    uint16_t val = 0;
    
    printf("radar ic value:\r\n");
    while(InitRegList[loop].addr) 
    {
        I2C_Read(I2C_ADDR_RADAR_Chip0, InitRegList[loop].addr, &val);
        printf("%02X=%04X\r\n", InitRegList[loop].addr, val);
        loop++;
    } 

#ifdef SUPPORT_DFFT_PEAK_CONFIG
    loop = 0;
    while(RegListDfftPeak[loop].addr) 
    {
        I2C_Read(I2C_ADDR_RADAR_Chip0, RegListDfftPeak[loop].addr, &val);
        printf("%02X=%04X\r\n", RegListDfftPeak[loop].addr, val);
        loop++;
    } 
#endif
}
#endif

#ifdef XenD101
static void ReplaceSpecialRegs(void)
{
    uint16_t loopPd = 0;
    uint16_t loopInit = 0;
    
    while (InitRegList[loopInit].addr)
    {
        if (InitRegList[loopInit].addr == RADAR_DIG_TSPI_CFG)
        {
            InitRegList[loopInit].val |= 0xf; /*mm32f0010 can not support high frequency spi clk*/
            break;
        }
        loopInit++;
    }

    while (RegListPD[loopPd].addr) 
    {
        loopInit = 0;
        while (InitRegList[loopInit].addr)
        {
            if (loopInit < REG_FIX_NUM)
            {
                loopInit++;
                continue;
            }
            if (InitRegList[loopInit].addr == RegListPD[loopPd].addr)
            {
                InitRegList[loopInit].val = RegListPD[loopPd].val;
                break;
            }
            loopInit++;
        }
        loopPd++;
    }
}
#endif

void Radar_Init(void)
{
    uint16_t loop = 0;

#if defined(EVBSN02_GE) || defined (XenD101_1815_ULP_BOARD)
    GPIO_PowerCtlPinInit();
    GPIO_PowerCtlOn();
    Delay(RADAR_POWER_ON_DELAY);
#endif
#ifdef XenD101
    ReplaceSpecialRegs();
#endif
#ifdef CONFIG_DEBUG
    DumpFlashRegValue();
#endif
    while (InitRegList[loop].addr) 
    {
        I2C_Write(I2C_ADDR_RADAR_Chip0, (uint8_t)(InitRegList[loop].addr), InitRegList[loop].val);
        loop++;
    } 
#ifdef XenD101
    Delay(RADAR_PD2NORMAL_DELAY);
    Radar_EnterNormalMode();
#endif
    
#ifdef SUPPORT_DFFT_PEAK_CONFIG
    ConfigDfftPeakPara();
#endif
#ifdef DEVMB	
    ConfigRadarChip1();   
#endif
#ifdef CONFIG_DEBUG
    DumpChipRegValue();
#endif
}

void Radar_PreInit(void)
{
    uint16_t loop = 0;

    while (InitRegList[loop].addr) 
    {
        I2C_Write(I2C_ADDR_RADAR_Chip0, (uint8_t)(InitRegList[loop].addr), InitRegList[loop].val);
        loop++;
    }   
}

void Radar_UpdateReg(uint16_t addr, uint16_t val)/*currently only update existing reg*/
{
    uint16_t loop = 0;

    while(InitRegList[loop].addr) 
    {   
        if (loop < REG_FIX_NUM)
        {
            loop++;
            continue;
        }
        
        if (InitRegList[loop].addr == addr)
        {
            InitRegList[loop].val = val;
            //Config_NeedFlashWrite();
            return;
        }
        loop++;
    }
}

void* Radar_GetRadarParaAddr(void)
{
    return (void*)&InitRegList;
}

uint32_t Radar_GetRadarParaLen(void)
{
    return sizeof(InitRegList);
}

//#ifdef SUPPORT_LOW_POWER
void Radar_EnterPDMode(void)
{
#ifdef XenD101_1815_ULP_BOARD
    GPIO_PowerCtlOff();
#else
    uint16_t loop = 0;
    
    while(RegListPD[loop].addr) 
    {
        I2C_Write(I2C_ADDR_RADAR_Chip0, (uint8_t)(RegListPD[loop].addr), RegListPD[loop].val);
        loop++;
    }
#endif
#ifdef MCU_LOW_POWER
    IWDG_Reload();
#else
    TIM3_Enable();
#endif
}
//#endif

void Radar_EnterNormalMode(void)
{
#ifdef XenD101_1815_ULP_BOARD
    GPIO_PowerCtlOn();
#else
    uint16_t loop = 0;
    
    while(RegListNormal[loop].addr) 
    {
        I2C_Write(I2C_ADDR_RADAR_Chip0, (uint8_t)(RegListNormal[loop].addr), RegListNormal[loop].val);
        loop++;
	}
#endif
}


