/**
  ******************************************************************************
  * @file    usb2k.h
  * $Author: wdluo $
  * $Revision: 447 $
  * $Date:: 2019-09-8 18:24:57 +0800 #$
  * @brief   usb2k��غ������������Ͷ���.
  ******************************************************************************
  * @attention
  *
  *<center><a href="http:\\www.toomoss.com">http://www.toomoss.com</a></center>
  *<center>All Rights Reserved</center></h3>
  * 
  ******************************************************************************
  */
#ifndef __USB2K_H_
#define __USB2K_H_

#include <stdint.h>
#ifndef OS_UNIX
#include <Windows.h>
#else
#include <unistd.h>
#ifndef WINAPI
#define WINAPI
#endif
#endif

//���庯�����ش������
#define K_SUCCESS                   (0)   //����ִ�гɹ�
#define K_ERR_NOT_SUPPORT           (-1)  //��������֧�ָú���
#define K_ERR_USB_WRITE_FAIL        (-2)  //USBд����ʧ��
#define K_ERR_USB_READ_FAIL         (-3)  //USB������ʧ��
#define K_ERR_CMD_FAIL              (-4)  //����ִ��ʧ��
#define K_ERR_SERVER_NO_RESPONSE    (-5)  //ECU����Ӧ
#define K_ERR_RESPONSE              (-6)  //ECU��Ӧ���ݴ���
#define K_ERR_SEND_DATA             (-7)  //���ݷ��ͳ���
#define K_ERR_CHECK_ERROR           (-8)  //���ݷ��ͳ���

typedef struct {
    //Define the length of the data field of a message, i.e. from the beginning of the
    //data field (Service Identification byte included) to Checksum byte (not included). A
    //message length of 1 to 63 bytes is possible. If DataLen = 0 then the additional length
    //byte is included.
    unsigned char DataLen:6;
    //0,1δ����
    //2-Header with address information, physical target address
    //3-Header with address information, functional target address
    unsigned char HeaderMode:2;
}K_MSG_FMT;

//����K���ݽṹ��
typedef struct _K_MSG{
  unsigned char Fmt;        //Format byte, K_MSG_FMT
  unsigned char TgtAddr;    //This is the target address for the message. It may be a physical or a functional address.
  unsigned char SrcAddr;    //Source address
  unsigned char Len;        //This byte is provided if the length in the header byte Fmt.DataLen is set to 0.
  unsigned char SId;        //Service Identification byte
  unsigned char Data[254];  //depending on service
  unsigned char CS;         //Checksum byte
}K_MSG;

#ifdef __cplusplus
extern "C"
{
#endif
    int WINAPI K_Init(int DevHandle, unsigned char Channel,int BaudRate);
    int WINAPI K_5BaudInit(int DevHandle, unsigned char Channel,unsigned char Addr,unsigned char *pKB1,unsigned char *pKB2);
    int WINAPI K_FastInit(int DevHandle, unsigned char Channel,K_MSG *pRequest,K_MSG *pResponse,unsigned char InterByteTimeMs);
    int WINAPI K_Request(int DevHandle, unsigned char Channel,K_MSG *pRequest,K_MSG *pResponse,unsigned char InterByteTimeMs,int ResponseTimeOutMs);
#ifdef __cplusplus
}
#endif
#endif

