// ПРИМЕР ПОЛУЧЕНИЯ НАСТРОЕК МОДУЛЯ:                                               //
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
//   Выводим параметры хранящиеся в энергонезависимой памяти модуля:               //
     Serial.print(            "F    = " ); Serial.print( sensor.getFrequency() );  //   Выводим F     частоту переменного тока используемого для измерений (50...5000 Гц).
     Serial.print( "Гц.\r\n"  "TDS1 = " ); Serial.print( sensor.getKnownTDS(1) );  //   Выводим TDS-1 минерализацию 1 калибровочной жидкости (50...10'000 ppm).
     Serial.print( "ppm.\r\n" "TDS2 = " ); Serial.print( sensor.getKnownTDS(2) );  //   Выводим TDS-2 минерализацию 2 калибровочной жидкости (50...10'000 ppm).
     Serial.print( "ppm.\r\n" "Ka   = " ); Serial.print( sensor.getKa()   , 3  );  //   Выводим Ka    множитель степенной функции (0,001...4'294'967,295) определяется при калибровке.
     Serial.print( ".\r\n"    "Kb   = " ); Serial.print( sensor.getKb()   , 2  );  //   Выводим Kb    модуль степени степенной функции (0,001...65,535) определяется при калибровке.
     Serial.print( ".\r\n\r\n"          );                                         //
                                                                                   //
//   Выводим дополнительные параметры используемые в рассчётах:                    //
     Serial.print(            "Kt   = " ); Serial.print( sensor.getKt()   , 4  );  //   Выводим Kt    температурный коэффициент жидкости (0,0001...6,5535) зависит от типа раствора, используется для приведения S к EC.
     Serial.print( ".\r\n"    "Kp   = " ); Serial.print( sensor.getKp()   , 2  );  //   Выводим Kp    коэффициент пересчёта EC в TDS (0,01...655,35) зависит от химического состава жидкости, обычно применяют 0.5, 0.64 или 0.7
     Serial.print( ".\r\n"    "T    = " ); Serial.print( sensor.get_T()   , 2  );  //   Выводим T     опорную температуру жидкости (0,00...100,00), обычно используют 20°С или 25°С.
     Serial.print( "*С.\r\n\r\n"        );                                         //
                                                                                   //
//   Выводим текущие значения напряжений:                                          //
     Serial.print(            "Vsens= " ); Serial.print( sensor.getVsens(), 4  );  //   Выводим Vsens напряжение на входе датчика при положительной полярности = 0,5Vcc...0,6Vcc ≈ 1,65...1,98 В.
     Serial.print( "В.\r\n"   "Vo   = " ); Serial.print( sensor.getVo()   , 4  );  //   Выводим Vo    напряжение виртуального 0 на входе ОУ = 0,5Vcc ≈ 1,65В.
     Serial.print( "В.\r\n"   "Vout = " ); Serial.print( sensor.getVout() , 4  );  //   Выводим Vout  напряжение на выходе ОУ = -Vsens R + Vo ( 1 + R ), где R = 4700 / сопротивление между выводами щупа.
     Serial.print( "В.\r\n"             );                                         //
}                                                                                  //
                                                                                   //
void loop(){                                                                       //
}                                                                                  //
