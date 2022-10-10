/**
  ******************************************************************************
  * @file           : radar.h
  * @author         : iclm team
  * @brief          : radar driver header file
  ******************************************************************************
  */
#ifndef __RADAR_H__
#define __RADAR_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>
#include "global_conf.h"

typedef enum  
{
	RAW_POINT_64 = 64,
    RAW_POINT_128 = 128,
    RAW_POINT_256 = 256,
    RAW_POINT_512 = 512,
    RAW_POINT_1024 = 1024,
}rawPointEnum;

typedef struct RADAR_REG
{
    uint16_t addr;
    uint16_t val;
}RADAR_REG_T;

typedef struct PEAK_THRES
{
    uint16_t rangeGate;
    uint16_t thVal;
}PEAK_THRES_T;

typedef enum  
{
	PEAK_ROI_MIN = 0,
    PEAK_ROI_MAX,
    PEAK_ACT_FNUM,
    PEAK_INACT_FNUM,
    PEAK_THRESHOLD
}peakParaEnum;

#define PEAK_TH_NUM                 (16)
typedef struct PEAK_PARA
{
    uint8_t roiMin;
    uint8_t roiMax;
    uint8_t activeFrameNum;
    uint8_t inactiveFrameNum;
    uint16_t threshold[PEAK_TH_NUM];
}PEAK_PARA_T;

#define I2C_ADDR_RADAR_Chip0		(0x40)
#define I2C_ADDR_RADAR_Chip1		(0x44)
#define REG_FIX_NUM                 (5)
#define RAW_MAP_NUM                 (5)

#define RADAR_DIG_FUN_SWITCH       (0x01)
#define RADAR_DIG_RAW_PEAK_NUM     (0x04)
#define RADAR_DIG_FFT_NUM          (0x05)
#define RADAR_DIG_TSPI_CFG         (0x06)
#define RADAR_DIG_FFT_SETTING      (0x0A)
#define RADAR_DIG_DFFT_DATA_NUM    (0x0D)
#define RADAR_DIG_DFFT_CHIRP_NUM   (0x0E)
#define RADAR_PAT_CHIRP_NUM        (0x44)

#define RADAR_DFFT_DATA_NUM_POS    (8)
#define RADAR_DFFT_CHIRP_NUM_POS   (8)
#define RADAR_RAW_POS              (8)

#define RADAR_PEAK_MASK            (0x1F)
#define RADAR_RAW_MASK             (0x07)

#define RADAR_FFT_DATA             BIT(2)
#define RADAR_DFFT_DATA            BIT(12)
#define RADAR_DFFT_PEAK_DATA       BIT(4)
#define RADAR_DSRAW_DATA           BIT(1)

#define DFFT_PEAK_REG_NUM                       (20)
#define RADAR_2DFFT_PEAK_RANGE_IDX             (0x14)
#define RADAR_2DFFT_PEAK_VELOCITY_IDX          (0x15)
#define RADAR_2DFFT_PEAK_TAR_FRAME_NUM         (0x17)

#if defined(FFT_ZERO_FILL_10CM) || defined (FFT_ZERO_FILL_20CM)
#define MIN_RANGE_IDX                           (1)
#define MIN_VELOCITY_IDX                        (4)
#define MAX_VELOCITY_IDX                        (27)
#define ACTIVE_FRAME_NUM                        (2)

#ifdef COVER_PANEL
#define MAX_RANGE_IDX                           (10)
#else
#define MAX_RANGE_IDX                           (25)
#endif
#define INACTIVE_FRAME_NUM                      (1)
#elif defined (CENTER_GRAVITY)
#define MIN_RANGE_IDX                           (0)
#define MIN_VELOCITY_IDX                        (4)
#define MAX_VELOCITY_IDX                        (27)
#define ACTIVE_FRAME_NUM                        (2)

#define MAX_RANGE_IDX                           (6)
#define INACTIVE_FRAME_NUM                      (1)
#elif defined(XenD101_HPS01)
#define MIN_RANGE_IDX                           (1)
#define MIN_VELOCITY_IDX                        (4)
#define MAX_VELOCITY_IDX                        (27)
#define ACTIVE_FRAME_NUM                        (2)

#ifdef XenD101_HPS01_1G
#define MAX_RANGE_IDX                           (15)
#else
#define MAX_RANGE_IDX                           (8)
#endif
#define INACTIVE_FRAME_NUM                      (1)
#else
#define MIN_RANGE_IDX                           (0)
#define MIN_VELOCITY_IDX                        (8)
#define MAX_VELOCITY_IDX                        (23)
#define ACTIVE_FRAME_NUM                        (2)

#define MAX_RANGE_IDX                           (2)
#define INACTIVE_FRAME_NUM                      (16)
#endif

#ifdef COVER_PANEL
#define RATIO                                   (0.55)
#define RATIO1                                  (0.58)
#define RATIO2                                  (0.6)
#endif

#define RANGE_THRESHOLD_REG_NUM                 (16)
#define RANGE_THRESHOLD_BASE                    (0x20)
#define RANGE_THRESHOLD_SHIFT                   (11)
#define RANGE_THRESHOLD_MANTISSA                ((1 << RANGE_THRESHOLD_SHIFT) - 1)
#define RANGE_GATE_0                            (0)
#define RANGE_GATE_1                            (1)
#define RANGE_GATE_2                            (2)

#if defined (FFT_ZERO_FILL_10CM) || defined (FFT_ZERO_FILL_20CM) || defined (XenD101_HPS01)
#define RANGE_GATE_ADJ_VAL                      (10)
#else
#define RANGE_GATE_ADJ_VAL                      (2)
#endif

#define RADAR_POWER_ON_DELAY       (6)
#define RADAR_PD2NORMAL_DELAY      (500)
#ifdef XenD101_1815_ULP_BOARD
#define POWER_DOWN_PERIOD           (60)  //ms
#elif defined(XenD101_HPS01)
#define POWER_DOWN_PERIOD           (82)  //ms
#else
#define POWER_DOWN_PERIOD           (66)  //ms
#endif

extern uint8_t g_ChannelCount;
extern uint8_t g_TxCount;
extern uint8_t g_maxChannelCount;

void Radar_Init(void);
void Radar_PreInit(void);
uint16_t Radar_GetFftPoint(void);
uint16_t Radar_GetRawPoint(void);
uint16_t Radar_GetOneFrameChirpNum(void);
uint8_t Radar_GetDataType(void);
uint16_t Radar_GetDfftDataNum(void);
uint16_t Radar_GetDfftChirpNum(void);
uint16_t Radar_GetDfftPeakSize(void);

void Radar_UpdateReg(uint16_t addr, uint16_t val);
void* Radar_GetRadarParaAddr(void);
uint32_t Radar_GetRadarParaLen(void);
void Radar_EnterNormalMode(void);
void Radar_EnterPDMode(void);
void* PEAK_GetSysParaAddr(void);
uint32_t PEAK_GetSysParaLen(void);
int8_t PEAK_ParaUpdate(uint16_t type, int32_t val);
int32_t PEAK_ParaRead(uint16_t type);

#ifdef __cplusplus
}
#endif

#endif


