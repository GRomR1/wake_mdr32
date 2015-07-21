//----------------------------------------------------------------------------

//Константы протокола WAKE:

//----------------------------------------------------------------------------

#ifndef WakeH
#define WakeH

//----------------------------------------------------------------------------

#define FEND  0xC0    //Frame END
#define FESC  0xDB    //Frame ESCape
#define TFEND 0xDC    //Transposed Frame END
#define TFESC 0xDD    //Transposed Frame ESCape
#define CRC_INIT 0xDE //Innitial CRC value
#define FRAME 16      //максимальная длина пакета

//RX process states:

enum { WAIT_FEND,     //ожидание приема FEND
       WAIT_ADDR,     //ожидание приема адреса
       WAIT_CMD,      //ожидание приема команды
       WAIT_NBT,      //ожидание приема количества байт в пакете
       WAIT_DATA,     //прием данных
       WAIT_CRC,      //ожидание окончания приема CRC
       WAIT_CARR };   //ожидание несущей

//TX process states:

enum { SEND_IDLE,     //состояние бездействия ==0
       SEND_ADDR,     //передача адреса
       SEND_CMD,      //передача команды
       SEND_NBT,      //передача количества байт в пакете
       SEND_DATA,     //передача данных
       SEND_CRC,      //передача CRC
       SEND_END };    //окончание передачи пакета

//Коды универсальных команд:

#define CMD_NOP     0 //нет операции
#define CMD_ERR     1 //ошибка приема пакета
#define CMD_ECHO    2 //передать эхо
#define CMD_INFO    3 //передать информацию об устройстве
#define CMD_SETADDR 4 //установить адрес
#define CMD_GETADDR 5 //прочитать адрес

//Коды ошибок:

#define ERR_NO 0x00   //no error
#define ERR_TX 0x01   //Rx/Tx error
#define ERR_BU 0x02   //device busy error
#define ERR_RE 0x03   //device not ready error
#define ERR_PA 0x04   //parameters value error
#define ERR_NR 0x05   //no replay
#define ERR_NC 0x06   //no carrier

//----------------------------------------------------------------------------

#endif
