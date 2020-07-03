  /*
  ******************************************************************************
  * @file     : USB2XXXCANTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX IIC test demo
  ******************************************************************************
  * @attention
  *
  * Copyright 2009-2014, usbxyz.com
  * http://www.usbxyz.com/
  * All Rights Reserved
  * 
  ******************************************************************************
  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usb_device.h"
#include "usb2can.h"

#define GET_FIRMWARE_INFO   1
#define CAN_MODE_LOOPBACK   1
#define CAN_SEND_MSG        1
#define CAN_GET_MSG         1
#define CAN_GET_STATUS      1
#define CAN_SCH             1

int main(int argc, const char* argv[])
{
#if GET_FIRMWARE_INFO
    DEVICE_INFO DevInfo;
#endif
    int DevHandle[10];
    int CANIndex = 0;//USB2XXXֻ֧��һ��ͨ�������Ըò�������Ϊ0
    bool state;
    int ret;
    //ɨ������豸
    ret = USB_ScanDevice(DevHandle);
    if(ret <= 0){
        printf("No device connected!\n");
        return 0;
    }
    //���豸
    state = USB_OpenDevice(DevHandle[0]);
    if(!state){
        printf("Open device error!\n");
        return 0;
    }
#if GET_FIRMWARE_INFO
    char FunctionStr[256]={0};
    //��ȡ�̼���Ϣ
    state = DEV_GetDeviceInfo(DevHandle[0],&DevInfo,FunctionStr);
    if(!state){
        printf("Get device infomation error!\n");
        return 0;
    }else{
        printf("Firmware Info:\n");
	    printf("Firmware Name:%s\n",DevInfo.FirmwareName);
        printf("Firmware Build Date:%s\n",DevInfo.BuildDate);
        printf("Firmware Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        printf("Hardware Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
	    printf("Firmware Functions:%s\n",FunctionStr);
    }
#endif
    //��ʼ������CAN
    CAN_INIT_CONFIG CANConfig;
#if CAN_MODE_LOOPBACK
    CANConfig.CAN_Mode = 1;//����ģʽ
#else
    CANConfig.CAN_Mode = 0;//����ģʽ
#endif
    CANConfig.CAN_ABOM = 0;//��ֹ�Զ�����
    CANConfig.CAN_NART = 1;//��ֹ�����ش�
    CANConfig.CAN_RFLM = 0;//FIFO��֮�󸲸Ǿɱ���
    CANConfig.CAN_TXFP = 1;//���������������˳��
    //���ò�����,������ = 100M/(BRP*(SJW+BS1+BS2))
    CANConfig.CAN_BRP = 2;
    CANConfig.CAN_BS1 = 15;
    CANConfig.CAN_BS2 = 5;
    CANConfig.CAN_SJW = 1;
    ret = CAN_Init(DevHandle[0],CANIndex,&CANConfig);
    if(ret != CAN_SUCCESS){
        printf("Config CAN failed!\n");
        return 0;
    }else{
        printf("Config CAN Success!\n");
    }
    //���ù��������������ã���������޷��յ�����
    CAN_FILTER_CONFIG CANFilter;
    CANFilter.Enable = 1;
    CANFilter.ExtFrame = 0;
    CANFilter.FilterIndex = 0;
    CANFilter.FilterMode = 0;
    CANFilter.MASK_IDE = 0;
    CANFilter.MASK_RTR = 0;
    CANFilter.MASK_Std_Ext = 0;
    CAN_Filter_Init(DevHandle[0],CANIndex,&CANFilter);
#if CAN_SEND_MSG//����CAN֡
    CAN_MSG CanMsg[5];
    for(int i=0;i<5;i++){
        CanMsg[i].ExternFlag = 0;
        CanMsg[i].RemoteFlag = 0;
        CanMsg[i].ID = i+1;
        CanMsg[i].DataLen = 8;
        for(int j=0;j<CanMsg[i].DataLen;j++){
            CanMsg[i].Data[j] = j;
        }
    }
    int SendedNum = CAN_SendMsg(DevHandle[0],CANIndex,CanMsg,5);
    if(SendedNum >= 0){
        printf("Success send frames:%d\n",SendedNum);
    }else{
        printf("Send CAN data failed!\n");
    }
#endif
#if CAN_GET_STATUS
    CAN_STATUS CANStatus;
    ret = CAN_GetStatus(DevHandle[0],CANIndex,&CANStatus);
    if(ret == CAN_SUCCESS){
        printf("TSR = %08X\n",CANStatus.TSR);
        printf("ESR = %08X\n",CANStatus.ESR);
    }else{
        printf("Get CAN status error!\n");
    }
#endif
    //��ʱ
#ifndef OS_UNIX
    Sleep(100);
#else
    usleep(100*1000);
#endif
#if CAN_GET_MSG
    CAN_MSG CanMsgBuffer[10240];
    int CanNum = CAN_GetMsg(DevHandle[0],CANIndex,CanMsgBuffer);
    if(CanNum > 0){
        printf("CanNum = %d\n",CanNum);
        for(int i=0;i<CanNum;i++){
            printf("CanMsg[%d].ID = %d\n",i,CanMsgBuffer[i].ID);
            printf("CanMsg[%d].TimeStamp = %d\n",i,CanMsgBuffer[i].TimeStamp);
            printf("CanMsg[%d].Data = ",i);
            for(int j=0;j<CanMsgBuffer[i].DataLen;j++){
                printf("%02X ",CanMsgBuffer[i].Data[j]);
            }
            printf("\n");
        }
    }else if(CanNum == 0){
        printf("No CAN data!\n");
    }else{
        printf("Get CAN data error!\n");
    }
#endif
#if CAN_SCH
    CAN_MSG CanSchMsg[5];
    for(int i=0;i<5;i++){
        CanSchMsg[i].ExternFlag = 0;
        CanSchMsg[i].RemoteFlag = 0;
        CanSchMsg[i].ID = (i<<4)|(i+1);
        CanSchMsg[i].DataLen = 8;
        for(int j=0;j<CanMsg[i].DataLen;j++){
            CanSchMsg[i].Data[j] = j;
        }
        CanSchMsg[i].TimeStamp =10;//֡���ʱ��Ϊ10ms,�������ã������е�ĳһ֡���ݸ���Ϊ0������֡������Ϻ󲻻�������к��������ݷ�����
    }
    ret = CAN_StartSchedule(DevHandle[0],CANIndex,CanSchMsg,5);
    if(ret == CAN_SUCCESS){
        printf("Start CAN Schedule Success!\n",SendedNum);
    }else{
        printf("Start CAN Schedule Failed!\n");
    }
    Sleep(10000);
    CAN_StopSchedule(DevHandle[0],CANIndex);
#endif

    //�ر��豸
    USB_CloseDevice(DevHandle[0]);
	return 0;
}

