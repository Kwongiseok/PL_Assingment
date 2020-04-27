#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#pragma warning(disable:4996)
#define MAX_L 300

void calc(int txt1, int txt2, char* operator);
int isStringInt(char* str); // isStringInt() 함수는 문자열이 정수인지를 알아내주는 함수 http://mwultong.blogspot.com/2007/06/c-string-is-double.html 에서 참조했다.
void push(int value);
int pop();
void file_load();

//void Toy_language(char* str, struct Node* tmp_node); //토큰화해서 트리구조로 저장해주는 역할을 한다.
//void postorder(Node* node_tmp);
void calc(int txt1, int txt2, char* operator);

int cnt_1 = 0; // '(' 의 개수
int cnt_2 = 0; // ')' 의 개수

char* input_data[MAX_L]; // 파일로부터 한줄 한줄 읽을 때
int line_num; // 들어온 명령어 개수 세줌

typedef struct Node
{
	struct Node* left_node;
	struct Node* right_node;
	char* txt;
}Node;
void postorder(Node* node_tmp);
int Toy_language(char* str, Node* tmp_node);
void interactive_mode(char*, Node *);
Node* node_list[MAX_L]; //명령어 한 줄당 한개의 노드 테이블이 필요함 
Node* tmp; //전역 변수 선언

/* 스택 구현 */
int stack[50];
int top = -1;
void push(int value)  // stack의 크기가 입력크기보다 클 것이기때문에 꽉찼을 경우에는 구현안함
{
	stack[++top] = value;
}

int pop()
{
	return stack[top--];
}

int isStringInt(char* str) // isStringInt() 함수는 문자열이 정수인지를 알아내주는 함수 http://mwultong.blogspot.com/2007/06/c-string-is-double.html 에서 참조했다.
{
	size_t size = strlen(str);
	if (size == 0) return 0; //0바이트 문자열 숫자 아니다.
	for (int i = 0; i < (int)size; i++)
	{
		if (str[i] < '0' || str[i] > '9') return 0; // 알파벳 등이 오면 숫자가 이낟.
	}
	return 1; //이외에는 숫자
}

void file_load()
{
	char file_name[50];
	printf("파일명을 입력하세요 >> ");
	scanf("%s", file_name);

	FILE* fp = fopen(file_name, "r");
	if (fp == NULL)
	{
		printf(" 에러 : 해당하는 파일명이 없습니다.\n"); return 0;
	}
	char c; // 임시저장용
	char tmp[100];
	int index = 0; // 문자 받은 거 세주는 역할
	int data_index = 0;  // input_data 의 인덱스역할
	line_num = 0;
	while ((c = fgetc(fp)) != EOF)
	{
		if (c == '\n') { //개행 문자라면 ,
			tmp[index] = '\0'; //문자열 처리
			index = 0;
			input_data[data_index] = (char*)malloc(strlen(tmp));
			strcpy(input_data[data_index], tmp);
			data_index++;
			line_num++;
		}
		else
		{
			tmp[index++] = c;
		}
	}

	fclose(fp);
}



int Toy_language(char* str, Node* tmp_node)
{
	int out;
	char a[100];
	char* context; // 공백으로 나누었을 때 뒤에 부분 저장
	char* tmp;
	strcpy(a, str);
	tmp = strtok_s(a, " ", &context); // 임시저장 + context 안에 공백이 없으면 마지막 문장이 된다.
	tmp_node->right_node = NULL; // 트리를 좌측으로 밀어내기 위해서
	//tmp_node->left_node = (char *)malloc(sizeof(char));
	if (tmp == NULL)
		return -1;
	if (tmp[0] == '(') { //명령문은 무조건 앞에 '(' 가 붙는다.
		tmp = tmp + 1;
		if (strcmp(tmp, "MINUS") == 0 || strcmp(tmp, "IF") == 0)
		{ 
			tmp_node->txt = (char*)malloc(sizeof(char));
			strcpy(tmp_node->txt, tmp); //값을 넣어준다.
			tmp_node->left_node = (Node*)malloc(sizeof(Node));
			out = Toy_language(context, tmp_node->left_node); // 재귀끝나고 오류일 때 전달
			if (out != 0)
				return out;
		}
		else
		{
			//printf("에러 : Undefined ");			
			return -10000; //-10000이 에러 코드이다.
		}
	}
	else if (isStringInt(tmp)) //0이상의 정수일 때
	{
		tmp_node->txt = (char*)malloc(sizeof(char));
		strcpy(tmp_node->txt, tmp);
		tmp_node->left_node = (Node*)malloc(sizeof(Node));
		Toy_language(context, tmp_node->left_node);
	}
	
	else if (tmp[0] == '-' && isdigit(tmp[1])) // 음의 정수 일 떄
	{
		tmp_node->txt = (char*)malloc(sizeof(char));
		strcpy(tmp_node->txt, tmp);
		tmp_node->left_node = (Node*)malloc(sizeof(Node));
		Toy_language(context, tmp_node->left_node);

	}
	else if(isdigit(tmp[0])&&tmp[strlen(tmp)-1]==')')  // 괄호와 같이 끼어져있는 숫자
	{
		int i = 0; char c[10]; char tmp_c[10]; // ')' 가 오기전까지 숫자를 받기 위해서
		if (strstr(context, " ") == NULL) //context 즉 뒷문장이 이제 하나남았을 경우
		{
			while (tmp[i] != ')' && isdigit(tmp[i]))
			{
				c[i] = tmp[i];
				i++;
			}
			c[i] = '\0'; //문자열 처리 
			tmp_node->txt = (char*)malloc(sizeof(char));
			strcpy(tmp_node->txt, c);				

			i = 0; //다시 초기화
			while (context[i] != ')' && isdigit(context[i]))
			{
				tmp_c[i] = context[i];
				i++;
			}

			tmp_c[i] = '\0'; //문자열 처리 
			tmp_node->left_node = (Node* )malloc(sizeof(Node));
			tmp_node->left_node->txt = (char*)malloc(sizeof(char));
			strcpy(tmp_node->left_node->txt, tmp_c);
						
			tmp_node->left_node->left_node = (Node* )malloc(sizeof(Node)); //맨 마지막 노드는 자식 노드들을 null로 초기화시켜준다
			tmp_node->right_node = (Node* )malloc(sizeof(Node));
			tmp_node->left_node->right_node = (Node*)malloc(sizeof(Node));
			tmp_node->left_node->right_node = NULL; tmp_node->right_node = NULL; tmp_node->left_node->left_node = NULL;
		}

		else { //뒷 문장이 더 남은 경우 똑같이 하되 재귀를 돌려줘야 된다.
			while (tmp[i] != ')' && isdigit(tmp[i]))
			{
				c[i] = tmp[i];
				i++;
			}
			c[i] = '\0'; //문자열 처리 
			tmp_node->txt = (char*)malloc(sizeof(char));
			strcpy(tmp_node->txt, c);
			
				
			tmp_node->left_node = (Node*)malloc(sizeof(Node));
			Toy_language(context, tmp_node->left_node);

		}
	}
	else if (tmp[0] == '-' && tmp[1] == '-') // --가 연속으로 올때
	{
		//printf("에러 : \'-\' 는 한번만 올 수 있습니다. ");  
		return -9999; // -9999 가 에러 코드이다.
	}
	else if (strstr(tmp, ".") != NULL) // 실수라면
	{
		//printf("에러 : 실수는 입력 할 수 없습니다."); 
		return -9998; // 9998 이 에러 코드이다.
	}
	else
	{
		//printf("에러 : 올바른 값을 넣지 않았습니다. "); 
		return -9997; // 9997 이 에러 코드이다
	}
	return 0;
}

void print_inst() // 입력받은 명령어를 출력한다.
{
	printf("파일 내용은\n");
	printf("------------------------\n");
	for (int i = 0; i < line_num; i++)
		printf("%s\n", input_data[i]);
	printf("------------------------\n");
	printf("입니다. \n\n");
	//명령어 한줄 입력받은 거 그냥 출력한다.
}

void postorder(Node* node_tmp) //후위순회로 숫자를 뽑고, 연산을 진행해준다.
{
	if (node_tmp != NULL)
	{
		postorder(node_tmp->left_node);
		postorder(node_tmp->right_node);
		if (isStringInt(node_tmp->txt) || node_tmp->txt[0] == '-' && isdigit(node_tmp->txt[1])) //정수 일 때
			push(atoi(node_tmp->txt));
		else if (strcmp(node_tmp->txt, "IF")==0 || strcmp(node_tmp->txt,"MINUS")==0) //IF 나 MINUS 일 경우
		{
			int t1 = pop(); int t2 = pop();
			calc(t1, t2, node_tmp->txt);
		}


	}
}

void calc(int txt1, int txt2, char* operator)
{
	if (strcmp(operator,"IF")==0) // IF일 경우
	{
		if (txt1 <= 0) // IF t1 t2 에서 t1이 음수이면 0 값을 갖는다.
			push(0);
		else if (txt1 > 0)
			push(txt2);
	}

	else if (strcmp(operator,"MINUS")==0) // MINUS 일 경우
	{
		int tmp = txt1 - txt2;
		push(tmp);
	}
}
void interactive_mode() // 사용자가 직접 입력할 때
{
	cnt_1 = 0; cnt_2 = 0;
	char txt[50];
	tmp = (Node*)malloc(sizeof(Node));
//	tmp->txt = (char*)malloc(sizeof(char));
	int Toy_result;
	int result;
	printf("문장을 입력하시오.\n");
	printf("------------------------\n");
	getchar(); // 버퍼 비우기
	scanf("%[^\n]s", txt); //공백 포함 문자를 받아들임
	char* init_data = (char*)malloc(sizeof(char));
	strcpy(init_data, txt);
	printf("------------------------\n");
	Toy_result = Toy_language(init_data, tmp);
	if (Toy_result == 0) {  //Toy_language 의 리턴값이 0이 아니면 오류가 발생했다는 것이다.
		postorder(tmp);
		result = pop();
		printf("결과 : %d\n", result);
	}
	for (int c = 0; c < strlen(txt); c++) // 괄호 개수를 세워준다. // '(' 나 ')' 가 틀렸는지 확인하기 위해서.
	{
		if (txt[c] == '(') cnt_1++;
		else if (txt[c] == ')') cnt_2++;
	}
	if (cnt_1 > cnt_2)
		Toy_result = -10001; // '(' 개수가 더많아서 ')'가 모자를 떄
	else if (cnt_1 < cnt_2)
		Toy_result = -10002; // ')' 개수가 더 많아서 '('가 모자를 때
	else if (Toy_result == -10000) printf("에러 : Undefined \n");
	else if (Toy_result == -9999) printf("에러 : \'-\' 는 한번만 올 수 있습니다. \n");
	else if (Toy_result == -9998) printf("에러 : 실수는 입력 할 수 없습니다.\n");
	else if (Toy_result == -9997) printf("에러 : 올바른 값을 넣지 않았습니다. \n");
	else if (Toy_result == -10001) printf("에러 : \'(\' 의 위치가 잘못되었습니다\n");
	else if (Toy_result == -10002) printf("에러 : \')\' 의 위치가 잘못되었습니다\n");
	printf("------------------------\n");

}
int main(int args, char* arg[])
{
	int user;
	int Toy_result;
	
	char *filename = (char *)malloc(sizeof(char)); //파일이름
	char file_tmp[50];
	while (1)
	{
		int result[50] = { NULL };
		printf("========================\n");
		printf("1. File Load\n");
		printf("2. Interactive Mode\n");
		printf("3. Exit\n");
		printf("========================\n");
		printf("메뉴를 선택하세요 >> ");
		scanf("%d", &user);
		if (user == 1)
		{
			
			file_load();

			for (int i = 0; i < line_num; i++)
			{
				cnt_1 = 0; cnt_2 = 0;

				node_list[i] = (Node*)malloc(sizeof(Node));
				Toy_result = Toy_language(input_data[i], node_list[i]);

				for (int c = 0; c < strlen(input_data[i]); c++) // 괄호 개수를 세워준다.
				{
					if (input_data[i][c] == '(') cnt_1++;
					else if (input_data[i][c] == ')') cnt_2++;
					
				}
				if (cnt_1 != cnt_2) // 괄호에의한 에러 처리
					Toy_result = -1; //임시로 -1로 저장

				if (Toy_result == 0)
				{
					postorder(node_list[i]);
					result[i] = pop();
				}             //마지막 스택에 남아있는 것은 연산 결과값이므로 result에 넣어준다.
				else result[i] = Toy_result; //에러 코드를 넣어준다

				if (cnt_1 > cnt_2)
					result[i] = -10001; // '(' 개수가 더많아서 ')'가 모자를 떄
				else if (cnt_1 < cnt_2)
					result[i] = -10002; // ')' 개수가 더 많아서 '('가 모자를 때

			}
			print_inst();
			for (int i = 0; i < line_num; i++)
			{
				if (result[i] == -10000) printf("에러 : Undefined \n");
				else if (result[i] == -9999) printf("에러 : \'-\' 는 한번만 올 수 있습니다. \n");
				else if (result[i] == -9998) printf("에러 : 실수는 입력 할 수 없습니다.\n");
				else if (result[i] == -9997) printf("에러 : 올바른 값을 넣지 않았습니다. \n");
				else if (result[i] == -10001) printf("에러 : \'(\' 의 위치가 잘못되었습니다\n");
				else if (result[i] == -10002) printf("에러 : \')\' 의 위치가 잘못되었습니다\n");
				else printf("결과 : %d\n", result[i]);
			}
			
		}
		else if (user == 2)
		{	
			interactive_mode(); // 사용자가 직접입력할 때 함수 사용
		}
		else if (user == 3) // 3일 때 게임 종료
		{
			printf("\n프로그램을 종료합니다.");
			break;
		}
		else
		{
			printf("MENU 1,2,3 중에서만 선택하십시오");
		}
	}

	return 0;
}
