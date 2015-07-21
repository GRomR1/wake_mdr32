﻿WakeProtocol Libary: MDR32F9Q2 Port
Copyright (C) 2015 Gainanov Ruslan <ruslan.r.gainanov@gmail.com>
================
Реализация протокола WAKE для МК серии MDR32F9Q2I (1986ВЕ92У, К1986ВЕ92QI)
-------------------------------
Протокол Wake был создан специально для реализации связи между компьютером (или управляющим контроллером) и внешними микроконтроллерными устройствами.

Протокол WAKE является логическим уровнем интерфейса управления оборудованием с помощью асинхронного последовательного канала. 
Физический уровень интерфейса протоколом не определяется, может использоваться, например, RS-232, RS-485 или USB. 
Протокол позволяет производить обмен пакетами данных (data frames) длиной до 255 байт с адресуемыми устройствами, которых может быть до 127. 
Формат данных может быть любым. Могут передаваться байтовые поля, многобайтовые слова или строки символов. 
Для контроля правильности передачи данных используется контрольная сумма (CRC-8).
Подробная информация о WAKE: <a href='http://digit-el.com/files/open/wake/wake.html'>http://digit-el.com/files/open/wake/wake.html</a>

МК основан на ядре ARM Cortex-M3. Ближайший аналог: STM32F103x. Подробная информация о МК: <a href='http://milandr.ru/index.php?mact=Products,cntnt01,details,0&cntnt01productid=57&cntnt01returnid=68'>http://digit-el.com/files/open/wake/wake.html</a>

Программа создавалась в среде KeilVision v4.74. Тестировалась на отладочной плате MDR32F9Q2I Rev2.0.
Используется UART2 (PORTF0 - Rx, PORTF1 - Tx) с параметрами:
* скорость обмена - 9600 бод
* число бит в посылке - 8
* количество стоп-бит - 1
* бит четности - нет

Прерывания UART2 не используются (вся работа в цикле).