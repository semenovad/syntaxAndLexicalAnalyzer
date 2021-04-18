#define _CRT_SECURE_NO_WARNINGS
#include<fstream>
#include<iostream>
#include<stdio.h>
using namespace std;

#define digit         1 //служебные слова
#define letter        2 //имя
#define quotetionmark 3 //константа
#define operattest    4 //операция
#define puncttest     5 //пунктуация
#define other         6 //другое
#define boolean       7 //bool
#define real          8 //real

FILE *list;
char symbols[100];

struct elem {
	char str[100];
	elem *next;
};
elem *head_service, *head_operations, *head_punctuation;

bool IsDigit(char s) {
	if (s <= '9' && s >= '0') return true;
	return false;
}
bool IsAlph(char s) {
	if (s <= 'z' && s >= 'A' || s == '_') return true;
	return false;
}
void Zero() {
	for (int i = 0; i < 100; i++) symbols[i] = 0;
	return;
}

void FilesIntoStruct2(elem **head, const char *FileName) {
	elem *q1;
	ifstream fin(FileName);
	char s;
	int i;
	*head = 0;
	if (fin.is_open() == false) return;
	s = fin.get();
	while (s != EOF) {
		q1 = new elem;
		i = 0;
		while (s != ' ' && s != '\n' && s != EOF) {
			q1->str[i] = s;
			s = fin.get();
			i++;
		}
		q1->str[i] = 0;
		q1->next = *head;
		*head = q1;
		while ((s == ' ' || s == '\n') && s != EOF) s = fin.get();
	}
	fin.close();
}
bool StringComp(const char *str1, const char *str2, int size) {
	bool rez = true;
	while (1) {
		if (*str1 != *str2) {
			rez = false;
			break;
		}
		if (*str1 == 0) break;
		if (size == 1) break;
		if (size != 0) size--;
		str1++;
		str2++;
	}
	return(rez);
}
bool Comparison(elem **head, int size) {
	elem *q;
	bool t = false;
	q = *head;
	int i = 0;
	while (q != NULL) {
		t = StringComp(&q->str[0], &symbols[0], size);
		if (t == true) return(true);
		q = q->next;
	}
	return(false);
}
void Output(int number, int s) {
	int i = 0;
	fprintf(list, "%d", s);
	if (number == 1) fprintf(list, " 1"); //служебное слово
	if (number == 2) fprintf(list, " 2"); //имя
	if (number == 3) fprintf(list, " 3"); //константа
	if (number == 4) fprintf(list, " 4"); //операция
	if (number == 5) fprintf(list, " 5"); //пунктуация
	if (number == 6) fprintf(list, " 6"); //другое
	if (number == 7) fprintf(list, " 7"); //bool
	if (number == 8) fprintf(list, " 8"); //real
	fprintf(list, " %s", &symbols[0]);
	fprintf(list, "\n");
	return;
}
void LexicAnalysator() {
	ifstream fin("program.txt");
	if (fin.is_open() == false) return;
	list = fopen("outputtt.txt", "w");
	int number = 0, k = 1;
	char s = fin.get();
	while (1) {
		while ((s == ' ' || s == '\n' || s == '\t') && s != EOF) {
			if (s == '\n') k++;
			s = fin.get();
		}
		if (s == EOF) break;
		number = other;
		if (IsDigit(s) == true) number = digit;
		if (IsAlph(s) == true) number = letter;
		if (s == '\'' || s == '"') number = quotetionmark;
		switch (number) {
		case digit: {
			Zero();
			int i = 0;
			bool flaq = false;
			while (IsDigit(s) == true && s != EOF) {
				symbols[i] = s;
				i++;
				s = fin.get();
				if (s == '.') {
					flaq = true;
					symbols[i] = s;
					i++;
					s = fin.get();
				}
			}
			if (flaq == true) Output(8, k); //real
			else Output(3, k); //константа
			break;
		}
		case letter: {
			Zero();
			int i = 0;
			while ((IsDigit(s) == true || IsAlph(s) == true) && s != EOF) {
				symbols[i] = s;
				i++;
				s = fin.get();
			}
			if (StringComp("true", &symbols[0], 0) == true || StringComp("false", &symbols[0], 0) == true) {
				Output(7, k); //bool
				break;
			}
			if (Comparison(&head_service, 0) == true) Output(1, k); //служебное слово
			else {
				if (Comparison(&head_operations, 0) == true) Output(4, k); //операция
				else Output(2, k); //имя
			}
			break;
		}
		case quotetionmark: {
			Zero();
			int i = 1;
			symbols[0] = s;
			s = fin.get();
			while ((s != '\'' && s != '"') && s != EOF) {
				symbols[i] = s;
				i++;
				s = fin.get();
			}
			symbols[i] = s;
			Output(3, k); //константа
			s = fin.get();
			break;
		}
		case other: {
			Zero();
			bool t = false;
			int i = 0;
			symbols[i] = s;
			if (Comparison(&head_operations, 1) == true) {
				while (1) {
					i++;
					s = fin.get();
					symbols[i] = s;
					if (Comparison(&head_operations, i + 1) == false) {
						symbols[i] = '\0';
						Output(4, k); //операция
						break;
					}
				}
			}
			else {
				if (Comparison(&head_punctuation, 1) == true) {
					while (1) {
						i++;
						s = fin.get();
						symbols[i] = s;
						if (Comparison(&head_punctuation, i + 1) == false) {
							symbols[i] = '\0';
							Output(5, k); //пунктуация
							break;
						}
					}
				}
				else {
					Output(6, k); //другое
					s = fin.get();
				}
			}
			break;
		}
		}
	}
	fclose(list);
	fin.close();
}

void Lexic() {
	FilesIntoStruct2(&head_service, "servicewords.txt");
	FilesIntoStruct2(&head_operations, "operations.txt");
	FilesIntoStruct2(&head_punctuation, "punctuation.txt");
	LexicAnalysator();
	//	fclose(list);
}