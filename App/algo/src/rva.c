/**
  ******************************************************************************
  * @file           : rva.c
  * @author         : iclm team
  * @brief          : algorithm for range velocity angle calculate
  ******************************************************************************
  */
  
#include <stdio.h>
#include <math.h>
#include "rva.h"
#include "arm_math.h"
#include "arm_const_structs.h"

static COMPLEX16_T rfftData[CHANNEL_MAX][CHIRP_MAX][FFT_POINTS];
static COMPLEXFLOAT_T dfftTmp[CHANNEL_MAX][FFT_POINTS][CHIRP_MAX];
static float32_t tmpBuf[CHIRP_HALF*2];
static COMPLEX32_T dopplerMean[CHANNEL_MAX][DFFT_POINTS];

static COMPLEX16_T dopplerData[CHANNEL_MAX][DFFT_POINTS];
static uint32_t dataAbs2[DFFT_POINTS];
static uint32_t bkgBuf[FRAME_COLS][BKG_CAL_LIM];
static uint32_t bkgEnergy[FRAME_COLS];
static RVA_T rvaBuf[BUFSIZE];

static DFFTPEAK_T dfftPeak[PEAK_NUM];

const float32_t hamming_window[CHIRP_MAX*2] =
{
	0.08000000, 0.08000000, 0.08941623, 0.08941623, 0.11727941, 0.11727941, 0.16244882, 0.16244882, 
	0.22307522, 0.22307522, 0.29667656, 0.29667656, 0.38023958, 0.38023958, 0.47034322, 0.47034322, 
	0.56329862, 0.56329862, 0.65530016, 0.65530016, 0.74258131, 0.74258131, 0.82156875, 0.82156875, 
	0.88902874, 0.88902874, 0.94219944, 0.94219944, 0.97890406, 0.97890406, 0.99763989, 0.99763989, 
	0.99763989, 0.99763989, 0.97890406, 0.97890406, 0.94219944, 0.94219944, 0.88902874, 0.88902874, 
	0.82156875, 0.82156875, 0.74258131, 0.74258131, 0.65530016, 0.65530016, 0.56329862, 0.56329862, 
	0.47034322, 0.47034322, 0.38023958, 0.38023958, 0.29667656, 0.29667656, 0.22307522, 0.22307522, 
	0.16244882, 0.16244882, 0.11727941, 0.11727941, 0.08941623, 0.08941623, 0.08000000, 0.08000000
};

void complex2abs2(COMPLEX16_T *matrix, uint16_t m_rows, uint16_t m_cols, uint32_t *abs2)
{
    uint16_t i = 0;
    uint16_t j = 0;
    int32_t absr = 0;
    int32_t absi = 0;
    uint32_t abss = 0;

    if (NULL == matrix || NULL == abs2)
    {
        return;
    }

	for (i = 0; i < m_rows; i++) 
    {
		for (j = 0; j < m_cols; j++) 
        {
			if ((j >= RANGE_LIM_MIN) && (j < RANGE_LIM_MAX))
			{
				absr = matrix[i*m_cols + j].re;
				absr *= absr;
				absi = matrix[i*m_cols + j].im;
				absi *= absi;
				abss = absr + absi;			
				abs2[i*m_cols + j] = abss;
			}
			else
			{
				abs2[i*m_cols + j] = 0;
			}
		}
	}
}

uint32_t findMaxPos_2d(uint32_t *matrix, int16_t m_rows, int16_t m_cols, int16_t *ptr_rmax_idx, int16_t *ptr_cmax_idx)
{
	int16_t i = 0;
    int16_t j = 0;
	uint32_t maxAbs = 0;
    
    *ptr_rmax_idx = 0;
    *ptr_cmax_idx = 0;
    
	for (i = 0; i < m_rows; i++) 
    {
		for (j = 0; j < m_cols; j++) 
        {
			if (maxAbs < matrix[i*m_cols + j]) 
            {
				maxAbs = matrix[i*m_cols + j];
				*ptr_rmax_idx = i;
				*ptr_cmax_idx = j;
			}
		}
	}
    
	return maxAbs;
}

uint32_t findColMaxVal(uint32_t *matrix, uint16_t m_rows, uint16_t m_cols, uint16_t col)
{
	uint16_t i = 0;
	uint32_t maxVal = 0;
    
    if (NULL == matrix)
    {
        return 0;
    }
    
	for (i = 0; i < m_rows; i++) 
    {
        if (matrix[i*m_cols + col] > maxVal)
        {
            maxVal = matrix[i*m_cols + col];
        }
	}
	
	return maxVal;
}

uint32_t calMeanVal(uint32_t *array, uint16_t a_len)
{
	uint64_t sum = 0;
    uint16_t i = 0;

    if (NULL == array || 0 == a_len)
    {
        return 0;
    }
    
    for (i = 0; i < a_len; i++) 
    {
        sum += array[i];
    }
    
	return (sum/a_len);
}

void calMeanRva(RVA_T *rvaBuf, uint16_t bufSize, RVA_T *rva)
{
    uint16_t i = 0;

    if (NULL == rvaBuf || NULL == rva)
    {
        return ;
    }

    rva->range = 0;
    rva->velocity = 0;
    rva->angle = 0;
    
    for (i = 0; i < bufSize; i++) 
    {
        rva->range += rvaBuf[i].range;
        rva->velocity += rvaBuf[i].velocity;
        rva->angle += rvaBuf[i].angle;
    }

    rva->range /= bufSize;
    rva->velocity /= bufSize;
    rva->angle /= bufSize;
}


float calAngle(int16_t rmaxIdx, int16_t cmaxIdx)
{
    int16_t a1Re = 0;
    int16_t a1Im = 0;
    int16_t a2Re = 0;
    int16_t a2Im = 0;
    int32_t re = 0;
    int32_t im = 0;
    float phase=0;
    float angle=0;

	a1Re = dopplerData[CHANNEL_0][rmaxIdx*FRAME_COLS + cmaxIdx].re;
	a1Im = dopplerData[CHANNEL_0][rmaxIdx*FRAME_COLS + cmaxIdx].im;
	a2Re = dopplerData[CHANNEL_1][rmaxIdx*FRAME_COLS + cmaxIdx].re;
	a2Im = dopplerData[CHANNEL_1][rmaxIdx*FRAME_COLS + cmaxIdx].im;
	re = a1Re*a2Re + a1Im*a2Im;
	im = a1Im*a2Re - a1Re*a2Im;

    if (re != 0 || im != 0)
    {
        phase = atan2(im, re);
    }
    else
    {
        phase = 0;
    }

    angle = (float)asin((double)phase * ANGLE_FACTOR);
    angle = angle * RIDIAN2DEGREE;

    return angle;
}

uint8_t calBkgEnergy(uint32_t *matrix, uint16_t m_rows, uint16_t m_cols)
{
    uint16_t m = 0;
    static uint16_t bakCalCnt = 0;
    
    if (bakCalCnt < BKG_CAL_LIM)
    {
        for (m = 0; m < FRAME_COLS; m++) 
        {
            bkgBuf[m][bakCalCnt] = findColMaxVal(matrix, m_rows, m_cols, m);
        }
        
        bakCalCnt++;
        return CAL_DOING;
    }
    else if (bakCalCnt == BKG_CAL_LIM)
    {
        for (m = 0; m < FRAME_COLS; m++) 
        {
            bkgEnergy[m] = calMeanVal(bkgBuf[m], BKG_CAL_LIM);
            //printf("bkgEnergy[%d]:%d\r\n", m, bkgEnergy[m]);
        }
        
        bakCalCnt++;
        return CAL_FININSH;
    }

    return CAL_FININSH;
}

void RmBkgEnergy()
{
    uint16_t m = 0;
    uint16_t n = 0;
    uint16_t k = 0;

	for (m = 0; m < FRAME_ROWS; m++) 
    {
		for (n = 0; n < FRAME_COLS; n++) 
        {
            k = m * FRAME_COLS + n;
            if (dataAbs2[k] > bkgEnergy[n])
            {
                dataAbs2[k] = dataAbs2[k] -  bkgEnergy[n];
            }
            else
            {
                dataAbs2[k] = 0;
            }
		}
	}
}

/*alpha=0.99(1-1/2^7), Q=16*/
void ClusterRemove()
{
    uint16_t chId = 0;
    uint16_t idx = 0;

    for (chId = 0; chId < CHANNEL_MAX; chId++)
    {
        for (idx = 0; idx < DFFT_POINTS; idx++)
        {
            dopplerMean[chId][idx].re = dopplerMean[chId][idx].re - (dopplerMean[chId][idx].re>>7) + (dopplerData[chId][idx].re<<9);
            dopplerData[chId][idx].re = dopplerData[chId][idx].re - (dopplerMean[chId][idx].re>>16);
            
            dopplerMean[chId][idx].im = dopplerMean[chId][idx].im - (dopplerMean[chId][idx].im>>7) + (dopplerData[chId][idx].im<<9);
            dopplerData[chId][idx].im = dopplerData[chId][idx].im - (dopplerMean[chId][idx].im>>16);
        }
    }
}

int16_t FloatToInt16(float in)
{    
    int16_t out = 0;

    if (in > INT16_MAX)
    {
        out = INT16_MAX;
    }
    else if (in < INT16_MIN)
    {
        out = INT16_MIN;
    }
    else
    {
        out = (int16_t)in;
    }

    return out;
}

static void Rfft2Dfft()
{
    uint16_t chId = 0;
    uint16_t chirpIdx = 0;
    uint16_t ridx = 0;
    uint16_t k = 0;

    for (chId = 0; chId < CHANNEL_MAX; chId++)
    {
        for (chirpIdx = 0; chirpIdx < CHIRP_MAX; chirpIdx++)
        {
            for (ridx = 0; ridx < FFT_POINTS; ridx++)
            {
                dfftTmp[chId][ridx][chirpIdx].re = rfftData[chId][chirpIdx][ridx].re;
                dfftTmp[chId][ridx][chirpIdx].im = rfftData[chId][chirpIdx][ridx].im;
            }
        }
    }

    for (chId = 0; chId < CHANNEL_MAX; chId++)
    {
        for (ridx = 0; ridx < FFT_POINTS; ridx++)
        {
            arm_mult_f32((float32_t*)dfftTmp[chId][ridx], hamming_window, (float32_t*)dfftTmp[chId][ridx], CHIRP_MAX*2);  
            arm_cfft_f32(&arm_cfft_sR_f32_len32, (float32_t*)dfftTmp[chId][ridx], 0, 1);
            
            //fftshift
    		arm_copy_f32((float32_t*)dfftTmp[chId][ridx], tmpBuf, CHIRP_HALF*2);
    		arm_copy_f32((float32_t*)(dfftTmp[chId][ridx] + CHIRP_HALF), (float32_t*)dfftTmp[chId][ridx], CHIRP_HALF*2);
    		arm_copy_f32(tmpBuf, (float32_t*)(dfftTmp[chId][ridx] + CHIRP_HALF), CHIRP_HALF*2);
        }
    }

    for (chId = 0; chId < CHANNEL_MAX; chId++)
    {
        for (ridx = 0; ridx < FFT_POINTS; ridx++)
        {
            for (chirpIdx = 0; chirpIdx < CHIRP_MAX; chirpIdx++)
            {
                k = chirpIdx*CHIRP_MAX + ridx;
                dopplerData[chId][k].re = FloatToInt16(dfftTmp[chId][ridx][chirpIdx].re / 10);
                dopplerData[chId][k].im = FloatToInt16(dfftTmp[chId][ridx][chirpIdx].im / 10);
            }
        }
    }

    ClusterRemove();
}

void DfftRvaCalc(uint16_t bufSize)
{
    uint32_t maxAbs2 = 0;
    int16_t rmaxIdx = 0;
    int16_t cmaxIdx = 0;
    RVA_T rva = {0};
    static uint16_t bufCnt = 0;
    static uint16_t noneCnt = 0;

    if (bufSize > BUFSIZE)
    {
        printf("Error: bufSize=%d\r\n", bufSize);
        return;
    }

    complex2abs2(dopplerData[CHANNEL_0], FRAME_ROWS, FRAME_COLS, dataAbs2);
    if (CAL_FININSH != calBkgEnergy(dataAbs2, FRAME_ROWS, FRAME_COLS))
    {
        return;
    }
	
    RmBkgEnergy();
	maxAbs2 = findMaxPos_2d(dataAbs2, FRAME_ROWS, FRAME_COLS, &rmaxIdx, &cmaxIdx);
    if (maxAbs2 < ABS2_THRESHOLD)
    {
        noneCnt++;
        if (noneCnt < NONE_THRESHOLD)
        {
            return;
        }
        else
        {
            rva.range = -1;
            rva.velocity = 0;
            rva.angle = 0;
        }
    }
    else
    {
        bufCnt++;
        if (bufCnt < bufSize)
        {
            rvaBuf[bufCnt].range = cmaxIdx * RANGE_RESOLUTION;
            rvaBuf[bufCnt].velocity = (rmaxIdx - (FRAME_ROWS>>1)) * VELOCITY_RESOLUTION;
            rvaBuf[bufCnt].angle = calAngle(rmaxIdx, cmaxIdx);
            return;
        }
        else
        {
            calMeanRva(rvaBuf, bufSize, &rva);
            bufCnt = 0;
        }
    }
    
    noneCnt = 0;
    printf("R:%.2f, V:%.2f A:%.2f\r\n", rva.range, rva.velocity, rva.angle);		
}

void DfftPeakRvCalc(uint16_t bufSize)
{
    uint8_t dIdx = 0;
    uint8_t rIdx = 0;
    uint16_t k = 0;
    uint32_t maxAbs2 = 0;
    RVA_T rva = {0};
    static uint16_t bufCnt = 0;
    static uint16_t noneCnt = 0;

    if (bufSize > BUFSIZE)
    {
        printf("Error: bufSize=%d\r\n", bufSize);
        return;
    }

	for(k = 0; k < PEAK_NUM; k++)
    {
        if (dfftPeak[k].val > maxAbs2)
        {
            maxAbs2 = dfftPeak[k].val;
            dIdx = dfftPeak[k].didx;
            rIdx = dfftPeak[k].ridx;
        }
    }
	
    if (maxAbs2 < PEAK_TH)
    {
        noneCnt++;
        if (noneCnt < NONE_THRESHOLD)
        {
            return;
        }
        else
        {
            rva.range = -1;
            rva.velocity = 0;
        }
    }
    else
    {
        bufCnt++;
        if (bufCnt < bufSize)
        {
            rvaBuf[bufCnt].range = rIdx * RANGE_RESOLUTION;
            rvaBuf[bufCnt].velocity = (dIdx - (FRAME_ROWS>>1)) * VELOCITY_RESOLUTION;
            return;
        }
        else
        {
            calMeanRva(rvaBuf, bufSize, &rva);
            bufCnt = 0;
        }
    }
    
    noneCnt = 0;
    printf("R:%.2f, V:%.2f\r\n", rva.range, rva.velocity);	
}

void RVA_CalcRva(uint8_t *pbuf, uint16_t bufLen, uint8_t channel, uint16_t index, uint8_t dataType)
{
    uint16_t k = 0;
    uint16_t bufSize = BUFSIZE;

    if (NULL == pbuf || channel >= CHANNEL_MAX)
    {
        return;
    }

    switch (dataType)
    {        
        case DATA_TYPE_FFT:
            if (index >= CHIRP_MAX)
            {
                printf("Error: index=%d\r\n", index);
                return;
            }

            for(k = 0; k < FFT_POINTS; k++)
            {
                rfftData[channel][index][k].re = (pbuf[(k<<2)+0] << 8) + pbuf[(k<<2)+1];
                rfftData[channel][index][k].im = (pbuf[(k<<2)+2] << 8) + pbuf[(k<<2)+3];
            }
            if (CHANNEL_1 == channel && (CHIRP_MAX - 1) == index)
            {
                Rfft2Dfft();
                bufSize = BUFSIZE >> 1;
                DfftRvaCalc(bufSize);
            }
            break;
        
        case DATA_TYPE_DFFT:
        	for(k = 0; k < DFFT_POINTS; k++)
            {
                dopplerData[channel][k].re = (pbuf[(k<<2)+0] << 8) + pbuf[(k<<2)+1];
                dopplerData[channel][k].im = (pbuf[(k<<2)+2] << 8) + pbuf[(k<<2)+3];
            }    

            if (CHANNEL_1 == channel)
            {
                DfftRvaCalc(bufSize);
            }
            break;

        case DATA_TYPE_DFFT_PEAK:
        	for(k = 0; k < PEAK_NUM; k++)
            {
                dfftPeak[k].didx = pbuf[k*PER_PEAK_SIZE+CH0_DIDX_SHIFT];
                dfftPeak[k].ridx = pbuf[k*PER_PEAK_SIZE+CH0_RIDX_SHIFT];
                dfftPeak[k].val = (pbuf[k*PER_PEAK_SIZE+CH0_VAL_SHIFT+0] << 24) + (pbuf[k*PER_PEAK_SIZE+CH0_VAL_SHIFT+1] << 16)
                                  + (pbuf[k*PER_PEAK_SIZE+CH0_VAL_SHIFT+2] << 8) + (pbuf[k*PER_PEAK_SIZE+CH0_VAL_SHIFT+3] << 0);
            }
            DfftPeakRvCalc(bufSize);
            break;
        
        default:
            printf("Error: unsupport dataType\r\n");
            return;
    }
}

