#ifndef BLAHLOG
#define BLAHLOG
#include <queue>
#include <stack>
#include <iostream>


class Msg{
	std::string message;
	int line;
	std::stack<Msg> trace;
	public:
	short lvl;
	Msg(int line,std::string message,int lvl):line(line),message(message),lvl(lvl){}
	Msg(int line,std::string message,int lvl,std::stack<Msg> trace):line(line),message(message),lvl(lvl),trace(trace){}
	void print(std::ostream&) const;
	bool operator<(const Msg m) const{
		return line>m.line;
	}

};

class Log{
	static std::priority_queue<Msg> messages;
	public:
	void static add(Msg m){
		messages.push(m);
	}
	void static print(std::ostream& os){
		while(!messages.empty()){
			messages.top().print(os);
			messages.pop();
		}
	}
};

#endif
