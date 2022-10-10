/**
  ******************************************************************************
  * @file    global_def.h
  * @author  iclm team
  * @brief   global define for whole project
  ******************************************************************************
  */
#ifndef __GLOBAL_DEF_H__
#define __GLOBAL_DEF_H__

#ifdef __cplusplus
 extern "C" {
#endif

#ifdef EVBSN01
#define RADAR_DEV_MODEL_TYPE	  (0x8000)
#define FIRMWARE_TYPE             (0) //General

#define RADAR_DEV_VER_MAJAR	      1
#define RADAR_DEV_VER_MINOR	      0
#define RADAR_DEV_VER_PATCH	      0
#endif

#ifdef DEVMB
#define RADAR_DEV_MODEL_TYPE	  (0x8001)
#define FIRMWARE_TYPE             (0) //General

#define RADAR_DEV_VER_MAJAR	      1
#define RADAR_DEV_VER_MINOR	      0
#define RADAR_DEV_VER_PATCH	      0
#endif

#ifdef ISK1101
#define RADAR_DEV_MODEL_TYPE	  (0x8002)
#define FIRMWARE_TYPE             (0) //General

#define RADAR_DEV_VER_MAJAR	      1
#define RADAR_DEV_VER_MINOR	      0
#define RADAR_DEV_VER_PATCH	      0
#endif

#ifdef EVBSN02_GE
#define RADAR_DEV_MODEL_TYPE	  (0x8003)
#define FIRMWARE_TYPE             (0) //General

#define RADAR_DEV_VER_MAJAR	      1
#define RADAR_DEV_VER_MINOR	      0
#define RADAR_DEV_VER_PATCH	      0
#endif

#ifdef XenD101
#if defined (FFT_ZERO_FILL_10CM) || defined (FFT_ZERO_FILL_20CM) || defined (XenD101_HPS01)
#define RADAR_DEV_MODEL_TYPE	  (0x8005) //XenD101B
#else
#define RADAR_DEV_MODEL_TYPE	  (0x8004) //XenD101
#endif

#if defined (XenD101_HPS01)
#define FIRMWARE_TYPE             (1) //HPS01

#define RADAR_DEV_VER_MAJAR	      1
#define RADAR_DEV_VER_MINOR	      0
#define RADAR_DEV_VER_PATCH	      0
#elif defined(SUPPORT_RV_OUTPUT)
#define FIRMWARE_TYPE             (2) //RM01

#define RADAR_DEV_VER_MAJAR	      1
#define RADAR_DEV_VER_MINOR	      0
#define RADAR_DEV_VER_PATCH	      0
#else
#define FIRMWARE_TYPE             (0) //MS01

#define RADAR_DEV_VER_MAJAR	      1
#define RADAR_DEV_VER_MINOR	      0
#define RADAR_DEV_VER_PATCH	      0
#endif
#endif

typedef enum
{
    CHANNEL_0        = 0, /*LNA1-TSPI_DA0*/
#ifdef STM32_PLATFORM
    CHANNEL_1        = 1, /*LNA2-TSPI_DA1*/
#endif
#ifdef DEVMB
    CHANNEL_2        = 2,
	CHANNEL_3        = 3,
#endif
    CHANNEL_MAX
}channelEnum;

typedef enum
{
    DATA_TYPE_FFT         = 0,
    DATA_TYPE_DFFT        = 1,
    DATA_TYPE_DFFT_PEAK   = 2,
	DATA_TYPE_DSRAW		  = 3,
    DATA_TYPE_MAX
}dataTypeEnum;

typedef enum
{
    SYS_MODE_PASSTHROUGH  = 0,
    SYS_MODE_MTT          = 1, 
    SYS_MODE_VS           = 2, 
	SYS_MODE_GR           = 3, 
    SYS_MODE_MAX
}sysModeEnum;

#ifdef __cplusplus
}
#endif

#endif


