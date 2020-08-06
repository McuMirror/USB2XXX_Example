  /*
  ******************************************************************************
  * @file     : USB2XXX_CANFD_Test.cpp
  * @Copyright: TOOMOSS 
  * @Revision : ver 1.0
  * @Date     : 2019/12/19 9:33
  * @brief    : USB2XXX CANFD test demo
  ******************************************************************************
  * @attention
  *
  * Copyright 2009-2019, TOOMOSS
  * http://www.toomoss.com/
  * All Rights Reserved
  * 
  ******************************************************************************
  */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "usb_device.h"
#include "usb2canfd.h"
int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    int DevHandle[10];
    int CANIndex = 0;//CANͨ����
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
    //��ʼ������CAN
    CANFD_INIT_CONFIG CANFDConfig;
    CANFDConfig.Mode = 1;        //0-����ģʽ��1-�Է�����ģʽ
    CANFDConfig.RetrySend = 1;   //ʹ���Զ��ش�
    CANFDConfig.ISOCRCEnable = 1;//ʹ��ISOCRC
    CANFDConfig.ResEnable = 1;   //ʹ���ڲ��ն˵��裨��������û���ն˵��裬�����ʹ���ն˵�����������������ݣ�
    //�����ʲ���������TCANLINPro�������Ĳ����ʼ��㹤�߼���
    //�ٲöβ����ʲ���,������=40M/NBT_BRP*(1+NBT_SEG1+NBT_SEG2)
    CANFDConfig.NBT_BRP = 1;
    CANFDConfig.NBT_SEG1 = 63;
    CANFDConfig.NBT_SEG2 = 16;
    CANFDConfig.NBT_SJW = 16;
    //���������ʲ���,������=40M/DBT_BRP*(1+DBT_SEG1+DBT_SEG2)
    CANFDConfig.DBT_BRP = 1;
    CANFDConfig.DBT_SEG1 = 15;
    CANFDConfig.DBT_SEG2 = 4;
    CANFDConfig.DBT_SJW = 4;
    ret = CANFD_Init(DevHandle[0],CANIndex,&CANFDConfig);
    if(ret != CANFD_SUCCESS){
        printf("CANFD Init Error!\n");
        return 0;
    }else{
        printf("CANFD Init Success!\n");
    }
    //���ù��������������ù�������Ĭ���ǽ�����������
    CANFD_FILTER_CONFIG CANFDFilter;
    CANFDFilter.Index = 0;//ȡֵ��ΧΪ��0~31
    CANFDFilter.Enable = 1;//0-��ֹ��1-ʹ��
    //����Ϊֻ������չ֡����
    CANFDFilter.ID_Accept = 0x80000000;
    CANFDFilter.ID_Mask = 0x80000000;
    ret = CANFD_SetFilter(DevHandle[0], CANIndex, &CANFDFilter, 1);
    if (ret != CANFD_SUCCESS){
        printf("Config filter failed!\n");
        return 0;
    }else{
        printf("Config filter success!\n");
    }
    //����CAN���ݽ���
    ret = CANFD_StartGetMsg(DevHandle[0], CANIndex);
    if (ret != CANFD_SUCCESS){
        printf("Start receive CANFD failed!\n");
        return 0;
    }else{
        printf("Start receive CANFD Success!\n");
    }
    //����CAN����
    CANFD_MSG CanMsg[5];
    for (int i = 0; i < 5; i++){
        CanMsg[i].Flags = CANFD_MSG_FLAG_FDF;//bit[0]-BRS,bit[1]-ESI,bit[2]-FDF,bit[6..5]-Channel,bit[7]-RXD
        CanMsg[i].DLC = 16;
        CanMsg[i].ID = i|CANFD_MSG_FLAG_IDE;
        for (int j = 0; j < CanMsg[i].DLC; j++){
            CanMsg[i].Data[j] = j;
        }
    }
    int SendedMsgNum = CANFD_SendMsg(DevHandle[0], CANIndex, CanMsg, 5);
    if (SendedMsgNum >= 0){
        printf("Success send frames:%d\n", SendedMsgNum);
    }else{
        printf("Send CAN data failed!\n");
        return 0;
    }
    //��������
    //��ʱ
#ifndef OS_UNIX
    Sleep(50);
#else
    usleep(50*1000);
#endif
    //��ȡ�������ݻ����е�����
    CANFD_MSG CanMsgBuffer[1024];
    int GetMsgNum = CANFD_GetMsg(DevHandle[0], CANIndex, CanMsgBuffer, 1024);
    if (GetMsgNum > 0){
        for (int i = 0; i < GetMsgNum; i++){
            printf("CanMsg[%d].ID = 0x%08X\n", i, CanMsgBuffer[i].ID & CANFD_MSG_FLAG_ID_MASK);
            printf("CanMsg[%d].TimeStamp = %d\n", i, CanMsgBuffer[i].TimeStamp);
            printf("CanMsg[%d].Data = ", i);
            for (int j = 0; j < CanMsgBuffer[i].DLC; j++)
            {
                printf("0x%02X ", CanMsgBuffer[i].Data[j]);
            }
            printf("\n");
        }
    }else if (GetMsgNum < 0){
        printf("Get CAN data error!\n");
    }
    //ֹͣ��������
    ret = CANFD_StopGetMsg(DevHandle[0], CANIndex);
    if (ret != CANFD_SUCCESS){
        printf("Stop receive CANFD failed!\n");
        return 0;
    }else{
        printf("Stop receive CANFD Success!\n");
    }
    return 0;
}

