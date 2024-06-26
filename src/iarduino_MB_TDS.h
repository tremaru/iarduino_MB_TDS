//	Библиотека для работы с TDS/EC-метром iarduino (датчиком минерализации жидкости) по шине Modbus:
//	Датчик минерализации жидкости (TDS/EC-метр) с дисплеем, RS485/Modbus: https://iarduino.ru/shop/Sensory-Datchiki/ec-tds-Modbus.html
//	Датчик минерализации жидкости (TDS/EC-метр) без дисплея, RS485/Modbus: https://iarduino.ru/shop/Sensory-Datchiki/ec-tds-Modbus-zumer.html
//  Версия: 1.1.5
//  Последнюю версию библиотеки Вы можете скачать по ссылке: https://iarduino.ru/file/600.html
//  Подробное описание функции бибилиотеки доступно по ссылке: https://wiki.iarduino.ru/page/ec-modbus/
//  Библиотека является собственностью интернет магазина iarduino.ru и может свободно использоваться и распространяться!
//  При публикации устройств или скетчей с использованием данной библиотеки, как целиком, так и её частей,
//  в том числе и в некоммерческих целях, просим Вас опубликовать ссылку: http://iarduino.ru
//  Автор библиотеки: Панькин Павел
//  Если у Вас возникли технические вопросы, напишите нам: shop@iarduino.ru
//	Дополнительно требуется установить библиотеку реализации протокола Modbus RTU/ASCII на шине UART->RS485: https://iarduino.ru/file/591.html

#ifndef iarduino_MB_TDS_h
#define iarduino_MB_TDS_h

#if defined(ARDUINO) && (ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <iarduino_Modbus.h>															//	Подключаем файл iarduino_Modbus.h - для работы по протоколу Modbus.
																						//
#define DEF_MODEL_TDS 8																	//	Идентификатор модели - константа (блок TDS/EC-метра с дисплеем).
#define DEF_MODEL_TDS_zummer 10															//	Идентификатор модели - константа (блок TDS/EC-метра с зуммером).
																						//
#define ERR_MB_TDS_Power	0b01000000													//	Флаг регистра диагностики указывающий на ожидание стабилизации после подачи питания.
#define ERR_MB_TDS_ADC		0b00100000													//	Флаг регистра диагностики указывающий на указывает на то, что напряжения считаны при обратной полярности меандра.
#define ERR_MB_TDS_Vout		0b00011000													//	Флаг регистра диагностики указывающий на некорректное напряжения на выходе ОУ (0>TDS>10'000).
#define ERR_MB_TDS_Vo		0b00000110													//	Флаг регистра диагностики указывающий на некорректное напряжение виртуального ноля (Vo != 0.5Vcc ± 0.05).
#define ERR_MB_TDS_5V		0b00000001													//	Флаг регистра диагностики указывающий на низкое напряжение питания 5В после DC-DC преобразователя < 4В. Стабильная работа не гарантируется.
#define MB_TDS_Ro			0															//	Параметр функции print()  указывающий выводить на дисплей общее измеренное сопротивление.
#define MB_TDS_S			1															//	Параметр функции print()  указывающий выводить на дисплей измеренную удельную электропроводность.
#define MB_TDS_EC			2															//	Параметр функции print()  указывающий выводить на дисплей приведённую удельную электропроводность.
#define MB_TDS_TDS			3															//	Параметр функции print()  указывающий выводить на дисплей количество растворённых твёрдых веществ (по умолчанию).
#define MB_TDS_5V			4															//	Параметр функции print()  указывающий выводить на дисплей измеренное напряжения питания на шине +5V.
#define MB_TDS_Vo			5															//	Параметр функции print()  указывающий выводить на дисплей напряжение виртуального 0 на входе ОУ.
#define MB_TDS_Vout			6															//	Параметр функции print()  указывающий выводить на дисплей напряжение на выходе ОУ.
#define MB_TDS_Vsen			7															//	Параметр функции print()  указывающий выводить на дисплей напряжение на входе датчика.
#define STABLE_S			6															//	Первый параметр функций getFluctuation() и setFluctuation().
#define UNSTABLE_S			7															//	Первый параметр функций getFluctuation() и setFluctuation().
																						//
class iarduino_MB_TDS{																	//
	public:																				//
	/**	Конструктор класса **/															//
		iarduino_MB_TDS					(ModbusClient&				);					//	Объявляем конструктор класса.											Параметры: ссылка на объект работы по протоколу Modbus.
	/**	Пользовательские функции **/													//
		bool			begin			(uint8_t id=0				);					//	Объявляем  функцию инициализации модуля.								Параметры функции: ID-адрес на шине. Функция возвращает результат инициализации: true-успех / false-провал.
		bool			reset			(void						);					//	Объявляем  функцию перезагрузки модуля.									Параметры функции: нет. Функция возвращает результат перезагрузки: true-успех / false-провал.
		bool			changeID		(uint8_t new_id				);					//	Объявляем  функцию смены ID-адреса на шине (выполняется >120мс).		Параметры функции: новый ID. Функция возвращает результат смены ID: true-успех / false-провал.
		bool			setSpeedMB		(uint32_t speed				);					//	Объявляем  функцию смены скорости передачи данных.						Параметры функции: новая скорость. Функция возвращает результат смены скорости: true-успех / false-провал. Скорость необходимо подтвердить в течении 2 секунд.
		bool			ackSpeedMB		(void						);					//	Объявляем  функцию подтверждения скорости (выполняется >50мс).			Параметры функции: нет. Функция возвращает результат подтверждения скорости: true-успех / false-провал. 
		bool			setTypeMB		(uint8_t type				);					//	Объявляем  функцию смены типа протокола Modbus.							Параметры функции: MODBUS_RTU/MODBUS_ASCII. Функция возвращает результат смены типа протокола Modbus: true-успех / false-провал. Тип необходимо подтвердить в течении 2 секунд.
		bool			ackTypeMB		(void						);					//	Объявляем  функцию подтверждения типа протокола (выполняется >50мс).	Параметры функции: нет. Функция возвращает результат подтверждения типа протокола Modbus: true-успех / false-провал. 
		bool			writeFlash		(uint16_t data				);					//	Объявляем  функцию записи пользовательского значения в Flash память.	Параметры функции: целое беззнаковое число от 0 до 65535. Функция возвращает результат записи: true-успех / false-провал. 
		int32_t			readFlash		(void						);					//	Объявляем  функцию чтения пользовательского значения из Flash памяти.	Параметры функции: нет. Функция возвращает прочитанное значение, или -1 при провале чтения.
		uint8_t			getID			(void						){ return valID;  }	//	Определяем функцию получения текущего ID (адрес модуля на шине).		Параметры функции: нет. Функция возвращает текущий ID-адрес, или 0 если модуль не определён.
		uint8_t			getVersion		(void						){ return valVers;}	//	Определяем функцию получения текущей версии прошивки модуля.			Параметры функции: нет. Функция возвращает версию прошивки, или 0 если модуль не определён.
		float			getPWR			(void						);					//	Объявляем  функцию получения напряжения питания +5V.					Параметры функции: нет. Функция возвращает напряжение питания в Вольтах, или -1 при провале чтения.
		bool			setIDLED		(bool on 					);					//	Объявляем  функцию смены состояния светодиода обнаружения устройства.	Параметры функции: состояние светодиода true/false. Функция возвращает результат изменения состояния: true-успех / false-провал.
		int32_t			getErr			(void						);					//	Объявляем  функцию получения флагов ошибок из регистра диагностики.		Параметры функции: нет. Функция возвращает значение регистра диагностики, каждый из 16 бит которого является флагом ошибки, или -1 при провале чтения.
		bool			setCalibration	(void						);					//	Объявляем  функцию запуска калибровки.									Параметры функции: нет. Функция возвращает результат запуска калибровки: true-успех / false-провал.
		int8_t			getCalibration	(void						);					//	Объявляем  функцию получения текущей стадии калибровки.					Параметры функции: нет. Функция возвращает стадию калибровки: 1 или 2, 0-не выполняется, 3-ожидание смены жидкости, или -1 при провале чтения.
		bool			setStopCalib	(void						);					//	Объявляем  функцию отмены калибровки.									Параметры функции: нет. Функция возвращает результат отмены калибровки: true-успех / false-провал.
		int8_t			getResultCalib	(void						);					//	Объявляем  функцию получения результата последней калибровки.			Параметры функции: нет. Функция возвращает флаг результат калибровки: true-успех / false-провал, или -1 при провале чтения.
		int8_t			getStability	(void						);					//	Объявляем  функцию получения флага нормализации показаний.				Параметры функции: нет. Функция возвращает флаг стабильности показаний проводимости: true-стабильны / false-меняются, или -1 при провале чтения.
		float			getFluctuation	(uint8_t type				);					//	Объявляем  функцию получения границы гистерезиса флуктуаций в %.		Параметры функции: тип границы STABLE_S/UNSTABLE_S. По этим границам определяется флаг стабильности показаний проводимости.
		bool			setFluctuation	(uint8_t type,  float limit	);					//	Объявляем  функцию установки границы гистерезиса флуктуаций в %.		Параметры функции: тип границы STABLE_S/UNSTABLE_S, значение границы в %. По этим границам определяется флаг стабильности показаний проводимости.
		int32_t			getFrequency	(void						);					//	Объявляем  функцию получения частоты переменного тока для измерений.	Параметры функции: нет.
		bool			setFrequency	(uint16_t f					);					//	Объявляем  функцию установки частоты переменного тока для измерений.	Параметры функции: частота от 50 до 5000Гц.
		int32_t			getKnownTDS		(uint8_t stage				);					//	Объявляем  функцию получения TDS калибровочных жидкостей.				Параметры функции: стадия 1 или 2. Функция возвращает TDS калибровочной жидкости, или -1 при провале чтения.
		bool			setKnownTDS		(uint8_t stage, uint16_t tds);					//	Объявляем  функцию установки TDS калибровочных жидкостей.				Параметры функции: стадия 1 или 2, TDS от 50 до 10'000ppm. Функция возвращает результат записи калибровочной жидкости: true-успех / false-провал.
		float			getKnownEC		(uint8_t stage				);					//	Объявляем  функцию получения EC  калибровочных жидкостей.				Параметры функции: стадия 1 или 2. Функция возвращает EC калибровочной жидкости, или -1 при провале чтения.
		bool			setKnownEC		(uint8_t stage, float ec	);					//	Объявляем  функцию установки EC  калибровочных жидкостей.				Параметры функции: стадия 1 или 2, EC от 0,01 до 20,00 мСм/мс. Функция возвращает результат записи калибровочной жидкости: true-успех / false-провал.
		float			getKa			(void						);					//	Объявляем  функцию получения множителя степенной функции.				Параметры функции: нет. Функция возвращает множитель степенной функции, или -1 при провале чтения.
		bool			setKa			(float Ka					);					//	Объявляем  функцию установки множителя степенной функции.				Параметры функции: множитель степенной функции от 0,001 до 4'294'967,295. Функция возвращает результат записи значения: true-успех / false-провал.
		float			getKb			(void						);					//	Объявляем  функцию получения степени   степенной функции.				Параметры функции: нет. Функция возвращает модуль степени степенной функции, или -1 при провале чтения.
		bool			setKb			(float Kb					);					//	Объявляем  функцию установки степени   степенной функции.				Параметры функции: модуль степени степенной функции от 0,001 до 65,535. Функция возвращает результат записи значения: true-успех / false-провал.
		float			getKt			(void						);					//	Объявляем  функцию получения температурного коэффициента жидкости.		Параметры функции: нет. Функция возвращает температурный коэффициент жидкости, или -1 при провале чтения.
		bool			setKt			(float Kt					);					//	Объявляем  функцию установки температурного коэффициента жидкости.		Параметры функции: температурный коэффициент от 0,0001 до 6,5535. Функция возвращает результат записи значения: true-успех / false-провал.
		float			getKp			(void						);					//	Объявляем  функцию получения коэффициента пересчёта.					Параметры функции: нет. Функция возвращает коэффициент пересчёта, или -1 при провале чтения.
		bool			setKp			(float Kp					);					//	Объявляем  функцию установки коэффициента пересчёта.					Параметры функции: коэффициент пересчёта от 0,01 до 655,35. Функция возвращает результат записи значения: true-успех / false-провал.
		bool			set_t			(float t					);					//	Объявляем  функцию установки реальной   температуры жидкости.			Параметры функции: температура от 0,00 до 100,00 °С. Функция возвращает результат записи значения: true-успех / false-провал.
		float			get_T			(void						);					//	Объявляем  функцию получения опорной    температуры жидкости.			Параметры функции: нет. Функция возвращает опорную температуру жидкости в °С, или -1 при провале чтения.
		bool			set_T			(float t					);					//	Объявляем  функцию установки опорной    температуры жидкости.			Параметры функции: температура от 0,00 до 100,00 °С. Функция возвращает результат записи значения: true-успех / false-провал.
		float			getVsens		(void						);					//	Объявляем  функцию получения напряжения на входе датчика.				Параметры функции: нет. Функция возвращает напряжение в В, или -1 при провале чтения.
		float			getVo			(void						);					//	Объявляем  функцию получения напряжения виртуального 0 на входе ОУ.		Параметры функции: нет. Функция возвращает напряжение в В, или -1 при провале чтения.
		float			getVout			(void						);					//	Объявляем  функцию получения напряжения на выходе ОУ.					Параметры функции: нет. Функция возвращает напряжение в В, или -1 при провале чтения.
		int32_t			getRo			(void						);					//	Объявляем  функцию получения общего измеренного сопротивления.			Параметры функции: нет. Функция возвращает общее измеренное сопротивление в Ом, или -1 при провале чтения.
		float			get_S			(void						);					//	Объявляем  функцию получения измеренной  удельной электропроводности.	Параметры функции: нет. Функция возвращает измеренную удельную электропроводность жидкости в мСм/см, или -1 при провале чтения.
		float			getEC			(void						);					//	Объявляем  функцию получения приведённой удельной электропроводности.	Параметры функции: нет. Функция возвращает приведённую удельную электропроводность жидкости в мСм/см, или -1 при провале чтения.
		int32_t			getTDS			(void						);					//	Объявляем  функцию получения количества растворённых твёрдых веществ.	Параметры функции: нет. Функция возвращает количество растворённых твёрдых веществ жидкости в ppm, или -1 при провале чтения.
		bool			print			(uint8_t type				);					//	Объявляем  функцию выбора данных выводимых на дисплей.					Параметры функции: тип выводимых данных: MB_TDS_Ro/S/EC/TDS/5V/Vo/Vout/Vsen. Функция возвращает результат записи значения: true-успех / false-провал.
	private:																			//	
	/**	Внутренние переменные **/														//
		ModbusClient*	objModbus;														//	Объявляем  указатель на объект работы по протоколу Modbus.				Указатель получит адрес объекта в конструкторе класса.
		uint8_t			valID			= 0;											//	Определяем переменную для хранения ID-адреса который был проверен.
		uint8_t			valVers			= 0;											//	Определяем переменную для хранения версии прошивки.
		uint8_t			maxRW			= 5;											//	Определяем максимальное количество попыток чтения/записи.
	/**	Внутренние функции **/															//
		int8_t			MB_readDO		(uint16_t reg							);		//	Дублер функции objModbus.coilRead();             Чтение одного регистра DO, из  модуля valID, maxRW попыток. Параметры функции: адрес регистра. Функция возвращает прочитанное значение (0/1), или -1 при неудаче.
		int8_t			MB_readDI		(uint16_t reg							);		//	Дублер функции objModbus.discreteInputRead();    Чтение одного регистра DI, из  модуля valID, maxRW попыток. Параметры функции: адрес регистра. Функция возвращает прочитанное значение (0/1), или -1 при неудаче.
		int32_t			MB_readAO		(uint16_t reg							);		//	Дублер функции objModbus.holdingRegisterRead();  Чтение одного регистра AO, из  модуля valID, maxRW попыток. Параметры функции: адрес регистра. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
		int32_t			MB_readAI		(uint16_t reg							);		//	Дублер функции objModbus.inputRegisterRead();    Чтение одного регистра AI, из  модуля valID, maxRW попыток. Параметры функции: адрес регистра. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
		uint16_t		MB_requestFrom	(uint8_t  type, uint16_t reg, uint16_t n);		//	Дублер функции objModbus.requestFrom();          Чтение нескольк регистров, из  модуля valID, maxRW попыток. Параметры функции: тип регистра (COILS/DISCRETE_INPUTS/HOLDING_REGISTERS/INPUT_REGISTERS), адрес первого регистра, количество регистров. Функция возвращает количество прочитанных значений, или 0 при неудаче.
		uint8_t			MB_getInfo		(uint8_t  id							);		//	Дублер функции objModbus.getInfo();              Чтение информации о устройстве модуля id   , maxRW попыток. Параметры функции: id модуля. Функция возвращает количество байт данных об устройстве, доступных для чтения функцией read() и available().
		int32_t			MB_diagnostic	(uint16_t func, uint16_t data=0			);		//	Дублер функции objModbus.diagnostic();           Выполнение команды диагностики модуля valID, maxRW попыток. Параметры функции: номер функции диагностики, данные. Функция возвращает данные результата выполнения функции диагностики, или -1 при неудаче.
		bool			MB_changeID		(uint8_t  newID							);		//	Дублер функции objModbus.changeID();             Сохранение нового ID     , для модуля valID, maxRW попыток. Параметры функции: новый id модуля. Функция возвращает результат сохранения адреса (0/1).
		bool			MB_writeDO		(uint16_t reg, bool val					);		//	Дублер функции objModbus.coilWrite();            Запись в один регистр  DO, для модуля valID, maxRW попыток. Параметры функции: адрес регистра, значение (0/1). Функция возвращает 1 при успехе, 0 при неудаче.
		bool			MB_writeAO		(uint16_t reg, uint16_t val				);		//	Дублер функции objModbus.holdingRegisterWrite(); Запись в один регистр  AO, для модуля valID, maxRW попыток. Параметры функции: адрес регистра, значение (0...65535). Функция возвращает 1 при успехе, 0 при неудаче.
};

#endif
