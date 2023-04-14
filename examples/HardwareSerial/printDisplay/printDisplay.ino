// ПРИМЕР ВЫБОРА ДАННЫХ ВЫВОДИМЫХ НА ДИСПЛЕЙ TDS-МЕТРА:                            //
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
     Serial.println(F("Для выбора выводимых на дисплей данных введите цифру:"));   //
     Serial.println(F("1 - Выводить общее измеренное сопротивление в Ом."));       //
     Serial.println(F("2 - Выводить удельную электропроводность в мСм/см."));      //
     Serial.println(F("3 - Выводить приведённую электропроводность в мСм/см."));   //
     Serial.println(F("4 - Выводить кол-во растворённых твёрдых веществ в ppm.")); //
     Serial.println(F("5 - Выводить напряжение питания на шине +5V в В."));        //
     Serial.println(F("6 - Выводить напряжение виртуального 0 на входе ОУ в В.")); //
     Serial.println(F("7 - Выводить напряжение на выходе ОУ в В."));               //
     Serial.println(F("8 - Выводить напряжение на входе датчика в В."));           //
}                                                                                  //
                                                                                   //
void loop(){                                                                       //
//   Ждём поступление данных из монитора последовательного порта:                  //
     if( Serial.available() ){                                                     //
     //  Преобразуем символ от '1' до '9' в цифру от 1 до 9:                       //
         uint8_t i=0;                                                              //
         while( Serial.available() ){                                              //
             char j=Serial.read(); if( j>'0' && j<'9' ){ i=j-'0'; } delay(10);     //
         }                                                                         //
     //  Меняем тип данных выводимых на дисплей:                                   //
         switch(i){                                                                //
             case 1: sensor.print(MB_TDS_Ro);                                      //   Указываем модулю выводить на дисплей общее измеренное сопротивление между выводами щупа.
                     Serial.print(F("1) Общее измеренное сопротивление "));        //   Сопротивление выводится в Ом целым числом.
                     Serial.print(F("'Ro' в Ом. \r\n"));                           //
                     break;                                                        //
             case 2: sensor.print(MB_TDS_S);                                       //   Указываем модулю выводить на дисплей измеренную удельную электропроводность.
                     Serial.print(F("2) Удельная электропроводность "));           //   Электропроводность выводится в мСм/см с двумя знаками после запятой.
                     Serial.print(F("'S' в мСм/см. \r\n"));                        //
                     break;                                                        //
             case 3: sensor.print(MB_TDS_EC);                                      //   Указываем модулю выводить на дисплей приведённую удельную электропроводность (по умолчанию).
                     Serial.print(F("3) Приведённая электропроводность "));        //   Электропроводность выводится в мСм/см с двумя знаками после запятой.
                     Serial.print(F("'EC' в мСм/см. \r\n"));                       //
                     break;                                                        //
             case 4: sensor.print(MB_TDS_TDS);                                     //   Указываем модулю выводить на дисплей общее количество растворённых твёрдых веществ.
                     Serial.print(F("4) Кол-во растворённых твёрдых веществ "));   //   Количество выводится в ppm (pro pro mille) целым числом.
                     Serial.print(F("'TDS' в ppm (мг/л воды). \r\n"));             //
                     break;                                                        //
             case 5: sensor.print(MB_TDS_5V);                                      //   Указываем модулю выводить на дисплей измеренное напряжения питания на шине +5V.
                     Serial.print(F("5) Напряжение питания на шине +5V "));        //   Напряжение выводится в В с тремя знаками после запятой.
                     Serial.print(F("'5V' в В. \r\n"));                            //
                     break;                                                        //
             case 6: sensor.print(MB_TDS_Vo);                                      //   Указываем модулю выводить на дисплей напряжение виртуального 0 на входе ОУ.
                     Serial.print(F("6) Напряжение виртуального 0 на входе ОУ ")); //   Напряжение выводится в В с тремя знаками после запятой.
                     Serial.print(F("'Vo' в В. \r\n"));                            //
                     break;                                                        //
             case 7: sensor.print(MB_TDS_Vout);                                    //   Указываем модулю выводить на дисплей напряжение на выходе ОУ.
                     Serial.print(F("7) Напряжение на выходе ОУ"));                //   Напряжение выводится в В с тремя знаками после запятой.
                     Serial.print(F("'Vout' в В. \r\n"));                          //
                     break;                                                        //
             case 8: sensor.print(MB_TDS_Vsen);                                    //   Указываем модулю выводить на дисплей напряжение на входе датчика при положительной полярности.
                     Serial.print(F("8) Напряжение на входе датчика"));            //   Напряжение выводится в В с тремя знаками после запятой.
                     Serial.print(F("'Vsen' в В. \r\n"));                          //
                     break;                                                        //
             default:Serial.print(F("Введите цифру от 1 до 8 включительно.\r\n")); //
         }                                                                         //
     }                                                                             //
//   Передаём данные модулю (один раз в секунду):                                  //
     if( millis()%1000==0 ){ sensor.set_t(25.0); }                                 //   Указываем модулю температуру жидкости в которую погружен щуп.
}                                                                                  //
