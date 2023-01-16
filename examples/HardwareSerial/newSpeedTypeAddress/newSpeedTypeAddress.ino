// ПРИМЕР СМЕНЫ АДРЕСА, СКОРОСТИ И ТИПА ПРОТОКОЛА MODBUS:                                              //
// На шине должен быть только один TDS-метр.                                                           //
// Текщий адрес модуля, скорость шины и тип протокола Modbus будут определены автоматически.           //
                                                                                                       //
#include <iarduino_Modbus.h>                                                                           //   Подключаем библиотеку для работы по протоколу Modbus.
#include <iarduino_MB_TDS.h>                                                                           //   Подключаем библиотеку для работы с TDS-метрами.
                                                                                                       //
ModbusClient    modbus(Serial1, 2);                                                                    //   Создаём объект для работы по протоколу Modbus указывая объект шины UART-RS485 и вывод DE конвертера UART-RS485.
iarduino_MB_TDS sensor(modbus);                                                                        //   Создаём объект для работы с TDS-метром указывая объект протокола Modbus.
                                                                                                       //
uint32_t        newSpeed   = 9600;                                                                     //   Новая скорость ( 2400 / 4800 / 9600 / 19200 / 38400 / 57600 / 115200 ).
uint8_t         newAddress = 5;                                                                        //   Новый адрес ( 1 - 247 ).
uint8_t         newType    = MODBUS_RTU;                                                               //   Новый тип протокола Modbus ( MODBUS_RTU / MODBUS_ASCII ).
                                                                                                       //
void setup(){                                                                                          //
     Serial.begin(9600 ); while(!Serial);                                                              //   Инициируем передачу данных в монитор последовательного порта, указав его скорость.
     modbus.begin();                                                                                   //   Инициируем работу по протоколу Modbus.
     modbus.setTimeout(10);                                                                            //   Указываем максимальное время ожидания ответа по протоколу Modbus.
     modbus.setDelay(4);                                                                               //   Указываем минимальный интервал между отправляемыми сообщениями по протоколу Modbus.
//   Находим текущий адрес TDS-метра, скорость и тип протокола Modbus:                                 //
//   - если тип протокола Modbus известен, то его изменение можно удалить.                             //
//   - если скорость известна, то строки с другими скоростями можно удалить.                           //
             bool f=false;                                                                             //   Определяем флаг для получения результатов из функций.
             Serial.print("Поиск TDS-метра...");                                                       //
     if(!f){ modbus.setTypeMB( MODBUS_RTU );         delay(10);                                      } //   Указываем тип протокола Modbus RTU.
     if(!f){ Serial1.begin(  2400); while(!Serial1); delay(10); Serial.print('.'); f=sensor.begin(); } //   Инициируем работу с TDS-метром на скорости   2400, без указания адреса (адрес будет найден автоматически).
     if(!f){ Serial1.begin(  4800); while(!Serial1); delay(10); Serial.print('.'); f=sensor.begin(); } //   Инициируем работу с TDS-метром на скорости   4800, без указания адреса (адрес будет найден автоматически).
     if(!f){ Serial1.begin(  9600); while(!Serial1); delay(10); Serial.print('.'); f=sensor.begin(); } //   Инициируем работу с TDS-метром на скорости   9600, без указания адреса (адрес будет найден автоматически).
     if(!f){ Serial1.begin( 19200); while(!Serial1); delay(10); Serial.print('.'); f=sensor.begin(); } //   Инициируем работу с TDS-метром на скорости  19200, без указания адреса (адрес будет найден автоматически).
     if(!f){ Serial1.begin( 38400); while(!Serial1); delay(10); Serial.print('.'); f=sensor.begin(); } //   Инициируем работу с TDS-метром на скорости  38400, без указания адреса (адрес будет найден автоматически).
     if(!f){ Serial1.begin( 57600); while(!Serial1); delay(10); Serial.print('.'); f=sensor.begin(); } //   Инициируем работу с TDS-метром на скорости  57600, без указания адреса (адрес будет найден автоматически).
     if(!f){ Serial1.begin(115200); while(!Serial1); delay(10); Serial.print('.'); f=sensor.begin(); } //   Инициируем работу с TDS-метром на скорости 115200, без указания адреса (адрес будет найден автоматически).
     if(!f){ modbus.setTypeMB( MODBUS_ASCII );       delay(10);                                      } //   Указываем тип протокола Modbus ASCII.
     if(!f){ Serial1.begin(  2400); while(!Serial1); delay(10); Serial.print('.'); f=sensor.begin(); } //   Инициируем работу с TDS-метром на скорости   2400, без указания адреса (адрес будет найден автоматически).
     if(!f){ Serial1.begin(  4800); while(!Serial1); delay(10); Serial.print('.'); f=sensor.begin(); } //   Инициируем работу с TDS-метром на скорости   4800, без указания адреса (адрес будет найден автоматически).
     if(!f){ Serial1.begin(  9600); while(!Serial1); delay(10); Serial.print('.'); f=sensor.begin(); } //   Инициируем работу с TDS-метром на скорости   9600, без указания адреса (адрес будет найден автоматически).
     if(!f){ Serial1.begin( 19200); while(!Serial1); delay(10); Serial.print('.'); f=sensor.begin(); } //   Инициируем работу с TDS-метром на скорости  19200, без указания адреса (адрес будет найден автоматически).
     if(!f){ Serial1.begin( 38400); while(!Serial1); delay(10); Serial.print('.'); f=sensor.begin(); } //   Инициируем работу с TDS-метром на скорости  38400, без указания адреса (адрес будет найден автоматически).
     if(!f){ Serial1.begin( 57600); while(!Serial1); delay(10); Serial.print('.'); f=sensor.begin(); } //   Инициируем работу с TDS-метром на скорости  57600, без указания адреса (адрес будет найден автоматически).
     if(!f){ Serial1.begin(115200); while(!Serial1); delay(10); Serial.print('.'); f=sensor.begin(); } //   Инициируем работу с TDS-метром на скорости 115200, без указания адреса (адрес будет найден автоматически).
//   Выводим результат обнаружения TDS-метра:                                                          //
     if(!f){ Serial.println("\r\nTDS-метр не найден"); return; }                                       //
     else  { Serial.println("\r\nTDS-метр найден"   );         }                                       //
//   Меняем адрес TDS-метра:                                                                           //
     f =     sensor.changeID(newAddress);                                                              //
     if(!f){ Serial.println("Адрес не изменён"); return; }                                             //
//   Устанавливаем новую скорость передачи данных TDS-метра:                                           //
     f =     sensor.setSpeedMB(newSpeed);                                                              //   После установки новой скорости, её нужно подтвердить в течении 2 секунд.
     if(!f){ Serial.println("Скорость не изменена"); return; }                                         //
//   Меняем скорость шины:                                                                             //
             Serial1.begin(newSpeed); while(!Serial1); delay(10);                                      //
//   Подтверждаем изменение скорости передачи данных TDS-метра:                                        //
     f =     sensor.ackSpeedMB();                                                                      //   Если скорость не подтверждена за 2 секунды, модуль вернёт предыдущую скорость.
     if(!f){ Serial.println("Скорость не подтверждена"); return; }                                     //
//   Устанавливаем новый тип протокола TDS-метра:                                                      //
     f =     sensor.setTypeMB(newType);                                                                //   После установки нового типа протокола, его нужно подтвердить в течении 2 секунд.
     if(!f){ Serial.println("Тип протокола не изменён"); return; }                                     //
//   Меняем тип протокола на шине:                                                                     //
             modbus.setTypeMB(newType); delay(10);                                                     //
//   Подтверждаем изменение типа протокола TDS-метра:                                                  //
     f =     sensor.ackTypeMB();                                                                       //   Если тип протокола не подтверждён за 2 секунды, модуль вернёт предыдущий протокол.
     if(!f){ Serial.println("Тип протокола не подтверждён"); return; }                                 //
//   Выводим сообшение об успешном изменении адреса, скорости и типа протокола:                        //
             Serial.println(         "Адрес, скорость и тип протокола успешно изменены"  );            //
             Serial.println((String) "Скорость = "+newSpeed                              );            //   Выводим скорость передачи данных.
             Serial.println((String) "Modbus   = "+(newType==MODBUS_ASCII?"ASCII":"RTU") );            //   Выводим тип протокола Modbus.
             Serial.println((String) "Адрес    = "+sensor.getID()                        );            //   Выводим адрес TDS-метра.
             Serial.println((String) "Версия   = "+sensor.getVersion()                   );            //   Выводим версию прошивки TDS-метра.
}                                                                                                      //
                                                                                                       //
void loop(){                                                                                           //
//   Мигаем светодиодом обнаружения устройства (на разъёме):                                           //
     sensor.setIDLED(false); delay(1000);                                                              //
     sensor.setIDLED(true ); delay(1000);                                                              //
}                                                                                                      //
