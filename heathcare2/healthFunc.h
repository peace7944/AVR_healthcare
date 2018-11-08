/*
 * healthFunc.h
 *
 * Created: 2018-10-11 오전 10:44:52
 *  Author: USER
 */ 


#ifndef HEALTHFUNC_H_
#define HEALTHFUNC_H_

unsigned int prekey = 0;
unsigned int msec = 0, timer_msec = 0, flicker_msec = 0, motor_msec = 0, calorie_time = 0;
unsigned int hour = 0, hour_ten = 0, minit = 0, minit_ten = 0, sec = 0, sec_ten = 0;

char str1[16] = {0,}, str2[16] = {0,};

bool timer_flag = false, flicker_on_toggle = true, calorie_flag = false;
int flicker_flag = 0;
int calorie_speed = 0;
float calorie = 0;

unsigned int my_getkey(){
	unsigned int key = 0, result = 0;
	for(int i=0;i<3;i++){
		if(i==0) PORTE = 0b11111110;
		if(i==1) PORTE = 0b11111101;
		if(i==2) PORTE = 0b11111011;
		
		_delay_us(5);
		
		key = (~PINE & 0xf0);
		if(key){
			result = key | (PORTE & 0x0f);
			if(result != prekey){
				prekey = result;
				return result;
			}
			else return 0;
		}
	}
	prekey = 0;
	return 0;
}

void flicker(bool flicker_on_toggle){
	if(flicker_on_toggle) sprintf(str2,"    %d%d:%d%d:%d%d    ",hour_ten,hour,minit_ten,minit,sec_ten,sec);
	else{
		if(flicker_flag == 0) sprintf(str2,"    _%d:%d%d:%d%d    ",hour,minit_ten,minit,sec_ten,sec);
		else if(flicker_flag == 1) sprintf(str2,"    %d_:%d%d:%d%d    ",hour_ten,minit_ten,minit,sec_ten,sec);
		else if(flicker_flag == 2) sprintf(str2,"    %d%d:_%d:%d%d    ",hour_ten,hour,minit,sec_ten,sec);
		else if(flicker_flag == 3) sprintf(str2,"    %d%d:%d_:%d%d    ",hour_ten,hour,minit_ten,sec_ten,sec);
		else if(flicker_flag == 4) sprintf(str2,"    %d%d:%d%d:_%d    ",hour_ten,hour,minit_ten,minit,sec);
		else if(flicker_flag == 5) sprintf(str2,"    %d%d:%d%d:%d_    ",hour_ten,hour,minit_ten,minit,sec_ten);
	}
}

#define sw1 30
#define sw2 46
#define sw3 78
#define sw5 142

#endif /* HEALTHFUNC_H_ */