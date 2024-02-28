/*******************************************************

程序功能：LCD12864液晶测试

开发环境：Keil3

硬件环境：CEPARK多功能开发学习板/实验箱（2013版），STC12C5A60S2，11.0592M晶振
																							   
接线说明：单片机P1口与底板JP41排针相连，分别为：P10-D0, P11-D1，P12-D2，P13-D3，P14-D4，P15-D5，P16-D6，P17-D7；
          单片机P34~P37口与底板JP36相连，分别为：P34-RS,P35-RW,P36-EN，P37-CS1，CS2悬空		

跳线说明：无

实验现象：在液晶上显示4行包含字符和汉字的信息。
			第一行："电子园论坛欢迎您"
			第二行：" BBS.CEPARK.COM "
			第三行："  2013年01月01日"
			第四行："    新年快乐！  "

									 
论坛支持：http://bbs.cepark.com

淘宝店铺：http://cepark.taobao.com

作者：cepark 

时间：2013-01-01（元旦快乐！）

*******************************************************/

#include "STC12C5A60S2.h"
#include "intrins.h"
# include <stdio.h>
# include <stdlib.h>
#include <string.h>
sbit DCOUT1 = P2^1;//定义电机信号输出端口1
sbit DCOUT2 = P2^2;//定义电机信号输出端口2
sbit LED1 = P2^3;//定义电机信号输出端口1
sbit LED2 = P2^4;//定义电机信号输出端口2
sbit LED3 = P2^5;//定义电机信号输出端口2
sbit IR=P3^2;  //红外接口标志
sbit LIGHT= P2^6;//光敏接口
sbit LCD12864_RS  =  P3^4;	 //RS控制引脚
sbit LCD12864_RW  =  P3^5;	 //RW控制引脚
sbit LCD12864_EN  =  P3^6;	 //EN控制引脚
sbit LCD12864_PSB  = P3^7;	  //CS1模式选择引脚，ST7920控制器，1为8位并行接口，0为串行接口
sbit DQ = P2^7;	//温度传感器引脚
sbit SPK=P2^0;    //定义喇叭端口
#define LCDPORT P1		//lcd数据引脚
#define key_4x4_port P0   //定义4x4键盘使用的单片机端口
xdata unsigned char irdata[33]; //33个高低电平的时间数据
xdata unsigned char key;//存放按键值
//unsigned char xdata ucStr1[]  =  "  合肥工业大学  ";		//显示信息1
//unsigned char xdata ucStr2[]  =  "冯  儒2021214879";		//显示信息2
//unsigned char xdata ucStr3[]  =  "邓裕遥2021214856";		//显示信息3
//unsigned char xdata ucStr4[]  =  "2023年09月12日  ";		//显示信息4
//unsigned char xdata ucStr5[]  =  "温度:   光照:   ";		//显示信息2
////unsigned char code ucStr5[]  =  "                ";		//显示信息2
//unsigned char xdata ucStr6[]  =  "鼓风: 关补光: 关";		//显示信息3
//unsigned char xdata ucStr7[]  =  "阈值:   阈值:   ";		//显示信息4
//unsigned char xdata ucStr8[]  =  "报警原因:       ";

idata unsigned int tmp_ = 30;
idata unsigned int light_ = 80;
idata unsigned int light = 70;
idata unsigned char flag_temper = 0;
idata unsigned char a;

idata unsigned int hour,minute,second,count;
char code Tab[16]="0123456789ABCDEF";
idata unsigned char  irtime;//红外用全局变量
bit irpro_ok,irok;
idata unsigned char IRcord[4];  //处理后的红外码，分别是 客户码，客户码，数据码，数据码反码

//while(x){
//	s[++cnt] = x%10+32;
//	x/=10;
//}
unsigned char Key_Scan(void);
void LCD12864_Init(void);		  //LCD12864初始化函数
void LCD12864_WriteInfomation(unsigned char ucData,bit bComOrData);	   //向LCD12864写入数据，bComOrData为1时写入的是数据，0时写入的是命令
void LCD12864_CheckBusy(void);		//忙检测函数
void LCD12864_DisplayOneLine(unsigned char ucPos,unsigned char *ucStr);	  //向LCD12864写入一行文字
void LCD12864_value(unsigned char position,unsigned char *p);
void Delay(unsigned int uiCount);

void DisplayOne(unsigned char Data); //

unsigned int t;

void Delay_us(unsigned int uiUs);	//us延时函数,12MHZ晶振有效
void Delay_ms(unsigned int uiMs);	//ms延时函数,12MHZ晶振有效
void DisplayOneCharOnAddr(unsigned char Data,unsigned char Addr); //在指定的位置上显示一个数字
void DS18B20_Init(void);   //DS18B20初始化
unsigned char DS18B20_ReadOneChar(void);   //读取一个数据
void DS18B20_WriteOneChar(unsigned char dat);	  //写入一个数据
unsigned int DS18B20_ReadTemperature(void);		  //读取温度
void Ir_work(void);																	//遥控
void Ircordpro(void);																//遥控
void ShowString (unsigned char line,char *ptr);			//遥控
void UART_Init(void);
void UART_Send_Byte(unsigned char ucData);
void UART_Send_Str(char *pStr);
void UART_Send_Enter(void);
void UART_Init(void);
void UartInit();
void Uartsend(unsigned char byte);

void tim0_isr (void) interrupt 1 using 1//定时器0中断服务函数
{
  irtime++;                             //用于计数2个下降沿之间的时间
}

/******************************************************************/
/*                    外部中断0函数                               */
/******************************************************************/
void ex0_isr (void) interrupt 0 using 0//外部中断0服务函数
{
  static unsigned char  i;             //接收红外信号处理
  static bit startflag;                //是否开始处理标志位

 if(startflag)                         
 {
   
    if(irtime<63&&irtime>=33)//引导码 TC9012的头码，9ms+4.5ms
            i=0;
     
    		irdata[i]=irtime;//存储每个电平的持续时间，用于以后判断是0还是1
    		irtime=0;
    		i++;
   			 if(i==33)
      			{
	  			 irok=1;
				 i=0;
	  			}
          }
	 
  	 else
		{irtime=0;startflag=1;}

}

/******************************************************************/
/*                   定时器0初始化                                */
/******************************************************************/
void TIM0init(void)//定时器0初始化
{

  TMOD=0x02;//定时器0工作方式2，TH0是重装值，TL0是初值
  TH0=0x00; //重载值
  TL0=0x00; //初始化值
  ET0=1;    //开中断
  TR0=1;    
}
/******************************************************************/
/*                   外部中断初始化                               */
/******************************************************************/
void EX0init(void)
{
 IT0 = 1;   //指定外部中断0下降沿触发，INT0 (P3.2)
 EX0 = 1;   //使能外部中断
 EA = 1;    //开总中断
}
/******************************************************************************
函数名称：main
函数功能：主函数，程序如路口
入口参数：无
返回值：无
备注：无
*******************************************************************************/
void main(void)
{	
	EX0init(); //初始化外部中断
	TIM0init();//初始化定时器
	LCD12864_Init();   //初始化液晶
	
	LCD12864_DisplayOneLine(0x80,"  合肥工业大学  ");	//显示信息1
	LCD12864_DisplayOneLine(0x90,"冯  儒2021214879");	//显示信息2
	LCD12864_DisplayOneLine(0x88,"邓裕遥2021214856");	//显示信息3
	LCD12864_DisplayOneLine(0x98,"2023年09月12日  ");	//显示信息4
	Delay_ms(3000);

	while(1)
	{
			LCD12864_DisplayOneLine(0x80,"温度:   光照: 70");	//显示信息5
			LCD12864_DisplayOneLine(0x90,"鼓风: 关补光: 关");	//显示信息6
			LCD12864_DisplayOneLine(0x88,"阈值: 30阈值: 80");	//显示信息7
			LCD12864_DisplayOneLine(0x98,"报警原因:       ");	//显示信息7
			

			while(1)
			{		char buf1[2];
					char buf2[2];
					char buf3[2];
					key_4x4_port = 0xF0;
					if((key_4x4_port & 0xF0) != 0xF0)
					{	
							
							key=Key_Scan();
							DisplayOne(key);//
							
						
					}
					t=DS18B20_ReadTemperature();
					
					sprintf(buf3,"%d",t);
					LCD12864_value(0x83,buf3);
					if(t > tmp_ )
						{
							DCOUT1=0;
							DCOUT2=1;
							Delay_ms(1);
							SPK=!SPK;
						}
					else
					{
						DCOUT1=0;
						DCOUT2=0;
					}
					if(light_-light>15)
					{
						LED1=0;
						LED2=0;
						LED3=0;

					}
					else if(light_-light>10 && light_-light<=15)
					{
						LED1=0;
						LED2=0;
						LED3=1;

					}
					else if(light_-light>0 && light_-light<=10)
					{
						LED1=0;
						LED2=1;
						LED3=1;

					}
					else{
						LED1=1;
						LED2=1;
						LED3=1;

					}
					if(t>tmp_ && light>=light_)
					{
						LCD12864_value(0x93,"开");
						LCD12864_value(0x97,"关");
						LCD12864_value(0x9d,"温");
						LCD12864_value(0x9e,"度");
					}
					else if(t>tmp_ && light<light_)
					{
						LCD12864_value(0x93,"开");
						LCD12864_value(0x97,"开");
						LCD12864_value(0x9d,"全");
						LCD12864_value(0x9e,"报");
					}
					else if(t<=tmp_ && light<light_)
					{
						LCD12864_value(0x93,"关");
						LCD12864_value(0x97,"开");
						LCD12864_value(0x9d,"光");
						LCD12864_value(0x9e,"照");
					}
					else 
					{
						LCD12864_value(0x93,"关");
						LCD12864_value(0x97,"关");
						LCD12864_value(0x9d," ");
						LCD12864_value(0x9e," ");
					}
					
					if(irok)                        //如果接收好了进行红外处理
							{   
							 Ircordpro();
							 irok=0;
							}

							if(irpro_ok)                   //如果处理好后进行工作处理，如按对应的按键后显示对应的数字等
							{
							 Ir_work();
								}
					
					UartInit();
					sprintf(buf1,"%d",tmp_);
					sprintf(buf2,"%d",light_);
					LCD12864_value(0x8b,buf1);
					LCD12864_value(0x8f,buf2);
					
			};
	};

}

/******************************************************************************
函数名称：LCD12864_WriteInfomation
函数功能：向LCD12864写入命令或者数据
入口参数：ucData-要写入液晶的数据或者命令的内容
		  bComOrData-命令或者数据的标志位选择，0或者1，其中
		  	1：写入的是数据
			0：写入的是命令
返回值：无
备注：无
*******************************************************************************/
void LCD12864_WriteInfomation(unsigned char ucData,bit bComOrData)
{
	LCD12864_CheckBusy();	//忙检测
	LCD12864_RW = 0;	//拉低RW
	LCD12864_RS = bComOrData;	//根据标志位判断写入的是命令还是数据
	Delay(150);				  //延时，等待操作
	LCDPORT = ucData;		  //将数据送至数据端口
	LCD12864_EN = 1;		  //使能信号
	Delay(150);				  //延时
	LCD12864_EN = 0;		//按照时序来操作
	Delay(150);
}

/******************************************************************************
函数名称：LCD12864_Init
函数功能：LCD12864液晶初始化
入口参数：无
返回值：无
备注：无
*******************************************************************************/
void LCD12864_Init(void)
{
	Delay(400);//延时
	LCD12864_PSB = 1;	//8位并口工作模式
	Delay(150);//延时


	LCD12864_WriteInfomation(0x30,0); //基本指令集
	Delay(150);
	LCD12864_WriteInfomation(0x08,0);  //显示设置
	Delay(150);
	LCD12864_WriteInfomation(0x10,0);	//光标设置
	Delay(150);
	LCD12864_WriteInfomation(0x0c,0);	//游标设置
	Delay(150);
	LCD12864_WriteInfomation(0x01,0);  //清屏
	Delay(150);
	LCD12864_WriteInfomation(0x06,0);	//进入点设定
	Delay(150);
		
}

/******************************************************************************
函数名称：LCD12864_CheckBusy
函数功能：忙检测
入口参数：无
返回值：无
备注：使用变量i做计时，避免液晶在死循环处停滞。
*******************************************************************************/
void LCD12864_CheckBusy(void)
{
	unsigned char i = 250;	//局部变量
	LCD12864_RS = 0;	  //拉低
	LCD12864_RW = 1;	  //拉高
	LCD12864_EN = 1;	  //使能
	while((i > 0) && (P0 & 0x80))i--;  //判断忙标志位
	LCD12864_EN = 0;	  //释放
}

/******************************************************************************
函数名称：LCD12864_DisplayOneLine
函数功能：显示一行汉字（8个汉字或者16个英文字符）
入口参数：position-要显示的行的首地址，可选值0x80,0x88,0x90,0x98,其中：
			0x80:液晶的第一行；
			0x88:液晶的第三行；
			0x90:液晶的第二行；
			0x98:液晶的第四行。
		  p-要显示的内容的首地址。
返回值：无
备注：无
*******************************************************************************/
void LCD12864_DisplayOneLine(unsigned char position,unsigned char *p)
{
	 unsigned char i;

	 LCD12864_WriteInfomation(position,0);//写入要显示文字的行的首地址
	 Delay(150);

	 for(i = 0;i<16;i++)		//依次执行写入操作
	 {
	 	 LCD12864_WriteInfomation(*p,1);
		 p++;
	 }
}

void LCD12864_value(unsigned char position,unsigned char *p)
{
	 unsigned char i;
	 LCD12864_WriteInfomation(position,0);//写入要显示文字的行的首地址
	 Delay(150);

	 for(i = 0;i<2;i++)		//依次执行写入操作
	 {
	 	 LCD12864_WriteInfomation(*p,1);
		 p++;
	 }
}
/******************************************************************************
函数名称：Delay
函数功能：延时函数
入口参数：uiCount-延时参数
返回值：无
备注：无
*******************************************************************************/
void Delay(unsigned int uiCount)
{
	while(uiCount--);
}





/******************************************************************************
函数名称：Key_Scan
函数功能：返回按键编码
入口参数：无
返回值：按键编码
备注：无
*******************************************************************************/
unsigned char Key_Scan(void)
{
	unsigned char temp;
	unsigned char ucBackValue;
	//====扫描第一列==========
	key_4x4_port=0xfe;   //送出P1.0位0电平去扫描
	temp= key_4x4_port;  //读出整个口得到的值
	temp=temp&0xf0;    //屏蔽低4位
	if (temp!=0xf0)      //假如高4位不全是1
	{ 
		Delay(5);      // 延时消抖再读
		temp=key_4x4_port;
		temp=temp&0xf0;
		if (temp!=0xf0)     //消抖后如果再次高4位确定不是全1
		{ 
			temp=key_4x4_port;  //读出此次按键的值
			switch(temp)
			{ 
				case 0xee:
				ucBackValue = 0;break;
				case 0xde:
				ucBackValue = 4;break;
				case 0xbe:
				ucBackValue = 8;
				break;
				case 0x7e:
				ucBackValue = 12;
				break;
				default:
					break;
			}
			while(temp!=0xf0)   //等待按键放开
			{ 
				temp=key_4x4_port;
				temp=temp&0xf0;
			}
		}
	}

	//====第二列送0==========
	key_4x4_port=0xfd;   //送出P3.1位0电平去扫描
	temp= key_4x4_port;  //读出整个口得到的值
	temp=temp&0xf0;    //屏蔽低4位
	if (temp!=0xf0)      //假如高4位不全是1
	{ 
		Delay(5);      // 延时消抖再读
		temp=key_4x4_port;
		temp=temp&0xf0;
		if (temp!=0xf0)     //消抖后如果再次高4位确定不是全1
		{ 
			temp=key_4x4_port;  //读出此次按键的值
			switch(temp)
			{ 
				case 0xed:
					ucBackValue = 1; break;
				case 0xdd:
					ucBackValue = 5; break;
				case 0xbd:
					ucBackValue = 9; break;
				case 0x7d:
					ucBackValue = 13; break;
				default:
					break;
		}
			while(temp!=0xf0)   //等待按键放开
			{ 
				temp=key_4x4_port;
				temp=temp&0xf0;
			}
		}
	}
	//====第3列送0==========
	key_4x4_port=0xfb;   //送出P3.2位0电平去扫描
	temp= key_4x4_port;  //读出整个口得到的值
	temp=temp&0xf0;    //屏蔽低4位
	if (temp!=0xf0)      //假如高4位不全是1
	{ 
		Delay(5);      // 延时消抖再读
		temp=key_4x4_port;
		temp=temp&0xf0;
		if (temp!=0xf0)     //消抖后如果再次高4位确定不是全1
		{ 
			temp=key_4x4_port;  //读出此次按键的值
			switch(temp)
			{ 
				case 0xeb:
				ucBackValue = 2; break;
				case 0xdb:
				ucBackValue = 6; break;
				case 0xbb:
				ucBackValue = 10; break;
				case 0x7b:
				ucBackValue = 14; break;
				default:
					break;
		}
		while(temp!=0xf0)   //等待按键放开
		{ 
			temp=key_4x4_port;
			temp=temp&0xf0;
		}
		}
	}
	//====第4列送0==========
	key_4x4_port=0xf7;   //送出P3.3位0电平去扫描
	temp= key_4x4_port;  //读出整个口得到的值
	temp=temp&0xf0;    //屏蔽低4位
	if (temp!=0xf0)      //假如高4位不全是1
	{ 
		Delay(5);      // 延时消抖再读
		temp=key_4x4_port;
		temp=temp&0xf0;
		if (temp!=0xf0)     //消抖后如果再次高4位确定不是全1
		{ 
			temp=key_4x4_port;  //读出此次按键的值
			switch(temp)
			{ 
				case 0xe7:
				ucBackValue = 3; break;
				case 0xd7:
				ucBackValue = 7; break;
				case 0xb7:
				ucBackValue = 11; break;
				case 0x77:
				ucBackValue = 15; break;
				default:
					break;
			}
			while(temp!=0xf0)   //等待按键放开
			{ 
				temp=key_4x4_port;
				temp=temp&0xf0;
			}
		}
	}
   
	return ucBackValue;
}

void DisplayOne(unsigned char Data)
{		char buf1[2];
		char buf2[2];
		sprintf(buf1,"%d",tmp_);
		sprintf(buf2,"%d",light_);
		switch(Data)
			{ 
				case 0:
				tmp_--;break;
				case 1:
				tmp_++;break;
				case 2:
				light_--;break;
				case 3:
				light_++;break;			
				case 4:
					UART_Init();
					UART_Send_Byte(11);
					UART_Send_Str(buf1);
					UART_Send_Str(buf2);
					UART_Send_Enter();break;
				
				default:
					break;
			}
			
}


void DS18B20_Init(void)
{
	unsigned char x=0;
	DQ = 1;    //DQ复位
	Delay_us(10);
	//Delay(8);  //稍做延时,10us
	DQ = 0;    //单片机将DQ拉低
	Delay_us(500);
	//Delay(80); //精确延时 大于 480us ,498us
	DQ = 1;    //拉高总线
	Delay_us(154);
	//Delay(14);	//154us
	x=DQ;      //稍做延时后 如果x=0则初始化成功 x=1则初始化失败
	Delay_us(212);
	//Delay(20); //212us
}



unsigned char DS18B20_ReadOneChar(void)
{
	unsigned char i=0;
	unsigned char dat = 0;
	for (i=8;i>0;i--)
	{
		DQ = 0; // 给脉冲信号
		dat>>=1;
		DQ = 1; // 给脉冲信号
		if(DQ)
		dat|=0x80;
		Delay_us(56);
		//Delay(4); //56us
	}
	return(dat);
}



void DS18B20_WriteOneChar(unsigned char dat)
{
	unsigned char i=0;
	for (i=8; i>0; i--)
	{
		DQ = 0;
		DQ = dat&0x01;
		Delay_us(66);
		//Delay(5); //66us
		DQ = 1;
		dat>>=1;
	}
}



unsigned int DS18B20_ReadTemperature(void)
{
	unsigned char a=0;
	unsigned char b=0;
	unsigned int t=0;
	DS18B20_Init();
	DS18B20_WriteOneChar(0xCC); // 跳过读序号列号的操作
	DS18B20_WriteOneChar(0x44); // 启动温度转换
	DS18B20_Init();
	DS18B20_WriteOneChar(0xCC); //跳过读序号列号的操作
	DS18B20_WriteOneChar(0xBE); //读取温度寄存器等（共可读9个寄存器） 前两个就是温度
	a=DS18B20_ReadOneChar();
	b=DS18B20_ReadOneChar();
	t=b;
	t<<=8;
	t=t|a;
	//t= t/2; //放大10倍输出并四舍五入---此行没用
	t = t*0.0625;

	return(t);
}




void Delay_us(unsigned int uiUs)	//us延时函数
{
	for(;uiUs > 0;uiUs--)
	{
		_nop_();
	}	
}



void Delay_ms(unsigned int uiMs)	//ms延时函数
{
	unsigned int i;
	for(;uiMs > 0;uiMs--)
	{
		for(i = 1498;i > 0;i--);
	}
}

//**********************************************************
//读取温度函数，返回温度的绝对值，并标注flag_temper，flag_temper=1表示负，flag_temper=0表示正
//**********************************************************
//unsigned int Get_temp(void)         //读取温度值 
//{  
//	float tt;
//	unsigned char a,b;
//	unsigned int temp;
//	if( DS18B20_Init() == 0 )                //初始化
//	{
//		DS18B20_Write_Byte(0xcc);          //忽略ROM指令
//		DS18B20_Write_Byte(0x44);          //温度转换指令
//		if( DS18B20_Init() == 0 )                 //初始化
//		{
//			DS18B20_Write_Byte(0xcc);          //忽略ROM指令
//			DS18B20_Write_Byte(0xbe);          //读暂存器指令
//			a = DS18B20_Read_Byte();           //读取到的第一个字节为温度LSB
//			b = DS18B20_Read_Byte();           //读取到的第一个字节为温度MSB
//			temp = b;                      //先把高八位有效数据赋于temp
//			temp <<= 8;                    //把以上8位数据从temp低八位移到高八位
//			temp = temp|a;                //两字节合成一个整型变量
//			if(temp>0xfff)
//			{
//				flag_temper=1;				//温度为负数
//				temp=(~temp)+1;
//			}
//			else
//			{																			   
//				flag_temper=0;				//温度为正或者0
//			}
//			
//			tt = temp*0.0625;              //得到真实十进制温度值
//			                                //因为DS18B20可以精确到0.0625度
//			                                //所以读回数据的最低位代表的是0.0625度
//			temp = tt*100;               //放大百倍
//			                                //这样做的目的将小数点也转换为可显示数字
//		}
//	}
//	return temp;
//}



void Ir_work(void)        //红外键值散转程序
  {
       
		  
		 char TimeNum[]="  ";
	   TimeNum[0] = Tab[IRcord[2]/16];  //处理数据码并显示
	   TimeNum[1] = Tab[IRcord[2]%16];
	   
//		 LCD12864_value(0x87,TimeNum);
     if(strcmp(TimeNum,"0C")==0)
				tmp_--;
		 else if(strcmp(TimeNum,"18")==0)
			  tmp_++;
		 else if(strcmp(TimeNum,"5E")==0)
			  light_--;
		 else if(strcmp(TimeNum,"08")==0)
			  light_++;
		 else tmp_++;
	   irpro_ok=0;           //处理完成后清楚标志位

  }

/******************************************************************/
/*                    红外解码函数处理                            */
/******************************************************************/
void Ircordpro(void)//红外码值处理函数
{ 
  unsigned char i, j, k;
  unsigned char cord,value;

  k=1;
  for(i=0;i<4;i++)      //处理4个字节
     {
      for(j=1;j<=8;j++) //处理1个字节8位
         {
          cord=irdata[k];
          if(cord>7)//大于某值为1，这个和晶振有绝对关系，这里使用12M计算，此值可以有一定误差
		    {
             value=value|0x80;
			}
          else 
		    {
             value=value;
			}
          if(j<8)
		    {
			 value=value>>1;
			}
           k++;
         }
     IRcord[i]=value;
     value=0;     
     } irpro_ok=1;//处理完毕标志位置1
    
}

void UART_Init(void)
{

		PCON |= 0x80;		//使能波特率倍速位SMOD
		SCON = 0x50;		//8位数据,可变波特率
		AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
		AUXR &= 0xFE;		//串口1选择定时器1为波特率发生器
		TMOD &= 0x0F;		//清除定时器1模式位
		TMOD |= 0x20;		//设定定时器1为8位自动重装方式
		TL1 = 0xB8;		//设定定时初值
		TH1 = 0xB8;		//设定定时器重装值
		ET1 = 0;		//禁止定时器1中断
		TR1 = 1;		//启动定时器1
	
}


/**************************************************************************
 - 功能描述：51单片机的串口发送字节的函数
 - 隶属模块：STC51串口操作
 - 函数属性：外部，使用户使用
 - 参数说明：ucData:要发送的一个字节
 - 返回说明：无
 - 注：发送一个字节，是串口发送的基础操作
 **************************************************************************/

void UART_Send_Byte(unsigned char ucData)
{
 TI = 0;
 SBUF = ucData;
 while(!TI);
 TI = 0;
}

/**************************************************************************
 - 功能描述：51单片机的串口发送0d 0a ，即回车换行
 - 隶属模块：STC51串口操作
 - 函数属性：外部，使用户使用
 - 参数说明：无
 - 返回说明：无
 - 注：此函数就是发送0d 0a这两个字节，在“超级终端”上会有回车换行的效果
 **************************************************************************/

void UART_Send_Enter()
{
 UART_Send_Byte(0x0d);
 UART_Send_Byte(0x0a);
}

/**************************************************************************
 - 功能描述：51单片机的串口发送字符串
 - 隶属模块：STC51串口操作
 - 函数属性：外部，使用户使用
 - 参数说明：s:指向字符串的指针
 - 返回说明：无
 - 注：发送完一条字符串，回车
 **************************************************************************/

void UART_Send_Str(char *pStr)
{
    
	while(*pStr != '\0')
	{
		UART_Send_Byte(*pStr++);
	}
	UART_Send_Enter();

}

void UartInit()		//9600bps@11.0592MHz
{
	PCON &= 0x7F;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xFD;			//设定定时初值
	TH1 = 0xFD;			//设定定时器重装值		
	TR1 = 1;			//启动定时器1
	ET1 = 0;        	//禁止定时器1中断
	EA=1;				//开放总中断 
	ES=1;				//开放串口中断
}
void Uartsend(unsigned char byte)//发送
{
	SBUF=byte;//把数据写入发送缓冲区SBUF
	//数据发送完成的标志是TI=1；所以等待数据传送完
			 
	
	while(TI==0);
	TI=0;//软件清零
}
void UART_ISR() interrupt 4//串口中断函数
{
	if(RI==1)//接收中断
	{	
		a = SBUF;
		switch(a)
		{
			case '0':
				tmp_--;break;
			case '1':
				tmp_++;break;
			case '2':
				light_--;break;
			case '3':
				light_++;break;
			default:
				tmp_=30;
				light_=80;
				break;
		}
		Uartsend(SBUF);//把接收的数据发送到计算机
		
		RI=0;//软件清零
	}
}

