#ifndef REGDESC
#define REGDESC
#include "blahsymtable.h"


class RegDescriptor{

	public:
		std::string name;
		bool isfloat;
		std::list<TVar*> vars;
		RegDescriptor(std::string name,bool isfloat):name(name),isfloat(isfloat){};
};

#endif
