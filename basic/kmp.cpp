//kmp table，时间复杂度为O(N)，返回结果存储在next中
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
//描述：kmp匹配，时间复杂度为O(N+M)
//@src：主串
//@start_pos:主串匹配起始位置
//@dest:模式串
//@kmp_table:kmp表，用kmp_get_next函数求得
//@return:返回主串匹配的位置，无匹配则返回-1
int kmp_match(string src,int start_pos,string dest,const vector<int>& kmp_table)
{
    int i = start_pos;
    int j = 0;
    int res=0;
    while (i<src.length() && j<dest.length())
    {
        if (j == -1 || src[i] == dest[j]) //分别增加1
        {
            i++;
            j++;
        }
        else
            j = kmp_table[j]; //模式串右移
    }
    if (j == dest.length())
        res = i - dest.length();
    else
        res = -1;
    return res;
}