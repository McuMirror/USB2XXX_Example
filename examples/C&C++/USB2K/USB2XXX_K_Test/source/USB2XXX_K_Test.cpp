  /*
  ******************************************************************************
  * @file     : USB2XXXGPIOTest.cpp
  * @Copyright: usbxyz 
  * @Revision : ver 1.0
  * @Date     : 2014/12/19 9:33
  * @brief    : USB2XXX GPIO test demo
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
#include "usb2k.h"
#include "usb_device.h"
#include <process.h>

int main(int argc, const char* argv[])
{
    DEVICE_INFO DevInfo;
    int DevHandle[10];
    bool state;
    int ret;
    unsigned int PinValue;
    //ɨ������豸
    ret = USB_ScanDevice(DevHandle);
    if(ret <= 0){
        printf("No device connected!\n");
        return 0;
    }else{
		printf("Found Devices:");
		for(int i=0;i<ret;i++){
			printf("%08X ",DevHandle[i]);
		}
		printf("\n");
	}
    //���豸
    state = USB_OpenDevice(DevHandle[0]);
    if(!state){
        printf("Open device error!\n");
        return 0;
    }
    //��ȡ�̼���Ϣ
    char FuncStr[256]={0};
    state = DEV_GetDeviceInfo(DevHandle[0],&DevInfo,FuncStr);
    if(!state){
        printf("Get device infomation error!\n");
        return 0;
    }else{
        printf("Firmware Info:\n");
        printf("    Name:%s\n",DevInfo.FirmwareName);
        printf("    Build Date:%s\n",DevInfo.BuildDate);
        printf("    Version:v%d.%d.%d\n",(DevInfo.FirmwareVersion>>24)&0xFF,(DevInfo.FirmwareVersion>>16)&0xFF,DevInfo.FirmwareVersion&0xFFFF);
        printf("    Version:v%d.%d.%d\n",(DevInfo.HardwareVersion>>24)&0xFF,(DevInfo.HardwareVersion>>16)&0xFF,DevInfo.HardwareVersion&0xFFFF);
        printf("    Functions:%08X\n",DevInfo.Functions);
        printf("    Functions:%s\n",FuncStr);
    }
    //��ʼ������K
    unsigned char TgtAddr = 0x33;
    K_MSG RequestMsg;
    K_MSG ResponseMsg;
    ret = K_Init(DevHandle[0],1,10400);
    if(ret != K_SUCCESS){
        printf("K_Init Error! %d\n",ret);
    }
    //5�����ʵķ�ʽ����ECU
    //ע�⣬����Ÿ���汾��ֻ��ʹ��ͨ��1��������ǵ�ͨ���汾��ֻ��ʹ��ͨ��0
#if 0
    do{
        unsigned char KB1=0,KB2=0;
        ret = K_5BaudInit(DevHandle[0],1,TgtAddr,&KB1,&KB2);
        if(ret != K_SUCCESS){
            printf("K_5BaudInit Error! ret = %d,KB1=0x%02X,KB2=0x%02X\n",ret,KB1,KB2);
        }else{
            printf("K_5BaudInit Success! KB1=0x%02X,KB2=0x%02X\n",KB1,KB2);
        }
    }while(ret != 0);
#else
    //���ٳ�ʼ������ECU
    do{
        RequestMsg.Fmt = 1;//�����ֽ�����1�ֽ�SID
        RequestMsg.Fmt |= (3<<6);
        RequestMsg.TgtAddr = TgtAddr;
        RequestMsg.SrcAddr = 0xF1;
        RequestMsg.SId = 0x81;
        ret = K_FastInit(DevHandle[0],1,&RequestMsg,&ResponseMsg,5);
        if(ret != K_SUCCESS){
            printf("K_Request Error! %d\n",ret);
        }else{
            printf("K_Request Success:\n");
            printf("--ResponseMsg.Fmt.HeaderMode = %d\n",ResponseMsg.Fmt>>6);
            printf("--ResponseMsg.Fmt.DataLen = %d\n",ResponseMsg.Fmt&0x3F);
            printf("--ResponseMsg.Fmt.TgtAddr = 0x%02X\n",ResponseMsg.TgtAddr);
            printf("--ResponseMsg.Fmt.SrcAddr = 0x%02X\n",ResponseMsg.SrcAddr);
            printf("--ResponseMsg.Fmt.SId = 0x%02X\n",ResponseMsg.SId);
            printf("--ResponseMsg.Fmt.Data = ");
            for(int i=0;i<(ResponseMsg.Fmt&0x3F-1);i++){//�����ֽ���Ӧ�ü�ȥ1�ֽ�SID
                printf("0x%02X ",ResponseMsg.Data[i]);
            }
            printf("\n");
        }
    }while(ret != 0);
#endif
    //����ϵͳ����ָ��
    RequestMsg.Fmt = 1;//�����ֽ�����1�ֽ�SID
    RequestMsg.Fmt |= (3<<6);
    RequestMsg.TgtAddr = TgtAddr;
    RequestMsg.SrcAddr = 0xF1;
    RequestMsg.SId = 0x81;
    ret = K_Request(DevHandle[0],1,&RequestMsg,&ResponseMsg,5,1000);
    if(ret != K_SUCCESS){
        printf("K_Request Error! %d\n",ret);
    }else{
        printf("K_Request Success:\n");
        printf("--ResponseMsg.Fmt.HeaderMode = %d\n",ResponseMsg.Fmt>>6);
        printf("--ResponseMsg.Fmt.DataLen = %d\n",ResponseMsg.Fmt&0x3F);
        printf("--ResponseMsg.Fmt.TgtAddr = 0x%02X\n",ResponseMsg.TgtAddr);
        printf("--ResponseMsg.Fmt.SrcAddr = 0x%02X\n",ResponseMsg.SrcAddr);
        printf("--ResponseMsg.Fmt.SId = 0x%02X\n",ResponseMsg.SId);
        printf("--ResponseMsg.Fmt.Data = ");
        for(int i=0;i<(ResponseMsg.Fmt&0x3F-1);i++){//�����ֽ���Ӧ�ü�ȥ1�ֽ�SID
            printf("0x%02X ",ResponseMsg.Data[i]);
        }
        printf("\n");
    }
    //�ر��豸
    USB_CloseDevice(DevHandle[0]);
    return 0;
}



