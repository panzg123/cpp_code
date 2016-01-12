//kmp table��ʱ�临�Ӷ�ΪO(N)�����ؽ���洢��next��
void kmp_get_next(string s, vector<int>& next)
{
    int length = s.size();
    int j = 1, k = 0;
    next[0] = -1;
    next[1] = 0;
    while (j<length-1)
    {
        if (s[j] == s[k])
        {
            j++;
            k++;
            next[j] = k;
        }
        else if (k == 0)
        {
            j++;
            next[j] = 0;
        }
        else
            k = next[k];
    }
}
//������kmpƥ�䣬ʱ�临�Ӷ�ΪO(N+M)
//@src������
//@start_pos:����ƥ����ʼλ��
//@dest:ģʽ��
//@kmp_table:kmp����kmp_get_next�������
//@return:��������ƥ���λ�ã���ƥ���򷵻�-1
int kmp_match(string src,int start_pos,string dest,const vector<int>& kmp_table)
{
    int i = start_pos;
    int j = 0;
    int res=0;
    while (i<src.length() && j<dest.length())
    {
        if (j == -1 || src[i] == dest[j]) //�ֱ�����1
        {
            i++;
            j++;
        }
        else
            j = kmp_table[j]; //ģʽ������
    }
    if (j == dest.length())
        res = i - dest.length();
    else
        res = -1;
    return res;
}