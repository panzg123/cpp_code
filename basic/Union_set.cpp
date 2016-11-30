#define MAX 1001   //�Լ��������ֵ
 
// father[x]��ʾx�ĸ��ڵ�
int father[MAX];
// rank[x]��ʾx����
int rank[MAX];
 
// ��ʼ��
void Make_Set(int n)
{
    for(int i=1; i<=n; ++i)
    {
        father[i] = i;
        rank[i] = 0;
    }
}
 
// ����
int Find_Set(int x)
{
    if(x != father[x])
        x = Find_Set(father[x]); //·��ѹ��
    return x;
}
 
// �ϲ�
void Union(int x, int y)
{
    x = Find_Set(x);
    y = Find_Set(y);
    if(x == y)  // x,y��ͬһ������
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
//hd1232 ��ͨ���� ʵ��
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