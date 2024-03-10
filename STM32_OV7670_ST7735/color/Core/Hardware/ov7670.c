#include "ov7670.h"

#define OV7670_ADDRESS 0x42

volatile uint8_t take_photo_flag = 1;  // 0空闲，1准备拍照，2拍照中

// i2c若是fastmode(400kHz)会导致初始化常常有问题
static void WrCmos7670(uint8_t regID, uint8_t regDat) {
    HAL_I2C_Mem_Write(&hi2c1, OV7670_ADDRESS, regID, I2C_MEMADD_SIZE_8BIT, &regDat, 1, 0x100);
}

static void set_Cmos7670reg(void) {
    // BLUE 0x01
    // AWB-蓝色通道增益 [00]~[FF]
    WrCmos7670(0x01, 0x01);

    // RED 0x02
    // AWB-红色通道增益 [00]~[FF]
    WrCmos7670(0x02, 0x00);  //这两个改了都没啥反应

    /**
     *Frame Rate Adjustment for x Mhz input clock
     *x fps, PCLK = xMhz
     */
    //WrCmos7670(0x11, 0x00);	// 内部时钟 CLKRC，对于拍照来说，值越大，越清楚，刷屏现像 越不重，摄像时，值过大 会造成跟不上
    // 00 时，可能是对于 2.8的屏，对于 2.4屏 会出现 7格分屏现象
    // 值越大时，摄头移动 图像拖尾就会越严重，反之OK
    // 3值适中，2已有3分屏，0有7分屏，5拖尾重，10已花屏
    // [7] 保留
    // [6] 直接使用外部时钟（没有预分频）
    // [5~0] 内部时钟分频    内部时钟 ＝ 输入时钟/([5~0] + 1)    [5~0] = 00000~11111

    WrCmos7670(0x11, 0x82);

    WrCmos7670(0x6b, 0x60);  //PLL*4 0x60
    WrCmos7670(0x2a, 0x00);  //虚拟像素高位插入
    WrCmos7670(0x2b, 0x00);  //虚拟像素低位插入
    WrCmos7670(0x92, 0x00);  // DM_LNL 空行低8位
    WrCmos7670(0x93, 0x00);  // DM_LNH 空行高8位
    WrCmos7670(0x3b, 0x02);  // 夜晚模式等一堆东西，0x02即可

    // COM7 0x12
    // 位[7]:	SCCB寄存器复位
    // 			0：不复位
    // 			1：复位
    // 位[6]:	保留
    // 位[5]:	输出格式-CIF
    // 位[4]:	输出格式-QVGA 240×320
    // 位[3]:	输出格式-QCIF
    // 位[2]:	输出格式-RGB(见下面）
    // 位[1]:	彩色条
    // 			0:非使能
    // 			1:使能
    // 位[0]:	输出格式-Raw RGB（见以下）
    // 						COM7[2] 	COM7[0]
    // YUV 					0 			0
    // RGB 					1 			0
    // Bayer RAW 			0 			1
    // Processed Bayer RAW 	1 			1
    WrCmos7670(0x12, 0x14);  //0x14 0001 0100

    //RGB555/565 option(must set COM7[2] = 1 and COM7[0] = 0)
    // 位[7：6]：数据形式-全范围输出使能
    // 		0X：输出范围：[10]到[F0]
    // 		10：输出范围：[01]到[FE]
    // 		11：输出范围：[00]到[FF]
    // 位[5：4]：RGB555/565操作（在COM7[2]=1和 COM7[0]=0时有效)（0x12）
    // 		X0:一般RGB输出
    // 		01：RGB565，在RGB444[1]为低时有效（0x8c）
    // 		11：RGB555，在RGB444[1]为低时有效
    // 位[3：0]：保留
    WrCmos7670(0x40, 0xd0);  //1101 0000
    //RGB444
    // 位[1]：RGB444使能，COM[4]=1 有效
    // 		0：非使能
    // 		1：使能
    // 位[0]：RGB444字形式
    // 		0：xR GB
    // 		1：RG Bx
    WrCmos7670(0x8c, 0x00);

    // 0x3a
    // 位[5]：负片使能
    // 0：正常
    // 1：负片
    WrCmos7670(0x3a, 0x04);  //默认值0x0D,设置成0x04就好
    //这俩无效的
    WrCmos7670(0x67, 0x80);
    WrCmos7670(0x68, 0x80);

    //上下或左右颠倒
    //默认0x01
    // 位[7：6]：保留
    // 位[5]：水平镜像使能
    // 		0：正常
    // 		1：镜像
    // 位[4]：竖直翻转使能
    // 		0：正常
    // 		1：翻转
    // 位[3]：保留
    // 位[2]：消除黑太阳使能
    // 位[1：0]：保留
    WrCmos7670(0x1e, 0x37);  //0011 0111

    //条纹滤波器 Setting for 24Mhz Input Clock,30fps for 60Hz light frequency
    // 位[7]：使能快速AGC/AEC算法
    // 位[6]：AEC-步长限制
    // 		0：步长限制与垂直同步
    // 		1：不限制步长
    // 位[5]：条纹滤波器打开/关闭-打开条纹滤波器 ,BD50ST(0x9D)或者BD60ST(0x9E)要设成1
    // 		0：关
    // 		1：开
    // 位[4：3]：保留
    // 位[2]：AGC使能 自动增益控制
    // 位[1]：AWB使能 自动白平衡
    // 位[0]：AEC使能 自动曝光
    WrCmos7670(0x13, 0xff);  //banding filer enable 默认e7
    WrCmos7670(0x9d, 0x4c);  //50Hz banding filer
    WrCmos7670(0x9e, 0x3f);  //60Hz banding filer
    WrCmos7670(0xa5, 0x05);  //3 step for 50Hz
    WrCmos7670(0xab, 0x07);  //4 step for 60Hz
    //WrCmos7670(0x3b, 0x02);//select 60Hz banding filer 上面出现过一次

    //Simple White Balance - 白平衡
    //WrCmos7670(0x13, 0xe7);//AWB、AGC、AGC Enable and ...上面出现过一次
    //AWBC - 自动白平衡控制(Automatic white balance control)
    WrCmos7670(0x43, 0x14);  //用户手册里这些寄存器的值都是保留(Reserved),不用设置的呀？
    WrCmos7670(0x44, 0xf0);
    WrCmos7670(0x45, 0x34);  //34  手册45
    WrCmos7670(0x46, 0x58);  //58  手册61
    WrCmos7670(0x47, 0x28);  //28  手册51
    WrCmos7670(0x48, 0x3a);  //3a  手册79  都没区别

    // WrCmos7670(0x59, 0x88);//用户手册连寄存器都是保留，初始值都没提供
    // WrCmos7670(0x5a, 0x88);
    // WrCmos7670(0x5b, 0x44);
    // WrCmos7670(0x5c, 0x67);
    // WrCmos7670(0x5d, 0x49);
    // WrCmos7670(0x5e, 0x0e);

    WrCmos7670(0x6a, 0x40);  // G通道 AWB增益		改了没啥变化
    WrCmos7670(0x6c, 0x02);  // AWBCTR3 AWB控制3	0a	手册默认02
    WrCmos7670(0x6d, 0x55);  // AWBCTR2 AWB控制2	55	手册默认55
    WrCmos7670(0x6e, 0xc0);  // AWBCTR1 AWB控制1	11	手册默认c0
    WrCmos7670(0x6f, 0x9a);  // AWBCTR0 AWB控制0	9f	手册默认9a	初步测试只有这位有反应，而且bit顺序要反过来，大的蓝，小的绿

    //AGC/AEC - Automatic Gain Control自动增益补偿/Automatic exposure Control自动曝光控制
    // AGC 自动增益控制 默认00
    WrCmos7670(0x00, 0x80);// AGC 自动增益控制 (值越大 能有效控制 黑像时刷花屏现象)	位[7:0]:AGC[7:0]
    //WrCmos7670(0x03, 0x06);// 0000 0110											位[7:6]:AGC[9:8]
    // WrCmos7670(0x14, 0x20);//0x38, limit the max gain
    // WrCmos7670(0x24, 0x75);// AEW AGC/AEC稳定运行区域上限
    // WrCmos7670(0x25, 0x63);// AEB AGC/AEC稳定运行区域下限
    // WrCmos7670(0x26, 0xA5);// VPT AGC/AEC快速运行区域

    WrCmos7670(0x32, 0x80);
    WrCmos7670(0x17, 0x16);
    WrCmos7670(0x18, 0x04);
    WrCmos7670(0x19, 0x02);
    WrCmos7670(0x1a, 0x7b);
    WrCmos7670(0x0c, 0x00);
    WrCmos7670(0x3e, 0x00);
    WrCmos7670(0x70, 0x3a);
    WrCmos7670(0x71, 0x35);  // 如果调整为WrCmos7670(0x71, 0x80);会显示8条彩色竖条，调试用WrCmos7670(0x71, 0x35);
    WrCmos7670(0x72, 0x11);
    WrCmos7670(0x73, 0x00);
    WrCmos7670(0xa2, 0x02);

    WrCmos7670(0x7a, 0x20);
    WrCmos7670(0x7b, 0x1c);
    WrCmos7670(0x7c, 0x28);
    WrCmos7670(0x7d, 0x3c);
    WrCmos7670(0x7e, 0x55);
    WrCmos7670(0x7f, 0x68);
    WrCmos7670(0x80, 0x76);
    WrCmos7670(0x81, 0x80);
    WrCmos7670(0x82, 0x88);
    WrCmos7670(0x83, 0x8f);
    WrCmos7670(0x84, 0x96);
    WrCmos7670(0x85, 0xa3);
    WrCmos7670(0x86, 0xaf);
    WrCmos7670(0x87, 0xc4);
    WrCmos7670(0x88, 0xd7);
    WrCmos7670(0x89, 0xe8);

    WrCmos7670(0x10, 0x40);  // 曝光值，默认40	改了没反应(其实是有的)
    WrCmos7670(0x0d, 0x00);
    WrCmos7670(0x14, 0x28);
    WrCmos7670(0x24, 0x75);
    WrCmos7670(0x25, 0x63);
    WrCmos7670(0x26, 0xA5);
    WrCmos7670(0x9f, 0x78);
    WrCmos7670(0xa0, 0x68);
    WrCmos7670(0xa1, 0x03);
    WrCmos7670(0xa6, 0xdf);
    WrCmos7670(0xa7, 0xdf);
    WrCmos7670(0xa8, 0xf0);
    WrCmos7670(0xa9, 0x90);
    WrCmos7670(0xaa, 0x94);

    WrCmos7670(0x0e, 0x61);
    WrCmos7670(0x0f, 0x4b);
    WrCmos7670(0x16, 0x02);
    WrCmos7670(0x21, 0x02);
    WrCmos7670(0x22, 0x91);
    WrCmos7670(0x29, 0x07);
    WrCmos7670(0x33, 0x0b);
    WrCmos7670(0x35, 0x0b);
    WrCmos7670(0x37, 0x1d);
    WrCmos7670(0x38, 0x71);
    WrCmos7670(0x39, 0x2a);
    WrCmos7670(0x3c, 0x78);
    WrCmos7670(0x4d, 0x40);
    WrCmos7670(0x4e, 0x20);
    WrCmos7670(0x69, 0x00);
    WrCmos7670(0x74, 0x19);
    WrCmos7670(0x8d, 0x4f);
    WrCmos7670(0x8e, 0x00);
    WrCmos7670(0x8f, 0x00);
    WrCmos7670(0x90, 0x00);
    WrCmos7670(0x91, 0x00);
    WrCmos7670(0x96, 0x00);
    WrCmos7670(0x9a, 0x80);
    WrCmos7670(0xb0, 0x84);
    WrCmos7670(0xb1, 0x0c);
    WrCmos7670(0xb2, 0x0e);
    WrCmos7670(0xb3, 0x82);
    WrCmos7670(0xb8, 0x0a);

    WrCmos7670(0x43, 0x14);
    WrCmos7670(0x44, 0xf0);
    WrCmos7670(0x45, 0x34);
    WrCmos7670(0x46, 0x58);
    WrCmos7670(0x47, 0x28);
    WrCmos7670(0x48, 0x3a);
    WrCmos7670(0x59, 0x88);
    WrCmos7670(0x5a, 0x88);
    WrCmos7670(0x5b, 0x44);
    WrCmos7670(0x5c, 0x67);
    WrCmos7670(0x5d, 0x49);
    WrCmos7670(0x5e, 0x0e);
    WrCmos7670(0x64, 0x04);
    WrCmos7670(0x65, 0x20);
    WrCmos7670(0x66, 0x05);
    WrCmos7670(0x94, 0x04);
    WrCmos7670(0x95, 0x08);
    WrCmos7670(0x6c, 0x0a);
    WrCmos7670(0x6d, 0x55);
    WrCmos7670(0x6e, 0x11);
    WrCmos7670(0x6a, 0x40);
    WrCmos7670(0x15, 0x00);

    WrCmos7670(0x4f, 0x80);
    WrCmos7670(0x50, 0x80);
    WrCmos7670(0x51, 0x00);
    WrCmos7670(0x52, 0x22);
    WrCmos7670(0x53, 0x5e);
    WrCmos7670(0x54, 0x80);
    WrCmos7670(0x58, 0x9e);

    WrCmos7670(0x41, 0x08);
    WrCmos7670(0x3f, 0x00);
    WrCmos7670(0x75, 0x05);
    WrCmos7670(0x76, 0xe1);
    WrCmos7670(0x4c, 0x00);
    WrCmos7670(0x77, 0x01);
    WrCmos7670(0x3d, 0xc2);
    WrCmos7670(0x4b, 0x09);
    WrCmos7670(0xc9, 0x60);
    WrCmos7670(0x41, 0x38);
    WrCmos7670(0x56, 0x40);

    WrCmos7670(0x34, 0x11);

    WrCmos7670(0xa4, 0x89);
    WrCmos7670(0x96, 0x00);
    WrCmos7670(0x97, 0x30);
    WrCmos7670(0x98, 0x20);
    WrCmos7670(0x99, 0x30);
    WrCmos7670(0x9a, 0x84);
    WrCmos7670(0x9b, 0x29);
    WrCmos7670(0x9c, 0x03);
    WrCmos7670(0x78, 0x04);

    WrCmos7670(0x79, 0x01);
    WrCmos7670(0xc8, 0xf0);
    WrCmos7670(0x79, 0x0f);
    WrCmos7670(0xc8, 0x00);
    WrCmos7670(0x79, 0x10);
    WrCmos7670(0xc8, 0x7e);
    WrCmos7670(0x79, 0x0a);
    WrCmos7670(0xc8, 0x80);
    WrCmos7670(0x79, 0x0b);
    WrCmos7670(0xc8, 0x01);
    WrCmos7670(0x79, 0x0c);
    WrCmos7670(0xc8, 0x0f);
    WrCmos7670(0x79, 0x0d);
    WrCmos7670(0xc8, 0x20);
    WrCmos7670(0x79, 0x09);
    WrCmos7670(0xc8, 0x80);
    WrCmos7670(0x79, 0x02);
    WrCmos7670(0xc8, 0xc0);
    WrCmos7670(0x79, 0x03);
    WrCmos7670(0xc8, 0x40);
    WrCmos7670(0x79, 0x05);
    WrCmos7670(0xc8, 0x30);
    WrCmos7670(0x79, 0x26);
    WrCmos7670(0x09, 0x00);

    //////////////////////////////////////////////////////////////
    //  WrCmos7670(0x3a, 0x04);
    // WrCmos7670(0x40, 0xd0);
    // WrCmos7670(0x12, 0x14);
    // WrCmos7670(0x32, 0x80);
    // WrCmos7670(0x17, 0x16);
    // WrCmos7670(0x18, 0x04);
    // WrCmos7670(0x19, 0x02);
    // WrCmos7670(0x1a, 0x7b);
    // WrCmos7670(0x03, 0x06);
    // WrCmos7670(0x0c, 0x00);
    // WrCmos7670(0x3e, 0x00);
    // WrCmos7670(0x70, 0x3a);
    // WrCmos7670(0x71, 0x35);
    // WrCmos7670(0x72, 0x11);
    // WrCmos7670(0x73, 0x00);
    // WrCmos7670(0xa2, 0x02);
    // WrCmos7670(0x11, 0x81);

    // WrCmos7670(0x7a, 0x20);
    // WrCmos7670(0x7b, 0x1c);
    // WrCmos7670(0x7c, 0x28);
    // WrCmos7670(0x7d, 0x3c);
    // WrCmos7670(0x7e, 0x55);
    // WrCmos7670(0x7f, 0x68);
    // WrCmos7670(0x80, 0x76);
    // WrCmos7670(0x81, 0x80);
    // WrCmos7670(0x82, 0x88);
    // WrCmos7670(0x83, 0x8f);
    // WrCmos7670(0x84, 0x96);
    // WrCmos7670(0x85, 0xa3);
    // WrCmos7670(0x86, 0xaf);
    // WrCmos7670(0x87, 0xc4);
    // WrCmos7670(0x88, 0xd7);
    // WrCmos7670(0x89, 0xe8);

    // WrCmos7670(0x13, 0xe0);
    // WrCmos7670(0x00, 0x00);

    // WrCmos7670(0x10, 0x00);
    // WrCmos7670(0x0d, 0x00);
    // WrCmos7670(0x14, 0x28);
    // WrCmos7670(0xa5, 0x05);
    // WrCmos7670(0xab, 0x07);
    // WrCmos7670(0x24, 0x75);
    // WrCmos7670(0x25, 0x63);
    // WrCmos7670(0x26, 0xA5);
    // WrCmos7670(0x9f, 0x78);
    // WrCmos7670(0xa0, 0x68);
    // WrCmos7670(0xa1, 0x03);
    // WrCmos7670(0xa6, 0xdf);
    // WrCmos7670(0xa7, 0xdf);
    // WrCmos7670(0xa8, 0xf0);
    // WrCmos7670(0xa9, 0x90);
    // WrCmos7670(0xaa, 0x94);
    // WrCmos7670(0x13, 0xe5);

    // WrCmos7670(0x0e, 0x61);
    // WrCmos7670(0x0f, 0x4b);
    // WrCmos7670(0x16, 0x02);
    // WrCmos7670(0x1e, 0x37);
    // WrCmos7670(0x21, 0x02);
    // WrCmos7670(0x22, 0x91);
    // WrCmos7670(0x29, 0x07);
    // WrCmos7670(0x33, 0x0b);
    // WrCmos7670(0x35, 0x0b);
    // WrCmos7670(0x37, 0x1d);
    // WrCmos7670(0x38, 0x71);
    // WrCmos7670(0x39, 0x2a);
    // WrCmos7670(0x3c, 0x78);
    // WrCmos7670(0x4d, 0x40);
    // WrCmos7670(0x4e, 0x20);
    // WrCmos7670(0x69, 0x00);

    // //WrCmos7670(0x6b, 0xe0);                        //设置PLL倍频,提高运行频率,赋值0x60为30pcs,0xe0为60pcs.
    // WrCmos7670(0x6b, 0x60);                            //30pcs

    // WrCmos7670(0x74, 0x19);
    // WrCmos7670(0x8d, 0x4f);
    // WrCmos7670(0x8e, 0x00);
    // WrCmos7670(0x8f, 0x00);
    // WrCmos7670(0x90, 0x00);
    // WrCmos7670(0x91, 0x00);
    // WrCmos7670(0x92, 0x00);
    // WrCmos7670(0x96, 0x00);
    // WrCmos7670(0x9a, 0x80);
    // WrCmos7670(0xb0, 0x84);
    // WrCmos7670(0xb1, 0x0c);
    // WrCmos7670(0xb2, 0x0e);
    // WrCmos7670(0xb3, 0x82);
    // WrCmos7670(0xb8, 0x0a);

    // WrCmos7670(0x43, 0x14);
    // WrCmos7670(0x44, 0xf0);
    // WrCmos7670(0x45, 0x34);
    // WrCmos7670(0x46, 0x58);
    // WrCmos7670(0x47, 0x28);
    // WrCmos7670(0x48, 0x3a);
    // WrCmos7670(0x59, 0x88);
    // WrCmos7670(0x5a, 0x88);
    // WrCmos7670(0x5b, 0x44);
    // WrCmos7670(0x5c, 0x67);
    // WrCmos7670(0x5d, 0x49);
    // WrCmos7670(0x5e, 0x0e);
    // WrCmos7670(0x64, 0x04);
    // WrCmos7670(0x65, 0x20);
    // WrCmos7670(0x66, 0x05);
    // WrCmos7670(0x94, 0x04);
    // WrCmos7670(0x95, 0x08);
    // WrCmos7670(0x6c, 0x0a);
    // WrCmos7670(0x6d, 0x55);
    // WrCmos7670(0x6e, 0x11);
    // WrCmos7670(0x6f, 0x9f);
    // WrCmos7670(0x6a, 0x40);
    // WrCmos7670(0x01, 0x40);
    // WrCmos7670(0x02, 0x40);
    // WrCmos7670(0x13, 0xe7);
    // WrCmos7670(0x15, 0x00);

    // WrCmos7670(0x4f, 0x80);
    // WrCmos7670(0x50, 0x80);
    // WrCmos7670(0x51, 0x00);
    // WrCmos7670(0x52, 0x22);
    // WrCmos7670(0x53, 0x5e);
    // WrCmos7670(0x54, 0x80);
    // WrCmos7670(0x58, 0x9e);

    // WrCmos7670(0x41, 0x08);
    // WrCmos7670(0x3f, 0x00);
    // WrCmos7670(0x75, 0x05);
    // WrCmos7670(0x76, 0xe1);
    // WrCmos7670(0x4c, 0x00);
    // WrCmos7670(0x77, 0x01);
    // WrCmos7670(0x3d, 0xc2);
    // WrCmos7670(0x4b, 0x09);
    // WrCmos7670(0xc9, 0x60);
    // WrCmos7670(0x41, 0x38);
    // WrCmos7670(0x56, 0x40);

    // WrCmos7670(0x34, 0x11);
    // WrCmos7670(0x3b, 0x02);

    // WrCmos7670(0xa4, 0x89);
    // WrCmos7670(0x96, 0x00);
    // WrCmos7670(0x97, 0x30);
    // WrCmos7670(0x98, 0x20);
    // WrCmos7670(0x99, 0x30);
    // WrCmos7670(0x9a, 0x84);
    // WrCmos7670(0x9b, 0x29);
    // WrCmos7670(0x9c, 0x03);
    // WrCmos7670(0x9d, 0x4c);
    // WrCmos7670(0x9e, 0x3f);
    // WrCmos7670(0x78, 0x04);

    // WrCmos7670(0x79, 0x01);
    // WrCmos7670(0xc8, 0xf0);
    // WrCmos7670(0x79, 0x0f);
    // WrCmos7670(0xc8, 0x00);
    // WrCmos7670(0x79, 0x10);
    // WrCmos7670(0xc8, 0x7e);
    // WrCmos7670(0x79, 0x0a);
    // WrCmos7670(0xc8, 0x80);
    // WrCmos7670(0x79, 0x0b);
    // WrCmos7670(0xc8, 0x01);
    // WrCmos7670(0x79, 0x0c);
    // WrCmos7670(0xc8, 0x0f);
    // WrCmos7670(0x79, 0x0d);
    // WrCmos7670(0xc8, 0x20);
    // WrCmos7670(0x79, 0x09);
    // WrCmos7670(0xc8, 0x80);
    // WrCmos7670(0x79, 0x02);
    // WrCmos7670(0xc8, 0xc0);
    // WrCmos7670(0x79, 0x03);
    // WrCmos7670(0xc8, 0x40);
    // WrCmos7670(0x79, 0x05);
    // WrCmos7670(0xc8, 0x30);
    // WrCmos7670(0x79, 0x26);
    // WrCmos7670(0x09, 0x00);
}

void ov7670_init(void) {
    HAL_GPIO_WritePin(CAM_RST_GPIO_Port, CAM_RST_Pin, GPIO_PIN_RESET);  // reset low
    HAL_Delay(20);
    HAL_GPIO_WritePin(CAM_RST_GPIO_Port, CAM_RST_Pin, GPIO_PIN_SET);  // reset high
    uint8_t data;
    data = 0x80;
    WrCmos7670(0x12, data);
    HAL_Delay(20);
    set_Cmos7670reg();
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == CAM_VSYNC_Pin) {  // 摄像头帧同步的中断
        if (take_photo_flag == 1) {
            take_photo_flag = 2;
            HAL_GPIO_WritePin(CAM_WRST_GPIO_Port, CAM_WRST_Pin, GPIO_PIN_RESET);  // OV7670_WRST = 0;  复位写指针
            HAL_GPIO_WritePin(CAM_WRST_GPIO_Port, CAM_WRST_Pin, GPIO_PIN_SET);  // OV7670_WRST = 1;
            HAL_GPIO_WritePin(CAM_WEN_GPIO_Port, CAM_WEN_Pin, GPIO_PIN_SET);  // OV7670_WREN = 1;  允许写入FIFO
        } else if (take_photo_flag == 2) {
            take_photo_flag = 0;
            HAL_GPIO_WritePin(CAM_WRST_GPIO_Port, CAM_WRST_Pin, GPIO_PIN_RESET);  // OV7670_WRST = 0;  复位写指针
            HAL_GPIO_WritePin(CAM_WRST_GPIO_Port, CAM_WRST_Pin, GPIO_PIN_SET);  // OV7670_WRST = 1;
            HAL_GPIO_WritePin(CAM_WEN_GPIO_Port, CAM_WEN_Pin, GPIO_PIN_RESET);  // OV7670_WREN = 0;  禁止写入FIFO
        } else {
            // take_photo_flag == 0,直接跳过
        }
    }
}

uint8_t camera_readData() {
    return (HAL_GPIO_ReadPin(CAM_D7_GPIO_Port, CAM_D7_Pin))<<7 |
        (HAL_GPIO_ReadPin(CAM_D6_GPIO_Port, CAM_D6_Pin))<<6 |
        (HAL_GPIO_ReadPin(CAM_D5_GPIO_Port, CAM_D5_Pin))<<5 |
        (HAL_GPIO_ReadPin(CAM_D4_GPIO_Port, CAM_D4_Pin))<<4 |
        (HAL_GPIO_ReadPin(CAM_D3_GPIO_Port, CAM_D3_Pin))<<3 |
        (HAL_GPIO_ReadPin(CAM_D2_GPIO_Port, CAM_D2_Pin))<<2 |
        (HAL_GPIO_ReadPin(CAM_D1_GPIO_Port, CAM_D1_Pin))<<1 |
        (HAL_GPIO_ReadPin(CAM_D0_GPIO_Port, CAM_D0_Pin));
}

void getCameraData(uint8_t* SCREEN) {
    __disable_irq();
    HAL_GPIO_WritePin(CAM_RRST_GPIO_Port, CAM_RRST_Pin, GPIO_PIN_RESET);  // OV7670_RRST = 0; // 开始复位读指针
    HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
    HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
    HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
    HAL_GPIO_WritePin(CAM_RRST_GPIO_Port, CAM_RRST_Pin, GPIO_PIN_SET);  // OV7670_RRST = 1; // 复位读指针结束
    HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
    uint16_t index = 0;
    for (uint16_t i = 0; i < 120; i++) {  // 对每一行
        for (uint16_t j = 0; j < 160; j++) {  // 对每一行的每一个像素
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
            SCREEN[index++] = camera_readData();  // 读数据
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
            SCREEN[index++] = camera_readData();  // 读数据
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
            // 跳过一个
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
        }
        for (uint16_t j = 0; j < 320; j++) {  // 跳过一行
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_RESET);  // OV7670_RCK_L;
            HAL_GPIO_WritePin(CAM_RCLK_GPIO_Port, CAM_RCLK_Pin, GPIO_PIN_SET);  // OV7670_RCK_H;
        }
    }
    __enable_irq();
}
