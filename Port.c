//----------------------------------------------------------------------------

//Модуль реализации протокола Wake

//----------------------------------------------------------------------------

#include "Port.h"

//------------------------------- Переменные: --------------------------------

char Rx_Sta,        //состояние процесса приема пакета
     Rx_Pre,        //предыдущий принятый байт
     Rx_Add,        //адрес, с которым сравнивается принятый
     Rx_Cmd,        //принятая команда
     Rx_Nbt,        //принятое количество байт в пакете
     Rx_Dat[FRAME], //массив принятых данных
     Rx_Crc,        //контрольная сумма принимаемого пакета
     Rx_Ptr;        //указатель на массив принимаемых данных

char Command;       //код команды на выполнение

char Tx_Sta,        //состояние процесса передачи пакета
     Tx_Pre,        //предыдущий переданный байт
     Tx_Add,        //адрес, передававемый в пакете
     Tx_Cmd,        //команда, передаваемая в пакете
     Tx_Nbt,        //количество байт данных в пакете
     Tx_Dat[FRAME], //массив данных для передачи
     Tx_Crc,        //контрольная сумма передаваемого пакета
     Tx_Ptr;        //указатель на массив передаваемых данных


//------------------------- Инициализация UART: ------------------------------

void Port_Init(void)
{
	//I/O ports init
	MDR_RST_CLK->PER_CLOCK 	|=(1<<29); 																//Разрешаем тактирование порта F
	MDR_PORTF->OE						|=0x0002;																	//PF1 на выход (TxD)
	MDR_PORTF->FUNC					|=0x0000000F;															//Назначаем функций UART
	MDR_PORTF->ANALOG				|=0x0003;																	//Переводим пины в цифровой режим
	MDR_PORTF->PWR					|=0x0000000F;															//Устанавливаем самый быстрый фронт
	
	//UART2 interface init
	MDR_RST_CLK->PER_CLOCK	|=(1<<7);																	//Разрешаем тактирование UART в PER_CLOCK
	MDR_RST_CLK->UART_CLOCK	|=(1<<25);																//Еще раз разрешаем тактирование UART - в UART_CLOCK
	MDR_RST_CLK->UART_CLOCK	|=0x0200;																	//Задаем делитель тактовой частоты UART2: UART2_CLK == HCLK/4 (48MHz/4 = 12 MHz)
	MDR_UART2->IMSC					=0;																				//Отключаем UART прерывания если включены
	MDR_UART2->IBRD					=(SystemCoreClock/4)/(16*BAUD);						//Настраиваем скорость - целую часть (4 - делитель MDR_RST_CLK->UART_CLOCK)
	MDR_UART2->FBRD					=((SystemCoreClock/4)*64/(16*BAUD))
														-(((SystemCoreClock/4)/(16*BAUD))*64);	//Настраиваем скорость - дробную часть
	MDR_UART2->LCR_H				=0x60;																		//Configure Data Bits and Parity (8bit - Data, No parity, 1bit - Stop, Без бит четности)
	MDR_UART2->LCR_H				|=(1<<4);																	//Включить FIFO buffer
	MDR_UART2->CR						=0x0300; 																	//Разрешение передачи и приема
	MDR_UART2->CR						|=0x0001;		 															//Включение UART
  //MDR_UART2->IMSC					|=0x0010; 															//Прерывания от приемника UARTRXINTR
  //MDR_UART2->IMSC					|=0x0020;																//Прерывания от передатчика UARTTXINTR
	//NVIC_EnableIRQ(UART2_IRQn);																			//Configure UART2 Interrupt 

  Rx_Add = 1;                         //адрес на прием
  Tx_Add = 1;                         //адрес на передачу
  Rx_Sta = WAIT_FEND;                 //ожидание пакета
  Tx_Sta = SEND_IDLE;                 //ничего пока не передаем
  Command = CMD_NOP;                  //нет команды на выполнение
}


//----------------------- Чтение данных порта: -------------------------------
void Port_Read(void)
{
	char error_flags = MDR_UART2->RSR_ECR;//чтение флагов ошибок
  char data_byte = MDR_UART2->DR;     //чтение данных
  char Pre = Rx_Pre;                  //сохранение старого пре-байта

  if(error_flags)                     //если обнаружены ошибки при приеме байта
  {
    Rx_Sta = WAIT_FEND;               //ожидание нового пакета
    Command = CMD_ERR;                //сообщаем об ошибке
    return;
  }

  if(data_byte == FEND)               //если обнаружено начало фрейма,
  {
    Rx_Pre = data_byte;               //то сохранение пре-байта,
    Rx_Crc = CRC_INIT;                //инициализация CRC,
    Rx_Sta = WAIT_ADDR;               //сброс указателя данных,
    Do_Crc8(data_byte, &Rx_Crc);      //обновление CRC,
    return;                           //выход
  }

  if(Rx_Sta == WAIT_FEND)             //-----> если ожидание FEND,
    return;                           //то выход

  Rx_Pre = data_byte;                 //обновление пре-байта
  if(Pre == FESC)                     //если пре-байт равен FESC,
  {
    if(data_byte == TFESC)            //а байт данных равен TFESC,
      data_byte = FESC;               //то заменить его на FESC
    else if(data_byte == TFEND)       //если байт данных равен TFEND,
           data_byte = FEND;          //то заменить его на FEND
         else
         {
           Rx_Sta = WAIT_FEND;        //для всех других значений байта данных,
           Command = CMD_ERR;         //следующего за FESC, ошибка
           return;
         }
  }
  else
  {
    if(data_byte == FESC)             //если байт данных равен FESC, он просто
      return;                         //запоминается в пре-байте
  }

  switch(Rx_Sta)
  {
  case WAIT_ADDR:                     //-----> ожидание приема адреса
    {
      if(data_byte & 0x80)            //если data_byte.7 = 1, то это адрес
      {
        data_byte = data_byte & 0x7F; //обнуляем бит 7, получаем истинный адрес
        if(!data_byte || data_byte == Rx_Add) //если нулевой или верный адрес,
        {
          Do_Crc8(data_byte, &Rx_Crc); //то обновление CRC и
          Rx_Sta = WAIT_CMD;          //переходим к приему команды
          break;
        }
        Rx_Sta = WAIT_FEND;           //адрес не совпал, ожидание нового пакета
        break;
      }                               //если data_byte.7 = 0, то
      Rx_Sta = WAIT_CMD;              //сразу переходим к приему команды
    }
  case WAIT_CMD:                      //-----> ожидание приема команды
    {
      if(data_byte & 0x80)            //проверка бита 7 данных
      {
        Rx_Sta = WAIT_FEND;           //если бит 7 не равен нулю,
        Command = CMD_ERR;            //то ошибка
        break;
      }
      Rx_Cmd = data_byte;             //сохранение команды
      Do_Crc8(data_byte, &Rx_Crc);    //обновление CRC
      Rx_Sta = WAIT_NBT;              //переходим к приему количества байт
      break;
    }
  case WAIT_NBT:                      //-----> ожидание приема количества байт
    {
      if(data_byte > FRAME)           //если количество байт > FRAME,
      {
        Rx_Sta = WAIT_FEND;
        Command = CMD_ERR;            //то ошибка
        break;
      }
      Rx_Nbt = data_byte;
      Do_Crc8(data_byte, &Rx_Crc);    //обновление CRC
      Rx_Ptr = 0;                     //обнуляем указатель данных
      Rx_Sta = WAIT_DATA;             //переходим к приему данных
      break;
    }
  case WAIT_DATA:                     //-----> ожидание приема данных
    {
      if(Rx_Ptr < Rx_Nbt)             //если не все данные приняты,
      {
        Rx_Dat[Rx_Ptr++] = data_byte; //то сохранение байта данных,
        Do_Crc8(data_byte, &Rx_Crc);  //обновление CRC
        break;
      }
      if(data_byte != Rx_Crc)         //если приняты все данные, то проверка CRC
      {
        Rx_Sta = WAIT_FEND;           //если CRC не совпадает,
        Command = CMD_ERR;            //то ошибка
        break;
      }
      Rx_Sta = WAIT_FEND;             //прием пакета завершен,
      Command = Rx_Cmd;               //загрузка команды на выполнение
      break;
    }
  }
}


//--------------------------- Передача пакета: -------------------------------

void Port_Write( void )
{
  char data_byte;

  if(Tx_Pre == FEND)                  //если производится стаффинг,
  {
    data_byte = TFEND;                //передача TFEND вместо FEND
    Tx_Pre = data_byte;
		MDR_UART2->DR = data_byte;
    return;
  }
  if(Tx_Pre == FESC)                  //если производится стаффинг,
  {
    data_byte = TFESC;                //передача TFESC вместо FESC
    Tx_Pre = data_byte;
		MDR_UART2->DR = data_byte;
    return;
  }

  switch(Tx_Sta)
  {
  case SEND_ADDR:                     //-----> передача адреса
    {
      if(Tx_Add)                      //если адрес не равен нулю, передаем его
      {
        data_byte = Tx_Add;
        Do_Crc8(data_byte, &Tx_Crc);  //вычисление CRC для истинного адреса
        data_byte |= 0x80;            //установка бита 7 для передачи адреса
        Tx_Sta = SEND_CMD;
        break;
      }                               //иначе сразу передаем команду
    }
  case SEND_CMD:                      //-----> передача команды
    {
      data_byte = Tx_Cmd & 0x7F;
      Tx_Sta = SEND_NBT;
      break;
    }
  case SEND_NBT:                      //-----> передача количества байт
    {
      data_byte = Tx_Nbt;
      Tx_Sta = SEND_DATA;
      Tx_Ptr = 0;                     //обнуление указателя данных для передачи
      break;
    }
  case SEND_DATA:                     //-----> передача данных
    {
      if(Tx_Ptr < Tx_Nbt)
        data_byte = Tx_Dat[Tx_Ptr++];  
      else
      {
        data_byte = Tx_Crc;           //передача CRC
        Tx_Sta = SEND_CRC;
      }
      break;
    }
  default:
    {
			//led0_set(); 								// ************** LED0 **************
      Tx_Sta = SEND_IDLE;             //передача пакета завершена
      return;
    }
  }

  if(Tx_Sta != SEND_CMD)              //если не передача адреса, то
    Do_Crc8(data_byte, &Tx_Crc);      //вычисление CRC
  Tx_Pre = data_byte;                 //сохранение пре-байта
  if(data_byte == FEND || data_byte == FESC)
    data_byte = FESC;                 //передача FESC, если нужен стаффинг
	
	MDR_UART2->DR = data_byte;
	while (!(MDR_UART2->FR & (1UL << 7))) //Буфер FIFO передатчика НЕ пуст. (TXFE)
	{
	};
}


//--------------------- Вычисление контрольной суммы: ------------------------

void Do_Crc8(char b, char *crc)
{
	char i;
  for(i = 0; i < 8; b = b >> 1, i++)
    if((b ^ *crc) & 1) *crc = ((*crc ^ 0x18) >> 1) | 0x80;
     else *crc = (*crc >> 1) & ~0x80;
}


//------------------- Инициализация передачи пакета: -------------------------

void Port_StartWrite(void)
{
  char data_byte = FEND;
  Tx_Crc = CRC_INIT;                  //инициализация CRC,
  Do_Crc8(data_byte, &Tx_Crc);        //обновление CRC
  Tx_Sta = SEND_ADDR;
  Tx_Pre = TFEND;
	MDR_UART2->DR = data_byte;
	do
		Port_Write();
	while(Tx_Sta != SEND_IDLE);
}

//---------------------- Проверка окончания передачи: ------------------------
/*
bool Port_TxDone(void)
{
  return(Tx_Sta == SEND_IDLE);
}
*/
//----------------------------------------------------------------------------
