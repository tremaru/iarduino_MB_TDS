#include "iarduino_MB_TDS.h"		//	╔═══════════════════════╗
									//	║ РЕГИСТР ДИАГНОСТИКИ   ║
									//	╠═══════════╤═══════════╣
									//	║ ERR_Power │ -6------  ║ Флаг ожидания стабилизации после подачи питания.
									//	║ ERR_ADC   │ --5-----  ║ Флаг указывает на то, что напряжения считаны при обратной полярности меандра.
									//	║ ERR_Vout  │ ---4----  ║ Флаг первышения напряжения на выходе ОУ (TDS > 10'000 ).
									//	║ ERR_Vout  │ ----3---  ║ Флаг занижения  напряжения на выходе ОУ (TDS < 0).
									//	║ ERR_Vo    │ -----2--  ║ Флаг первышения напряжения виртуального ноля (Vo > 0.5Vcc + 0.05).
									//	║ ERR_Vo    │ ------1-  ║ Флаг занижения  напряжения виртуального ноля (Vo < 0.5Vcc - 0.05).
									//	║ ERR_5V    │ -------0  ║ Флаг занижения  напряжения питания 5В после DC-DC преобразователя < 4В. Стабильная работа не гарантируется.
									//	╚═══════════╧═══════════╝
									//	╔═══════════════╦═══════╗
									//	║ РЕГИСТРЫ «DO» ║ Биты: ║
									//	╠═══════════════╬═══════╣
#define DO_HL_DATA		0			//	║ DO_HL_DATA    ║   0   ║ Включение светодиода обнаружения устройства.
#define DO_CALC			1			//	║ DO_CALC       ║   0   ║ Выполнить калибровку.
									//	╟───────────────╫───────╢
//						0x0100		//	║ DO_CHANGE_ID  ║   0   ║ Флаг устанавливается самостоятельно после подтверждения смены адреса на шине.
//						0x0101		//	║ DO_BAN_ID 1   ║   0   ║ Бит  запрещает изменять или устанавливать адрес 1.      Только для режима смены адреса на случайное симло.
//						...			//	║ DO_BAN_ID ... ║   0   ║ Биты запрещают изменять или устанавливать адреса 2-246. Только для режима смены адреса на случайное симло.
//						0x01F7		//	║ DO_BAN_ID 247 ║   0   ║ Бит  запрещает изменять или устанавливать адрес 147.    Только для режима смены адреса на случайное симло.
									//	╚═══════════════╩═══════╝
									//	╔═══════════════╦═══════╗
									//	║ РЕГИСТРЫ «DI» ║ Биты: ║
									//	╠═══════════════╬═══════╣
#define DI_CALC_ERR		0			//	║ DI_CALC_ERR   ║   0   ║ Флаг ошибки калибровки (0-нет, 1-ошибка). Регистр сбрасывается при нажатии на кнопку калибровки или установке DO_CALC.
#define DI_STATUS_1		1			//	║ DI_STATUS_1   ║   0   ║ Флаг выполнения 1 стадии калибровки (0-нет, 1-выполняется).
#define DI_STAY			2			//	║ DI_STAY       ║   0   ║ Флаг ожидания смены калибровочной жидкости (0-нет, 1-ожидается).
#define DI_STATUS_2		3			//	║ DI_STATUS_2   ║   0   ║ Флаг выполнения 2 стадии калибровки (0-нет, 1-выполняется).
#define DI_STABLE_S		4			//	║ DI_STABLE_S   ║   0   ║ Флаг нормализации показаний проводимости.
									//	╚═══════════════╩═══════╝
									//	╔═══════════════╦═══════╗
									//	║ РЕГИСТРЫ «AO» ║Данные:║
									//	╠═══════════════╬═══════╣
#define AO_FREQUENCY	0			//	║ AO_FREQUENCY  ║ 2000  ║ Частота смены полярности датчика 50-5000 Гц.
									//	╟───────────────╫───────╢
#define AO_KNOWN_TDS_1	1			//	║ AO_KNOWN_TDS_1║ 500   ║ Известное TDS от 50 до 10000 ppm, для 1 стадии калибровки.
#define AO_KNOWN_TDS_2	2			//	║ AO_KNOWN_TDS_2║ 1500  ║ Известное TDS от 50 до 10000 ppm, для 2 стадии калибровки.
									//	╟───────────────╫───────╢
#define AO_Ka_L			3			//	║ AO_Ka_L       ║500.000║ Множитель степенной функции (младшее слово) в тысячных долях.
#define AO_Ka_H			4			//	║ AO_Ka_H       ║500.000║ Множитель степенной функции (старшее слово) в тысячных долях.
#define AO_Kb			5			//	║ AO_Kb         ║  5.000║ Модуль степени степенной функции в тысячных долях.
#define AO_STABLE_S		6			//	║ AO_STABLE_S   ║  0.5  ║ Процент допустимой флуктуаций проводимости (в десятых долях) при котором устанавливается флаг нормализации DI_STABLE_S используемый при калибровке.
#define AO_UNSTABLE_S	7			//	║ AO_UNSTABLE_S ║  1.0  ║ Процент высокой    флуктуаций проводимости (в десятых долях) при котором сбрасывается    флаг нормализации DI_STABLE_S используемый при калибровке.
									//	╟───────────────╫───────╢
#define AO_CALC_SAVE	8			//	║ AO_CALC_SAVE  ║   0   ║ Код разрешения записи калибровочных значений в Flash память. Сбрасывается автоматически. Должен быть установлен в значение (0x2709) при запуске калибровки и до записи калибровочных значений напрямую через регистры (без калибровки).
									//	╟───────────────╫───────╢
#define AO_Kt			9			//	║ AO_Kt         ║ 0.0200║ Температурный коэффициент жидкости в десятитысячных долях.
#define AO_Kp			10			//	║ AO_Kp         ║  0.50 ║ Коэффициент пересчёта EC (мкСм/см) в TDS (ppm) в сотых долях.
#define AO_t			11			//	║ AO_t          ║ 25.00 ║ Температура жидкости (текущая) в сотых долях °С. Требуется обязательно указывать t перед установкой битов калибровки и чтением показаний.
#define AO_T			12			//	║ AO_T          ║ 25.00 ║ Опорная температура в сотых долях °С (обычно T = 20 или 25). Температура к которой требуется привести значение S для получения EC и TDS.
									//	╟───────────────╫───────╢
#define AO_DISPLAY		13			//	║ AO_DISPLAY    ║   0   ║ Тип данных на дисплее: 0-Ro, 1-S, 2-EC, 3-TDS, 4-5V, 5-Vo, 6-Vout, 7-Vsen.
									//	╟───────────────╫───────╢
//						0x0100		//	║ AO_ACK_ID     ║   0   ║ Запись значения 0xF0A5 подтверждает смену адреса на шине. При чтении всегда возвращает 0.
#define AO_ACK_SPEED	0x0101		//	║ AO_ACK_SPEED  ║   0   ║ Запись значения 0xF0A5 подтверждает смену скорости  шины. При чтении всегда возвращает 0.
#define AO_ACK_TYPE		0x0102		//	║ AO_ACK_TYPE   ║   0   ║ Запись значения 0xF0A5 подтверждает смену типа протокола. При чтении всегда возвращает 0.
//						0x0110		//	║ AO_SET_ID     ║  xxID ║ Запись приводит к смене адреса на шине, xx=00 до отключения питания, xx=FF с сохранеием в ПЗУ. Требуется подтверждение. При чтении xx=случайное число.
#define AO_SET_SPEED	0x0111		//	║ AO_SET_SPEED  ║  xxSP ║ Запись приводит к смене скорости  шины, xx=00 до отключения питания, xx=FF с сохранеием в ПЗУ. Требуется подтверждение. При чтении xx=случайное число.
#define AO_SET_TYPE		0x0112		//	║ AO_SET_TYPE   ║  xxTP ║ Запись приводит к смене типа протокола, xx=00 до отключения питания, xx=FF с сохранеием в ПЗУ. Требуется подтверждение. При чтении xx=случайное число.
#define AO_USER_DATA	0x0120		//	║ AO_USER_DATA  ║   0   ║ Пользовательское число хранится в Flash памяти модуля.
									//	╚═══════════════╩═══════╝
									//	╔═══════════════╦═══════╗
									//	║ РЕГИСТРЫ «AI» ║Данные:║
									//	╠═══════════════╬═══════╣
#define AI_5V			0			//	║ AI_5V         ║   0   ║ Напряжение питания 5В после DC-DC преобразователя в тысячных долях вольт ( 0...65,535В ).
									//	╟───────────────╫───────╢
#define AI_V0			1			//	║ AI_V0         ║   0   ║ Напряжение виртуального 0 на входе ОУ в десятитысячных долях В. ( 0...6,5535В ).
#define AI_VOUT			2			//	║ AI_VOUT       ║   0   ║ Напряжение на выходе усилителя в десятитысячных долях В. ( 0...6,5535В ).
#define AI_VSEN			3			//	║ AI_VSEN       ║   0   ║ Напряжение на входе щупа в десятитысячных долях В. ( 0...6,5535В ).
									//	╟───────────────╫───────╢
#define AI_Ro			4			//	║ AI_Ro         ║   0   ║ Общее измеренное сопротивление в даОм (1 дека Ом = 10 Ом)
#define AI_S			5			//	║ AI_S          ║   0   ║ Удельная электропроводность в мкСм/см.
#define AI_EC			6			//	║ AI_EC         ║   0   ║ Удельная электропроводность в мкСм/см для T°С.
#define AI_TDS			7			//	║ AI_TDS        ║   0   ║ Количество растворенных в воде примесей в ppm.
									//	╚═══════════════╩═══════╝
																															//
//		КОНСТРУКТОР КЛАССА:																									//
		iarduino_MB_TDS::iarduino_MB_TDS(ModbusClient &obj){objModbus = &obj;}												//	&obj - ссылка на объект для работы по протоколу Modbus.
																															//
//		ФУНКЦИЯ ИНИЦИАЛИЗАЦИИ МОДУЛЯ:																						//	Возвращает результат инициализации: true-успех / false-провал.
bool	iarduino_MB_TDS::begin(uint8_t id){																					//	id - адрес модуля на шине.
			objModbus->codeError=ERROR_GATEWAY_NO_DEVICE;																	//	Ошибкой выполнения данной функции может быть только отсутствие устройства.
		//	Самостоятельный поиск id устройства:																			//
			if( id==0 )								{ if( objModbus->findID(DEF_MODEL_TDS)        ){id=objModbus->read();}}	//	Если адрес не указан, ищем адрес первого устройства с совпавшим идентификатором.
			if( id==0 )								{ if( objModbus->findID(DEF_MODEL_TDS_zummer) ){id=objModbus->read();}}	//	Если адрес не указан, ищем адрес первого устройства с совпавшим идентификатором.
		//	Проверяем устройство:																							//
			if( id==0 )								{ return false; }														//	Адрес устройства не указан и не найден.
			if( objModbus->getInfo(id)<15 )			{ return false; }														//	Устройство не найдено, или информация об устройстве неполная.
			if( objModbus->read()!=0x77 )			{ return false; }														//	Идентификатор линейки устройств не соответствует устройствам iArduino.
				objModbus->read();																							//	Индикатор пуска не проверяем (00=OFF, FF=ON).
			if( objModbus->read()!=id )				{ return false; }														//	Адрес полученный из информации об устройстве не совпадает с фактическим адресом устройства.
			uint8_t model = objModbus->read();																				//	Получаем идентификатор модели устройства.
			if( model!=DEF_MODEL_TDS && model!=DEF_MODEL_TDS_zummer ){ return false; }										//	Идентификатор устройства не совпадает с DEF_MODEL_TDS и DEF_MODEL_TDS_zummer.
		//	Устройство прошло проверку:																						//
			valID=id;																										//	Сохраняем адрес устройства.
			valVers=objModbus->read();																						//	Сохраняем версию прошивки устройства.
			return true;																									//
}																															//
																															//
//		ФУНКЦИЯ ПЕРЕЗАГРУЗКИ МОДУЛЯ:																						//	Возвращает результат перезагрузки: true-успех / false-провал.
bool	iarduino_MB_TDS::reset(void){																						//	
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			return ( objModbus->diagnostic(valID,1,0xFF00)<0? false:true );													//	Выполняем команду диагностики, функция 0x0001 с очисткой журнала событий 0xFF00.
}																															//
																															//
//		ФУНКЦИЯ СМЕНЫ ID-АДРЕСА НА ШИНЕ:																					//	Возвращает результат смены ID: true-успех / false-провал.
bool	iarduino_MB_TDS::changeID(uint8_t id){																				//	id - новый адрес модуля на шине (1-247).
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( !objModbus->changeID(valID, id) ){ return false; }															//	Меняем адрес устройства с valID на id.
			valID=id; return true;																							//	Сохраняем навый адрес и возвращаем флаг успеха.
}																															//
																															//
//		ФУНКЦИЯ СМЕНЫ СКОРОСТИ ПЕРЕДАЧИ ДАННЫХ:																				//	Возвращает результат смены скорости: true-успех / false-провал. Скорость необходимо подтвердить в течении 2 секунд.
bool	iarduino_MB_TDS::setSpeedMB(uint32_t s){																			//	s - скорость передачи данных (2400/4800/9600/19200/38400/57600/115200)
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( s!=2400 && s!=4800 && s!=9600 && s!=19200 && s!=38400 && s!=57600 && s!=115200 ){ return false; }			//	Проверяем значение скорости.
			return objModbus->holdingRegisterWrite(valID, AO_SET_SPEED, 0xFF00|(s/2400));									//	Записываем значение 0xFFXX в регистр "Holding Register" AO[AO_SET_SPEED]. Функция возвращает 1 при успехе, 0 при неудаче.
}																															//
																															//
//		ФУНКЦИЯ ПОДТВЕРЖДЕНИЯ СКОРОСТИ ПЕРЕДАЧИ ДАННЫХ:																		//	Возвращает результат подтверждения скорости: true-успех / false-провал. 
bool	iarduino_MB_TDS::ackSpeedMB(void){																					//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( !objModbus->holdingRegisterWrite(valID, AO_ACK_SPEED, 0xF0A5) ){ return false; }							//	Записываем значение 0xF0A5 в регистр "Holding Register" AO[AO_ACK_SPEED]. Функция возвращает 1 при успехе, 0 при неудаче.
			delay(50);																										//	Ждём 50 мс, пока модуль не сохранит новую скорость в flash память.
			return true;																									//	Возвращаем флаг успеха.
}																															//
																															//
//		ФУНКЦИЯ СМЕНЫ ТИПА ПРОТОКОЛА MODBUS:																				//	Возвращает результат смены типа протокола Modbus: true-успех / false-провал. Тип необходимо подтвердить в течении 2 секунд.
bool	iarduino_MB_TDS::setTypeMB(uint8_t type){																			//	type - тип протокола Modbus (MODBUS_RTU/MODBUS_ASCII).
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( type!=MODBUS_RTU && type!=MODBUS_ASCII ){ return false; }													//	Проверяем значение типа протокола Modbus.
			return objModbus->holdingRegisterWrite(valID, AO_SET_TYPE, (type==MODBUS_ASCII?0xFFFF:0xFF00));					//	Записываем значение 0xFFXX в регистр "Holding Register" AO[AO_SET_TYPE]. Функция возвращает 1 при успехе, 0 при неудаче.
}																															//
																															//
//		ФУНКЦИЯ ПОДТВЕРЖДЕНИЯ ТИПА ПРОТОКОЛА MODBUS:																		//	Возвращает результат подтверждения типа протокола Modbus: true-успех / false-провал. 
bool	iarduino_MB_TDS::ackTypeMB(void){																					//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( !objModbus->holdingRegisterWrite(valID, AO_ACK_TYPE, 0xF0A5) ){ return false; }								//	Записываем значение 0xF0A5 в регистр "Holding Register" AO[AO_ACK_TYPE]. Функция возвращает 1 при успехе, 0 при неудаче.
			delay(50);																										//	Ждём 50 мс, пока модуль не сохранит новый тип протокола Modbus в flash память.
			return true;																									//	Возвращаем флаг успеха.
}																															//
																															//
//		ФУНКЦИЯ ЗАПИСИ ПОЛЬЗОВАТЕЛЬСКОГО ЗНАЧЕНИЯ В FLASH ПАМЯТЬ МОДУЛЯ:													//	Возвращает результат записи: true-успех / false-провал.
bool	iarduino_MB_TDS::writeFlash(uint16_t data){																			//	data - целое беззнаковое число от 0 до 65535.
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( !objModbus->holdingRegisterWrite(valID, AO_USER_DATA, data) ){ return false; }								//	Записываем значение data в регистр "Holding Register" AO[AO_USER_DATA]. Функция возвращает 1 при успехе, 0 при неудаче.
			delay(50);																										//	Ждём 50 мс, пока модуль не сохранит записанное значение в flash память.
			return true;																									//	Возвращаем флаг успеха.
}																															//
																															//
//		ФУНКЦИЯ ЧТЕНИЯ ПОЛЬЗОВАТЕЛЬСКОГО ЗНАЧЕНИЯ ИЗ FLASH ПАМЯТИ:															//	Возвращает прочитанное значение, или -1 при провале чтения.
int32_t	iarduino_MB_TDS::readFlash(void){																					//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1; }													//	Ошибка синтаксиса, модуль не инициализирован.
			return objModbus->holdingRegisterRead(valID, AO_USER_DATA);														//	Читаем значение из регистра "Holding Register" AO[AO_USER_DATA]. Функция возвращает значение, или -1 при неудаче.
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ НАПРЯЖЕНИЯ ПИТАНИЯ 5В ПОСЛЕ DC-DC ПРЕОБРАЗОВАТЕЛЯ:												//	Возвращает напряжение питания в Вольтах, или -1 при провале чтения.
float	iarduino_MB_TDS::getPWR(void){																						//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1.0f; }												//	Ошибка синтаксиса, модуль не инициализирован.
			int32_t Vcc = objModbus->inputRegisterRead(valID, AI_5V);														//	Читаем напряжение из регистра "Input Register" AI[AI_5V]. Функция возвращает значение, или -1 при неудаче.
			if( Vcc<0 ){ return -1.0f; }else{ return ((float)Vcc)/1000.0f; }												//	Возвращаем -1 (ошибка), или напряжение в Вольтах.
}																															//
																															//
//		ФУНКЦИЯ ИЗМЕНЕНИЯ СОСТОЯНИЯ СВЕТОДИОДА ОБНАРУЖЕНИЯ УСТРОЙСТВА:														//	Возвращает результат изменения состояния светодиода: true-успех / false-провал.
bool	iarduino_MB_TDS::setIDLED(bool f){																					//	f - состояние светодиода (true/false)
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			return objModbus->coilWrite(valID, DO_HL_DATA, f);																//	Записываем f в регистр "Coil" DO[DO_HL_DATA].
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ ФЛАГОВ ОШИБОК ИЗ РЕГИСТРА ДИАГНОСТИКИ:															//	Возвращает значение регистра диагностики, каждый из 16 бит которого является флагом ошибки, или -1 при провале чтения.
int32_t	iarduino_MB_TDS::getErr(void){																						//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1; }													//	Ошибка синтаксиса, модуль не инициализирован.
			return objModbus->diagnostic(valID, 2);																			//	Выполняем команду диагностики, номер функции диагностики 0x0002 = получить регистр диагностики. Функция возвращает данные результата выполнения функции диагностики, или -1 при неудаче.
}																															//
																															//
//		ФУНКЦИЯ ЗАПУСКА КАЛИБРОВКИ:																							//	Возвращает результат запуска калибровки: true-успех / false-провал.
bool	iarduino_MB_TDS::setCalibration(void){																				//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( !objModbus->holdingRegisterWrite(valID, AO_CALC_SAVE, 0x2709) ){ return false; }							//	Записываем значение 0x2709 в регистр "Holding Register" AO[AO_CALC_SAVE]. Функция возвращает 1 при успехе, 0 при неудаче.
			if( !objModbus->coilWrite           (valID, DO_CALC     , true  ) ){ return false; }							//	Записываем единицу в регистр "Coil" DO[DO_CALC].
			return true;																									//
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ ТЕКУЩЕЙ СТАДИИ КАЛИБРОВКИ:																		//	Возвращает стадию калибровки: 1 или 2, 0-не выполняется, 3-ожидание смены жидкости, или -1 при провале чтения.
int8_t	iarduino_MB_TDS::getCalibration(void){																				//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1; }													//	Ошибка синтаксиса, модуль не инициализирован.
			if( objModbus->requestFrom(valID, DISCRETE_INPUTS, DI_STATUS_1, 3) != 3 ){ return -1; }							//	Читаем регистры "Discrete Inputs" начиная с регистра DI[DI_STATUS_1], всего 3 регистра. Функция возвращает количество прочитанных значений, или 0 при неудаче.
			if( objModbus->read() ){ return 1; }																			//	Если установлен регистр "Discrete Inputs" DI[DI_STATUS_1], значит выполняется 1 стадия калибровки.
			if( objModbus->read() ){ return 3; }																			//	Если установлен регистр "Discrete Inputs" DI[DI_STAY    ], значит ожидания смена калибровочной жидкости.
			if( objModbus->read() ){ return 2; }																			//	Если установлен регистр "Discrete Inputs" DI[DI_STATUS_2], значит выполняется 2 стадия калибровки.
			return 0;																										//	Если ни один из перечисленных регистров не установлен    , значит калибровка не выполняется.
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ РЕЗУЛЬТАТА ПОСЛЕДНЕЙ КАЛИБРОВКИ:																	//	Возвращает флаг результат калибровки: true-успех / false-провал, или -1 при провале чтения.
int8_t	iarduino_MB_TDS::getResultCalib(void){																				//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1; }													//	Ошибка синтаксиса, модуль не инициализирован.
			int8_t i = objModbus->discreteInputRead(valID, DI_CALC_ERR);													//	Читаем регистр ошибки калибровки "Discrete Inputs" DI[DI_CALC_ERR]. Функция возвращает прочитанное значение (0/1), или -1 при неудаче.
			if( i==0 ){ i=1; }else if( i==1 ){ i=0; } return i;																//	Меняем флаг ошибки на флаг результата (0=1, 1=0) и возвращаем его.
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ ФЛАГА НОРМАЛИЗАЦИИ ПОКАЗАНИЙ:																		//	Возвращает флаг стабильности проводимости: true-стабильны / false-меняются, или -1 при провале чтения.
int8_t	iarduino_MB_TDS::getStability(void){																				//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1; }													//	Ошибка синтаксиса, модуль не инициализирован.
			return objModbus->discreteInputRead(valID, DI_STABLE_S);														//	Возвращаем значение регистра "Discrete Inputs" DI[DI_STABLE_S]. Функция возвращает прочитанное значение (0/1), или -1 при неудаче.
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ ГРАНИЦЫ ГИСТЕРЕЗИСА ФЛУКТУАЦИЙ В ПРОЦЕНТАХ:														//	Возвращает границу флуктуаций в % по которой определяется флаг стабильности показаний проводимости, или -1 при провале чтения.
float	iarduino_MB_TDS::getFluctuation(uint8_t type){																		//	type - тип границы STABLE_S/UNSTABLE_S.
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1.0f; }												//	Ошибка синтаксиса, модуль не инициализирован.
			float i= -1.0f;																									//
			if( type==STABLE_S   ){ i=(float)objModbus->holdingRegisterRead(valID, AO_STABLE_S  ); }						//	Читаем регистр "Holding Register" AO[AO_STABLE_S  ]. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
			if( type==UNSTABLE_S ){ i=(float)objModbus->holdingRegisterRead(valID, AO_UNSTABLE_S); }						//	Читаем регистр "Holding Register" AO[AO_UNSTABLE_S]. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
			if( i>=0 ){ i/=10.0f; } return i;																				//	Возврашаем границу флуктуаций проводимости в десятых долях %.
}																															//
																															//
//		ФУНКЦИЯ УСТАНОВКИ ГРАНИЦЫ ГИСТЕРЕЗИСА ФЛУКТУАЦИЙ В ПРОЦЕНТАХ:														//	Возвращает результат записи границы флуктуаций: true-успех / false-провал.
bool	iarduino_MB_TDS::setFluctuation(uint8_t type, float limit){															//	type - тип границы STABLE_S/UNSTABLE_S, limit - процент флуктуации проводимости за пределами которого будет меняться флаг стабильности показаний проводимости.
			bool i;																											//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( type!=STABLE_S && type!=UNSTABLE_S ){ return false; }														//	Возвращаем флаг ошибки синтаксиса.
			if( limit<0.1f || limit>100.0f ){ return false; }																//	Возвращаем флаг ошибки синтаксиса.
			if( !objModbus->holdingRegisterWrite(valID, AO_CALC_SAVE, 0x2709) ){ return false; }							//	Записываем значение 0x2709 в регистр "Holding Register" AO[AO_CALC_SAVE]. Функция возвращает 1 при успехе, 0 при неудаче.
			if( type==STABLE_S   ){ i = objModbus->holdingRegisterWrite(valID, AO_STABLE_S  , (uint16_t)(limit*10.0f) ); }	//	Записываем в регистр "Holding Register" AO[AO_STABLE_S  ] процент отклонения проводимости в десятых долях. Функция возвращает 1 при успехе, 0 при неудаче.
			if( type==UNSTABLE_S ){ i = objModbus->holdingRegisterWrite(valID, AO_UNSTABLE_S, (uint16_t)(limit*10.0f) ); }	//	Записываем в регистр "Holding Register" AO[AO_UNSTABLE_S] процент отклонения проводимости в десятых долях. Функция возвращает 1 при успехе, 0 при неудаче.
			delay(50);																										//	Ждём 50 мс, пока модуль не сохранит новые данные в flash память.
			return i;																										//
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ ЧАСТОТЫ ПЕРЕМЕННОГО ТОКА ДЛЯ ИЗМЕРЕНИЙ:															//	Возвращает частоту в Гц, или -1 при провале чтения.
int32_t	iarduino_MB_TDS::getFrequency(void){																				//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1; }													//	Ошибка синтаксиса, модуль не инициализирован.
			return objModbus->holdingRegisterRead(valID, AO_FREQUENCY);														//	Читаем частоту из регистра "Holding Register" AO[AO_FREQUENCY]. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
}																															//
																															//
//		ФУНКЦИЯ УСТАНОВКИ ЧАСТОТЫ ПЕРЕМЕННОГО ТОКА ДЛЯ ИЗМЕРЕНИЙ:															//	Возвращает результат записи: true-успех / false-провал.
bool	iarduino_MB_TDS::setFrequency(uint16_t f){																			//	f - частота от 50 до 5000Гц.
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( !objModbus->holdingRegisterWrite(valID, AO_CALC_SAVE, 0x2709) ){ return false; }							//	Записываем значение 0x2709 в регистр "Holding Register" AO[AO_CALC_SAVE]. Функция возвращает 1 при успехе, 0 при неудаче.
			if( f<50 || f>5000  ){ return false; }																			//	Возвращаем флаг ошибки синтаксиса.
			bool i = objModbus->holdingRegisterWrite(valID, AO_FREQUENCY, f);												//	Записываем в регистр "Holding Register" AO[AO_FREQUENCY] частоту в Гц. Функция возвращает 1 при успехе, 0 при неудаче.
			delay(50);																										//	Ждём 50 мс, пока модуль не сохранит новые данные в flash память.
			return i;																										//
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ TDS КАЛИБРОВОЧНЫХ ЖИДКОСТЕЙ:																		//	Возвращает TDS калибровочной жидкости, или -1 при провале чтения.
int32_t	iarduino_MB_TDS::getKnownTDS(uint8_t stage){																		//	stage - стадия 1 или 2.
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1; }													//	Ошибка синтаксиса, модуль не инициализирован.
			int32_t i=-1;																									//
			if( stage==1 ){ i=objModbus->holdingRegisterRead(valID, AO_KNOWN_TDS_1); }										//	Читаем регистр "Holding Register" AO[AO_KNOWN_TDS_1]. Функция возвращает прочитанное значение (0...65'535), или -1 при неудаче.
			if( stage==2 ){ i=objModbus->holdingRegisterRead(valID, AO_KNOWN_TDS_2); }										//	Читаем регистр "Holding Register" AO[AO_KNOWN_TDS_2]. Функция возвращает прочитанное значение (0...65'535), или -1 при неудаче.
			return i;																										//	Возврашаем количество растворенных примесей в калибровочной жидкости в ppm.
}																															//
																															//
//		ФУНКЦИЯ УСТАНОВКИ TDS КАЛИБРОВОЧНЫХ ЖИДКОСТЕЙ:																		//	Возвращает результат записи: true-успех / false-провал.
bool	iarduino_MB_TDS::setKnownTDS(uint8_t stage, uint16_t tds){															//	stage - стадия 1 или 2, tds - количество растворенных примесей от 50 до 10'000.
			bool i;																											//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( stage!=1 && stage!=2 ){ return false; }																		//	Возвращаем флаг ошибки синтаксиса.
			if( tds<50 || tds>10000  ){ return false; }																		//	Возвращаем флаг ошибки синтаксиса.
			if( !objModbus->holdingRegisterWrite(valID, AO_CALC_SAVE, 0x2709) ){ return false; }							//	Записываем значение 0x2709 в регистр "Holding Register" AO[AO_CALC_SAVE]. Функция возвращает 1 при успехе, 0 при неудаче.
			if( stage==1 ){ i = objModbus->holdingRegisterWrite(valID, AO_KNOWN_TDS_1, tds ); }								//	Записываем в регистр "Holding Register" AO[AO_KNOWN_TDS_1] количество растворенных примесей в ppm. Функция возвращает 1 при успехе, 0 при неудаче.
			if( stage==2 ){ i = objModbus->holdingRegisterWrite(valID, AO_KNOWN_TDS_2, tds ); }								//	Записываем в регистр "Holding Register" AO[AO_KNOWN_TDS_2] количество растворенных примесей в ppm. Функция возвращает 1 при успехе, 0 при неудаче.
			delay(50);																										//	Ждём 50 мс, пока модуль не сохранит новые данные в flash память.
			return i;																										//
}																															//
																															//
//		ФУНКЦИИ ДУБЛЁРЫ КАЛИБРОВОЧНЫХ ЖИДКОСТЕЙ (УКАЗЫВАЕМ EC в мСм/см, а не TDS в ppm):									//
float	iarduino_MB_TDS::getKnownEC(uint8_t stage			){ float Kp=getKp(); if(Kp<0){return -1.0f;}	return				(float)getKnownTDS(stage) / Kp / 1000.0f;					}
bool	iarduino_MB_TDS::setKnownEC(uint8_t stage, float ec	){ float Kp=getKp(); if(Kp<0){return false;}	return				       setKnownTDS(stage, (uint16_t)(Kp * ec * 1000.0f) );	}
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ МНОЖИТЕЛЯ СТЕПЕННОЙ ФУНКЦИИ:																		//	Возвращает  множитель степенной функции, или -1 при провале чтения.
float	iarduino_MB_TDS::getKa(void){																						//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1.0f; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( objModbus->requestFrom(valID, HOLDING_REGISTERS, AO_Ka_L, 2) != 2 ){ return -1; }							//	Читаем регистры "Holding Registers" начиная с регистра DI[AO_Ka_L], всего 2 регистра. Функция возвращает количество прочитанных значений, или 0 при неудаче.
			uint32_t i = objModbus->read(); i |= (objModbus->read()<<16);													//	Сохраняем значения прочитанных регистров в переменную i.
			return (float)i/1000.0f;																						//	Возврашаем значение полученное в тысячных долях.
}																															//
																															//
//		ФУНКЦИЯ УСТАНОВКИ МНОЖИТЕЛЯ СТЕПЕННОЙ ФУНКЦИИ:																		//	Возвращает результат записи: true-успех / false-провал.
bool	iarduino_MB_TDS::setKa(float Ka){																					//	Ka - множитель степенной функции от 0,001 до 4'294'967,295.
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( Ka<0.001f || Ka>1000000.0f ){ return false; }																//	Возвращаем флаг ошибки синтаксиса.
			if( !objModbus->holdingRegisterWrite(valID, AO_CALC_SAVE, 0x2709) ){ return false; }							//	Записываем значение 0x2709 в регистр "Holding Register" AO[AO_CALC_SAVE]. Функция возвращает 1 при успехе, 0 при неудаче.
			uint32_t i = (uint32_t)(Ka*1000.0f);																			//
			if( !objModbus->beginTransmission(valID, HOLDING_REGISTERS, AO_Ka_L, 2) ){ return false; }						//	Инициируем запись в регистры "Holding Registers" начиная с DI[AO_Ka_L], всего 2 регистра. Функция возвращает 1 при успехе, 0 при неудаче.
			if( !objModbus->write( (uint16_t)(i&0x0000FFFFLL) ) ){ return false; }											//	Ставим в очередь на запись значение для регистра DI[AO_Ka_L].
			if( !objModbus->write( (uint16_t)(i>>16)          ) ){ return false; }											//	Ставим в очередь на запись значение для регистра DI[AO_Ka_H].
			bool j = objModbus->endTransmission();																			//	Выполняем инициированную ранее запись. Функция возвращает 1 при успехе, 0 при неудаче.
			delay(50);																										//	Ждём 50 мс, пока модуль не сохранит новые данные в flash память.
			return j;																										//
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ МОДУЛЯ СТЕПЕНИ СТЕПЕННОЙ ФУНКЦИИ:																	//	Возвращает модуль степени степенной функции, или -1 при провале чтения.
float	iarduino_MB_TDS::getKb(void){																						//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1.0f; }												//	Ошибка синтаксиса, модуль не инициализирован.
			float i=(float)objModbus->holdingRegisterRead(valID, AO_Kb);													//	Читаем модуль степени степенной функции из регистра "Holding Register" AO[AO_Kb]. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
			if( i>=0 ){ i/=1000.0f; } return i;																				//	Возврашаем модуль степени полученный в тысячных долях.
}																															//
																															//
//		ФУНКЦИЯ УСТАНОВКИ СТЕПЕНИ СТЕПЕННОЙ ФУНКЦИИ:																		//	Возвращает результат записи: true-успех / false-провал.
bool	iarduino_MB_TDS::setKb(float Kb){																					//	Kb - модуль степени степенной функции от 0,001 до 65,535.
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( !objModbus->holdingRegisterWrite(valID, AO_CALC_SAVE, 0x2709) ){ return false; }							//	Записываем значение 0x2709 в регистр "Holding Register" AO[AO_CALC_SAVE]. Функция возвращает 1 при успехе, 0 при неудаче.
			if( Kb<0.001f || Kb>65.535f ){ return false; }																	//	Возвращаем флаг ошибки синтаксиса.
			bool i = objModbus->holdingRegisterWrite(valID, AO_Kb, (uint16_t)(Kb*1000.0f) );								//	Записываем в регистр "Holding Register" AO[AO_Kb] модуль степени в тысячных долях. Функция возвращает 1 при успехе, 0 при неудаче.
			delay(50);																										//	Ждём 50 мс, пока модуль не сохранит новые данные в flash память.
			return i;																										//
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ ТЕМПЕРАТУРНОГО КОЭФФИЦИЕНТА ЖИДКОСТИ:																//	Возвращает температурный коэффициент жидкости, или -1 при провале чтения.
float	iarduino_MB_TDS::getKt(void){																						//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1.0f; }												//	Ошибка синтаксиса, модуль не инициализирован.
			float i=(float)objModbus->holdingRegisterRead(valID, AO_Kt);													//	Читаем температурный коэффициент из регистра "Holding Register" AO[AO_Kt]. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
			if( i>=0 ){ i/=10000.0f; } return i;																			//	Возврашаем температурный коэффициент полученный в десятитысячных долях.
}																															//
																															//
//		ФУНКЦИЯ УСТАНОВКИ ТЕМПЕРАТУРНОГО КОЭФФИЦИЕНТА ЖИДКОСТИ:																//	Возвращает результат записи: true-успех / false-провал.
bool	iarduino_MB_TDS::setKt(float Kt){																					//	Kt - температурный коэффициент от 0,0001 до 6,5535.
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( Kt<0.0001f || Kt>6.5535f ){ return false; }																	//	Возвращаем флаг ошибки синтаксиса.
			return objModbus->holdingRegisterWrite(valID, AO_Kt, (uint16_t)(Kt*10000.0) );									//	Записываем в регистр "Holding Register" AO[AO_Kt] температурный коэффициент в десятитысячных долях. Функция возвращает 1 при успехе, 0 при неудаче.
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ КОЭФФИЦИЕНТА ПЕРЕСЧЁТА:																			//	Возвращает коэффициент пересчёта, или -1 при провале чтения.
float	iarduino_MB_TDS::getKp(void){																						//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1.0f; }												//	Ошибка синтаксиса, модуль не инициализирован.
			float i=(float)objModbus->holdingRegisterRead(valID, AO_Kp);													//	Читаем коэффициент пересчёта из регистра "Holding Register" AO[AO_Kp]. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
			if( i>=0 ){ i/=100.0f; } return i;																				//	Возврашаем коэффициент пересчёта полученный в сотых долях.
}																															//
																															//
//		ФУНКЦИЯ УСТАНОВКИ КОЭФФИЦИЕНТА ПЕРЕСЧЁТА:																			//	Возвращает результат записи: true-успех / false-провал.
bool	iarduino_MB_TDS::setKp(float Kp){																					//	Kp - коэффициент пересчёта от 0,01 до 655,35.
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( Kp<0.01f || Kp>655.35f ){ return false; }																	//	Возвращаем флаг ошибки синтаксиса.
			return objModbus->holdingRegisterWrite(valID, AO_Kp, (uint16_t)(Kp*100.0) );									//	Записываем в регистр "Holding Register" AO[AO_Kp] коэффициент пересчёта в сотых долях. Функция возвращает 1 при успехе, 0 при неудаче.
}																															//
																															//
//		ФУНКЦИЯ УСТАНОВКИ РЕАЛЬНОЙ ТЕМПЕРАТУРЫ ЖИДКОСТИ:																	//	Возвращает результат записи: true-успех / false-провал.
bool	iarduino_MB_TDS::set_t(float t){																					//	t - температура от 0,00 до 100,00 °С.
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( t<0.00f || t>100.0f ){ return false; }																		//	Возвращаем флаг ошибки синтаксиса.
			return objModbus->holdingRegisterWrite(valID, AO_t, (uint16_t)(t*100.0) );										//	Записываем в регистр "Holding Register" AO[AO_t] температуру в сотых долях. Функция возвращает 1 при успехе, 0 при неудаче.
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ ОПОРНОЙ ТЕМПЕРАТУРЫ ЖИДКОСТИ:																		//	Возвращает опорную температуру жидкости в °С, или -1 при провале чтения.
float	iarduino_MB_TDS::get_T(void){																						//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1.0f; }												//	Ошибка синтаксиса, модуль не инициализирован.
			float i=(float)objModbus->holdingRegisterRead(valID, AO_T);														//	Читаем опорную температуру из регистра "Holding Register" AO[AO_T]. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
			if( i>=0 ){ i/=100.0f; } return i;																				//	Возврашаем опорную температуру полученную в сотых долях.
}																															//
																															//
//		ФУНКЦИЯ УСТАНОВКИ ОПОРНОЙ ТЕМПЕРАТУРЫ ЖИДКОСТИ:																		//	Возвращает результат записи: true-успех / false-провал.
bool	iarduino_MB_TDS::set_T(float t){																					//	t - температура от 0,00 до 100,00 °С.
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( t<0.00f || t>100.0f ){ return false; }																		//	Возвращаем флаг ошибки синтаксиса.
			return objModbus->holdingRegisterWrite(valID, AO_T, (uint16_t)(t*100.0) );										//	Записываем в регистр "Holding Register" AO[AO_T] опорную температуру в сотых долях. Функция возвращает 1 при успехе, 0 при неудаче.
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ НАПРЯЖЕНИЯ НА ВХОДЕ ДАТЧИКА:																		//	Возвращает напряжение в В, или -1 при провале чтения.
float	iarduino_MB_TDS::getVsens(void){																					//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1.0f; }												//	Ошибка синтаксиса, модуль не инициализирован.
			float i=(float)objModbus->inputRegisterRead(valID, AI_VSEN);													//	Читаем напряжение на входе датчика из регистра "Input Register" AI[AI_VSEN]. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
			if( i>=0 ){ i/=10000.0f; } return i;																			//	Возврашаем напряжение на входе датчика полученное в десятитысячных долях В.
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ НАПРЯЖЕНИЯ ВИРТУАЛЬНОГО 0 НА ВХОДЕ ОУ:															//	Возвращает напряжение в В, или -1 при провале чтения.
float	iarduino_MB_TDS::getVo(void){																						//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1.0f; }												//	Ошибка синтаксиса, модуль не инициализирован.
			float i=(float)objModbus->inputRegisterRead(valID, AI_V0);														//	Читаем напряжение виртуального ноля на входе ОУ из регистра "Input Register" AI[AI_V0]. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
			if( i>=0 ){ i/=10000.0f; } return i;																			//	Возврашаем напряжение виртуального ноля на входе ОУ полученное в десятитысячных долях В.
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ НАПРЯЖЕНИЯ НА ВЫХОДЕ ОУ:																			//	Возвращает напряжение в В, или -1 при провале чтения.
float	iarduino_MB_TDS::getVout(void){																						//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1.0f; }												//	Ошибка синтаксиса, модуль не инициализирован.
			float i=(float)objModbus->inputRegisterRead(valID, AI_VOUT);													//	Читаем напряжение на выходе ОУ из регистра "Input Register" AI[AI_VOUT]. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
			if( i>=0 ){ i/=10000.0f; } return i;																			//	Возврашаем напряжение на выходе ОУ полученное в десятитысячных долях В.
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ ОБЩЕГО ИЗМЕРЕННОГО СОПРОТИВЛЕНИЯ:																	//	Возвращает общее измеренное сопротивление в Ом, или -1 при провале чтения.
int32_t	iarduino_MB_TDS::getRo(void){																						//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1; }													//	Ошибка синтаксиса, модуль не инициализирован.
			int32_t i = objModbus->inputRegisterRead(valID, AI_Ro);															//	Читаем общее измеренное сопротивление из регистра "Input Register" AI[AI_Ro]. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
			if( i>=0 ){ i*=10; } return i;																					//	Возврашаем общее измеренное сопротивление полученное в даОм (1 дека Ом = 10 Ом).
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ ИЗМЕРЕННОЙ УДЕЛЬНОЙ ЭЛЕКТРОПРОВОДНОСТИ:															//	Возвращает измеренную удельную электропроводность жидкости в мСм/см, или -1 при провале чтения.
float	iarduino_MB_TDS::get_S(void){																						//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1; }													//	Ошибка синтаксиса, модуль не инициализирован.
			float i=(float)objModbus->inputRegisterRead(valID, AI_S);														//	Читаем измеренную удельную электропроводность из регистра "Input Register" AI[AI_S]. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
			if( i>=0 ){ i/=1000.0f; } return i;																				//	Возврашаем измеренную удельную электропроводность в мСм/см.
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ ПРИВЕДЁННОЙ УДЕЛЬНОЙ ЭЛЕКТРОПРОВОДНОСТИ:															//	Возвращает приведённую удельную электропроводность жидкости в мСм/см, или -1 при провале чтения.
float	iarduino_MB_TDS::getEC(void){																						//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1; }													//	Ошибка синтаксиса, модуль не инициализирован.
			float i=(float)objModbus->inputRegisterRead(valID, AI_EC);														//	Читаем приведённую удельную электропроводность из регистра "Input Register" AI[AI_EC]. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
			if( i>=0 ){ i/=1000.0f; } return i;																				//	Возврашаем приведённую удельную электропроводность в мСм/см.
}																															//
																															//
//		ФУНКЦИЯ ПОЛУЧЕНИЯ КОЛИЧЕСТВА РАСТВОРЁННЫХ ТВЁРДЫХ ВЕЩЕСТВ:															//	Возвращает количество растворённых твёрдых веществ жидкости в ppm, или -1 при провале чтения.
int32_t	iarduino_MB_TDS::getTDS(void){																						//
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return -1; }													//	Ошибка синтаксиса, модуль не инициализирован.
			return objModbus->inputRegisterRead(valID, AI_TDS);																//	Возвращаем количество растворённых твёрдых веществ из регистра "Input Register" AI[AI_TDS]. Функция возвращает прочитанное значение (0...65535), или -1 при неудаче.
}																															//
																															//
//		ФУНКЦИЯ ВЫБОРА ДАННЫХ ВЫВОДИМЫХ НА ДИСПЛЕЙ:																			//	Возвращает результат записи: true-успех / false-провал.
bool	iarduino_MB_TDS::print(uint8_t type){																				//	type - тип выводимых данных: MB_TDS_Ro/S/EC/TDS/5V/Vo/Vout/Vsen.
			objModbus->codeError=ERROR_SYNTAX; if( !valID ){ return false; }												//	Ошибка синтаксиса, модуль не инициализирован.
			if( type==MB_TDS_Ro		){;}else																				//	Указано выводить на дисплей общее измеренное сопротивление.
			if( type==MB_TDS_S		){;}else																				//	Указано выводить на дисплей измеренную удельную электропроводность.
			if( type==MB_TDS_EC		){;}else																				//	Указано выводить на дисплей приведённую удельную электропроводность.
			if( type==MB_TDS_TDS	){;}else																				//	Указано выводить на дисплей количество растворённых твёрдых веществ (по умолчанию).
			if( type==MB_TDS_5V		){;}else																				//	Указано выводить на дисплей измеренное напряжения питания на шине +5V.
			if( type==MB_TDS_Vo		){;}else																				//	Указано выводить на дисплей напряжение виртуального 0 на входе ОУ.
			if( type==MB_TDS_Vout	){;}else																				//	Указано выводить на дисплей напряжение на выходе ОУ.
			if( type==MB_TDS_Vsen	){;}else																				//	Указано выводить на дисплей напряжение на входе датчика.
			{ return false; }																								//	Ошибка синтаксиса, указанный параметр некорректен.
			return objModbus->holdingRegisterWrite(valID, AO_DISPLAY, type);												//	Записываем в регистр "Holding Register" AO[AO_DISPLAY] тип выводимых данных. Функция возвращает 1 при успехе, 0 при неудаче.
}																															//
