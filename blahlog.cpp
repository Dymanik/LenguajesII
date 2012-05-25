#include "blahlog.h"
#include <string>

void Msg::print(std::ostream &os) const{
	std::string s;
	switch(lvl){
		case 0:
			s="\tIn ";
			break;
		case 1:
			s="Warning: ";
			break;
		case 2:
			s="Error: ";
			break;
	}

	os<<s<<message;
	if(line>0){
		os<<" on line "<<line<<":";
	}
	os<<std::endl;
}

/*
void Log::add(Msg m){
	messages.push(m);
}

void static Log::print(std::ostream& os){
	while(!messages.empty()){
		messages.top().print(os);
		messages.pop();
	}
}
*/
