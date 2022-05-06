/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/

#include "stm32l1xx.h"

#define DEFAULT_TIME	(int)30

TIM_HandleTypeDef tim2;
int time = DEFAULT_TIME;
int actual_position = 1;
int run = 0;
int msecs = 0;

static void SystemClock_Config(void);

int main(void)
{
	HAL_Init();
	SystemClock_Config();			// ustawienie zegara na 32MHz, funkcja zaczerpniêta z dokumentacji do³¹czonej do biblioteki HAL

	//pod³¹czenie portów A i B oraz TIM2 do taktowania zegara
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_TIM2_CLK_ENABLE();

	GPIO_InitTypeDef gpio; // obiekt gpio bêd¹cy konfiguracj¹ portów GPIO

	gpio.Pin = GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15;	// przyciski
	gpio.Mode = GPIO_MODE_IT_RISING;					// jako wejœcia obs³uguj¹ce przerwanie od zbocza narastaj¹cego
	gpio.Pull = GPIO_PULLUP;							// z rezystorami pull-up
	HAL_GPIO_Init(GPIOA, &gpio);						// inicjalizacja

	gpio.Pin = GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7;	// katody wyœwietlacza 7 seg. LED
	gpio.Mode = GPIO_MODE_OUTPUT_PP;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &gpio);

	gpio.Pin = GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 |
			GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7 | GPIO_PIN_14 | GPIO_PIN_15;	// segmenty A - G wyœwietlacza 7 seg. LED, dioda LED i przekaŸnik œwietlówek
	HAL_GPIO_Init(GPIOB, &gpio);

	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);					// aktywacja przerwañ od wejœc 10 - 15

	// ustawienia timera TIM2
	tim2.Instance = TIM2;
	tim2.Init.Period = 10 - 1;
	tim2.Init.Prescaler = (SystemCoreClock / 10000) - 1;
	tim2.Init.ClockDivision = 0;
	tim2.Init.CounterMode = TIM_COUNTERMODE_UP;

	HAL_TIM_Base_Init(&tim2);
	HAL_TIM_Base_Start_IT(&tim2);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_SET); // domyœlnie stan wysoki na katodach wyœwietlacza

	while(1)
	{

	}
}

// Przerwania od przycisków - sterowanie
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	// Zwiêkszenie czasu naœwietlania
	if(run == 0 && GPIO_Pin == GPIO_PIN_15) {
		if(time < 999) {
			time++;
		}
	}
	// Zmniejszenie czasu naœwietlania
	if(run == 0 && GPIO_Pin == GPIO_PIN_13) {
		if(time > 0) {
			time--;
		}
	}

	// Uruchomienie naœwietlania
	if(GPIO_Pin == GPIO_PIN_14 && time > 0) {
		run = 1;
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);	// za³¹czenie przekaŸnika œwietlówek UV
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_SET);	// za³¹czenie diody LED sygnalizuj¹cej naœwietlanie
	}
}

void TIM2_IRQHandler(void)
{
	HAL_TIM_IRQHandler(&tim2);
}

void DisplayDigit(int position, int digit)
{
	// Wygaszenie wyœwietlacza
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7, GPIO_PIN_RESET);
	// Za³¹czenie segmentów wyœwietlacza
	switch(digit)
	{
	case 0:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5, GPIO_PIN_SET);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_SET);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_6, GPIO_PIN_SET);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_6, GPIO_PIN_SET);
		break;
	case 4:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_SET);
		break;
	case 5:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_SET);
		break;
	case 6:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_SET);
		break;
	case 7:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2, GPIO_PIN_SET);
		break;
	case 8:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_SET);
		break;
	case 9:
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_5 | GPIO_PIN_6, GPIO_PIN_SET);
	}
	// Za³¹czenie katody dla odpowiedniej pozycji
	switch(position)
	{
	case 1:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
		break;
	case 3:
		HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET);
		break;
	}
}

// Przerwanie obs³uguj¹ce wyœwietlacz i pomiar czasu w tracie naœwietlania
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	// -------- Odliczanie czasu naœwietlania ----------- //
	if(run == 1) {
		msecs++;
		if(msecs == 1000) {
			time--;
			msecs = 0;
			if(time == 0) {
				run = 0;
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);	// wy³¹czenie przekaŸnika
				HAL_GPIO_WritePin(GPIOB, GPIO_PIN_15, GPIO_PIN_RESET);	// wy³¹czenie diody LED
			}
		}
	}

	// ------------ Obs³uga wyœwietlacza ------------ //
	int digit = 0;
	actual_position++;

	// Pomijanie wyœwietlania zer wiod¹cych
	if(actual_position > 3){
		actual_position = 1;
	} else {
		if(actual_position > 2 && time / 100 < 1) {
			actual_position = 1;
		} else {
			if(actual_position > 1 && time / 10 < 1) {
				actual_position = 1;
			}
		}
	}

	// Ustalenie cyfry do wyœwietlenia na danej pozycji
	if(actual_position == 1) {
		digit = time % 10;
	} else {
		if(actual_position == 2) {
			digit = (time / 10) % 10;
		} else {
			if(actual_position == 3) {
				digit = time / 100;
			}
		}
	}
	DisplayDigit(actual_position, digit);
}

static void SystemClock_Config(void)
{
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};

  /* Enable HSE Oscillator and Activate PLL with HSE as source */
  RCC_OscInitStruct.OscillatorType      = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState            = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource       = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL          = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PLLDIV          = RCC_PLL_DIV3;

  /* Set Voltage scale1 as MCU will run at 32MHz */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /* Poll VOSF bit of in PWR_CSR. Wait until it is reset to 0 */
  while (__HAL_PWR_GET_FLAG(PWR_FLAG_VOS) != RESET) {};

  /* Select PLL as system clock source and configure the HCLK, PCLK1 and PCLK2
  clocks dividers */
  RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
}
