#include "sys.h"
#include "delay.h"  
#include "usart.h"  
#include "led.h"
#include "lcd.h"
#include "usmart.h"		
#include "hc05.h" 	 
#include "usart3.h" 	
#include "key.h" 	 
#include "string.h"	 

extern u8 X,Y;
int keymodel=0;
void GPIO_INIT2(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA,GPIOE时钟
 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//WK_UP对应引脚PA0
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输入模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100M
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA0
	GPIO_ResetBits(GPIOA,GPIO_Pin_11);
}

int main(void)
{
	int expandy;
	int goal;
	int goal_ang;
	u8 i;
	u8 key;           //保存键值
	int d;  //发射距离
	int dis[]={35,36,37,38,39,42,43,44,45,48,50};  //距离数组
	/*****************
	12v  227
	*****************/
	int ans=0;
	int ang=0;
	int angle=0;
	u8 reclen=0;  
	uart_init(115200);
	usart3_init(115200);	 

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);      //初始化延时函数
	
	usmart_dev.init(84); 		//初始化USMART		
	LED_Init();					//初始化LED
	KEY_Init();					//初始化按键
	GPIO_INIT();
	TIM14_PWM_Init(199,7199);
  TIM12_PWM_Init(199,7199);
	GPIO_INIT2();
	
	LCD_Init();
	LCD_ShowString(30,30,200,16,16,"ALIENTEK STM32F4 ^_^");	
	LCD_ShowString(30,50,200,16,16,"HC05 BLUETOOTH COM TEST");	
	LCD_ShowString(30,70,200,16,16,"ATOM@ALIENTEK");
	
 	while(HC05_Init()) 		//初始化ATK-HC05模块  
	{
		LCD_ShowString(30,90,200,16,16,"ATK-HC05 Error!"); 
		delay_ms(500);
		LCD_ShowString(30,90,200,16,16,"Please Check!!!"); 
		delay_ms(100);
	}	
	
	LCD_ShowString(30,110,200,16,16,"Dis:");	
	LCD_ShowString(30,130,200,16,16,"Ang:");
  LCD_ShowString(30,190,200,16,16,"Area:");	
	LCD_ShowString(30,210,200,16,16,"Dist:");	
	
	POINT_COLOR=BLUE;
	USART3_RX_STA=0;
	
 	while(1) 
	{		   
		key=KEY_Scan(0);		//得到键值
		if(key)
		{
			switch(key)
			{
				/************************************基础部分第1问*****************************************************/			
				case KEY0_PRES:	//控制LED0翻转
					TIM_SetCompare1(TIM12,180);
					TIM_SetCompare1(TIM14,190);
					LCD_ShowString(30,150,200,16,16,"The first problem~~~"); 
					LED0=!LED0;
					//接收数据
						if(USART3_RX_STA&0X8000)			//接收到一次数据了
						{
							reclen=USART3_RX_STA&0X7FFF;	//得到数据长度    //3
							USART3_RX_BUF[reclen]=0;	 	//加入结束符
							
							printf("USART3_RX_BUF7788:%s\n",USART3_RX_BUF);

							USART3_RX_STA=0;		
							/**************距离d********/
							ans=(USART3_RX_BUF[0]-'0')*100+(USART3_RX_BUF[1]-'0')*10+(USART3_RX_BUF[2]-'0');
							if(ans>300)d=300;
									else if(ans<200)d=200;
									else d=ans;
							LCD_ShowNum(60,110,d,5,16);
							/**********自动瞄准**************/
								
							/***********上电时间************/
							TIM_SetCompare1(TIM14,185);
							TIM1_Delay500MS(10);
							GPIO_SetBits(GPIOA,GPIO_Pin_11);
							TIM1_Delay500MS(dis[(d-200)/10]); 
							GPIO_ResetBits(GPIOA,GPIO_Pin_11);
							
							GPIO_SetBits(GPIOA,GPIO_Pin_13);
							TIM1_Delay500MS(1); 
							GPIO_ResetBits(GPIOA,GPIO_Pin_13);
						}
						TIM1_Delay500MS(50); 
						TIM_SetCompare1(TIM12,180);
					  TIM_SetCompare1(TIM14,190);
					break;
						
			   /*********************************基础部分第2问********************************************************/			
				 case KEY1_PRES:	
					 TIM_SetCompare1(TIM12,180);
						TIM_SetCompare1(TIM14,190);
					 		LCD_ShowString(30,150,200,16,16,"The second problem~~~"); 
							if(USART3_RX_STA&0X8000)			//接收到一次数据了
						  {
								reclen=USART3_RX_STA&0X7FFF;	//得到数据长度    //3
								USART3_RX_BUF[reclen]=0;	 	//加入结束符
								printf("USART3_RX_BUF7788:%s\n",USART3_RX_BUF);
								printf(":%d\n",reclen);
								USART3_RX_STA=0;		
								/******************距离d****************/
								ans=(USART3_RX_BUF[0]-'0')*100+(USART3_RX_BUF[1]-'0')*10+(USART3_RX_BUF[2]-'0');
								if(ans>300)d=300;
									else if(ans<200)d=200;
									else d=ans;
								LCD_ShowNum(60,110,d,5,16);
								/*******************角度angle*************/	
									//角度是负数
								if(USART3_RX_BUF[4]=='-')		
								{
									LCD_ShowString(63,130,200,16,16,"-");	
									//角度是1位数
									if(reclen==6)
									{
										ang=(USART3_RX_BUF[5]-'0');
										angle=-ang;
										LCD_ShowNum(65,130,ang,5,16);				
									}
									//角度是2位数
									else
									{
										  ang=(USART3_RX_BUF[5]-'0')*10+(USART3_RX_BUF[6]-'0');
										  if(ang>30)
											{
												angle=-30;
												LCD_ShowNum(65,130,30,5,16);		
											}
											else
											{
												angle=-ang;
												LCD_ShowNum(65,130,ang,5,16);							
											}					
									}
								}
								//角度是正数
								else 
								{
									if(reclen==5)//1位数
									{
										ang=(USART3_RX_BUF[4]-'0');
										angle=ang;
										LCD_ShowNum(60,130,angle,5,16);		
									}
									else   //2位数
									{
										ang=(USART3_RX_BUF[4]-'0')*10+(USART3_RX_BUF[5]-'0');
										angle=ang;
										if(ang>30)
										{
											LCD_ShowNum(60,130,30,5,16);
										}
										else
											LCD_ShowNum(60,130,angle,5,16);
									}
								}
								/************自动瞄准*******************/
								if(angle==0)
									TIM_SetCompare1(TIM12,180);
								else if(angle>0)
									TIM_SetCompare1(TIM12,180-abs(angle)*3/30);
								else if(angle<0)
									TIM_SetCompare1(TIM12,180+abs(angle)*3/30);
								TIM1_Delay500MS(10);
								
								
								/*************上电时间***********************/
								TIM_SetCompare1(TIM14,185);
							TIM1_Delay500MS(10);
							GPIO_SetBits(GPIOA,GPIO_Pin_11);
							TIM1_Delay500MS(dis[(d-200)/10]); 
							GPIO_ResetBits(GPIOA,GPIO_Pin_11);
							
							GPIO_SetBits(GPIOA,GPIO_Pin_13);
							TIM1_Delay500MS(1); 
							GPIO_ResetBits(GPIOA,GPIO_Pin_13);
							}
							TIM1_Delay500MS(50); 
						TIM_SetCompare1(TIM12,180);
					  TIM_SetCompare1(TIM14,190);
				  break;
							
				 /********************************************发挥部分第一问*********************************************/
				 case KEY2_PRES :
					 TIM_SetCompare1(TIM12,180);
					 TIM_SetCompare1(TIM14,190);
					 LCD_ShowString(30,150,200,16,16,"The first expand~~~"); 
							TIM_SetCompare1(TIM12,190 );
						 //从-60到30旋转，检测到圆盘后暂停
						 goal=0;
						 for(i=190;i>=175;i--)
						 {
								TIM_SetCompare1(TIM12,i);
							 LCD_ShowNum(65,190,X,5,16);
							 LCD_ShowNum(65,210,Y,5,16);
							 if(goal<X)
							 {
									goal=X;
									goal_ang=i;
							 }
							 delay_ms(150);
						 };
						 	
						 TIM_SetCompare1(TIM12,goal_ang-3);
						TIM1_Delay500MS(10);
						/*************上电时间***********************/
						TIM_SetCompare1(TIM14,185);
						TIM1_Delay500MS(10);
						 
						GPIO_SetBits(GPIOA,GPIO_Pin_11);
						 if(Y<200)expandy=200;
						 else if(Y>300)expandy=300;
						 else expandy=Y;
						 LCD_ShowNum(65,210,expandy,5,16);
						TIM1_Delay500MS(dis[(expandy-200)/10]); 
						GPIO_ResetBits(GPIOA,GPIO_Pin_11);
						 
						 
						GPIO_SetBits(GPIOA,GPIO_Pin_13);
						TIM1_Delay500MS(1); 
						GPIO_ResetBits(GPIOA,GPIO_Pin_13);
							TIM1_Delay500MS(50); 
						  TIM_SetCompare1(TIM12,180);
					    TIM_SetCompare1(TIM14,190);
					break;
						 
				 /********************************************发挥部分第二问*********************************************/
					case WKUP_PRES:
						TIM_SetCompare1(TIM12,180);
					 TIM_SetCompare1(TIM14,190);
					 LCD_ShowString(30,150,200,16,16,"The second expand~~~"); 
					 TIM_SetCompare1(TIM12,185 );
						 //从-60到30旋转，检测到圆盘后暂停
						 goal=0;
						 for(i=185;i>=175;i--)
						 {
								TIM_SetCompare1(TIM12,i);
							 LCD_ShowNum(65,190,X,5,16);
							 if(goal<X)
							 {
									goal=X;
									goal_ang=i;
							 }
							 delay_ms(150);
						 };
						 	
						 TIM_SetCompare1(TIM12,goal_ang);
						TIM1_Delay500MS(10);
						/*************上电时间***********************/
						TIM_SetCompare1(TIM14,185);
						TIM1_Delay500MS(10);
						GPIO_SetBits(GPIOA,GPIO_Pin_11);
						TIM1_Delay500MS(45); 
						GPIO_ResetBits(GPIOA,GPIO_Pin_11);
						GPIO_SetBits(GPIOA,GPIO_Pin_13);
						TIM1_Delay500MS(1); 
						GPIO_ResetBits(GPIOA,GPIO_Pin_13);
							TIM1_Delay500MS(50); 
						  TIM_SetCompare1(TIM12,180);
					    TIM_SetCompare1(TIM14,190);
					 break;


			}
		}	 															     				   
	}											    
}
