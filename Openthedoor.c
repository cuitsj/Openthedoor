#include<reg52.h>
sfr WDT_CONTR=0xc1;
unsigned char  count=0,jd=2,a=0,flag=0,i=0,mima[]="sun4011hold28B";
sbit pwm=P2^5;
void main()
{
	WDT_CONTR=0x37;        //设置看门狗溢出时间为8秒
	TMOD=0x21;             //设置定时器1为工作方式2，定时器0为工作方式1
	TH1=0xfd;
	TL1=0xfd;              //设置波特率为9600
	TR1=1;                 //启动定时器1
	REN=1;                 //允许串行接收位
	SM0=0;                 
	SM1=1;                 //串行口工作方式1
	RI=0;                  //接收中断标志
	TI=0;                  //发送中断标志
	EA=1;                  //开总中断
	ES=1;                  //串行口中断允许位
	TH0=0xfe;              
	TL0=0x33;              //0.5ms 11.0592MHz
	ET0=1;                 //开定时器1中断
	TR0=1;                 //启动定时器0
while(1)
{
if (flag==1)  //flag==1说明收到了数据
{	
    flag=0;   //清零flag
	if(a==mima[i])
	{
		i++;	
	}
	else
	{
		i=0;
	}           //从密码的第一位开始比对，判断是否存在不正确的密码，如果存在又从第一位开始比对
	if(i==14) //密码位数为14，如果密码全部对应则开门
	{ 
		jd=4;   //10%的占空比
		i=0;    //使密码从第一位开始比对
		ES=0;   //关闭串口中断
		SBUF='Y';//发送Y
		while(!TI);//等待发送中断标志置位
		TI=0;      //清零发送中断标志
		ES=1;      //打开串口中断
	}
	}
if (jd==4)
{
	WDT_CONTR=0x37;//确保8秒后关门
	while(1);
}
WDT_CONTR=0x37;//喂狗
}
}

void ser() interrupt 4   //串行口中断
{
	RI=0;   //清空接收中断标志
	a=SBUF; //将收到的数据赋给a
	flag=1; //flag==1说明收到数据
}
void timer0() interrupt 1  //定时器0中断
{
	TH0=0xfe;
	TL0=0x33;   //重装初值
	if(count<jd)
	{
		pwm=1;
	}
	else
	{
		pwm=0;
	}
	count++;
	count=count%40;//除40取余数
}