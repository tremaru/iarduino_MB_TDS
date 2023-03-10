// ПРИМЕР СБРОСА ВСЕХ НАСТРОЕК В ЗАВОДСКИЕ:                                        //
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
                                                                                   //
//   Все указанные параметры будут сохранены в энергонезависимую память модуля:    //
     bool f[5];                                                                    //   Создаём массив из 5 флагов.
     f[0] = sensor.setFrequency(   2000);                                          //   Частота переменного тока    (50...5'000 Гц   ) снижает эффект поляризации при измерениях. По умолчанию 2кГц.
     f[1] = sensor.setKnownTDS (1,  500);                                          //   Минерализация жидкости      (50...10'000 ppm ) для 1 стадии калибровки модуля.            По умолчанию 500 ppm.
     f[2] = sensor.setKnownTDS (2, 1500);                                          //   Минерализация жидкости      (50...10'000 ppm ) для 2 стадии калибровки модуля.            По умолчанию 1500 ppm.
     f[3] = sensor.setKa       ( 500.0f);                                          //   Множителя степенной функции (0,001...4294967 ) используется для вычисления проводимости.  По умолчанию 500,000.
     f[4] = sensor.setKb       (   5.0f);                                          //   Модуль степени функцииь     (0,001...65,535  ) используется для вычисления проводимости.  По умолчанию   5,000.
                                                                                   //
//   Выводим результат записи настроек:                                            //
     if(f[0]&f[1]&f[2]&f[3]&f[4]==true){Serial.println("Все настройки записаны");} //
     if( f[0]==false ){ Serial.println("Не записана частота переменного тока"  );} //
     if( f[1]==false ){ Serial.println("Не записана минерализация 1"           );} //
     if( f[2]==false ){ Serial.println("Не записана минерализация 2"           );} //
     if( f[3]==false ){ Serial.println("Не записан  множитель функции"         );} //
     if( f[4]==false ){ Serial.println("Не записана степень   функции"         );} //
}                                                                                  //
                                                                                   //
void loop(){                                                                       //
}                                                                                  //
