  /*
  ******************************************************************************
  * @file     : USB2XXXLINTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX LIN test demo
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
#include "usb2lin_ex.h"

#define GET_FIRMWARE_INFO       1//��ȡ�̼���Ϣ
#define LIN_MASTER_TEST         1//����ģʽ��д����
#define LIN_SLAVE_TEST          0//�ӻ�ģʽ��д����
#define LIN_MASTER_SCH_TEST     0//����ģʽ���ȱ����

int main(int argc, const char* argv[])
{
#if GET_FIRMWARE_INFO
    DEVICE_INFO DevInfo;
#endif
    int DevHandle[10];
    int LINMasterIndex = 0;
    int LINSlaveIndex = 0;
    int DevIndex = 0;
    bool state;
    int ret;
    char *MSGTypeStr[]={"UN","MW","MR","SW","SR","BK","SY","ID","DT","CK"};
    char *CKTypeStr[]={"STD","EXT","USER","NONE","ERROR"};
    //ɨ������豸
    ret = USB_ScanDevice(DevHandle);
    if(ret <= 0){
        printf("No device connected!\n");
        return 0;
    }
    /*
    //��������ͬʱ���ж��LIN������ʱ�����Ը����豸��ָ����Ӧ���豸
    for(int i=0;i<ret;i++){
        if(DevHandle[i]==0x1C0){
            DevIndex = i;
            break;
        }
    }
    */
    //���豸
    state = USB_OpenDevice(DevHandle[DevIndex]);
    if(!state){
        printf("Open device error!\n");
        return 0;
    }
#if GET_FIRMWARE_INFO
    char FunctionStr[256]={0};
    //��ȡ�̼���Ϣ
    state = DEV_GetDeviceInfo(DevHandle[DevIndex],&DevInfo,FunctionStr);
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
        printf("Firmware SerialNumber:%08X%08X%08X\n",DevInfo.SerialNumber[0],DevInfo.SerialNumber[1],DevInfo.SerialNumber[2]);
    }
#endif
//    USB_ResetDevice(DevHandle[DevIndex]);//��λ�豸���Ժ���
//    return 0;
#if LIN_MASTER_SCH_TEST||LIN_MASTER_TEST
    //��ʼ������LIN
    ret = LIN_EX_Init(DevHandle[DevIndex],LINMasterIndex,19200,1);
    if(ret != LIN_EX_SUCCESS){
        printf("Config LIN failed!\n");
        return 0;
    }else{
        printf("Config LIN Success!\n");
    }
#endif
#if LIN_MASTER_TEST
    LIN_EX_MSG LINMsg[5];
    LIN_EX_MSG LINOutMsg[10];
    unsigned int MsgLen = 5;
    LINMsg[0].MsgType = LIN_EX_MSG_TYPE_BK;
    LINMsg[0].Timestamp = 10;//���͸�֡����֮�����ʱʱ�䣬��С��������Ϊ1
    //����4��IDȫ��Ϊ������������ģʽ
    for(int f=1;f<MsgLen;f++){
        LINMsg[f].MsgType = LIN_EX_MSG_TYPE_MW;//����д����ģʽ
        LINMsg[f].DataLen = 8;
        for(int i=0;i<LINMsg[1].DataLen;i++){
            LINMsg[f].Data[i] = (f<<4)|i;
        }
        LINMsg[f].Timestamp = 10;//���͸�֡����֮�����ʱʱ�䣬��С��������Ϊ1
        LINMsg[f].CheckType = LIN_EX_CHECK_EXT;//���ú�У�����ͺ������������У�������Զ�����У������
        LINMsg[f].PID = f+1;
    }
    ret = LIN_EX_MasterSync(DevHandle[DevIndex],LINMasterIndex,LINMsg,LINOutMsg,MsgLen);
    if(ret < LIN_EX_SUCCESS){
        printf("MasterSync LIN failed!\n");
        return 0;
    }else{
        printf("MsgLen = %d\n",ret);
        for(int i=0;i<ret;i++){
            printf("%s SYNC[%02X] PID[%02X] ",MSGTypeStr[LINOutMsg[i].MsgType],LINOutMsg[i].Sync,LINOutMsg[i].PID);
            for(int j=0;j<LINOutMsg[i].DataLen;j++){
                printf("%02X ",LINOutMsg[i].Data[j]);
            }
            printf("[%s][%02X] [%02d:%02d:%02d.%03d]\n",CKTypeStr[LINOutMsg[i].CheckType],LINOutMsg[i].Check,(LINOutMsg[i].Timestamp/36000000)%60,(LINOutMsg[i].Timestamp/600000)%60,(LINOutMsg[i].Timestamp/10000)%60,(LINOutMsg[i].Timestamp/10)%1000);
        }
    }
    //����4��IDȫ��Ϊ����������ģʽ
    for(int f=1;f<MsgLen;f++){
        LINMsg[f].MsgType = LIN_EX_MSG_TYPE_MR;//����������ģʽ
        LINMsg[f].Timestamp = 10;//������ȡ������֮�����ʱʱ�䣬��С��������Ϊ1
        LINMsg[f].PID = f+1;
    }
    ret = LIN_EX_MasterSync(DevHandle[DevIndex],LINMasterIndex,LINMsg,LINOutMsg,MsgLen);
    if(ret < LIN_EX_SUCCESS){
        printf("MasterSync LIN failed!\n");
        return 0;
    }else{
        printf("MsgLen = %d\n",ret);
        for(int i=0;i<ret;i++){
            printf("%s SYNC[%02X] PID[%02X] ",MSGTypeStr[LINOutMsg[i].MsgType],LINOutMsg[i].Sync,LINOutMsg[i].PID);
            for(int j=0;j<LINOutMsg[i].DataLen;j++){
                printf("%02X ",LINOutMsg[i].Data[j]);
            }
            printf("[%s][%02X] [%02d:%02d:%02d.%03d]\n",CKTypeStr[LINOutMsg[i].CheckType],LINOutMsg[i].Check,(LINOutMsg[i].Timestamp/36000000)%60,(LINOutMsg[i].Timestamp/600000)%60,(LINOutMsg[i].Timestamp/10000)%60,(LINOutMsg[i].Timestamp/10)%1000);
        }
    }
#endif

#if LIN_SLAVE_TEST
    //��ʼ������LIN
    ret = LIN_EX_Init(DevHandle[DevIndex],LINSlaveIndex,9600,0);
    if(ret != LIN_EX_SUCCESS){
        printf("Config LIN failed!\n");
        return 0;
    }else{
        printf("Config LIN Success!\n");
    }
    printf("Start Get LIN Data...\n");
    //����IDΪLIN_EX_MSG_TYPE_SWģʽ�����������Ϳ��Զ�ȡ������
    LIN_EX_MSG LINSlaveMsg[10];
    for(int i=0;i<10;i++){
        LINSlaveMsg[i].PID = i;
        LINSlaveMsg[i].CheckType = LIN_EX_CHECK_EXT;
        LINSlaveMsg[i].DataLen = 7;
        for(int j=0;j<LINSlaveMsg[i].DataLen;j++){
            LINSlaveMsg[i].Data[j]=(i<<4)|j;
        }
        LINSlaveMsg[i].MsgType = LIN_EX_MSG_TYPE_SW;//�ӻ���������ģʽ
    }
    ret = LIN_EX_SlaveSetIDMode(DevHandle[DevIndex],LINSlaveIndex,LINSlaveMsg,10);
    if(ret != LIN_EX_SUCCESS){
        printf("Config LIN ID Mode failed!\n");
        return 0;
    }else{
        printf("Config LIN ID Mode Success!\n");
    }
    //ѭ����ȡ���յ������ݣ��ò�����������LIN�������ݼ��
    while(1)
    {
        LIN_EX_MSG LINMsg[1024];//������������һ�㣬��ֹ�洦
        int ret = LIN_EX_SlaveGetData(DevHandle[DevIndex],LINSlaveIndex,LINMsg);
        for(int i=0;i<ret;i++){
            printf("%s SYNC[%02X] PID[%02X] ",MSGTypeStr[LINMsg[i].MsgType],LINMsg[i].Sync,LINMsg[i].PID);
            for(int j=0;j<LINMsg[i].DataLen;j++){
                printf("%02X ",LINMsg[i].Data[j]);
            }
            printf("[%s][%02X] [%02d:%02d:%02d.%03d]\n",CKTypeStr[LINMsg[i].CheckType],LINMsg[i].Check,(LINMsg[i].Timestamp/36000000)%60,(LINMsg[i].Timestamp/600000)%60,(LINMsg[i].Timestamp/10000)%60,(LINMsg[i].Timestamp/10)%1000);
        }
        Sleep(1000);
    }
#endif
#if LIN_MASTER_SCH_TEST
    LIN_EX_MSG LINMsg[6];
    LIN_EX_MSG LINOutMsg[10];
    unsigned int MsgLen = 6;
    LINMsg[0].MsgType = LIN_EX_MSG_TYPE_BK;
    LINMsg[0].Timestamp = 20;//��ǰ֡����ʱ����һ֡���ݷ��͵�ʱ��������λΪ����
    for(int f=1;f<MsgLen;f++){
        LINMsg[f].MsgType = LIN_EX_MSG_TYPE_MW;
        LINMsg[f].DataLen = 8;
        for(int i=0;i<LINMsg[1].DataLen;i++){
            LINMsg[f].Data[i] = (f<<4)|i;
        }
        LINMsg[f].Timestamp = 20+f*10;//��ǰ֡����ʱ����һ֡���ݷ��͵�ʱ��������λΪ����
        LINMsg[f].CheckType = LIN_EX_CHECK_EXT;
        LINMsg[f].PID = f+1;
    }
    //ID=6����֡Ϊ����������ģʽ��Ҳ������������֡ͷ���ӻ���������
    LINMsg[5].MsgType = LIN_EX_MSG_TYPE_MR;
    LINMsg[5].PID = 6;
    LINMsg[5].Timestamp = 20;
    //�������ȱ�
    ret = LIN_EX_MasterStartSch(DevHandle[DevIndex],LINMasterIndex,LINMsg,MsgLen);
    if(ret < LIN_EX_SUCCESS){
        printf("Master schedule start failed!\n");
        return 0;
    }else{
        printf("Master schedule start success!\n");
    }
    //ѭ����ȡ���ȱ�����
    for(int i=0;i<10;i++){
        Sleep(1000);
        ret = LIN_EX_MasterGetSch(DevHandle[DevIndex],LINMasterIndex,LINOutMsg);
        if(ret < LIN_EX_SUCCESS){
            printf("Master get schedule failed!\n");
            return 0;
        }else{
            printf("[0]MsgLen = %d\n",ret);
            for(int i=0;i<ret;i++){
                printf("[0]%s SYNC[%02X] PID[%02X] ",MSGTypeStr[LINOutMsg[i].MsgType],LINOutMsg[i].Sync,LINOutMsg[i].PID);
                for(int j=0;j<LINOutMsg[i].DataLen;j++){
                    printf("%02X ",LINOutMsg[i].Data[j]);
                }
                printf("\n");
            }
        }
    }
    //ֹͣ���ȱ�
    LIN_EX_MasterStopSch(DevHandle[DevIndex],LINMasterIndex);
#endif
    //�ر��豸
    USB_CloseDevice(DevHandle[DevIndex]);
	return 0;
}

