//----------------------------------------------------------------------------

//Модуль реализации выполнения команд Wake

//----------------------------------------------------------------------------

#include "Commands.h"

//----------------------------------------------------------------------------


//------------------------------- Переменные: --------------------------------

const char Info[] = {"MDR32F9Q2 Rev2.0\n\0"}; //имя устройства

extern char Rx_Cmd,          //принятая команда
            Rx_Nbt,          //принятое количество байт в пакете
            Rx_Dat[FRAME];   //массив принятых данных

extern char Command;         //код принятой команды на выполнение

extern char Tx_Cmd,          //команда, передаваемая в пакете
            Tx_Nbt,          //количество байт данных в пакете
            Tx_Dat[FRAME];   //массив данных для передачи


bool _powerStatus = false;
bool _lightStatus = false;
bool _highTemperature = false;
bool _hookWarning = false;
bool _currentMode = true; //Опоры или Cтойка, стрела, лебедка (1/0)
//----------------------------------------------------------------------------

//---------------------- Передача ответа на команду: -------------------------

void Tx_Replay(char n, char err)
{
  Tx_Nbt = n;                     //количество байт
  Tx_Dat[0] = err;                //код ошибка
  Tx_Cmd = Command;               //команда
  Port_StartWrite();              //инициализация передачи
  Command = CMD_NOP;              //команда обработана
}

//---------------------------- Выполнение команд: ----------------------------

void Commands_Exe(void)
{
  char i;		
  switch(Command)
  {
  case CMD_ERR: //обработка ошибки
    {
      Tx_Replay(1, ERR_TX);
      break;
    }
  case CMD_ECHO: //команда "эхо"
    {
      for(i = 0; i < Rx_Nbt && i < FRAME; i++)
        Tx_Dat[i] = Rx_Dat[i];
      Tx_Replay(Rx_Nbt, Tx_Dat[0]);
      break;
    }
  case CMD_INFO: //команда "инфо"
    {
      char ch = 1;
      for(i = 0; i < FRAME && ch; i++)
        ch = Tx_Dat[i] = Info[i];
      Tx_Replay(i, Tx_Dat[0]);
      break;
    }
  case soundSignal: //гудок
    {
			//if(Rx_Dat[0] == 1) "гудим" else "не гудим";
			//Tx_Replay(1, ERR_NO);
      break;
    }
  case speedButton: //скорость
    {
			//if(Rx_Dat[0] == 1) "быстро" else "медленно";
			//Tx_Replay(1, ERR_NO);
      break;
    }
  case modeCrOrP: //режим работы (Опоры или Cтойка, стрела, лебедка)
    {
			//if(Rx_Dat[0] == 1) "Опоры" else "Cтойка, стрела, лебедка";
			if(Rx_Dat[0] == 1 || Rx_Dat[0] == 0)
				_currentMode = Rx_Dat[0];
			Tx_Replay(1, _currentMode);
      break;
    }
  case modeCrOrPStatus: //режим работы (Опоры или Cтойка, стрела, лебедка)
    {
			//if(Rx_Dat[0] == 1) "Опоры" else "Cтойка, стрела, лебедка";
			//if(Rx_Dat[0] == 3)
			Tx_Replay(1, _currentMode);
      break;
    }
  case powerButton: //включить питание 
    {
			if(Rx_Dat[0] == 1 || Rx_Dat[0] == 0)
				_powerStatus=Rx_Dat[0];
			Tx_Replay(1, _powerStatus);
      break;
    }
	case powerStatus: //узнать включено ли питание 
    {
			Tx_Replay(1, _powerStatus);
      break;
    }
  case lightButton: //включить свет
    {			
			if(Rx_Dat[0] == 1 || Rx_Dat[0] == 0)
				_lightStatus=Rx_Dat[0];
			Tx_Replay(1, _lightStatus);
      break;
    }
	case lightStatus: //узнать включен ли свет 
    {
			Tx_Replay(1, _lightStatus);
      break;
    }
	case highTemperature: //узнать есть ли опасность перегрева РЖ
    {
			Tx_Replay(1, _highTemperature);
      break;
    }
	case hookWarning: //узнать перекручен ли трос лебедки
    {
			Tx_Replay(1, _hookWarning);
      break;
    }
  case pillar: //поворот стойки
    {
			//if(Rx_Dat[0] == ??) ... else ...;
			#if SEND_REPLY
      Tx_Replay(1, ERR_NO);
			#endif
      break;
    }
  case derrick: //подъем или опускание подъемной стрелы
    {
			#if SEND_REPLY
      Tx_Replay(1, ERR_NO);
			#endif
      break;
    }
  case outrigger: //подъем или опускание выносной стрелы
    {
			#if SEND_REPLY
      Tx_Replay(1, ERR_NO);
			#endif
      break;
    }
  case telescopic: //выдвижение или втягивание телескопической стрелы
    {
			#if SEND_REPLY
      Tx_Replay(1, ERR_NO);
			#endif
      break;
    }
  case hook: //подъем или опускание лебедки
    {
			#if SEND_REPLY
      Tx_Replay(1, ERR_NO);
			#endif
      break;
    }
  case leftCrutch: //подъем или опускание левой опоры
    {
			#if SEND_REPLY
      Tx_Replay(1, ERR_NO);
			#endif
      break;
    }
  case rightCrutch: //подъем или опускание правой опоры
    {
			#if SEND_REPLY
      Tx_Replay(1, ERR_NO);
			#endif
      break;
    }
  }
}

//----------------------------------------------------------------------------
