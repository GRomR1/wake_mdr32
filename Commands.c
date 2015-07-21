//----------------------------------------------------------------------------

//Модуль реализации выполнения команд Wake

//----------------------------------------------------------------------------

#include "Commands.h"

//----------------------------------------------------------------------------


//------------------------------- Переменные: --------------------------------

const char Info[] = {"MDR32F9Q2 Rev2.0\0"}; //имя устройства

extern char Rx_Cmd,          //принятая команда
            Rx_Nbt,          //принятое количество байт в пакете
            Rx_Dat[FRAME];   //массив принятых данных

extern char Command;         //код принятой команды на выполнение

extern char Tx_Cmd,          //команда, передаваемая в пакете
            Tx_Nbt,          //количество байт данных в пакете
            Tx_Dat[FRAME];   //массив данных для передачи

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
  case CMD_SETMODE: //установка режима управления
    {
      //cnLock = Rx_Dat[0] & 0x01;
      //cnMute = Rx_Dat[0] & 0x02;
      //Menu_Lock(cnLock, cnMute);
      Tx_Replay(1, ERR_NO);
      break;
    }
  case CMD_GETMODE: //чтение режима управления
    {
      //char d = 0;
      //if(cnLock) d |= 0x01;
      //if(cnMute) d |= 0x02;
      //Tx_Dat[1] = d;
      Tx_Replay(2, ERR_NO);
      break;
    }
  case CMD_SETPAR: //установка параметра
    {
      //char n = Rx_Dat[0];         //номер параметра
      //char c = Rx_Dat[1];         //номер канала
      //long par = DWORD(Rx_Dat[5], Rx_Dat[4], Rx_Dat[3], Rx_Dat[2]);
      //Menu_SetPar(n, c, par);     //установка параметра
      Tx_Replay(1, ERR_NO);
      break;
    }
  case CMD_GETPAR: //чтение параметра
    {
      //long par = Menu_GetPar(Rx_Dat[0], Rx_Dat[1]);
      //Tx_Dat[1] = BYTE1(par);
      //Tx_Dat[2] = BYTE2(par);
      //Tx_Dat[3] = BYTE3(par);
      //Tx_Dat[4] = BYTE4(par);
      Tx_Replay(5, ERR_NO);
      break;
    }
  case CMD_GETSELPAR: //чтение текущего параметра
    {
      //char n = Menu_GetSelPar();  //номер параметра
      //char c = Menu_GetSelChan(); //номер канала
      //long par = Menu_GetPar(n, c);
      //Tx_Dat[1] = n;
      //Tx_Dat[2] = c;
      //Tx_Dat[3] = BYTE1(par);
      //Tx_Dat[4] = BYTE2(par);
      //Tx_Dat[5] = BYTE3(par);
      //Tx_Dat[6] = BYTE4(par);
      Tx_Replay(7, ERR_NO);
      break;
    }
  }
}

//----------------------------------------------------------------------------
