#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<fstream>
#include<stdio.h>
using namespace std;

#define functional 1 //��������� �����
#define name       2 //���
#define constant   3 //���������
#define operat     4 //��������
#define punct      5 //����������
#define other      6 //������

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
������� ��������� �������� �� ������������ ���� ���������� ��������.
�������: true   - ������� ������������� ����
false  - ������� �� ������������� ���� ��� ���� �� �������� ������������ (isMandatory=false)
������ - ������� �� ������������� ���� ��� ���� �������� ������������ (isMandatory=true)
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
������� ��������� �������� �� ������������ ������ ���������� ��������.
�������: true   - ������� ������������� ������
false  - ������� �� ������������� ������ ��� ���� �� �������� ������������ (isMandatory=false)
������ - ������� �� ������������� ������ ��� ���� �������� ������������ (isMandatory=true)
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
������� ��������� �������� �� <������>.
�������: true   - <������> �������
false  - <������> �� ������� � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <������> �� ������� � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkSection(elem **q, bool isMandatory) {

	// <���> = ( <���> {,<���>} )
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
������� ��������� �������� �� <���>.
�������: true   - <���> ������
false  - <���> �� ������ � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <���> �� ������ � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkType(elem **q, bool isMandatory) {

	// integer | boolean | real | <���> 
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
������� ��������� �������� �� <������1>.
�������: true   - <������1> �������
false  - <������1> �� ������� � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <������1> �� ������� � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkSection1(elem **q, bool isMandatory) {

	// <���> {,<���>} : <���>
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
������� ��������� �������� �� <�������� ���������>.
�������: true   - <�������� ���������> �������
false  - <�������� ���������> �� ������� � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <�������� ���������> �� ������� � ��� ���� �������� ������������ (isMandatory=true)
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
������� ��������� �������� �� <�������� ��������>.
�������: true   - <�������� ��������> �������
false  - <�������� ��������> �� ������� � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <�������� ��������> �� ������� � ��� ���� �������� ������������ (isMandatory=true)
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
������� ��������� �������� �� <�������� ���������>.
�������: true   - <�������� ���������> �������
false  - <�������� ���������> �� ������� � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <�������� ���������> �� ������� � ��� ���� �������� ������������ (isMandatory=true)
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
������� ��������� �������� �� <����. ����>.
�������: true   - <����. ����> ������
false  - <����. ����> �� ������ � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <����. ����> �� ������ � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkSpecAtom(elem **q, bool isMandatory) {

	// <�����> | <�����. ��������>) | not <����>
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
������� ��������� �������� �� <����>.
�������: true   - <����> ������
false  - <����> �� ������ � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <����> �� ������ � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkAtom(elem **q, bool isMandatory) {

	// <���> | (<���������>) | <����. ����>
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
������� ��������� �������� �� <����>.
�������: true   - <����> ������
false  - <����> �� ������ � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <����> �� ������ � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkTerm(elem **q, bool isMandatory) {

	// <����> {<�������� ���������><����>}
	if (checkAtom(q, isMandatory) == false)	return(false);
	while (1) {
		if (checkMultiplicationOperation(q, false) == false) return(true);
		checkAtom(q, true);
	}
}

/*
������� ��������� �������� �� <������� ���������>.
�������: true   - <������� ���������> ������
false  - <������� ���������> �� ������ � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <������� ���������> �� ������ � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkSimpleExpression(elem **q, bool isMandatory) {

	// <����> {<�������� ��������><����>}
	if (checkTerm(q, isMandatory) == false)	return(false);
	while (1) {
		if (checkAdditionOperation(q, false) == false) return(true);
		checkTerm(q, true);
	}
}

/*
������� ��������� �������� �� <���������>.
�������: true   - <���������> ������
false  - <���������> �� ������ � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <���������> �� ������ � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkExpression(elem **q, bool isMandatory) {

	// <������� ���������> | <���������><�������� ���������><���������>
	if (checkSimpleExpression(q, isMandatory) == false) return(false);
	while (1) {
		if (checkQuotientOperation(q, false) == false) return(true);
		checkExpression(q, true);
	}
}

/*
������� ��������� �������� �� <�������� ������������>.
�������: true   - <�������� ������������> ������
false  - <�������� ������������> �� ������ � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <�������� ������������> �� ������ � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkAssignOperator(elem **q, bool isMandatory) {

	// <���> := <���������> |  <���> := <�����. ������������� ����>
	if (checkElType(q, name, isMandatory) == false) return(false);
	checkString(q, ":", true);
	checkString(q, "=", true);
	if (checkExpression(q, false) == true) return(true);
	if (checkElType(q, name, false) == true) return(true);

	cout << "ERROR: expected AssignOperator in line " << (*q)->line << endl;
	throw 1;
}

/*
������� ��������� �������� �� <�������� ������>.
�������: true   - <�������� ������> ������
false  - <�������� ������> �� ������ � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <�������� ������> �� ������ � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkOutputOperator(elem **q, bool isMandatory) {

	// writeln(<���������> {,<���������>})
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
������� ��������� �������� �� <��������� ��������>.
�������: true   - <��������� ��������> ������
false  - <��������� ��������> �� ������ � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <��������� ��������> �� ������ � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkCompositeOperator(elem **q, bool isMandatory) {

	// begin <��������> {;<��������>} end
	if (checkString(q, "begin", isMandatory) == false) return(false);
	checkOperator(q, true);
	while (1) {
		if (checkString(q, "end", false) == true) return(true);
		checkString(q, ";", true);
		if (checkString(q, "end", false) == true) return(true);   // *** � ������ ������� ';' ����� end (� ������� �������� ������ �� �����) 
		checkOperator(q, true);
	}
}

/*
������� ��������� �������� �� <����� ��������>.
�������: true   - <����� ��������> �������
false  - <����� ��������> �� ������� � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <����� ��������> �� ������� � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkVariantLabel(elem **q, bool isMandatory) {

	// <�����> | <���������� ��������> | <���>
	if (checkElType(q, constant, false) == true) return(true);
	if (checkElType(q, name, false) == true) return(true);
	if (isMandatory) {
		cout << "ERROR: expected VariantLabel in line " << (*q)->line << endl;
		throw 1;
	}
	return(false);
}

/*
������� ��������� �������� �� <�������>.
�������: true   - <�������> ������
false  - <�������> �� ������ � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <�������> �� ������ � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkVariant(elem **q, bool isMandatory) {

	// <����� ��������> : <��������>
	if (checkVariantLabel(q, isMandatory) == false) return(false);
	checkString(q, ":", true);
	checkOperator(q, true);
	return(true);
}

/*
������� ��������� �������� �� <�������� ������>.
�������: true   - <�������� ������> ������
false  - <�������� ������> �� ������ � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <�������� ������> �� ������ � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkCaseOperator(elem **q, bool isMandatory) {

	// case <���������> of <�������> {;�������} endcase
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
������� ��������� �������� �� <�������� ����� � ������������>.
�������: true   - <�������� ����� � ������������> ������
false  - <�������� ����� � ������������> �� ������ � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <�������� ����� � ������������> �� ������ � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkWhileOperator(elem **q, bool isMandatory) {

	// while <���������> do <��������>
	if (checkString(q, "while", false) == false) return(false);
	checkExpression(q, true);
	checkString(q, "do", true);
	checkOperator(q, true);
	return(true);
}

/*
������� ��������� �������� �� <����. ��������>.
�������: true   - <����. ��������> ������
false  - <����. ��������> �� ������ � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <����. ��������> �� ������ � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkSpecOperator(elem **q, bool isMandatory) {

	// <�������� ������> | <�������� ����� � ������������>
	if (checkCaseOperator(q, false) == true) return(true);
	if (checkWhileOperator(q, false) == true) return(true);
	if (isMandatory) {
		cout << "ERROR: expected SpecOperator in line " << (*q)->line << endl;
		throw 1;
	}
	return(false);
}

/*
������� ��������� �������� �� <��������>.
�������: true   - <��������> �������
false  - <��������> �� ������� � ��� ���� �� �������� ������������ (isMandatory=false)
������ - <��������> �� ������� � ��� ���� �������� ������������ (isMandatory=true)
*/
bool checkOperator(elem **q, bool isMandatory) {

	// <�������� ������������> | <�������� ������> | <��������� ��������> | <����. ��������>
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
������� �������������� ������ ���������.
�������: true   - ������� ������
false  - ������� �� ����� �������
*/
bool checkProgram(elem **q) {

	try {
		// program <���>;
		checkString(q, "program", true);
		checkElType(q, name, true);
		checkString(q, ";", true);

		// <������ ��������>
		// {<������ �����>}
		if (checkString(q, "type", false)) {
			// <������> {;<������>};
			checkSection(q, true);
			while (1) {
				checkString(q, ";", true);
				if (checkSection(q, false) == false) break;
			}
		}
		// {<������ ����������>}
		if (checkString(q, "var", false)) {
			// <������1> {;<������1>};
			checkSection1(q, true);
			while (1) {
				checkString(q, ";", true);
				if (checkSection1(q, false) == false) break;
			}
		}

		// <������ ����������>
		checkString(q, "begin", true);
		checkOperator(q, true);
		while (1) {
			if (checkString(q, "end", false) == true) break;
			checkString(q, ";", true);
			if (checkString(q, "end", false) == true) break;     // *** � ������ ������� ';' ����� end (� ������� �������� ������ �� �����) 
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