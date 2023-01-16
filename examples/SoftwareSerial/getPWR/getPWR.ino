// ПРИМЕР ЧТЕНИЯ НАПРЯЖЕНИЯ ПИТАНИЯ 5В ПОСЛЕ DC-DC ПРЕОБРАЗОВАТЕЛЯ:                //
                                                                                   //
#include <SoftwareSerial.h>                                                        //   Подключаем библиотеку для работы с программной шиной UART.
#include <iarduino_Modbus.h>                                                       //   Подключаем библиотеку для работы по протоколу Modbus.
#include <iarduino_MB_TDS.h>                                                       //   Подключаем библиотеку для работы с TDS-метрами.
                                                                                   //
SoftwareSerial  rs485(8,9);                                                        //   Создаём объект для работы с программной шиной UART-RS485 указывая выводы RX, TX.
ModbusClient    modbus(rs485, 2);                                                  //   Создаём объект для работы по протоколу Modbus указывая объект программной шины UART-RS485 и вывод DE конвертера UART-RS485.
iarduino_MB_TDS sensor(modbus);                                                    //   Создаём объект для работы с TDS-метром указывая объект протокола Modbus.
                                                                                   //
void setup(){                                                                      //
     Serial.begin(9600); while(!Serial);                                           //   Инициируем передачу данных в монитор последовательного порта, указав его скорость.
     rs485.begin(9600); while(!rs485);                                             //   Инициируем работу с программной шиной UART-RS485 указав её скорость.
     modbus.begin();                                                               //   Инициируем работу по протоколу Modbus.
//   modbus.setTimeout(10);                                                        //   Указываем максимальное время ожидания ответа по протоколу Modbus.
//   modbus.setDelay(4);                                                           //   Указываем минимальный интервал между отправляемыми сообщениями по протоколу Modbus.
//   modbus.setTypeMB( MODBUS_RTU );                                               //   Указываем тип протокола Modbus: MODBUS_RTU (по умолчанию), или MODBUS_ASCII.
     sensor.begin(5);                                                              //   Инициируем работу с TDS-метром, указав его адрес. Если адрес не указан sensor.begin(), то он будет найден, но это займёт некоторое время.
//   sensor.setKt(0.02f);                                                          //   Указываем температурный коэффициент жидкости. Значение зависит от типа раствора. Используется для приведения S к EC. По умолчанию 0.02 = 2%.
//   sensor.setKp(0.5f);                                                           //   Указываем коэффициент пересчёта EC в TDS. Значение зависит от химического состава жидкости. Обычно применяют 0.5, 0.64 или 0.7. По умолчанию 0.5.
//   sensor.set_T(25);                                                             //   Указываем опорную температуру жидкости. Обычно используют 20°С или 25°С. По умолчанию 25°С.
}                                                                                  //
                                                                                   //
void loop(){                                                                       //
     float i = sensor.getPWR();                                                    //   Получаем напряжение питания 5В после DC-DC преобразователей.
     if( i<0 ){ Serial.println("Ошибка чтения"); }                                 //
     else     { Serial.println((String)"Напряжение питания "+i+" В."); }           //
     delay(1000);                                                                  //   Ждём 1 секунду.
}                                                                                  //