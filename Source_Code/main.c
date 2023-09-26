/************************LIB*******************************/
#include "STD_TYPES.h"
#include "BIT_MATH.h"

/***********************MCAL******************************/
#include "RCC_interface.h"
#include "GPIO_interface.h"
#include "NVIC_Interface.h"
#include "EXTI_interface.h"
#include "STK_interface.h"
#include "AFIO_interface.h"


/***********************HAL******************************/
#include "LED.h"
#include "PushButton.h"

/*******************Prototypes***************************/
void APP_EXTILine6Implementation(void);
void APP_NormalExectution(void);
void APP_YellowExectution(void);

/******************Definitions***************************/
#define REQUIRED_SYSTK_TIME          15000.0
#define REQUIRED_YELLOW_SYSTK         1000

#define REQUIRED_DELAY_TIME         500000
#define REQUIRED_YELLOW_DELAY       200000

#define NORMAL_EXECUTION                 0 /*< The green led is glowing***/
#define Yellow_EXECUTION                 1 /*< The yellow led is blinking*/
static u8 APP_Case = NORMAL_EXECUTION;



int main(void)
{
	/*Enable RCC For SysClock*/
	MCAL_RCC_InitSysClock();
	/**< Enable RCC for Peripherals */
	MCAL_RCC_EnablePeripheral(RCC_APB2, RCC_APB2ENR_IOPAEN);
	MCAL_RCC_EnablePeripheral(RCC_APB2, RCC_APB2ENR_IOPBEN);
	MCAL_RCC_EnablePeripheral(RCC_APB2, RCC_APB2ENR_AFIOEN);
	/*< Initiating leds*/
	HAL_LED_Init(LED_PORTA, LED_PIN0);
	HAL_LED_Init(LED_PORTA, LED_PIN1);
	HAL_LED_Init(LED_PORTA, LED_PIN2);
	HAL_LED_Init(LED_PORTB, LED_PIN3);
	HAL_LED_Init(LED_PORTB, LED_PIN4);
	HAL_LED_Init(LED_PORTB, LED_PIN5);
	HAL_LED_Init(LED_PORTB, LED_PIN6);
	/*<Initiating the pushbutton*/
	HAL_PushButton_Init(PB_PORTA, PB_PIN6, ACTIVE_HIGH);
	/*<Initiating the external interrupt*/
	
	MCAL_AFIO_SetEXTIConfiguration(EXTI_LINE6, GPIO_PORTA);
	
	EXTI_vInit();
	
	EXTI_InitForGPIO(EXTI_LINE6, GPIO_PORTA);
	
	EXTI_SetTrigger(EXTI_LINE6, EXTI_RISING_EDGE);
	
	EXTI_EnableLine(EXTI_LINE6);
	
	MCAL_EXTI_SetCallBack(APP_EXTILine6Implementation);
	
	/*< Enable the IRQ of the EXTI*/
	MCAL_NVIC_vSetPriority(NVIC_EXTI9_5_IRQn, 0, 0);
	MCAL_NVIC_EnableIRQ(NVIC_EXTI9_5_IRQn);
	while(1)
	{
		APP_Case = NORMAL_EXECUTION;
		APP_NormalExectution();
	}
	return 0;
}

void APP_NormalExectution(void)
{
	/*< Defining a variable to use it in implementing the blinking yellow led*/
	u32 Local_YellowLedCounter = 0;
	
	/*<Execution*/
	HAL_LED_On(LED_PORTA, LED_PIN0);
	HAL_LED_On(LED_PORTB, LED_PIN3);
	MCAL_STK_SetDelay_ms(REQUIRED_SYSTK_TIME);	
	APP_Case = Yellow_EXECUTION;
	HAL_LED_Off(LED_PORTA, LED_PIN0);
	HAL_LED_Off(LED_PORTB, LED_PIN3);
	while(Local_YellowLedCounter < REQUIRED_SYSTK_TIME)
	{
		MCAL_STK_SetDelay_ms(REQUIRED_YELLOW_SYSTK);
		HAL_LED_Toggle(LED_PORTA, LED_PIN1);
		HAL_LED_Toggle(LED_PORTB, LED_PIN4);
		Local_YellowLedCounter += REQUIRED_YELLOW_SYSTK;
	}
	HAL_LED_Off(LED_PORTA, LED_PIN1);
	HAL_LED_Off(LED_PORTB, LED_PIN4);	
	HAL_LED_On(LED_PORTA, LED_PIN2);
	HAL_LED_On(LED_PORTB, LED_PIN5);
	MCAL_STK_SetDelay_ms(REQUIRED_SYSTK_TIME);
	HAL_LED_Off(LED_PORTA, LED_PIN2);
	HAL_LED_Off(LED_PORTB, LED_PIN5);
}

void APP_YellowExectution(void)
{
	/*< Defining a variable to use it in the implementation*/
	u32 Local_YellowLedCounter = 0;
	u32 Local_counter = 0; /*< For the delay loop*/
	
	/*<Execution*/
	APP_Case = Yellow_EXECUTION;
	Local_counter = 0;
	while(Local_YellowLedCounter < REQUIRED_DELAY_TIME)
	{
		HAL_LED_Toggle(LED_PORTA, LED_PIN1);
		HAL_LED_Toggle(LED_PORTB, LED_PIN4);
		for(; Local_counter < REQUIRED_YELLOW_DELAY; Local_counter++);
		Local_YellowLedCounter += REQUIRED_YELLOW_DELAY;
	}
	HAL_LED_Off(LED_PORTA, LED_PIN1);
	HAL_LED_Off(LED_PORTB, LED_PIN4);
	HAL_LED_On(LED_PORTA, LED_PIN2);
	HAL_LED_On(LED_PORTB, LED_PIN5);
	Local_counter = 0;
	for(; Local_counter < REQUIRED_DELAY_TIME; Local_counter++);
	HAL_LED_Off(LED_PORTA, LED_PIN2);
	HAL_LED_Off(LED_PORTB, LED_PIN5);
}


void APP_EXTILine6Implementation(void)
{
	u8 Local_ButtonState = 0;
	u32 Local_counter = 0;
	HAL_LED_On(LED_PORTB, LED_PIN6);
	for(; Local_counter < 500000; Local_counter++);
	HAL_LED_Off(LED_PORTB, LED_PIN6);
	MCAL_GPIO_GetPinValue(PB_PORTA, PB_PIN6, &Local_ButtonState);
	if(Local_ButtonState == 0)
	{
		if(APP_Case == NORMAL_EXECUTION)
		{
			APP_YellowExectution();
		}
	}
}