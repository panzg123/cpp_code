/*
DP�������TSP���⣬ʱ�临�Ӷ�ΪO(2^N * N^2)������С��ģ�����ʹ��
��ֻ��Ҫ��·����Ϣ�洢����Ӿ���matrix�м��ɣ�Ĭ��Ϊ0�ڵ�Ϊ��㣬�ص�0�ڵ㣬�������Ҫ�����ڵ�����㣬����Ҫ�Ĵ�������Լ�����Ӿ���
���⣬Ŀǰֻ�������·�����ȣ����洢·����Ϣ
*/
#include<vector>
#include <algorithm>
#include <iostream>
using namespace std;

//��������
void combine_help(vector<vector<int>> &result, int n, int k, int length, vector<int> &path, int target);
vector<vector<int>> combine(int n, int k);

vector<vector<int>> matrix = {
	{ 0, 3, 6, 7 },
	{ 5, 0, 2, 3 },
	{ 6, 4, 0, 2 },
	{ 3, 7, 5, 0 }
};

//��ȡ���е�����
inline int get_size(int size)
{
	return 1 << size;
}
//����״̬��������״̬��Ŀ�ļ�����Ҫ��ȥ���
int state_size = get_size(matrix.size() - 1);
//dp״̬
vector<vector<int>> dp(matrix.size(), vector<int>(state_size,INT_MAX));
//����״̬����ö�٣���״̬�ĳ�ʼ����init��������
vector<vector<int>> state_set;
//= {
//	{ 0, 0, 0 },
//	{ 0, 0, 1 },
//	{ 0, 0, 2 },
//	{ 0, 0, 3 },
//	{ 0, 1, 2 },
//	{ 0, 1, 3 },
//	{ 0, 2, 3 },
//	{ 1, 2, 3 }
//};
//todo:����Ҫ����ʵ�����Զ���������״̬���ϣ����Զ���ʼ��state_set
//init������Ҫ��ʼ��state_set״̬
void init()
{
	state_set.push_back(vector<int>(matrix.size()-1, 0));
	for (int i = 1; i < matrix.size();i++)
	{
		auto ret = combine(matrix.size() - 1, i);
		for each (auto var in ret)
		{
			//���0����֤matrix.size()-1����
			var.insert(var.begin(), matrix.size() - 1 - var.size(), 0);
			state_set.push_back(var);
		}
	}
}

//�����C(n,k),��DFS,�Ӽ���[1....n-1]����ѡk����
vector<vector<int>> combine(int n, int k)
{
	vector<vector<int>> result;
	if (n < k) return result;
	vector<int> path;
	combine_help(result, n, 1, 0, path, k);
	return result;
}
void combine_help(vector<vector<int>> &result, int n, int k, int length, vector<int> &path, int target)
{
	if (k <= n+1 && length == target)
	{
		result.push_back(path);
		return;
	}
	if (k > n) return;
	//ѡ
	path.push_back(k);
	combine_help(result, n, k + 1, length + 1, path, target);
	path.pop_back();
	//��ѡ
	combine_help(result, n, k + 1, length, path, target);
}


//�жϼ���s���Ƿ����point
bool is_exist(int point,vector<int> s)
{
	auto ret = find(s.begin(), s.end(),point);
	if (ret == s.end())
		return false;
	else
		return true;
}
//�Ӽ���Num��ȥ����idxԪ�أ������¼��ϵ�����
int get_idx(vector<int> num,int idx)
{
	int ret = 0;
	num[idx] = 0;
	sort(num.begin(), num.end());
	auto find_ret =  find(state_set.begin(), state_set.end(), num);
	return distance(state_set.begin(),find_ret);
}
//״̬ת�Ʒ���Ϊ  d(i, V��) = min { Cik +  d(k, V��-{k}) }
//Ĭ��Ϊ��0��㣬����0�ڵ㣬������Ѿ��뱣����dp[0][state_size-1]��
//ʱ�临�Ӷ�ΪO(2^N * N^2)������С��ģ�����ʹ��
void TSP()
{
	//ֱ�Ӵӽڵ�i���ؽڵ�0����ʼ������
	for (int i = 1; i < matrix.size();i++)
	{
		dp[i][0] = matrix[i][0];
	}
	for (int i = 1;i<state_size;i++)
	{
		for (int j = 1; j < matrix.size();j++)
		{
			if (!is_exist(j, state_set[i]))
			{
				for (int k = 0; k < matrix.size()-1; k++)
				{
					if (state_set[i][k] != 0)
					{
						int idx = get_idx(state_set[i], k);
						dp[j][i] = min(dp[j][i], matrix[j][state_set[i][k]] + dp[state_set[i][k]][idx]);
					}
				}
			}
		}
	}
	//�����0����������ڵ��·����Ϣ
	for (int k = 0; k < matrix.size() - 1; k++)
	{
		if (state_set[state_size - 1][k] != 0)
		{
			int idx = get_idx(state_set[state_size - 1], k);
			dp[0][state_size-1] = min(dp[0][state_size-1],
			matrix[0][state_set[state_size-1][k]] + dp[state_set[state_size-1][k]][get_idx(state_set[state_size-1], k)]);
		}
	}
}

int main()
{
	init();
	TSP();
	//������·������
	cout << dp[0][state_size-1] << endl;

	return 0;
}