/*
 * heathcare2.cpp
 *
 * Created: 2018-11-05 오전 9:52:39
 * Author : USER
 */ 

#include <avr/io.h>
#define F_CPU 16000000  // cpu 클럭 설정
#define __DELAY_BACKWARD_COMPATIBLE__
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>
#include <stdio.h>
#include "healthFunc.h"
#include "lcd.h"

ISR(TIMER0_OVF_vect){
	flicker_msec++;
	motor_msec++;
	if(timer_flag) timer_msec++;
	if(calorie_flag) calorie_time++;
	
	if(hour_ten >= 2 && hour >= 4){hour_ten = hour = minit_ten = minit = sec_ten = sec = timer_msec = 0;} // timer
	if(hour >= 10){hour_ten++; hour = 0;}
	if(minit_ten >= 6){hour++; minit_ten = 0;}
	if(minit >= 10){minit_ten++; minit = 0;}
	if(sec_ten >= 6){minit++; sec_ten = 0;}
	if(sec >= 10){sec_ten++; sec = 0;}
	if(timer_msec >= 1000){sec++; timer_msec = 0;}
	
	if(flicker_msec >= 500){ // flicker
		flicker_on_toggle = !flicker_on_toggle;
		flicker_msec = 0;
	}
	
	if(motor_msec > 100) motor_msec = 0; // motor
	
	if(calorie_flag){  // calorie
		if(calorie_speed >= 15) calorie += 140;
		else calorie += 105;
	}
}

void powerOn(){
	lcd_putsf(0,0,(unsigned char *)"  Health Care");
	lcd_putsf(0,1,(unsigned char *)"  Bike System...");
	PORTD = 0x00;
	PORTB = 0x20;
	_delay_ms(500);
	
	lcd_clear();
	PORTD = 0xff;
	_delay_ms(500);
	
	lcd_putsf(0,0,(unsigned char *)"  Health Care");
	lcd_putsf(0,1,(unsigned char *)"  Bike System...");
	PORTD = 0x00;
	_delay_ms(500);
	lcd_clear();
	PORTD = 0xff;
	PORTB = 0xff;
}

int main(void)
{
	//timer init
	TCNT0 = 6;  // TCNT0가 256이 될때 overflow -> 0  1/16mHz = 0.0000000625, 0.0000000625 * 64 = 0.000004 분주한 속도
	TIMSK = 0x01;  // 0b00000001, timer 0 enable
	TCCR0 = 0b00000100;  // 0b00000100, 분주비 = 64
	sei();  // 숫자를 카운트
	
	//led init
	DDRD = 0xff;
	PORTD = 0xff;

	lcd_init();
	lcd_clear();
	
	// btn init
	DDRE = 0x0f;
	PORTE = 0xff;
	
	ADCSRA = 0xe7;  // Control and status register (= 0b1110 0111)
	ADMUX = 0x00;
	
	// motor
	DDRB = 0xff;
	PORTB = 0xff;
	
	powerOn();

    /* Replace with your application code */
    while (1)
    {
		while(1){
			unsigned int key = my_getkey();
			lcd_putsf(0,0,(unsigned char*)" <Time Setting> ");
			flicker(flicker_on_toggle);
			lcd_putsf(0,1,(unsigned char*)str2);
			if(sw3 == key){
				if(flicker_flag == 0){
					hour_ten++;
					if(hour_ten == 3) hour_ten = 0;
				}
				else if(flicker_flag == 1){
					hour++;
					if(hour_ten == 2 && hour == 4) hour = 0;
					if(hour == 10) hour = 0;
				}
				else if(flicker_flag == 2){
					minit_ten++;
					if(minit_ten == 6) minit_ten = 0;
				}
				else if(flicker_flag == 3){
					minit++;
					if(minit == 10) minit = 0;
				}
				else if(flicker_flag == 4){
					sec_ten++;
					if(sec_ten == 6) sec_ten = 0;
				}
				else if(flicker_flag == 5){
					sec++;
					if(sec == 10) sec = 0;
				}				
			}
			
			if(sw5 == key){
				flicker_flag++;
				if(flicker_flag >= 6){
					flicker_flag = 0;
					break;
				}
			}
		}
		
		while(1){
			unsigned int key = my_getkey();
			lcd_putsf(0,0,(unsigned char *)"  Health Care   ");
			
			if(sw5 == key) timer_flag = true;
			
			if(key == sw1){
				timer_flag = true;
				calorie_flag = true;
				break;
			}

			sprintf(str2,"    %d%d:%d%d:%d%d    ",hour_ten,hour,minit_ten,minit,sec_ten,sec);
			lcd_putsf(0,1,(unsigned char *)str2);	
		}
		
		int speed = 0;
		while(1){
			unsigned int key = my_getkey();
			
			int vres = ADC;
			int led = vres / 125;
			unsigned int motor_speed = vres / 10;
			
			if(led){
				PORTD = 0xff;
				for(int i=0;i<led;i++){
					PORTD = PORTD << 1;	
				}
			}
			else PORTD = 0xff;
			
			if(motor_speed >= motor_msec) PORTB = 0x20;
			else PORTB = 0xff;
			
			if(vres > 1000) speed = vres - int(vres % 1000);
			else speed = vres;
			speed = int(speed /20);
			calorie_speed = speed;
			
			sprintf(str1,"Velocity: %02dKm/H",speed);
			lcd_putsf(0,0,(unsigned char *)str1);
			sprintf(str2,"    %d%d:%d%d:%d%d    ",hour_ten,hour,minit_ten,minit,sec_ten,sec);
			lcd_putsf(0,1,(unsigned char *)str2);
			
			if(sw2 == key){
				timer_flag = false;
				calorie_flag = false;
				PORTB = 0xff;
				break;
			}
		}
		
		unsigned int calorie_sec = 0, calorie_min = 0, calorie_hour = 0;
		while(1){
			
			unsigned int key = my_getkey();

			calorie_sec = (calorie_time / 1000) % 60;
			calorie_min = (calorie_sec / 60) % 60;
			calorie_hour = (calorie_min / 60) % 24;
			
			sprintf(str1,"Time    %02d:%02d:%02d",calorie_hour, calorie_min, calorie_sec);
			lcd_putsf(0,0,(unsigned char *)str1);
			
			sprintf(str2,"Calorie    %dKcal",(int)(calorie / 1000));
			lcd_putsf(0,1,(unsigned char *)str2);
			
			if(sw5 == key){
				hour_ten = hour = minit_ten = minit = sec_ten = sec = timer_msec = 0;
				break;
			}
		}
    }
}

