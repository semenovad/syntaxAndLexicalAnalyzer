#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<fstream>
#include<stdio.h>
using namespace std;

#define functional 1 //служебные слова
#define name       2 //имя
#define constant   3 //константа
#define operat     4 //операция
#define punct      5 //пунктуация
#define other      6 //другое

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

void FilesIntoStruct(char *FileName) {
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
		fin.get();
		fin.getline(&q1->str[0], 100);

		q1->next = 0;
		*curr_next = q1;
		curr_next = &(q1->next);
	}
}

bool StringComp(char *str1, char *str2, int size) {
	bool rez = true;
	while (1) {
		if (*str1 != *str2) {
			rez = false;
			break;
		}
		if (*str1 == 0) break;
		if (size != 0) {
			size--;
			if (size == 0) break;
		}
		str1++;
		str2++;
	}
	return(rez);
}
bool IsString(char *str1, char *str2) {
	bool t;
	t = StringComp(str1, str2, 0);
	if (t == false) {
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
			cout << "ERROR: expected type = " << type << " in line " << (*q)->line << endl;
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
bool checkString(elem **q, char *str, bool isMandatory) {

	IsEOF(*q);
	if (IsString(&((*q)->str[0]), str) == false) {
		if (isMandatory) {
			cout << "ERROR: expected string= \"" << str << "\" in line " << (*q)->line << endl;
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

	// <имя> {,<имя>} : <тип>
	if (checkElType(q, name, isMandatory) == false) return false;
	while (1) {
		if (checkString(q, ":", false) == true) break;
		checkString(q, ",", true);
		checkElType(q, name, true);
	}
	checkType(q, true);
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
		return (true);
	}
	if (checkElType(q, constant, false) == true) return(true);
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

	// <имя> | (<выражение>) | <спец. атом>
	if (checkElType(q, name, false) == true) return(true);
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

	// <атом> {<операция умножения><атом>}
	if (checkAtom(q, isMandatory) == false)	return(false);
	while (1) {
		if (checkMultiplicationOperation(q, false) == false) return(true);
		checkAtom(q, true);
	}
}

/*
Функция выполняет проверку на <простое выражение>.
Возврат: true   - <простое выражение> найден
false  - <простое выражение> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <простое выражение> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkSimpleExpression(elem **q, bool isMandatory) {

	// <терм> {<операция сложения><терм>}
	if (checkTerm(q, isMandatory) == false)	return(false);
	while (1) {
		if (checkAdditionOperation(q, false) == false) return(true);
		checkTerm(q, true);
	}
}

/*
Функция выполняет проверку на <выражение>.
Возврат: true   - <выражение> найден
false  - <выражение> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <выражение> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkExpression(elem **q, bool isMandatory) {

	// <простое выражение> | <выражение><операция отношения><выражение>
	if (checkSimpleExpression(q, isMandatory) == false) return(false);
	while (1) {
		if (checkQuotientOperation(q, false) == false) return(true);
		checkExpression(q, true);
	}
}

/*
Функция выполняет проверку на <оператор присваивания>.
Возврат: true   - <оператор присваивания> найден
false  - <оператор присваивания> не найден и при этом не является обязательным (isMandatory=false)
ошибка - <оператор присваивания> не найден и при этом является обязательным (isMandatory=true)
*/
bool checkAssignOperator(elem **q, bool isMandatory) {

	// <имя> := <выражение> |  <имя> := <конст. перечислимого типа>
	if (checkElType(q, name, isMandatory) == false) return(false);
	checkString(q, ":", true);
	checkString(q, "=", true);
	if (checkExpression(q, false) == true) return(true);
	if (checkElType(q, name, false) == true) return(true);

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


int main() {
	bool rez;
	elem *currentElem;

	FilesIntoStruct("../analyze/outputtt.txt");

	currentElem = head;
	rez = checkProgram(&currentElem);
	if (rez == true) cout << "Success analyzing!!!" << endl;
	else            cout << "Error analyzing!!!" << endl;
	system("pause");
	return(0);
}