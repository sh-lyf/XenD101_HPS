/**
  ******************************************************************************
  * @file           : usb_transfer.h
  * @author         : iclm team
  * @brief          : usb transfer header file
  ******************************************************************************
  */
#ifndef __USB_TRANSFER_H__
#define __USB_TRANSFER_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#define USB_DATA_QUEUE_SIZE             (5)
#define USB_TRANSFER_STACK_SIZE         (512)

#define USB_BUF_CNT                     (USB_DATA_QUEUE_SIZE + 2)
#define USB_BUF_LEN                     (2000)
#define USB_SMALL_PKT_LEN               (256)
#define USB_SEND_WAIT                   (1)

#define USB_PKT_THRESHOLD               (20)
#define USB_PKT_CHECK_TICK              (300)
#define USB_PKT_INC_PER_CHECK           (1)

void UsbTransfer_TaskInit(void);
void UsbTransfer_Send(uint8_t* pBuffer, uint16_t uBufLen);
void UsbTransfer_Send2UsbDataQueue(void *usbData);
void UsbTransfer_UsbPktCntCallBack(void);
void UsbTransfer_ResetUsbBuffer(void);
void UsbTransfer_DecUsbTxLen(void);

#ifdef __cplusplus
}
#endif

#endif

