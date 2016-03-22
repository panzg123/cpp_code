/*
DP方法求解TSP问题，时间复杂度为O(2^N * N^2)，对于小规模点可以使用
你只需要将路径信息存储在领接矩阵matrix中即可，默认为0节点为起点，回到0节点，如果你需要其他节点做起点，则需要改代码或者自己改领接矩阵
另外，目前只计算最短路径长度，不存储路径信息
*/
#include<vector>
#include <algorithm>
#include <iostream>
using namespace std;

//函数声明
void combine_help(vector<vector<int>> &result, int n, int k, int length, vector<int> &path, int target);
vector<vector<int>> combine(int n, int k);

vector<vector<int>> matrix = {
	{ 0, 3, 6, 7 },
	{ 5, 0, 2, 3 },
	{ 6, 4, 0, 2 },
	{ 3, 7, 5, 0 }
};

//获取排列的总数
inline int get_size(int size)
{
	return 1 << size;
}
//所有状态的数量，状态数目的计算需要减去起点
int state_size = get_size(matrix.size() - 1);
//dp状态
vector<vector<int>> dp(matrix.size(), vector<int>(state_size,INT_MAX));
//所有状态集合枚举，该状态的初始化由init函数负责
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
//todo:还需要代码实现来自动计算所有状态集合，即自动初始化state_set
//init函数主要初始化state_set状态
void init()
{
	state_set.push_back(vector<int>(matrix.size()-1, 0));
	for (int i = 1; i < matrix.size();i++)
	{
		auto ret = combine(matrix.size() - 1, i);
		for each (auto var in ret)
		{
			//填充0，保证matrix.size()-1长度
			var.insert(var.begin(), matrix.size() - 1 - var.size(), 0);
			state_set.push_back(var);
		}
	}
}

//求组合C(n,k),用DFS,从集合[1....n-1]中任选k个数
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
	//选
	path.push_back(k);
	combine_help(result, n, k + 1, length + 1, path, target);
	path.pop_back();
	//不选
	combine_help(result, n, k + 1, length, path, target);
}


//判断集合s中是否存在point
bool is_exist(int point,vector<int> s)
{
	auto ret = find(s.begin(), s.end(),point);
	if (ret == s.end())
		return false;
	else
		return true;
}
//从集合Num中去除第idx元素，返回新集合的索引
int get_idx(vector<int> num,int idx)
{
	int ret = 0;
	num[idx] = 0;
	sort(num.begin(), num.end());
	auto find_ret =  find(state_set.begin(), state_set.end(), num);
	return distance(state_set.begin(),find_ret);
}
//状态转移方程为  d(i, V’) = min { Cik +  d(k, V’-{k}) }
//默认为从0起点，返回0节点，最后的最佳距离保存在dp[0][state_size-1]中
//时间复杂度为O(2^N * N^2)，对于小规模点可以使用
void TSP()
{
	//直接从节点i返回节点0，初始化距离
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
	//从起点0，经过其余节点的路径信息
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
	//输出最短路径长度
	cout << dp[0][state_size-1] << endl;

	return 0;
}