

#include "StdTypes.h"
#include "DIO_Interface.h"
#include "LCD_Interface.h"
#include "ADC_Interface.h"
#include "KeyPad_Interface.h"
#include "SENSORS_Interface.h"
#include "SENSORS_Cfg.h"
#include "TEMP_Filter.h"
#include "FireSystem_Private.h"
#include "FireSystem_Interface.h"
#include "FireSystem_Cfg.h"
#include "MOTOR_Interface.h"

#define  F_CPU   8000000
#include <util/delay.h>

static FireSystem_State_t SystemState=FINE;
static FireSystem_Damage_t DamageState=REVERSABLE;
static c8* PASSWORD_Key="123456";
static u16 TEMP=30;

static u8 string_len(c8*str)
{
	u8 i;
	for(i=0;str[i]; i++);
	return i;
}

static u8 string_compare(c8 *str1,c8*str2)
{
	u8 i;
	u8 l1=string_len(str1);
	u8 l2=string_len(str2);

	if(l1!=l2)
	{
		return 0;
	}
	else
	{
		for(i=0; str1[i]; i++)
		{
			if(str1[i]!=str2[i])
			{
				return 0;
			}

		}
		return 1;
	}

}

void FireSystem_Init(void)
{
	ADC_SetInterruptCallBack(SENSORS_ISR_CallBack);
	ADC_StartConversion_INT(LM35_CH);
}

void FireSystem_Runnable(void)
{
	
	u8 key_input;
	u8 i=0;
	c8 pass_input_str [PASSWORD_LEN+1];
	
	FireSystem_GUI(NORMAL);
	TEMP=TEMP_Filter_GetFilteredTemp();
	u16 smoke=SENSORS_GetSmoke();
	if (TEMP>500)
	{
		if (DamageState==REVERSABLE)
		{
			SystemState=HEAT;
		}
		else
		{
			SystemState=FIRE;
		}
		//ADC_StartConversion_INT(SMOKE_CH);
		if (smoke>50)
		{
			SystemState=FIRE;
			DamageState=IRREVERSABLE;
			MOTOR_CW(M3);
		}
	}
	else if (TEMP<=450 && DamageState==REVERSABLE)
	{
		SystemState=FINE;
		MOTOR_Stop(M3);
	}
	if(DamageState==IRREVERSABLE)
	{
		key_input=KEYPAD_GetKey();
		if (key_input=='1')
		{
			FireSystem_GUI(PASSWORD);
			for (i=0;key_input!='=';)
			{
				key_input=KEYPAD_GetKey();
				if (key_input!=NO_KEY)
				{
					LCD_WriteChar(key_input);
					pass_input_str[i]=key_input;
					i++;
				}
			}
			pass_input_str[i-1]=0;
			if (string_compare(pass_input_str,PASSWORD_Key))
			{
				DamageState=REVERSABLE;
				SystemState=FINE;
				MOTOR_Stop(M3);
			}
			else
			{
				FireSystem_GUI(WRONG_PASSWORD);
				_delay_ms(400);
			}
		}
	}
	
	//ADC_StartConversion_INT(LM35_CH);
}


static void FireSystem_GUI(GUI_State_t state)
{
	if (state==NORMAL)
	{
		
		if (SystemState==FINE)
		{
			LCD_WriteStringGoto(line1,0,"State : ");
			LCD_WriteStringGoto(line2,0,"Temp : ");
			LCD_WriteStringGoto(line1,8,"FINE  ");
			LCD_WriteString(" ");
			LCD_GoTo(line2,7);
			LCD_WriteNumber(TEMP/10);
			LCD_WriteString("C  ");
			DIO_WritePin(GREEN_LED,HIGH);
			DIO_WritePin(YELLOW_LED,LOW);
			DIO_WritePin(RED_LED,LOW);
		}
		else if (SystemState==HEAT)
		{
			LCD_WriteStringGoto(line1,0,"State : ");
			LCD_WriteStringGoto(line2,0,"Temp : ");
			LCD_WriteStringGoto(line1,8,"HEAT  ");
			LCD_WriteString(" ");
			LCD_GoTo(line2,7);
			LCD_WriteNumber(TEMP/10);
			LCD_WriteString("C  ");
			DIO_WritePin(YELLOW_LED,HIGH);
			DIO_WritePin(GREEN_LED,LOW);
			DIO_WritePin(RED_LED,LOW);
		}
		else if (SystemState==FIRE)
		{
			LCD_WriteStringGoto(line1,0,"State : ");
			LCD_WriteStringGoto(line1,8,"FIRE  ");
			LCD_WriteString(" ");
			LCD_GoTo(line2,0);
			LCD_WriteNumber(TEMP/10);
			LCD_WriteString("C reset press1");
			DIO_WritePin(RED_LED,HIGH);
			DIO_WritePin(YELLOW_LED,LOW);
			DIO_WritePin(GREEN_LED,LOW);
		}
	}
	else if (state==PASSWORD)
	{
		LCD_WriteStringGoto(line1,0,"Enter Password:");
		LCD_WriteStringGoto(line2,0,"                ");
		LCD_GoTo(line2,0);
		
	}
	else
	{
		LCD_WriteStringGoto(line1,0," WRONG Password");
		LCD_WriteStringGoto(line2,0,"                ");
		LCD_GoTo(line2,0);
	}
}