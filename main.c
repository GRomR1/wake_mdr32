/*
 * WakeProtocol Libary: MDR32F9Q2 Port
 * Copyright (C) 2015 Gainanov Ruslan <ruslan.r.gainanov@gmail.com>
 *
 * Реализация протокола WAKE для МК серии MDR32F9Q2I (1986ВЕ92У, К1986ВЕ92QI).
 * 
 * Протокол Wake был создан специально для реализации связи между компьютером 
 * (или управляющим контроллером) и внешними микроконтроллерными устройствами.
 * 
 * Протокол WAKE является логическим уровнем интерфейса управления оборудованием 
 * с помощью асинхронного последовательного канала. 
 * Физический уровень интерфейса протоколом не определяется, может использоваться, 
 * например, RS-232, RS-485 или USB. 
 *
 * Протокол позволяет производить обмен пакетами данных (data frames) длиной 
 * до 255 байт с адресуемыми устройствами, которых может быть до 127. 
 * Формат данных может быть любым. Могут передаваться байтовые поля, 
 * многобайтовые слова или строки символов. 
 *
 * Для контроля правильности передачи данных используется контрольная сумма (CRC-8).
 * Подробная информация о WAKE:
 * http://digit-el.com/files/open/wake/wake.html
 *
 * МК основан на ядре ARM Cortex-M3. Ближайший аналог: STM32F103x.
 * Подробная информация о МК:
 * http://milandr.ru/index.php?mact=Products,cntnt01,details,0&cntnt01productid=57&cntnt01returnid=68
 *
 * Программа создавалась в среде KeilVision v4.74
 *
 * Тестировалась на отладочной плате MDR32F9Q2I Rev2.0
 *
 * Используется UART2 (PORTF0 - Rx, PORTF1 - Tx) с параметрами:
 * скорость обмена - 9600 бод
 * число бит в посылке - 8
 * количество стоп-бит - 1
 * бит четности - нет
 * Прерывания UART2 не используются (вся работа в цикле)
 */

//----------------------------------------------------------------------------

#include <MDR32F9x.h>
#include <MDR32F9x_config.h>
#include "Port.h"
#include "Wake.h"
#include "Commands.h"
#include "led.h"


//------------------------------- Переменные: --------------------------------


//---------------------------- Прототипы функций: ----------------------------

void HSE_48MHz_init (void); 				//установка CPU_CLK от HSE с умножением PLL=6 (CPU_CLK=48MHz)


//-------------------------------- Функия main: ------------------------------

int main( void )
{
    HSE_48MHz_init();
		led_init();
		Port_Init();
	
		#if 1
	
		while (1)
		{
			while(!(MDR_UART2->FR & (1 << 4)))//Буфер FIFO приемника НЕ пуст. (NOT RXFE)
			{
				Port_Read();
			};
			Commands_Exe();
    }
		
		#else
		
		#endif
}


//---------------------- Установка CPU_CLK от HSE: ---------------------------

void HSE_48MHz_init (void)
{
	MDR_RST_CLK->HS_CONTROL = 0x1;                      // Enable HSE oscillator
  while (MDR_RST_CLK->CLOCK_STATUS == 0x00) __NOP();  // wait while HSE startup
	MDR_RST_CLK->PLL_CONTROL |=(6-1)*0x100;							// PLL=6 - CPU CLK 48 MHz
	MDR_RST_CLK->PLL_CONTROL |=0x04;										// Enable PLL for CPU
	while (!(MDR_RST_CLK->CLOCK_STATUS & 0x02)) __NOP();
  MDR_RST_CLK->CPU_CLOCK = 0x106;                     // switch to HSE (8 MHz)
	SystemCoreClockUpdate();                            // Get Core Clock Frequency	
}
