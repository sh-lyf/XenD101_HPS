/**
  ******************************************************************************
  * @file           : rva.h
  * @author         : iclm team
  * @brief          : rva head file
  ******************************************************************************
  */

#ifndef __RVA_H__
#define __RVA_H__

#include "config.h"
#include <stdint.h>

typedef float float32_t;

typedef enum
{
    RANGE_FFT_DATA = 0,
    DOPPLER_FFT_DATA
}DATA_TYPE;

#define CAL_FININSH    	    (0)  	
#define CAL_DOING    	    (1)  

#define FRAME_ROWS          (32)        
#define FRAME_COLS          (32)        
#define DFFT_POINTS         (FRAME_ROWS * FRAME_COLS)
#define BKG_CAL_LIM         (30)

#define FFT_POINTS          FRAME_COLS
#define CHIRP_MAX           FRAME_ROWS
#define CHIRP_HALF          (CHIRP_MAX >> 1)

#define PEAK_NUM            (9)
#define PER_PEAK_SIZE       (12)
#define CH0_DIDX_SHIFT      (0)
#define CH0_RIDX_SHIFT      (1)
#define CH0_VAL_SHIFT       (4)
#define PEAK_TH 	        (2000)

#define ABS2_THRESHOLD    	(5000)
#define NONE_THRESHOLD    	(30)
#define BUFSIZE    	        (20)

#define RANGE_LIM_MIN    	(3)  								
#define RANGE_LIM_MAX    	(30)  							


#define RANGE_RESOLUTION        (0.059) //m
#define VELOCITY_RESOLUTION     (0.18)  //m/s
#define ANGLE_FACTOR            (0.32)  //lamda/2/pi/d
#define RIDIAN2DEGREE           (57)

typedef struct COMPLEX16
{
    int16_t re;
    int16_t im;
} COMPLEX16_T;

typedef struct COMPLEX32
{
    int32_t re;
    int32_t im;
} COMPLEX32_T;

typedef struct COMPLEXFLOAT
{
    float32_t re;
    float32_t im;
}COMPLEXFLOAT_T;

typedef struct RVA
{
    float range;
    float velocity;
    float angle;
} RVA_T;

typedef struct DFFTPEAK
{
    uint8_t didx;
    uint8_t ridx;
    uint32_t val;
} DFFTPEAK_T;

void RVA_CalcRva(uint8_t *pbuf, uint16_t bufLen, uint8_t channel, uint16_t index, uint8_t dataType);

#endif


