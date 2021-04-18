#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<fstream>
#include<stdio.h>
#include<string.h>
using namespace std;

#define functional 1 //служебные слова
#define name       2 //имя
#define constant   3 //константа
#define operat     4 //операция
#define punct      5 //пунктуация
#define other      6 //другое
#define boolean    7 //bool
#define real       8 //real

struct elem {
	char str[100];
	int num;
	int line;
	elem *next;
};

elem *head;
char *error;

bool checkExpression(elem **q, bool isMandatory);
bool checkOperator(elem **q, bool isMandatory);
bool checkAtom(elem **q, bool isMandatory);

void pushElem(const char *string);
void popElem(char *string);
void addElemsFromStackToTid(char *idType);
void pushIdToStack(char *idName);
void pushOpToStack(char *opName);
void checkOp(int line);
void checkNot(int line);
void eqType(int line);
void eqBool(int line);

/////////////////////////////////////////////////////////////////////////////////////
// Общие функции

void FilesIntoStruct(const char *FileName) {
	elem *q1, **curr_next;
	ifstream fin(FileName);

	if (fin.is_open() == false) return;
	curr_next = &head;
	while (fin.eof() == false) {
		q1 = new elem;
		fin >> q1->line;

		if (fin.eof() == true) {
			delete q1;
			break;
		}
		fin >> q1->num;

		if (fin.eof() == true) {
			delete q1;
			break;
		}
		fin.get();		                // Пропуск пробела перед строкой со значением
		fin.getline(&q1->str[0], 100);  // Сохраняем все символы до конца строки

		q1->next = 0;
		*curr_next = q1;
		curr_next = &(q1->next);
	}
}

bool StringComp(const char *str1, const char *str2, int size);

bool IsString(const char *str1, const char *str2) {
	bool t;
	t = StringComp(str1, str2, 0);
	if (t == false) {
		//		sprintf(error, "expected \"%s\"", str2);
		return(false);
	}
	return(true);
}
bool IsEOF(elem *q) {
	if (q == NULL) {
		cout << "ERROR: unexpected end of file" << endl;
		throw 2;
	}
	return(false);
}

/////////////////////////////////////////////////////////////////////////////////////
// Синтаксический анализатор

/*
Функция выполняет проверку на соответствие типу ожидаемого элемента.
Возврат: true   - элемент соответствует типу
false  - элемент не соответствует типу при этом не является обязательным (isMandatory=false)
ошибка - элемент не соответствует типу при этом является обязательным (isMandatory=true)
*/
bool checkElType(elem **q, int type, bool isMandatory) {

	IsEOF(*q);
	if ((*q)->num != type) {
		if (isMandatory) {
			cout << "ERROR: expected type=" << type << " in line " << (*q)->line << endl;
			throw 1;
		}
		return(false);
	}
	*q = (*q)->next;
	return(true);
}

/*
Функция выполняет проверку на соответствие строки ожидаемого элемента.
Возврат: true   - элемент соответствует строки
false  - элемент не соответствует строки при этом не является обязательным (isMandatory=false)
ошибка - элемент не соответствует строки при этом является обязательным (isMandatory=true)
*/
bool checkString(elem **q, const char *str, bool isMandatory) {

	IsEOF(*q);
	if (IsString(&((*q)->str[0]), str) == false) {
		if (isMandatory) {
			cout << "ERROR: expected string=/" << str << "/ in line " << (*q)->line << endl;
			throw 1;
		}
		return(false);
	}
	*q = (*q)->next;
	return(true);
}

/*
Функция выполняет проверку на <секция>.
Возврат: true   - <секция> найдена
false  - <секция> не найдена и при этом не является обязательным (isMandatory=false)
ошибка - <секция> не найдена и при этом является обязательным (isMandatory=true)
*/
bool checkSection(elem **q, bool isMandatory) {

	// <имя> = ( <имя> {,<имя>} )
	if (checkElType(q, name, isMandatory) == false) return(false);
	checkString(q, "=", true);
	checkString(q, "(", true);
	checkElType(q, name, true);
	while (1) {
		if (checkString(q, ")", false) == true) return(true);
		checkString(q, ",", true);
		checkElType(q, name, true);
	}
}

/*
Функция выполняет проверку на <тип>.
Возврат: true   - <тип> найден
false  - <тип> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <тип> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkType(elem **q, bool isMandatory) {

	// integer | boolean | real | <имя> 
	if (checkString(q, "integer", false) == true) return(true);
	if (checkString(q, "boolean", false) == true) return(true);
	if (checkString(q, "real", false) == true) return(true);
	if (checkElType(q, name, false) == true) return(true);
	if (isMandatory) {
		cout << "ERROR: expected Type in line " << (*q)->line << endl;
		throw 1;
	}
	return(false);
}

/*
Функция выполняет проверку на <секция1>.
Возврат: true   - <секция1> найдена
false  - <секция1> не найдена и при этом не является обязательным (isMandatory=false)
ошибка - <секция1> не найдена и при этом является обязательным (isMandatory=true)
*/
bool checkSection1(elem **q, bool isMandatory) {
	char *el_name;

	// <имя> {,<имя>} : <тип>
	el_name = &(*q)->str[0];
	if (checkElType(q, name, isMandatory) == false) return false;
	pushElem(el_name);									// Семантический анализатор
	while (1) {
		if (checkString(q, ":", false) == true) break;
		checkString(q, ",", true);
		el_name = &(*q)->str[0];
		checkElType(q, name, true);
		pushElem(el_name);								// Семантический анализатор
	}
	el_name = &(*q)->str[0];
	checkType(q, true);
	addElemsFromStackToTid(el_name);				// Семантический анализатор
	return(true);
}

/*
Функция выполняет проверку на <операция отношения>.
Возврат: true   - <операция отношения> найдена
false  - <операция отношения> не найдена и при этом не является обязательным (isMandatory=false)
ошибка - <операция отношения> не найдена и при этом является обязательным (isMandatory=true)
*/
bool checkQuotientOperation(elem **q, bool isMandatory) {

	// > | < | <> | <= | >= | =
	if (checkString(q, ">", false) == true) return(true);
	if (checkString(q, "<", false) == true) return(true);
	if (checkString(q, "<>", false) == true) return(true);
	if (checkString(q, "<=", false) == true) return(true);
	if (checkString(q, ">=", false) == true) return(true);
	if (checkString(q, "=", false) == true) return(true);
	if (isMandatory) {
		cout << "ERROR: expected QuotientOperation in line " << (*q)->line << endl;
		throw 1;
	}
	return(false);
}

/*
Функция выполняет проверку на <операция сложения>.
Возврат: true   - <операция сложения> найдена
false  - <операция сложения> не найдена и при этом не является обязательным (isMandatory=false)
ошибка - <операция сложения> не найдена и при этом является обязательным (isMandatory=true)
*/
bool checkAdditionOperation(elem **q, bool isMandatory) {

	// + | - | or
	if (checkString(q, "+", false) == true) return(true);
	if (checkString(q, "-", false) == true) return(true);
	if (checkString(q, "or", false) == true) return(true);
	if (isMandatory) {
		cout << "ERROR: expected AdditionOperation in line " << (*q)->line << endl;
		throw 1;
	}
	return(false);
}

/*
Функция выполняет проверку на <операция умножения>.
Возврат: true   - <операция умножения> найдена
false  - <операция умножения> не найдена и при этом не является обязательным (isMandatory=false)
ошибка - <операция умножения> не найдена и при этом является обязательным (isMandatory=true)
*/
bool checkMultiplicationOperation(elem **q, bool isMandatory) {

	// * | div | and | / | mod
	if (checkString(q, "*", false) == true) return(true);
	if (checkString(q, "div", false) == true) return(true);
	if (checkString(q, "and", false) == true) return(true);
	if (checkString(q, "/", false) == true) return(true);
	if (checkString(q, "mod", false) == true) return(true);
	if (isMandatory) {
		cout << "ERROR: expected MultiplicationOperation in line " << (*q)->line << endl;
		throw 1;
	}
	return(false);
}

/*
Функция выполняет проверку на <спец. атом>.
Возврат: true   - <спец. атом> найден
false  - <спец. атом> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <спец. атом> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkSpecAtom(elem **q, bool isMandatory) {

	// <конст> | <логич. значение>) | not <атом>
	if (checkString(q, "not", false) == true) {
		checkAtom(q, true);
		checkNot((*q)->line);									// Семантический анализ
		return (true);
	}
	if (checkElType(q, constant, false) == true) {
		pushElem("integer");						// Семантический анализ
		return(true);
	}
	/*if (checkElType(q, real, false) == true) {
	pushElem("real");						// Семантический анализ
	return(true);
	}*/
	if (checkElType(q, boolean, false) == true) {
		pushElem("boolean");						// Семантический анализ
		return(true);
	}
	if (isMandatory) {
		cout << "ERROR: expected SpecAtom in line " << (*q)->line << endl;
		throw 1;
	}
	return(false);
}

/*
Функция выполняет проверку на <атом>.
Возврат: true   - <атом> найден
false  - <атом> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <атом> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkAtom(elem **q, bool isMandatory) {
	char *el_name;

	// <имя> | (<выражение>) | <спец. атом>
	el_name = &(*q)->str[0];
	if (checkElType(q, name, false) == true) {
		pushIdToStack(el_name);					// Семантический анализ
		return(true);
	}
	if (checkString(q, "(", false) == true) {
		checkExpression(q, true);
		checkString(q, ")", true);
		return(true);
	}
	if (checkSpecAtom(q, false) == true) return(true);

	if (isMandatory) {
		cout << "ERROR: expected Atom in line " << (*q)->line << endl;
		throw 1;
	}
	return(false);
}

/*
Функция выполняет проверку на <терм>.
Возврат: true   - <терм> найден
false  - <терм> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <терм> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkTerm(elem **q, bool isMandatory) {
	char *el_name;

	// <атом> {<операция умножения><атом>}
	if (checkAtom(q, isMandatory) == false)	return(false);
	while (1) {
		el_name = &(*q)->str[0];
		if (checkMultiplicationOperation(q, false) == false) return(true);
		pushOpToStack(el_name);				// Семантический анализ
		checkAtom(q, true);
		checkOp((*q)->line);									// Семантический анализ
	}
}

/*
Функция выполняет проверку на <простое выражение>.
Возврат: true   - <простое выражение> найден
false  - <простое выражение> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <простое выражение> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkSimpleExpression(elem **q, bool isMandatory) {
	char *el_name;

	// <терм> {<операция сложения><терм>}
	if (checkTerm(q, isMandatory) == false)	return(false);
	while (1) {
		el_name = &(*q)->str[0];
		if (checkAdditionOperation(q, false) == false) return(true);
		pushOpToStack(el_name);				// Семантический анализ
		checkTerm(q, true);
		checkOp((*q)->line);									// Семантический анализ
	}
}

/*
Функция выполняет проверку на <выражение>.
Возврат: true   - <выражение> найден
false  - <выражение> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <выражение> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkExpression(elem **q, bool isMandatory) {
	char *el_name;

	// <простое выражение> | <выражение><операция отношения><выражение>
	if (checkSimpleExpression(q, isMandatory) == false) return(false);
	while (1) {
		el_name = &(*q)->str[0];
		if (checkQuotientOperation(q, false) == false) return(true);
		pushOpToStack(el_name);				// Семантический анализ
		checkExpression(q, true);
		checkOp((*q)->line);									// Семантический анализ
	}
}

/*
Функция выполняет проверку на <оператор присваивания>.
Возврат: true   - <оператор присваивания> найден
false  - <оператор присваивания> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <оператор присваивания> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkAssignOperator(elem **q, bool isMandatory) {
	char *el_name;

	// <имя> := <выражение> |  <имя> := <конст. перечислимого типа>
	el_name = &(*q)->str[0];
	if (checkElType(q, name, isMandatory) == false) return(false);
	pushIdToStack(el_name);				// Семантический анализ
	checkString(q, ":", true);
	checkString(q, "=", true);
	if (checkExpression(q, false) == true) {
		eqType((*q)->line);								// Семантический анализ
		return(true);
	}
	el_name = &(*q)->str[0];
	if (checkElType(q, name, false) == true) {
		pushIdToStack(el_name);			// Семантический анализ
		eqType((*q)->line);								// Семантический анализ
		return(true);
	}
	cout << "ERROR: expected AssignOperator in line " << (*q)->line << endl;
	throw 1;
}

/*
Функция выполняет проверку на <оператор вывода>.
Возврат: true   - <оператор вывода> найден
false  - <оператор вывода> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <оператор вывода> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkOutputOperator(elem **q, bool isMandatory) {

	// writeln(<выражение> {,<выражение>})
	if (checkString(q, "writeln", isMandatory) == false) return(false);
	checkString(q, "(", true);
	checkExpression(q, true);
	while (1) {
		if (checkString(q, ")", false) == true) return(true);
		checkString(q, ",", true);
		checkExpression(q, true);
	}
}

/*
Функция выполняет проверку на <составной оператор>.
Возврат: true   - <составной оператор> найден
false  - <составной оператор> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <составной оператор> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkCompositeOperator(elem **q, bool isMandatory) {

	// begin <оператор> {;<оператор>} end
	if (checkString(q, "begin", isMandatory) == false) return(false);
	checkOperator(q, true);
	while (1) {
		if (checkString(q, "end", false) == true) return(true);
		checkString(q, ";", true);
		if (checkString(q, "end", false) == true) return(true);   // *** в случае наличия ';' перед end (в строгом варианте строка не нужна) 
		checkOperator(q, true);
	}
}

/*
Функция выполняет проверку на <метка варианта>.
Возврат: true   - <метка варианта> найдена
false  - <метка варианта> не найдена и при этом не является обязательным (isMandatory=false)
ошибка - <метка варианта> не найдена и при этом является обязательным (isMandatory=true)
*/
bool checkVariantLabel(elem **q, bool isMandatory) {

	// <конст> | <логическое значение> | <имя>
	if (checkElType(q, constant, false) == true) return(true);
	if (checkElType(q, name, false) == true) return(true);
	if (isMandatory) {
		cout << "ERROR: expected VariantLabel in line " << (*q)->line << endl;
		throw 1;
	}
	return(false);
}

/*
Функция выполняет проверку на <вариант>.
Возврат: true   - <вариант> найден
false  - <вариант> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <вариант> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkVariant(elem **q, bool isMandatory) {

	// <метка варианта> : <оператор>
	if (checkVariantLabel(q, isMandatory) == false) return(false);
	checkString(q, ":", true);
	checkOperator(q, true);
	return(true);
}

/*
Функция выполняет проверку на <оператор выбора>.
Возврат: true   - <оператор выбора> найден
false  - <оператор выбора> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <оператор выбора> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkCaseOperator(elem **q, bool isMandatory) {

	// case <выражение> of <вариант> {;вариант} endcase
	if (checkString(q, "case", false) == false) return(false);
	checkExpression(q, true);
	checkString(q, "of", true);
	checkVariant(q, true);
	while (1) {
		if (checkString(q, "endcase", false) == true) return(true);
		checkString(q, ";", true);
		checkVariant(q, true);
	}
}

/*
Функция выполняет проверку на <оператор цикла с предусловием>.
Возврат: true   - <оператор цикла с предусловием> найден
false  - <оператор цикла с предусловием> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <оператор цикла с предусловием> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkWhileOperator(elem **q, bool isMandatory) {

	// while <выражение> do <оператор>
	if (checkString(q, "while", false) == false) return(false);
	checkExpression(q, true);
	eqBool((*q)->line);								// Семантический анализ
	checkString(q, "do", true);
	checkOperator(q, true);
	return(true);
}

/*
Функция выполняет проверку на <спец. оператор>.
Возврат: true   - <спец. оператор> найден
false  - <спец. оператор> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <спец. оператор> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkSpecOperator(elem **q, bool isMandatory) {

	// <оператор выбора> | <оператор цикла с предусловием>
	if (checkCaseOperator(q, false) == true) return(true);
	if (checkWhileOperator(q, false) == true) return(true);
	if (isMandatory) {
		cout << "ERROR: expected SpecOperator in line " << (*q)->line << endl;
		throw 1;
	}
	return(false);
}

/*
Функция выполняет проверку на <оператор>.
Возврат: true   - <оператор> найдена
false  - <оператор> не найдена и при этом не является обязательным (isMandatory=false)
ошибка - <оператор> не найдена и при этом является обязательным (isMandatory=true)
*/
bool checkOperator(elem **q, bool isMandatory) {

	// <оператор присваивания> | <оператор вывода> | <составной оператор> | <спец. оператор>
	if (checkAssignOperator(q, false) == true) return(true);
	if (checkOutputOperator(q, false) == true) return(true);
	if (checkCompositeOperator(q, false) == true) return(true);
	if (checkSpecOperator(q, false) == true) return(true);
	if (isMandatory) {
		cout << "ERROR: expected Operator in line " << (*q)->line << endl;
		throw 1;
	}
	return(false);
}

/*
Функция синтаксический разбор программы.
Возврат: true   - удачный разбор
false  - неудача во время разбора
*/
bool checkProgram(elem **q) {

	try {
		// program <имя>;
		checkString(q, "program", true);
		checkElType(q, name, true);
		checkString(q, ";", true);

		// <раздел описаний>
		// {<раздел типов>}
		if (checkString(q, "type", false)) {
			// <секция> {;<секция>};
			checkSection(q, true);
			while (1) {
				checkString(q, ";", true);
				if (checkSection(q, false) == false) break;
			}
		}
		// {<раздел переменных>}
		if (checkString(q, "var", false)) {
			// <секция1> {;<секция1>};
			checkSection1(q, true);
			while (1) {
				checkString(q, ";", true);
				if (checkSection1(q, false) == false) break;
			}
		}

		// <раздел операторов>
		checkString(q, "begin", true);
		checkOperator(q, true);
		while (1) {
			if (checkString(q, "end", false) == true) break;
			checkString(q, ";", true);
			if (checkString(q, "end", false) == true) break;     // *** в случае наличия ';' перед end (в строгом варианте строка не нужна) 
			checkOperator(q, true);
		}
		checkString(q, ".", true);

	}
	catch (int error_num) {
		return(false);
	}
	return(true);
}

/////////////////////////////////////////////////////////////////////////////////////
// Семантический анализатор

/*
Функции для работы со стеком.
*/
#define stackStringMax	64
struct stackElemStruct {
	stackElemStruct *next;
	char string[stackStringMax];
};
stackElemStruct *firstStackElem;

void initStack() {
	firstStackElem = 0;
}
bool isStackEmpty() {
	if (firstStackElem == 0) return(true);
	return(false);
}
void pushElem(const char *string) {
	stackElemStruct *q;

	q = new stackElemStruct;
	q->next = firstStackElem;
	firstStackElem = q;
	strncpy(q->string, string, stackStringMax);
}
void popElem(char *string) {
	stackElemStruct *q;

	if (isStackEmpty() == true) {
		cout << "ERROR: stack empty error" << endl;
		throw 1;
	}
	q = firstStackElem;
	firstStackElem = q->next;
	strncpy(string, q->string, stackStringMax);
	free(q);
}

#define idNameStringMax		64
#define idTypeStringMax		16
#define idValueStringMax	64
struct idParametersStruct {
	idParametersStruct *next;

	char idName[idNameStringMax];
	char idType[idTypeStringMax];
	long idValue;
	char idValueString[idValueStringMax];
};
idParametersStruct *firstTidElem;

char typeNames[][idTypeStringMax] = {
	"integer",
	"real",
	"boolean",
	name,
	0
};

/*
Функции для работы с таблицей идентификаторов.
*/
void initTid() {
	firstTidElem = 0;
}
bool isIdInTid(char *idName) {
	idParametersStruct *idEl;
	idEl = firstTidElem;
	while (idEl != 0) {
		if (strncmp(idEl->idName, idName, idNameStringMax) == 0) {
			return(true);
		}
		idEl = idEl->next;
	}
	return(false);
}
bool getTypeForId(char *idName, char *idType) {
	idParametersStruct *idEl;
	int i = 0;

	while (typeNames[i][0] != 0) {
		if (strncmp(idName, &typeNames[i][0], idTypeStringMax) == 0) {
			strncpy(idType, idName, idTypeStringMax);
			return(true);
		}
		i++;
	}
	idEl = firstTidElem;
	while (idEl != 0)
	{
		if (strncmp(idName, idEl->idName, idNameStringMax) == 0)
		{
			strncpy(idType, idEl->idType, idTypeStringMax);
			return(true);
		}
		idEl = idEl->next;
	}
	return(false);
}
void addIdToTid(char *idName, char *idType) {
	idParametersStruct *q;

	if (isIdInTid(idName) == true) {
		cout << "ERROR: double identificator: \"" << idName << "\"" << endl;
		throw 1;
	}
	q = new idParametersStruct;
	q->next = firstTidElem;
	firstTidElem = q;
	strncpy(q->idName, idName, idNameStringMax);
	strncpy(q->idType, idType, idTypeStringMax);
	q->idValue = 0;
	q->idValueString[0] = 0;
}
void addElemsFromStackToTid(char *idType) {
	char idName[idNameStringMax];

	while (isStackEmpty() == false) {
		popElem(&idName[0]);
		addIdToTid(&idName[0], idType);
	}
}

/*
Функции для анализа выражений.
*/
struct opParametersStruct {
	char op[idNameStringMax];
	char type1[idNameStringMax];
	char type2[idNameStringMax];
	char opType[idTypeStringMax];
};
opParametersStruct opParameters[] = {
	{ "+", "integer", "integer", "integer" },
	{ "+", "integer", "real", "real" },
	{ "+", "real", "integer", "real" },
	{ "+", "real", "real", "real" },
	{ "-", "integer", "integer", "integer" },
	{ "-", "integer", "real", "real" },
	{ "-", "real", "integer", "real" },
	{ "-", "real", "real", "real" },
	{ "*", "integer", "integer", "integer" },
	{ "*", "integer", "real", "real" },
	{ "*", "real", "integer", "real" },
	{ "*", "real", "real", "real" },
	{ "/", "integer", "integer", "real" },
	{ "/", "integer", "real", "real" },
	{ "/", "real", "integer", "real" },
	{ "/", "real", "real", "real" },
	{ "div", "integer", "integer", "integer" },
	{ "mod", "integer", "integer", "integer" },
	{ ">", "integer", "integer", "boolean" },
	{ ">", "real", "real", "boolean" },
	{ "<", "integer", "integer", "boolean" },
	{ "<", "real", "real", "boolean" },
	{ ">=", "integer", "integer", "boolean" },
	{ ">=", "real", "real", "boolean" },
	{ "<=", "integer", "integer", "boolean" },
	{ "<=", "real", "real", "boolean" },
	{ "<>", "integer", "integer", "boolean" },
	{ "<>", "real", "real", "boolean" },
	{ "<>", "boolean", "boolean", "boolean" },
	{ "=", "integer", "integer", "boolean" },
	{ "=", "real", "real", "boolean" },
	{ "=", "boolean", "boolean", "boolean" },
	{ 0, 0, 0, 0 }
};

struct eqParametersStruct {
	char type1[idNameStringMax];
	char type2[idNameStringMax];
};
eqParametersStruct eqParameters[] = {
	{ "integer", "integer" },
	{ "integer", "real" },
	{ "real", "real" },
	{ "boolean", "boolean" },
	{ name, name }, 
	{ 0, 0, 0, 0 }
};

void pushIdToStack(char *idName) {
	if (isIdInTid(idName) == false) {
		cout << "ERROR: identificator type not defined: \"" << idName << "\"" << endl;
		throw 1;
	}
	pushElem(idName);
}
void pushOpToStack(char *opName) {
	pushElem(opName);
}
void getOpType(char *op, char *type1, char *type2, char *opType, int line) {
	int i = 0;
	while (opParameters[i].op[0] != 0) {
		if (strncmp(&opParameters[i].op[0], op, idNameStringMax) == 0) {
			if (strncmp(&opParameters[i].type1[0], type1, idTypeStringMax) == 0) {
				if (strncmp(&opParameters[i].type2[0], type2, idTypeStringMax) == 0) {
					strncpy(opType, &opParameters[i].opType[0], idTypeStringMax);
					return;
				}
			}
		}
		i++;
	}
	cout << "ERROR: operation not found: \"" << op << "\", line " << line << endl;
	throw 1;
}
void checkOp(int line) {
	char idName1[idNameStringMax];
	char idType1[idTypeStringMax];
	char idName2[idNameStringMax];
	char idType2[idTypeStringMax];
	char idOp[idNameStringMax];
	char idOpType[idTypeStringMax];

	popElem(&idName1[0]);
	getTypeForId(&idName1[0], &idType1[0]);
	popElem(&idOp[0]);
	popElem(&idName2[0]);
	getTypeForId(&idName2[0], &idType2[0]);
	getOpType(&idOp[0], &idType1[0], &idType2[0], &idOpType[0], line);
	pushElem(&idOpType[0]);
}
void checkNot(int line) {
	char idName1[idNameStringMax];
	char idType1[idTypeStringMax];

	popElem(&idName1[0]);
	getTypeForId(&idName1[0], &idType1[0]);
	if (strncmp(&idType1[0], "boolean", idTypeStringMax) == 0) {
		pushElem("boolean");
		return;
	}
	cout << "ERROR: check not error: \"" << idName1 << "\", line " << line << endl;
	throw 1;
}

/*
Контроль контекстных условий в операторах.
*/
void checkEqType(char *type1, char *type2, int line) {
	int i = 0;
	while (eqParameters[i].type1[0] != 0) {
		if (strncmp(&eqParameters[i].type1[0], type1, idTypeStringMax) == 0) {
			if (strncmp(&eqParameters[i].type2[0], type2, idTypeStringMax) == 0) {
				return;
			}
		}
		i++;
	}
	cout << "ERROR: type comparison error: line " << line << endl;
	throw 1;
}

void eqType(int line) {
	char idName1[idNameStringMax];
	char idType1[idTypeStringMax];
	char idName2[idNameStringMax];
	char idType2[idTypeStringMax];

	popElem(&idName1[0]);
	getTypeForId(&idName1[0], &idType1[0]);
	popElem(&idName2[0]);
	getTypeForId(&idName2[0], &idType2[0]);
	checkEqType(&idType1[0], &idType2[0], line);
}
void eqBool(int line) {
	char idName1[idNameStringMax];
	char idType1[idTypeStringMax];

	popElem(&idName1[0]);
	getTypeForId(&idName1[0], &idType1[0]);
	if (strncmp(&idType1[0], "boolean", idTypeStringMax) == 0) return;
	cout << "ERROR: boolean comparison error: line " << line << endl;
	throw 1;
}


void Syntax() {
	bool rez;
	elem *currentElem;

	initStack();
	initTid();

	FilesIntoStruct("outputtt.txt");

	currentElem = head;
	rez = checkProgram(&currentElem);
	if (rez == true) cout << "Success analyzing!!!" << endl;
	else cout << "Error analyzing!!!" << endl;
}
