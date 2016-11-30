#define MAX 1001   //自己设置最大值
 
// father[x]表示x的父节点
int father[MAX];
// rank[x]表示x的秩
int rank[MAX];
 
// 初始化
void Make_Set(int n)
{
    for(int i=1; i<=n; ++i)
    {
        father[i] = i;
        rank[i] = 0;
    }
}
 
// 查找
int Find_Set(int x)
{
    if(x != father[x])
        x = Find_Set(father[x]); //路径压缩
    return x;
}
 
// 合并
void Union(int x, int y)
{
    x = Find_Set(x);
    y = Find_Set(y);
    if(x == y)  // x,y在同一个集合
        return;
    if(rank[x] > rank[y])
        father[y] = x;
    else if(rank[x] < rank[y])
        father[x] = y;
    else
    {
        rank[y]++;
        father[x] = y;
    }
}
//hd1232 畅通工程 实例
int N, M;
int main()
{
    int a, b;
    while(scanf("%d", &N) && N)
    {
        for(int i=1; i<=N; ++i)
            Make_Set(i);
        scanf("%d", &M);
        while(M--)
        {
            scanf("%d %d", &a, &b);
            Union(a, b);
        }
        int cnt = -1;
        for(int i=1; i<=N; ++i)
            if(father[i]==i)
                cnt++;
        printf("%d\n", cnt);
    }
    return 0;
}