// ПРИМЕР ПОЛУЧЕНИЯ ПРОВОДИМОСТИ ЖИДКОСТИ И КОЛИЧЕСТВА ТВЕРДЫХ РАСТВОРИМЫХ ВЕЩЕСТВ://
                                                                                   //
#include <iarduino_Modbus.h>                                                       //   Подключаем библиотеку для работы по протоколу Modbus.
#include <iarduino_MB_TDS.h>                                                       //   Подключаем библиотеку для работы с TDS-метрами.
                                                                                   //
ModbusClient    modbus(Serial1, 2);                                                //   Создаём объект для работы по протоколу Modbus указывая объект шины UART-RS485 и вывод DE конвертера UART-RS485.
iarduino_MB_TDS sensor(modbus);                                                    //   Создаём объект для работы с TDS-метром указывая объект протокола Modbus.
                                                                                   //
void setup(){                                                                      //
     Serial.begin(9600); while(!Serial);                                           //   Инициируем передачу данных в монитор последовательного порта, указав его скорость.
     Serial1.begin(9600); while(!Serial1);                                         //   Инициируем работу с шиной UART-RS485 указав её скорость.
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
//   Передаём данные модулю:                                                       //
     sensor.set_t(25.0);                                                           //   Указываем модулю температуру жидкости в которую погружен щуп.
     delay(1000);                                                                  //
//   Получаем данные от модуля:                                                    //
     int32_t tds = sensor.getTDS();                                                //   Получаем количество растворённых твёрдых веществ в жидкости.
     float   ec  = sensor.getEC();                                                 //   Получаем приведённую удельную электропроводность жидкости.
     float   s   = sensor.get_S();                                                 //   Получаем измеренную  удельную электропроводность жидкости.
     int32_t Ro  = sensor.getRo();                                                 //   Получаем общее измеренное сопротивление между выводами датчика.
//   Информируем о отсутствии ответа:                                              //
     if( tds<0 || ec<0 || s<0 || Ro<0 ){                                           //   Если не получен любой из параметров, то ...
         Serial.println( F("Нет ответа от TDS-метра.") ); return;                  //   Сообщаем о ошибке чтения.
     }                                                                             //
//   Выводим результат:                                                            //
     Serial.print( (String) "TDS " + tds + " ppm, "    );                          //   Выводим количество растворённых твёрдых веществ в жидкости (минерализацию).
     Serial.print( (String) "EC "  + ec  + " мСм/см, " );                          //   Выводим приведённую удельную электропроводность жидкости.
     Serial.print( (String) "S "   + s   + " мСм/см, " );                          //   Выводим измеренную  удельную электропроводность жидкости.
     Serial.print( (String) "Ro "  + Ro  + " Ом.\r\n"  );                          //   Выводим общее измеренное сопротивление между выводами датчика.
}                                                                                  //
