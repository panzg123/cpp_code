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

typedef vector<string>::size_type line_no;              //定义行号
typedef set<line_no>::const_iterator line_it;           //行号，即set的迭代器
typedef map<string, shared_ptr<set<line_no> > > wmType; //单词、行号集合的关联容器
typedef wmType::const_iterator wmIter;                  //关联容器迭代器
typedef shared_ptr<set<line_no> > lineType;             //行号集合
typedef set<line_no>::const_iterator lineIter;          //行号集合迭代器

//内联函数，单词复数形式
inline
string make_plural(size_t ctr, const string &word,
const string &ending)
{
	return (ctr > 1) ? word + ending : word;
}

//查询结果保存
class QueryResult 
{
	friend ostream& print(ostream&, const QueryResult&);           //打印查询结果
public:
	QueryResult(string s,                                          //构造函数
		shared_ptr<set<line_no> > p,
		shared_ptr<vector<string> > f) :
		sought(s), lines(p), file(f) { }
	set<line_no>::size_type size() const  { return lines->size(); } //行数量
	line_it begin() const { return lines->begin(); }                //首部
	line_it end() const   { return lines->end(); }                  //尾部
	shared_ptr<vector<string> > get_file() { return file; }         //文本
private:
	string sought;                     //查询单词
	shared_ptr<set<line_no> > lines;   //出现的行号
	shared_ptr<vector<string> > file;  //文本信息
};

class TextQuery 
{
public:
	TextQuery(ifstream&);
	QueryResult query(const string&) const;
private:
	shared_ptr<vector<string> > file;            // 文本信息
	map<string,shared_ptr<set<line_no> > > wm;   // 单词、行号集合的映射
	static string cleanup_str(const string&);    // 将单词转换小写

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
	//逐行读取文本
	while (getline(is,text))
	{
		file->push_back(text);              //保存此行文本
		int n = file->size() - 1;           //获取行号
		istringstream line(text);         
		string word;
		while (line >> word)                //获取每个单词
		{
			//如果单词不在vm中，则在vm中添加一项
			lineType &lines = wm[word];     //lines是一个引用，此处是一个shared_ptr
			if (!line)
				lines.reset(new set<line_no>);//分配一个set
			lines->insert(n);              //将行号保存到set中
		}
	}
}

QueryResult
TextQuery::query(const string &sought) const
{
	//如果未找到，返回一个空的set
	static lineType nodata(new set<line_no>);
	//查询
	wmIter loc = wm.find(cleanup_str(sought));
	if (loc == wm.end())
		return QueryResult(sought, nodata, file); //未找到
	else
		return QueryResult(sought, loc->second, file);
}


ostream &print(ostream & os, const QueryResult &qr)
{
	//打印单词出现的次数
	os << qr.sought << " occurs " << qr.lines->size() << " "
		<< make_plural(qr.lines->size(), "time", "s") << endl;

	//打印行号，打印行文本
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
		if (!(cin >> s) || s == "q")   //出入查询单词
			break;
		print(cout, tq.query(s)) << endl; //打印查询结果
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