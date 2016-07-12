// C++ Primer,chapter 12, TextQuery
#include <iostream>
#include <memory>
#include <vector>
#include <set>
#include <string>
#include <fstream>
#include <sstream>
#include <map>

using namespace std;

typedef vector<string>::size_type line_no;              //�����к�
typedef set<line_no>::const_iterator line_it;           //�кţ���set�ĵ�����
typedef map<string, shared_ptr<set<line_no> > > wmType; //���ʡ��кż��ϵĹ�������
typedef wmType::const_iterator wmIter;                  //��������������
typedef shared_ptr<set<line_no> > lineType;             //�кż���
typedef set<line_no>::const_iterator lineIter;          //�кż��ϵ�����

//�������������ʸ�����ʽ
inline
string make_plural(size_t ctr, const string &word,
const string &ending)
{
	return (ctr > 1) ? word + ending : word;
}

//��ѯ�������
class QueryResult 
{
	friend ostream& print(ostream&, const QueryResult&);           //��ӡ��ѯ���
public:
	QueryResult(string s,                                          //���캯��
		shared_ptr<set<line_no> > p,
		shared_ptr<vector<string> > f) :
		sought(s), lines(p), file(f) { }
	set<line_no>::size_type size() const  { return lines->size(); } //������
	line_it begin() const { return lines->begin(); }                //�ײ�
	line_it end() const   { return lines->end(); }                  //β��
	shared_ptr<vector<string> > get_file() { return file; }         //�ı�
private:
	string sought;                     //��ѯ����
	shared_ptr<set<line_no> > lines;   //���ֵ��к�
	shared_ptr<vector<string> > file;  //�ı���Ϣ
};

class TextQuery 
{
public:
	TextQuery(ifstream&);
	QueryResult query(const string&) const;
private:
	shared_ptr<vector<string> > file;            // �ı���Ϣ
	map<string,shared_ptr<set<line_no> > > wm;   // ���ʡ��кż��ϵ�ӳ��
	static string cleanup_str(const string&);    // ������ת��Сд

};

string TextQuery::cleanup_str(const string &word)
{
	string ret;
	for (string::const_iterator it = word.begin();it != word.end(); ++it) 
	{
		if (!ispunct(*it))
			ret += tolower(*it);
	}
	return ret;
}

TextQuery::TextQuery(ifstream &is)
	:file(new vector<string>)
{
	string text;
	//���ж�ȡ�ı�
	while (getline(is,text))
	{
		file->push_back(text);              //��������ı�
		int n = file->size() - 1;           //��ȡ�к�
		istringstream line(text);         
		string word;
		while (line >> word)                //��ȡÿ������
		{
			//������ʲ���vm�У�����vm�����һ��
			lineType &lines = wm[word];     //lines��һ�����ã��˴���һ��shared_ptr
			if (!line)
				lines.reset(new set<line_no>);//����һ��set
			lines->insert(n);              //���кű��浽set��
		}
	}
}

QueryResult
TextQuery::query(const string &sought) const
{
	//���δ�ҵ�������һ���յ�set
	static lineType nodata(new set<line_no>);
	//��ѯ
	wmIter loc = wm.find(cleanup_str(sought));
	if (loc == wm.end())
		return QueryResult(sought, nodata, file); //δ�ҵ�
	else
		return QueryResult(sought, loc->second, file);
}


ostream &print(ostream & os, const QueryResult &qr)
{
	//��ӡ���ʳ��ֵĴ���
	os << qr.sought << " occurs " << qr.lines->size() << " "
		<< make_plural(qr.lines->size(), "time", "s") << endl;

	//��ӡ�кţ���ӡ���ı�
	for (lineIter num = qr.lines->begin();num != qr.lines->end(); ++num)
		os << "\t(line " << *num + 1 << ") "<< *(qr.file->begin() + *num) << endl;
	return os;
}


void runQueries(ifstream &infile)
{
	TextQuery tq(infile);
	while (true) 
	{
		cout << "enter word to look for, or q to quit: ";
		string s;
		if (!(cin >> s) || s == "q")   //�����ѯ����
			break;
		print(cout, tq.query(s)) << endl; //��ӡ��ѯ���
	}
}

int main() 
{

	ifstream infile("data.txt");
	if (infile)
		runQueries(infile);
	getchar();
	return 0;
}