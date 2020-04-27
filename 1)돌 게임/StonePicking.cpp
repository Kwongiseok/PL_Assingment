#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <vector>

using namespace std;
double INF = 9999;

struct Node {
	int alpha;
	int beta;
	Node * left_node;
	Node * right_node;

	int number;
};                              // 노드 생성

bool issosu(int val)
{
	int half = val/2+1;

	for(int i=2;i<half;i++)
	{
		if(val%i==0)
			return false;
	}
	return true;
}

bool isterminal(struct Node * np)
{
	if(np->right_node==NULL && np->left_node==NULL)
		return true;
	else
		return false;
}
struct Node * create_node(int number)
{
	struct Node * new_node = (Node *)malloc(sizeof(Node));
	new_node -> left_node = NULL;
	new_node -> right_node = NULL;
	new_node -> number = number;

	return new_node;
};                               // 새로운 노드 생성

void AppendNode(struct Node * parent, struct Node * child)
{
	if(parent->left_node==NULL)
	{
		parent->left_node = child;
	}
	else
	{
		struct Node * tmp = parent->left_node;

		while (tmp->right_node != NULL)
		{
			tmp=tmp->right_node;
		}
		tmp->right_node = child;
	}
};                                          // 노드 추가

void make_map(int list_stones[], int last_num,struct Node * parent_node,int depth,int length)
{

	int num = length-1;
	int tmp_list [num] = {0,};

	for(int i=0; i<length;i++)
	{

		if ((list_stones[i]%last_num == 0) || (last_num%list_stones[i] ==0))
				{
					struct Node * tmp_node = create_node(list_stones[i]);
					AppendNode(parent_node, tmp_node);
					int tmp=list_stones[i];
					for (int c=0; c<i+1; c++)
						tmp_list[c] = list_stones[c];
					for(int j=i; j<num;j++)          //num = length - 1
						tmp_list[j] = list_stones[j+1];
					if(depth+1!=2)
						make_map(tmp_list,tmp,tmp_node,depth+1,num);
				}

	}


}                                     //max depth = 2                               // 이진트리로 게임 전체판 구현

double evaluation(int list_stones[], int last_num) // bool 플레이어랑 last_num =0 으로 승패 가릴 수 있을듯 //
{
	int cnt = 0;
	for(int i=0; i<sizeof(list_stones);i++)
	{
		if (list_stones[i] == 1 && last_num != 1)
			return 0;
	}
	//list_stones 에서 last_num을 뺀경우
	for (int i=0; i<sizeof(list_stones);i++)
	{
		if(list_stones[i]==last_num)
		{
			for(int j=i ; j<sizeof(list_stones)-1;j++)
				list_stones[j]=list_stones[j+1];
		}
	}
	if(last_num == 1)
	{
		for(int i=0; i<sizeof(list_stones)-1;i++) // 개수가 하나적어져서 그걸 반영
			{
				if(list_stones[i] != 0)
					cnt+=1;
			}
		if(cnt%2==0)
			return -0.5;
		else
			return 0.5;
	}
	else if(issosu(last_num))
	{
		for(int i=0; i<sizeof(list_stones);i++)
		{		if(list_stones[i]%last_num == 0)
					cnt++;
		}
		if(cnt%2==0)
			return -0.7;
		else
			return 0.7;
	}
	else
	{
		for(int i=0; i<sizeof(list_stones);i++)
		{
			if(issosu(list_stones[i]))
				if(last_num%list_stones[i])
					cnt++;
		}
		if(cnt%2==0)
			return -0.6;
		else
			return 0.6;
	}
}

double alphabeta(struct Node * np,int depth,double alpha,double beta,bool maximizingPlayer,int list_stones[],int last_num, int *pn,int length, int *cnt)
{

	struct Node * current = np->left_node;

	int num = length-1;
	*pn = -1;

	int tmp_list [num] = {0};

	double v;
	if (( depth==0||isterminal(np)) && maximizingPlayer)
	{
		return evaluation(list_stones,last_num);
	}
	else if((depth==0 || isterminal(np)) && ~maximizingPlayer)
	{
		return -evaluation(list_stones,last_num);
	}
	if (maximizingPlayer)
	{	*pn = current->number;
		v=-INF;
		while(true)
		{
			for(int i=0;i<length;i++)
				{
					tmp_list[i] = list_stones[i];
				if(list_stones[i]==current->number)
				{

					for(int j=i;j<length-1;j++)
					{
						tmp_list[j] = list_stones[j+1];
					}

				}
				}
			v = max(v,alphabeta(current,depth-1,alpha,beta,false,tmp_list,current->number,pn,num,cnt));
				if(v > alpha)
				{
					*pn = current->number;
					*cnt = *cnt+1;
				}
				alpha=max(alpha,v);

					if(alpha>=beta) //B -cut off
					{
						return v;
					}
					if(current->right_node == NULL)
						break;
			current = current->right_node;
		}
		return v;
	}

	else
	{	*pn=current->number;
		v=INF;
		while(true)
		{
			for(int i=0;i<length;i++)
				{
					tmp_list[i] = list_stones[i];

				if(list_stones[i]==current->number)
				{

					for(int j=i;j<length-1;j++)
					{
						tmp_list[j] = list_stones[j+1];
					}

				}

				}
			v = min(v,alphabeta(current,depth-1,alpha,beta,true,tmp_list,current->number,pn,num,cnt));
				if(beta > v)
				{
					*pn = current->number;
					 *cnt = *cnt+1;
				}
				if(alpha>=beta) //A -cut off
							{
								return v;
							}
				beta=min(beta,v);
				if (current->right_node==NULL)
					break;
			current = current->right_node;
		}
		return v;
	}
	*pn = current->number;
}

int main()
{
	int num_of_stones = 0;
	int num_of_taken_stones = 0;
	int last_num = 0;
	int alpha = -INF;
	int beta = INF;
	int depth = 2;
	int  PN = -1;
	bool maximizingPlayer;
	int cnt = 1;
	string strInput;	// 띄어쓰기 기준으로 N개의 수를 입력받을 String
	vector<int> vecNum;


	cout << "play ";

	// 숫자 입력 받기
	getline(cin, strInput);

	// 문자열 출력

	string strNum = "";	// 각각의 숫자를 저장할 임시 String
	for(int i = 0; i < strInput.length(); i++)
	{
		if(strInput.at(i) == ' ')
		{

			vecNum.push_back( atoi(strNum.c_str()) );
			strNum = "";
		}
		else
		{
			// 띄어쓰기가 나올 때까지 문자 더함
			strNum += strInput.at(i);
			continue;
		}
	}
	vecNum.push_back( atoi(strNum.c_str()) );	// 마지막 숫자도 벡터에 추가

	num_of_stones = vecNum[0];
	num_of_taken_stones = vecNum[1];
	int list_of_taken_stones[num_of_taken_stones] ={0,};


	if(vecNum[1]!=0)
	{
		for(int i=0;i<num_of_taken_stones;i++)
		{
			list_of_taken_stones[i] = vecNum[i+2];
		}
		last_num = list_of_taken_stones[num_of_taken_stones-1];
	}



	int list_stones[num_of_stones] = {0};


	for(int i=0; i<num_of_stones; i++)
		list_stones[i] = i+1;



	for(int i=0; i<num_of_taken_stones;i++)
	{
		for(int j=0;j<sizeof(list_stones)/sizeof(int);j++)

		{
			if (list_of_taken_stones[i]==list_stones[j])
				{
				for(int c = j;c<(sizeof(list_stones)/sizeof(int)-1);c++)
					list_stones[c]=list_stones[c+1];
				list_stones[sizeof(list_stones)/sizeof(int)-1]=0;
				}
		}
	}

	int T_list_stones[num_of_stones-num_of_taken_stones] = {0,};

	for(int i=0;i<sizeof(T_list_stones)/sizeof(int);i++)
		T_list_stones[i] = list_stones[i];


	struct Node * parent = create_node(0);

	int length = sizeof(T_list_stones)/sizeof(int);

	make_map(T_list_stones,last_num,parent,0,length);

	if(num_of_taken_stones==0 || num_of_taken_stones%2==0)
		maximizingPlayer = true;
	else
		maximizingPlayer = false;


	double cal = 0;
	alphabeta(parent,2,alpha,beta,maximizingPlayer,list_stones,last_num,&PN,length,&cnt);

	cout <<"Best Move : " <<PN <<endl;
	int tmp = cnt;

	while (PN != -1)
	{

		num_of_taken_stones++;
		if(num_of_taken_stones==0 || num_of_taken_stones%2==0)
			maximizingPlayer = true;
		else
			maximizingPlayer = false;

		struct Node * parent = create_node(0);
		last_num = PN;
		length--;
		int second_list[length] ={0,};


		for(int i=0;i<length+1;i++)
			if(T_list_stones[i] == last_num)
			{
				for(int j=0; j<i+1;j++)
					second_list[j]=T_list_stones[j];
				for(int c=i;c<length;c++)
					second_list[c] = T_list_stones[c+1];

			}
		for(int i=0;i<length+1;i++)
			T_list_stones[i] = 0;
		for(int i=0;i<length;i++)
			T_list_stones[i] = second_list[i];

		make_map(second_list,PN,parent,0,length);


		alphabeta(parent,2,alpha,beta,maximizingPlayer,second_list,PN,&PN,length,&cnt);


		if(PN==-1)
		{
			if(maximizingPlayer)
				cal = -1.0;
			else
				cal = 1.0;
		break;
		}


	}

	cout.setf(ios::fixed);
	cout.precision(1);
	cout << "Calculated Value : "<< cal << endl;;
	cout <<"Number of visited node : "<<tmp <<endl;
	cout <<"Max depth : " <<depth << endl;


	return 0;

}
