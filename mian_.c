/*******************************************************

�����ܣ�LCD12864Һ������

����������Keil3

Ӳ��������CEPARK�๦�ܿ���ѧϰ��/ʵ���䣨2013�棩��STC12C5A60S2��11.0592M����
																							   
����˵������Ƭ��P1����װ�JP41�����������ֱ�Ϊ��P10-D0, P11-D1��P12-D2��P13-D3��P14-D4��P15-D5��P16-D6��P17-D7��
          ��Ƭ��P34~P37����װ�JP36�������ֱ�Ϊ��P34-RS,P35-RW,P36-EN��P37-CS1��CS2����		

����˵������

ʵ��������Һ������ʾ4�а����ַ��ͺ��ֵ���Ϣ��
			��һ�У�"����԰��̳��ӭ��"
			�ڶ��У�" BBS.CEPARK.COM "
			�����У�"  2013��01��01��"
			�����У�"    ������֣�  "

									 
��̳֧�֣�http://bbs.cepark.com

�Ա����̣�http://cepark.taobao.com

���ߣ�cepark 

ʱ�䣺2013-01-01��Ԫ�����֣���

*******************************************************/

#include "STC12C5A60S2.h"
#include "intrins.h"
# include <stdio.h>
# include <stdlib.h>
#include <string.h>
sbit DCOUT1 = P2^1;//�������ź�����˿�1
sbit DCOUT2 = P2^2;//�������ź�����˿�2
sbit LED1 = P2^3;//�������ź�����˿�1
sbit LED2 = P2^4;//�������ź�����˿�2
sbit LED3 = P2^5;//�������ź�����˿�2
sbit IR=P3^2;  //����ӿڱ�־
sbit LIGHT= P2^6;//�����ӿ�
sbit LCD12864_RS  =  P3^4;	 //RS��������
sbit LCD12864_RW  =  P3^5;	 //RW��������
sbit LCD12864_EN  =  P3^6;	 //EN��������
sbit LCD12864_PSB  = P3^7;	  //CS1ģʽѡ�����ţ�ST7920��������1Ϊ8λ���нӿڣ�0Ϊ���нӿ�
sbit DQ = P2^7;	//�¶ȴ���������
sbit SPK=P2^0;    //�������ȶ˿�
#define LCDPORT P1		//lcd��������
#define key_4x4_port P0   //����4x4����ʹ�õĵ�Ƭ���˿�
xdata unsigned char irdata[33]; //33���ߵ͵�ƽ��ʱ������
xdata unsigned char key;//��Ű���ֵ
//unsigned char xdata ucStr1[]  =  "  �Ϸʹ�ҵ��ѧ  ";		//��ʾ��Ϣ1
//unsigned char xdata ucStr2[]  =  "��  ��2021214879";		//��ʾ��Ϣ2
//unsigned char xdata ucStr3[]  =  "��ԣң2021214856";		//��ʾ��Ϣ3
//unsigned char xdata ucStr4[]  =  "2023��09��12��  ";		//��ʾ��Ϣ4
//unsigned char xdata ucStr5[]  =  "�¶�:   ����:   ";		//��ʾ��Ϣ2
////unsigned char code ucStr5[]  =  "                ";		//��ʾ��Ϣ2
//unsigned char xdata ucStr6[]  =  "�ķ�: �ز���: ��";		//��ʾ��Ϣ3
//unsigned char xdata ucStr7[]  =  "��ֵ:   ��ֵ:   ";		//��ʾ��Ϣ4
//unsigned char xdata ucStr8[]  =  "����ԭ��:       ";

idata unsigned int tmp_ = 30;
idata unsigned int light_ = 80;
idata unsigned int light = 70;
idata unsigned char flag_temper = 0;
idata unsigned char a;

idata unsigned int hour,minute,second,count;
char code Tab[16]="0123456789ABCDEF";
idata unsigned char  irtime;//������ȫ�ֱ���
bit irpro_ok,irok;
idata unsigned char IRcord[4];  //�����ĺ����룬�ֱ��� �ͻ��룬�ͻ��룬�����룬�����뷴��

//while(x){
//	s[++cnt] = x%10+32;
//	x/=10;
//}
unsigned char Key_Scan(void);
void LCD12864_Init(void);		  //LCD12864��ʼ������
void LCD12864_WriteInfomation(unsigned char ucData,bit bComOrData);	   //��LCD12864д�����ݣ�bComOrDataΪ1ʱд��������ݣ�0ʱд���������
void LCD12864_CheckBusy(void);		//æ��⺯��
void LCD12864_DisplayOneLine(unsigned char ucPos,unsigned char *ucStr);	  //��LCD12864д��һ������
void LCD12864_value(unsigned char position,unsigned char *p);
void Delay(unsigned int uiCount);

void DisplayOne(unsigned char Data); //

unsigned int t;

void Delay_us(unsigned int uiUs);	//us��ʱ����,12MHZ������Ч
void Delay_ms(unsigned int uiMs);	//ms��ʱ����,12MHZ������Ч
void DisplayOneCharOnAddr(unsigned char Data,unsigned char Addr); //��ָ����λ������ʾһ������
void DS18B20_Init(void);   //DS18B20��ʼ��
unsigned char DS18B20_ReadOneChar(void);   //��ȡһ������
void DS18B20_WriteOneChar(unsigned char dat);	  //д��һ������
unsigned int DS18B20_ReadTemperature(void);		  //��ȡ�¶�
void Ir_work(void);																	//ң��
void Ircordpro(void);																//ң��
void ShowString (unsigned char line,char *ptr);			//ң��
void UART_Init(void);
void UART_Send_Byte(unsigned char ucData);
void UART_Send_Str(char *pStr);
void UART_Send_Enter(void);
void UART_Init(void);
void UartInit();
void Uartsend(unsigned char byte);

void tim0_isr (void) interrupt 1 using 1//��ʱ��0�жϷ�����
{
  irtime++;                             //���ڼ���2���½���֮���ʱ��
}

/******************************************************************/
/*                    �ⲿ�ж�0����                               */
/******************************************************************/
void ex0_isr (void) interrupt 0 using 0//�ⲿ�ж�0������
{
  static unsigned char  i;             //���պ����źŴ���
  static bit startflag;                //�Ƿ�ʼ�����־λ

 if(startflag)                         
 {
   
    if(irtime<63&&irtime>=33)//������ TC9012��ͷ�룬9ms+4.5ms
            i=0;
     
    		irdata[i]=irtime;//�洢ÿ����ƽ�ĳ���ʱ�䣬�����Ժ��ж���0����1
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
/*                   ��ʱ��0��ʼ��                                */
/******************************************************************/
void TIM0init(void)//��ʱ��0��ʼ��
{

  TMOD=0x02;//��ʱ��0������ʽ2��TH0����װֵ��TL0�ǳ�ֵ
  TH0=0x00; //����ֵ
  TL0=0x00; //��ʼ��ֵ
  ET0=1;    //���ж�
  TR0=1;    
}
/******************************************************************/
/*                   �ⲿ�жϳ�ʼ��                               */
/******************************************************************/
void EX0init(void)
{
 IT0 = 1;   //ָ���ⲿ�ж�0�½��ش�����INT0 (P3.2)
 EX0 = 1;   //ʹ���ⲿ�ж�
 EA = 1;    //�����ж�
}
/******************************************************************************
�������ƣ�main
�������ܣ���������������·��
��ڲ�������
����ֵ����
��ע����
*******************************************************************************/
void main(void)
{	
	EX0init(); //��ʼ���ⲿ�ж�
	TIM0init();//��ʼ����ʱ��
	LCD12864_Init();   //��ʼ��Һ��
	
	LCD12864_DisplayOneLine(0x80,"  �Ϸʹ�ҵ��ѧ  ");	//��ʾ��Ϣ1
	LCD12864_DisplayOneLine(0x90,"��  ��2021214879");	//��ʾ��Ϣ2
	LCD12864_DisplayOneLine(0x88,"��ԣң2021214856");	//��ʾ��Ϣ3
	LCD12864_DisplayOneLine(0x98,"2023��09��12��  ");	//��ʾ��Ϣ4
	Delay_ms(3000);

	while(1)
	{
			LCD12864_DisplayOneLine(0x80,"�¶�:   ����: 70");	//��ʾ��Ϣ5
			LCD12864_DisplayOneLine(0x90,"�ķ�: �ز���: ��");	//��ʾ��Ϣ6
			LCD12864_DisplayOneLine(0x88,"��ֵ: 30��ֵ: 80");	//��ʾ��Ϣ7
			LCD12864_DisplayOneLine(0x98,"����ԭ��:       ");	//��ʾ��Ϣ7
			

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
						LCD12864_value(0x93,"��");
						LCD12864_value(0x97,"��");
						LCD12864_value(0x9d,"��");
						LCD12864_value(0x9e,"��");
					}
					else if(t>tmp_ && light<light_)
					{
						LCD12864_value(0x93,"��");
						LCD12864_value(0x97,"��");
						LCD12864_value(0x9d,"ȫ");
						LCD12864_value(0x9e,"��");
					}
					else if(t<=tmp_ && light<light_)
					{
						LCD12864_value(0x93,"��");
						LCD12864_value(0x97,"��");
						LCD12864_value(0x9d,"��");
						LCD12864_value(0x9e,"��");
					}
					else 
					{
						LCD12864_value(0x93,"��");
						LCD12864_value(0x97,"��");
						LCD12864_value(0x9d," ");
						LCD12864_value(0x9e," ");
					}
					
					if(irok)                        //������պ��˽��к��⴦��
							{   
							 Ircordpro();
							 irok=0;
							}

							if(irpro_ok)                   //�������ú���й��������簴��Ӧ�İ�������ʾ��Ӧ�����ֵ�
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
�������ƣ�LCD12864_WriteInfomation
�������ܣ���LCD12864д�������������
��ڲ�����ucData-Ҫд��Һ�������ݻ������������
		  bComOrData-����������ݵı�־λѡ��0����1������
		  	1��д���������
			0��д���������
����ֵ����
��ע����
*******************************************************************************/
void LCD12864_WriteInfomation(unsigned char ucData,bit bComOrData)
{
	LCD12864_CheckBusy();	//æ���
	LCD12864_RW = 0;	//����RW
	LCD12864_RS = bComOrData;	//���ݱ�־λ�ж�д��������������
	Delay(150);				  //��ʱ���ȴ�����
	LCDPORT = ucData;		  //�������������ݶ˿�
	LCD12864_EN = 1;		  //ʹ���ź�
	Delay(150);				  //��ʱ
	LCD12864_EN = 0;		//����ʱ��������
	Delay(150);
}

/******************************************************************************
�������ƣ�LCD12864_Init
�������ܣ�LCD12864Һ����ʼ��
��ڲ�������
����ֵ����
��ע����
*******************************************************************************/
void LCD12864_Init(void)
{
	Delay(400);//��ʱ
	LCD12864_PSB = 1;	//8λ���ڹ���ģʽ
	Delay(150);//��ʱ


	LCD12864_WriteInfomation(0x30,0); //����ָ�
	Delay(150);
	LCD12864_WriteInfomation(0x08,0);  //��ʾ����
	Delay(150);
	LCD12864_WriteInfomation(0x10,0);	//�������
	Delay(150);
	LCD12864_WriteInfomation(0x0c,0);	//�α�����
	Delay(150);
	LCD12864_WriteInfomation(0x01,0);  //����
	Delay(150);
	LCD12864_WriteInfomation(0x06,0);	//������趨
	Delay(150);
		
}

/******************************************************************************
�������ƣ�LCD12864_CheckBusy
�������ܣ�æ���
��ڲ�������
����ֵ����
��ע��ʹ�ñ���i����ʱ������Һ������ѭ����ͣ�͡�
*******************************************************************************/
void LCD12864_CheckBusy(void)
{
	unsigned char i = 250;	//�ֲ�����
	LCD12864_RS = 0;	  //����
	LCD12864_RW = 1;	  //����
	LCD12864_EN = 1;	  //ʹ��
	while((i > 0) && (P0 & 0x80))i--;  //�ж�æ��־λ
	LCD12864_EN = 0;	  //�ͷ�
}

/******************************************************************************
�������ƣ�LCD12864_DisplayOneLine
�������ܣ���ʾһ�к��֣�8�����ֻ���16��Ӣ���ַ���
��ڲ�����position-Ҫ��ʾ���е��׵�ַ����ѡֵ0x80,0x88,0x90,0x98,���У�
			0x80:Һ���ĵ�һ�У�
			0x88:Һ���ĵ����У�
			0x90:Һ���ĵڶ��У�
			0x98:Һ���ĵ����С�
		  p-Ҫ��ʾ�����ݵ��׵�ַ��
����ֵ����
��ע����
*******************************************************************************/
void LCD12864_DisplayOneLine(unsigned char position,unsigned char *p)
{
	 unsigned char i;

	 LCD12864_WriteInfomation(position,0);//д��Ҫ��ʾ���ֵ��е��׵�ַ
	 Delay(150);

	 for(i = 0;i<16;i++)		//����ִ��д�����
	 {
	 	 LCD12864_WriteInfomation(*p,1);
		 p++;
	 }
}

void LCD12864_value(unsigned char position,unsigned char *p)
{
	 unsigned char i;
	 LCD12864_WriteInfomation(position,0);//д��Ҫ��ʾ���ֵ��е��׵�ַ
	 Delay(150);

	 for(i = 0;i<2;i++)		//����ִ��д�����
	 {
	 	 LCD12864_WriteInfomation(*p,1);
		 p++;
	 }
}
/******************************************************************************
�������ƣ�Delay
�������ܣ���ʱ����
��ڲ�����uiCount-��ʱ����
����ֵ����
��ע����
*******************************************************************************/
void Delay(unsigned int uiCount)
{
	while(uiCount--);
}





/******************************************************************************
�������ƣ�Key_Scan
�������ܣ����ذ�������
��ڲ�������
����ֵ����������
��ע����
*******************************************************************************/
unsigned char Key_Scan(void)
{
	unsigned char temp;
	unsigned char ucBackValue;
	//====ɨ���һ��==========
	key_4x4_port=0xfe;   //�ͳ�P1.0λ0��ƽȥɨ��
	temp= key_4x4_port;  //���������ڵõ���ֵ
	temp=temp&0xf0;    //���ε�4λ
	if (temp!=0xf0)      //�����4λ��ȫ��1
	{ 
		Delay(5);      // ��ʱ�����ٶ�
		temp=key_4x4_port;
		temp=temp&0xf0;
		if (temp!=0xf0)     //����������ٴθ�4λȷ������ȫ1
		{ 
			temp=key_4x4_port;  //�����˴ΰ�����ֵ
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
			while(temp!=0xf0)   //�ȴ������ſ�
			{ 
				temp=key_4x4_port;
				temp=temp&0xf0;
			}
		}
	}

	//====�ڶ�����0==========
	key_4x4_port=0xfd;   //�ͳ�P3.1λ0��ƽȥɨ��
	temp= key_4x4_port;  //���������ڵõ���ֵ
	temp=temp&0xf0;    //���ε�4λ
	if (temp!=0xf0)      //�����4λ��ȫ��1
	{ 
		Delay(5);      // ��ʱ�����ٶ�
		temp=key_4x4_port;
		temp=temp&0xf0;
		if (temp!=0xf0)     //����������ٴθ�4λȷ������ȫ1
		{ 
			temp=key_4x4_port;  //�����˴ΰ�����ֵ
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
			while(temp!=0xf0)   //�ȴ������ſ�
			{ 
				temp=key_4x4_port;
				temp=temp&0xf0;
			}
		}
	}
	//====��3����0==========
	key_4x4_port=0xfb;   //�ͳ�P3.2λ0��ƽȥɨ��
	temp= key_4x4_port;  //���������ڵõ���ֵ
	temp=temp&0xf0;    //���ε�4λ
	if (temp!=0xf0)      //�����4λ��ȫ��1
	{ 
		Delay(5);      // ��ʱ�����ٶ�
		temp=key_4x4_port;
		temp=temp&0xf0;
		if (temp!=0xf0)     //����������ٴθ�4λȷ������ȫ1
		{ 
			temp=key_4x4_port;  //�����˴ΰ�����ֵ
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
		while(temp!=0xf0)   //�ȴ������ſ�
		{ 
			temp=key_4x4_port;
			temp=temp&0xf0;
		}
		}
	}
	//====��4����0==========
	key_4x4_port=0xf7;   //�ͳ�P3.3λ0��ƽȥɨ��
	temp= key_4x4_port;  //���������ڵõ���ֵ
	temp=temp&0xf0;    //���ε�4λ
	if (temp!=0xf0)      //�����4λ��ȫ��1
	{ 
		Delay(5);      // ��ʱ�����ٶ�
		temp=key_4x4_port;
		temp=temp&0xf0;
		if (temp!=0xf0)     //����������ٴθ�4λȷ������ȫ1
		{ 
			temp=key_4x4_port;  //�����˴ΰ�����ֵ
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
			while(temp!=0xf0)   //�ȴ������ſ�
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
	DQ = 1;    //DQ��λ
	Delay_us(10);
	//Delay(8);  //������ʱ,10us
	DQ = 0;    //��Ƭ����DQ����
	Delay_us(500);
	//Delay(80); //��ȷ��ʱ ���� 480us ,498us
	DQ = 1;    //��������
	Delay_us(154);
	//Delay(14);	//154us
	x=DQ;      //������ʱ�� ���x=0���ʼ���ɹ� x=1���ʼ��ʧ��
	Delay_us(212);
	//Delay(20); //212us
}



unsigned char DS18B20_ReadOneChar(void)
{
	unsigned char i=0;
	unsigned char dat = 0;
	for (i=8;i>0;i--)
	{
		DQ = 0; // �������ź�
		dat>>=1;
		DQ = 1; // �������ź�
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
	DS18B20_WriteOneChar(0xCC); // ����������кŵĲ���
	DS18B20_WriteOneChar(0x44); // �����¶�ת��
	DS18B20_Init();
	DS18B20_WriteOneChar(0xCC); //����������кŵĲ���
	DS18B20_WriteOneChar(0xBE); //��ȡ�¶ȼĴ����ȣ����ɶ�9���Ĵ����� ǰ���������¶�
	a=DS18B20_ReadOneChar();
	b=DS18B20_ReadOneChar();
	t=b;
	t<<=8;
	t=t|a;
	//t= t/2; //�Ŵ�10���������������---����û��
	t = t*0.0625;

	return(t);
}




void Delay_us(unsigned int uiUs)	//us��ʱ����
{
	for(;uiUs > 0;uiUs--)
	{
		_nop_();
	}	
}



void Delay_ms(unsigned int uiMs)	//ms��ʱ����
{
	unsigned int i;
	for(;uiMs > 0;uiMs--)
	{
		for(i = 1498;i > 0;i--);
	}
}

//**********************************************************
//��ȡ�¶Ⱥ����������¶ȵľ���ֵ������עflag_temper��flag_temper=1��ʾ����flag_temper=0��ʾ��
//**********************************************************
//unsigned int Get_temp(void)         //��ȡ�¶�ֵ 
//{  
//	float tt;
//	unsigned char a,b;
//	unsigned int temp;
//	if( DS18B20_Init() == 0 )                //��ʼ��
//	{
//		DS18B20_Write_Byte(0xcc);          //����ROMָ��
//		DS18B20_Write_Byte(0x44);          //�¶�ת��ָ��
//		if( DS18B20_Init() == 0 )                 //��ʼ��
//		{
//			DS18B20_Write_Byte(0xcc);          //����ROMָ��
//			DS18B20_Write_Byte(0xbe);          //���ݴ���ָ��
//			a = DS18B20_Read_Byte();           //��ȡ���ĵ�һ���ֽ�Ϊ�¶�LSB
//			b = DS18B20_Read_Byte();           //��ȡ���ĵ�һ���ֽ�Ϊ�¶�MSB
//			temp = b;                      //�ȰѸ߰�λ��Ч���ݸ���temp
//			temp <<= 8;                    //������8λ���ݴ�temp�Ͱ�λ�Ƶ��߰�λ
//			temp = temp|a;                //���ֽںϳ�һ�����ͱ���
//			if(temp>0xfff)
//			{
//				flag_temper=1;				//�¶�Ϊ����
//				temp=(~temp)+1;
//			}
//			else
//			{																			   
//				flag_temper=0;				//�¶�Ϊ������0
//			}
//			
//			tt = temp*0.0625;              //�õ���ʵʮ�����¶�ֵ
//			                                //��ΪDS18B20���Ծ�ȷ��0.0625��
//			                                //���Զ������ݵ����λ�������0.0625��
//			temp = tt*100;               //�Ŵ�ٱ�
//			                                //��������Ŀ�Ľ�С����Ҳת��Ϊ����ʾ����
//		}
//	}
//	return temp;
//}



void Ir_work(void)        //�����ֵɢת����
  {
       
		  
		 char TimeNum[]="  ";
	   TimeNum[0] = Tab[IRcord[2]/16];  //���������벢��ʾ
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
	   irpro_ok=0;           //������ɺ������־λ

  }

/******************************************************************/
/*                    ������뺯������                            */
/******************************************************************/
void Ircordpro(void)//������ֵ������
{ 
  unsigned char i, j, k;
  unsigned char cord,value;

  k=1;
  for(i=0;i<4;i++)      //����4���ֽ�
     {
      for(j=1;j<=8;j++) //����1���ֽ�8λ
         {
          cord=irdata[k];
          if(cord>7)//����ĳֵΪ1������;����о��Թ�ϵ������ʹ��12M���㣬��ֵ������һ�����
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
     } irpro_ok=1;//������ϱ�־λ��1
    
}

void UART_Init(void)
{

		PCON |= 0x80;		//ʹ�ܲ����ʱ���λSMOD
		SCON = 0x50;		//8λ����,�ɱ䲨����
		AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
		AUXR &= 0xFE;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
		TMOD &= 0x0F;		//�����ʱ��1ģʽλ
		TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
		TL1 = 0xB8;		//�趨��ʱ��ֵ
		TH1 = 0xB8;		//�趨��ʱ����װֵ
		ET1 = 0;		//��ֹ��ʱ��1�ж�
		TR1 = 1;		//������ʱ��1
	
}


/**************************************************************************
 - ����������51��Ƭ���Ĵ��ڷ����ֽڵĺ���
 - ����ģ�飺STC51���ڲ���
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����ucData:Ҫ���͵�һ���ֽ�
 - ����˵������
 - ע������һ���ֽڣ��Ǵ��ڷ��͵Ļ�������
 **************************************************************************/

void UART_Send_Byte(unsigned char ucData)
{
 TI = 0;
 SBUF = ucData;
 while(!TI);
 TI = 0;
}

/**************************************************************************
 - ����������51��Ƭ���Ĵ��ڷ���0d 0a �����س�����
 - ����ģ�飺STC51���ڲ���
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵������
 - ����˵������
 - ע���˺������Ƿ���0d 0a�������ֽڣ��ڡ������նˡ��ϻ��лس����е�Ч��
 **************************************************************************/

void UART_Send_Enter()
{
 UART_Send_Byte(0x0d);
 UART_Send_Byte(0x0a);
}

/**************************************************************************
 - ����������51��Ƭ���Ĵ��ڷ����ַ���
 - ����ģ�飺STC51���ڲ���
 - �������ԣ��ⲿ��ʹ�û�ʹ��
 - ����˵����s:ָ���ַ�����ָ��
 - ����˵������
 - ע��������һ���ַ������س�
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
	PCON &= 0x7F;		//�����ʲ�����
	SCON = 0x50;		//8λ����,�ɱ䲨����
	TMOD &= 0x0F;		//�����ʱ��1ģʽλ
	TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TL1 = 0xFD;			//�趨��ʱ��ֵ
	TH1 = 0xFD;			//�趨��ʱ����װֵ		
	TR1 = 1;			//������ʱ��1
	ET1 = 0;        	//��ֹ��ʱ��1�ж�
	EA=1;				//�������ж� 
	ES=1;				//���Ŵ����ж�
}
void Uartsend(unsigned char byte)//����
{
	SBUF=byte;//������д�뷢�ͻ�����SBUF
	//���ݷ�����ɵı�־��TI=1�����Եȴ����ݴ�����
			 
	
	while(TI==0);
	TI=0;//�������
}
void UART_ISR() interrupt 4//�����жϺ���
{
	if(RI==1)//�����ж�
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
		Uartsend(SBUF);//�ѽ��յ����ݷ��͵������
		
		RI=0;//�������
	}
}

