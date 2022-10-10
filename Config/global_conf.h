/**
  ******************************************************************************
  * @file    global_conf.h
  * @author  iclm team
  * @brief   global config for whole project
  ******************************************************************************
  */
#ifndef __GLOBAL_CONF_H__
#define __GLOBAL_CONF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/**********function switch*************/
#ifdef XenD101
#define XenD101_COMMON_PROJ
//#define HUAWEI_PANEL_PROJ
//#define FFT_HARDWARE_TEST  /* use FFT.txt */

#define SUPPORT_NEW_PIN
#ifdef XenD101_COMMON_PROJ   /* default is MS01 */
#define XenD101_HPS01        /* use regs_hps.txt */
//#define CENTER_GRAVITY

#if defined(CENTER_GRAVITY)
#define SUPPORT_RV_OUTPUT
#endif

#if defined(XenD101_HPS01)
//#define XenD101_HPS01_1G    /* use regs_hps_1g.txt */
#endif

#if !defined(CENTER_GRAVITY) && !defined(XenD101_HPS01)
#define SUPPORT_DATA_PASSTHROUGH
#endif

//#define SUPPORT_LOW_POWER
#endif

#ifdef HUAWEI_PANEL_PROJ
#define SUPPORT_RV_OUTPUT
#define FFT_ZERO_FILL_10CM  /* use regs_zero_fill_10cm.txt */
//#define FFT_ZERO_FILL_20CM  /* use regs_zero_fill_20cm.txt */
//#define COVER_PANEL
#endif

#ifdef FFT_HARDWARE_TEST
#define SUPPORT_DATA_PASSTHROUGH
#endif

#define SUPPORT_ABD
#define SUPPORT_DFFT_PEAK_CONFIG
//#define PRINT_BY_UART2
#define XenD101_1815_2_BOARD
#ifdef SUPPORT_LOW_POWER
//#define MCU_LOW_POWER
#endif
#endif

#ifdef EVBSN01
#define SUPPORT_DATA_PASSTHROUGH
//#define SUPPORT_RVA_CALC
//#define SUPPORT_TSPI_IO
#endif

#ifdef EVBSN02_GE
#define SUPPORT_DATA_PASSTHROUGH
//#define SUPPORT_RVA_CALC
#endif

#ifdef ISK1101
#define SUPPORT_DATA_PASSTHROUGH
#endif

/**********para config****************/
#ifdef XenD101
#define UPLOAD_SAMPLE_RATE        (16)
#define RADAR_DATA_MAX_LEN        (108)//(128) //support DFFT PEAK(108) and FFT 32 point(128)
#endif

#ifdef EVBSN01
#define UPLOAD_SAMPLE_RATE        (1)
#define RADAR_DATA_MAX_LEN        (4096)
#endif

#ifdef EVBSN02_GE
#define UPLOAD_SAMPLE_RATE        (1)
#define RADAR_DATA_MAX_LEN        (4096)
#endif

#ifdef ISK1101
#define UPLOAD_SAMPLE_RATE        (16)
#define RADAR_DATA_MAX_LEN        (256)
#endif

/**********debug**********************/
#define CONFIG_DEBUG

#include "global_def.h"

#ifdef __cplusplus
}
#endif

#endif


