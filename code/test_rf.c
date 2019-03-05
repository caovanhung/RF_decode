#include "N76E003.h"
#include "Common.h"
//#include "Delay.h"
#include "SFR_Macro.h"
#include "Function_define.h"
//#include "intrins.h"
//#include "stdio.h"
//#include "math.h"

#define time1 30
#define time2 60
#define time3 90
#define time4 120
#define time5 1000
#define time6 6000
#define Flash 200
#define Max_start 2500


#define Max_short2 65
#define Min_short2 45


#define Min_head2 500
#define Max_head2 600

#define Max_bit2 3000

#define Time_1k 1000
#define Time_2k5 2500
#define Time_5k 5000
#define Time_10k 10000
#define Time_20k 20000
#define Time_50k 50000
#define ADDR_base 0x109A
#define LINE_1 0X80
#define LINE_2 0XC0


#define Max_time_kiem_tra 1000
#define Min_time_kiem_tra 150

sbit LCD_RS = P1^4;
sbit LCD_RW = P1^3;
sbit LCD_E = P1^2;
sbit LCD_D4 = P1^1;
sbit LCD_D5 = P1^0;
sbit LCD_D6 = P0^0;
sbit LCD_D7 = P0^1;
sbit SPEAKER = P0^6;
sbit RF  = P0^5;

unsigned char da1527[2][3];
unsigned char decode_ok=0,bedside=0;  
unsigned int short_k=0;
unsigned char data_rf[3];
long  num;
int g;
unsigned char dat[3]={0x1e,0xc5,0x2d};
unsigned char line[] = {0x00,0x80,0xc0};
unsigned int i_high=0,i_low=0;
void Delay_RF(unsigned long int u32CNT)
{
    clr_T1M;                                		//T0M=0, Timer0 Clock = Fsys/12
    TMOD = 0x12;                         		  //Timer1 is 16-bit mode
    TR1=1;                            		    //Start Timer1
    while (u32CNT != 0)
    {
        TL1 = LOBYTE(65535-12);	//Find  define in "Function_define.h" "TIMER VALUE"
        TH1 = HIBYTE(65535-12);
        while (TF1 != 1);       		            //Check Timer1 Time-Out Flag
        TF1=0;
        u32CNT --;
    }
    TR1=0;                       			        //Stop Timer1
}

	
void delay_ms(unsigned int Time)
{
   unsigned int i,j,t;
   t=Time;
   for(i=0;i<t;i++)
   {
      for(j=0;j<125;j++);
   }
}
void delay_us(unsigned int Time)
{
   unsigned int i,j;
   for(i=0;i<Time;i++)
   {
      for(j=0;j<2;j++);
   }
}
//unsigned char check_bedside()
//{
//   unsigned int k,cnt=0;
//	 unsigned int xdata head_k=0; 	
//   short_k=0;
//   while(!RF){}; 
//		for(k=0;k<10;k++)
//   {		 
//     while((RF) && (short_k<Max_short2))    
//       {
//        Delay_RF(1);
//          short_k++;
//       }
//      head_k=0;	
//     while((!RF) && (head_k<Max_short2))
//         {
//          Delay_RF(1);
//          head_k++;	
//         }
//     
//        if (((short_k>=Min_check_time)&&(short_k<=Max_check_time) )&& (Min_check_time<=head_k) && (head_k<=Max_check_time))
//         //if(short_k
//				 {
//       cnt++; 
//         }	 
//	   }
//if (cnt>0) return 1;
//else return 0;
//}

unsigned char Get_Kiemtra_loai()
{
	int kk=5,ff=0;
	while(kk--)
	 {
		i_high=0;
		while(!RF);
		while(RF && (i_high< Max_time_kiem_tra))
		 {
				i_high++;
				delay_us(1);         
		 }
		i_low=0;
		while(!RF && (i_low< Max_time_kiem_tra))
		 {
				i_low++;
				delay_us(1);         
		 }	
		if((i_high>Min_time_kiem_tra) && (i_high<Max_time_kiem_tra)  && (i_low> Min_time_kiem_tra) && (i_low< Max_time_kiem_tra)) ff++;
	  }
	if(ff>3) 
	   return 0;
	else return 1;
} 
void RF_decode(void)
{
      unsigned char ii,k,rep;
	    unsigned int xung;
      unsigned int xdata head_k=0; 
      short_k=0;
     while(!RF){}; 
     while((RF) && (short_k<Max_short2))    
       {
        Delay_RF(1);
          short_k++;
       }
      head_k=0;	
     while(!RF)
         {
          Delay_RF(1);
          head_k++;	
         }
 
 if (((short_k>=Min_short2)&&(short_k<=Max_short2) )&& (Min_head2<=head_k) && (head_k<=Max_head2))
      
        {
       //  SPEAKER=0;
            for(rep=0;rep<2;rep++)    
               {  
                 for(ii=0;ii<3;ii++)
                   {
                    for(k=0;k<8;k++)
                       {                       
                      xung=0;
                    while((RF) && (xung<Max_bit2)) 
                        {
                           Delay_RF(1);
                           xung++;
                        }// 
                        
                           if((short_k<(xung*2) &&(short_k>(xung/2))))
                        {
													// da1527[rep][ii]|=(1<<(7-k));
                          da1527[rep][ii]&=~(1<<((7-k)));
                          // SPEAKER=~SPEAKER;
                        }                               
                               else if(xung<=(short_k/2)&&(xung>0))
                               
                               {
																 //da1527[rep][ii]&=~(1<<((7-k)));
                               da1527[rep][ii]|=(1<<(7-k));
                               //SPEAKER=~SPEAKER;
                               }
                                 else {return;}  
                            
                           
                    while(!RF) {};                     
                   }
                  }  
                while(RF){Delay_RF(1);} ;                     
                head_k=0;while(!RF) {Delay_RF(1);head_k++;}  
                if((Min_head2>head_k) || (head_k>Max_head2) ) {return; }
                 } 
            //+++++++++++++++++++++++++bien doi code++++++++++++++++++++++++++++++++++++++++  
           if((da1527[0][0]==0xff) && (da1527[0][1]==0xff) && (da1527[0][2]==0xff))
            {
            return;
            }
            
            if((da1527[0][0]==da1527[1][0]) && (da1527[0][1]==da1527[1][1]) && (da1527[0][2]==da1527[1][2]))   
                 {
//                 data_rf[0] = da1527[0];
//                 data_rf[1] = da1527[1];
//                 data_rf[2] = da1527[2];

                 decode_ok=1;
                
                 }
            else return;
 }           


else return;


}

 void lcd1602_enable(void)
{
   LCD_E=1;   
   delay_us(3);       
   LCD_E=0;
   delay_us(5);   
}
// --------------------------------------------------
// Ham Gui 4 Bit Du Lieu Ra LCD
 void lcd1602_send_4bit_data ( unsigned char cX )
{
 LCD_D4 = cX & 0x01; 
 LCD_D5 = (cX>>1)&1;
 LCD_D6 = (cX>>2)&1;
 LCD_D7 = (cX>>3)&1;
}

// --------------------------------------------------
// Ham Gui 1 Lenh Cho LCD
 void lcd1602_send_command (unsigned char cX )
{
	 delay_us(800);
   lcd1602_send_4bit_data  ( cX >>4 );       // send 4 bit high  
   lcd1602_enable() ;
   lcd1602_send_4bit_data ( cX  );                // send 4 bit low  
   lcd1602_enable() ;
}

// --------------------------------------------------
// Ham Khoi Tao LCD
 void lcd_init ( void )
{
   lcd1602_send_4bit_data ( 0x00 );
   delay_ms(200);
   
   LCD_RS=0;   
   LCD_RW=0;
   LCD_E=0;                                                   // che do gui lenh
   
   lcd1602_send_4bit_data ( 0x03 );          // ket noi 8 bit
   lcd1602_enable() ;
   lcd1602_enable () ;
   lcd1602_enable () ;
   lcd1602_send_4bit_data ( 0x02 );               // ket noi 4 bit
   lcd1602_enable() ;
   
   lcd1602_send_command( 0x2C );                 // giao thuc 4 bit, hien thi 2 hang, ki tu 5x8
   lcd1602_send_command( 0x80);
   lcd1602_send_command( 0x0C);                      // cho phep hien thi man hinh
   lcd1602_send_command( 0x06 );                  // tang ID, khong dich khung hinh
   lcd1602_send_command( 0X01 );         // xoa toan bo khung hinh
}

// --------------------------------------------------
// Ham Thiet Lap Vi Tri Con Tro
void lcd_gotoxy(unsigned char x, unsigned char y)
{
  unsigned char address;
  if(!y)
  address = (LINE_1+x);
  else
  address = (LINE_2+x);
  delay_ms(3);
  lcd1602_send_command(address);
  delay_ms(1);
}

// --------------------------------------------------
// Ham Xoa Man Hinh LCD
void lcd_clear(void)
{
   lcd1602_send_command( 0X01 );  
  delay_us(300);
}

// --------------------------------------------------
// Ham Gui 1 Ky Tu Len LCD
 void lcd_putchar ( unsigned int cX )
{
   LCD_RS=1;   
   lcd1602_send_command( cX );
  LCD_RS=0;
}

// --------------------------------------------------
// Ham Gui 1 Chuoi Ky Tu Len LCD
void lcd_puts(char *s)
{
   while (*s)
   {
         lcd_putchar(*s);
      s++;
   }
}

void lcd_puts_seri(unsigned long int seri ,char h ,char c)
    {
    unsigned char trieu,tramnghin,chucnghin,nghin,tram,chuc,donvi; 
	trieu=seri/1000000;
    tramnghin=(seri%1000000)/100000;
    chucnghin=((seri%1000000)%100000)/10000;
    nghin=((((seri%1000000)%100000))%10000)/1000;
    tram =(((((seri%1000000)%100000))%10000)%1000)/100;
    chuc =((((((seri%1000000)%100000))%10000)%1000)%100)/10;
    donvi=seri%10;   
    if(seri==0)
    {
        lcd_gotoxy(c,h-5);
        lcd_putchar('[');
			  lcd_putchar('-');
        lcd_putchar('-'); 
        lcd_putchar('-');
        lcd_putchar('-'); 
        lcd_putchar('-');  
        lcd_putchar('-');
        lcd_putchar('-'); 
        lcd_putchar(']');    
    }
        else     
        
        {   
					      lcd_gotoxy(c,h-5);
					      lcd_putchar('[');
					      if(seri>1000000)
                {
                lcd_putchar(trieu+48);
                }									
                 if(seri>100000)
                {
                lcd_putchar(tramnghin+48);
                } 
                if(seri>10000)
                {
                lcd_putchar(chucnghin+48); 
                }
                  if(seri>1000)
                {
                lcd_putchar(nghin+48); 
                }
                  if(seri>100)
                {                
                lcd_putchar(tram+48);
                }  
                  if(seri>10)
                {                
                lcd_putchar(chuc+48);
                }
                lcd_putchar(donvi+48);
								//lcd_gotoxy(c,h);
								lcd_putchar(']');

         }
    }
void main (void)
{
 

 CKSWT = 0x10;
 CKEN  = 0x21;
	
 P1M1  = 0x00;
 P1M2  = 0x1F;
// P1M1  = 0x00;
// P1M2  = 0x00;
 P0M1  = 0x20;
 P0M2  = 0x43;
 P3M1 = 0x00;
 P3M2 = 0x00;
	
 TMOD = 0x12;	
// TL0=TH0 = 0xc8; 
 
	EA=1;
//	ET0=1;
	//TR0=1;
  SPEAKER=0;
 delay_ms(4);
lcd_init();	
lcd_clear();	
	lcd_gotoxy(2,0);
	lcd_puts("KIEM TRA SONG");
	lcd_gotoxy(2,1);
	lcd_puts("plese wait...!");
  delay_ms(10000);
  lcd_clear();
 	lcd_gotoxy(2,0);
	lcd_puts("DANG DOC MA...");
//	while(1)
//		
//	{
//  SPEAKER=Get_Kiemtra_loai();
//  }
		
		
		
		// Dung ham RF_DECODE() de kiem tra code sau do retune ve bien decode_ok de hien thi.
	while(1)     
{
	
	RF_decode();
	//check_bedside();
	if(decode_ok==1)
		{
			SPEAKER=1;
			g=0;
			while(g<20)
				{
					if(!Get_Kiemtra_loai()) bedside=1;
					g++;
				}
			lcd_gotoxy(2,0);
			lcd_puts("PHAT HIEN MA");
			SPEAKER=1;
			delay_ms(1000);
			SPEAKER=0;
			delay_ms(1000);
			SPEAKER=1;
			delay_ms(1000);
			SPEAKER=0;
		
			
			if(bedside==1)
				{
					num=0;
					num= (num<<8) + da1527[0][2];
					num= (num<<8) + da1527[0][1];
					num=num+10000;
					lcd_puts_seri(num,2,1);
					lcd_gotoxy(9,1);
					lcd_puts("PHIM ");
					lcd_putchar(da1527[0][0]);
       					
				}
			else	
				{
					num=0;
					num= (num<<8) + da1527[0][2];
					num= (num<<8) + da1527[0][1];
					num= (num<<8) + da1527[0][0];
					lcd_gotoxy(2,1);
					lcd_puts_seri(num,2,4);

				}
			da1527[0][0]=0;
			da1527[0][1]=0;
			da1527[0][2]=0;
			bedside=0;
			decode_ok=0;
    }
}
}