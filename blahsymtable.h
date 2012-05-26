#ifndef BLAHSYMTABLE
#define BLAHSYMTABLE
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
class TVar;

typedef std::list<int> sizelist; 

class Tuple{
	public:
		std::string id;
		int scope;
		Tuple(std::string id,int scope):id(id),scope(scope){}
		bool operator==(const Tuple a) const{
			return id==a.id && scope==a.scope;
		}
};

namespace std{
	template<> class hash<Tuple>{
		public:
			size_t operator()(const Tuple &s) const { 
				return hash<string>()(s.id)+s.scope;
			}
	};
};

class TElement{
	public:
		std::string name;
		bool isType;
		TElement(std::string name,bool isType=false):name(name),isType(isType){}
		virtual ~TElement(){}
};

//TYPES
class TType: public TElement {
	public:
		unsigned size;
		bool isArr;
		bool isNumeric;
		bool isStruct;
		bool basic;

		TType(std::string name,unsigned size,bool basic=false, bool numeric=false, bool struc=false,bool isArr=false):TElement(name,true),size(size),basic(basic),isNumeric(numeric),isStruct(struc),isArr(isArr){};

        bool operator==(const TType &t2)const{
            return name == t2.name;
        }
};


//Basic Types
class TInteger: public TType{
	public:
		TInteger():TType("Integer",4,true,true){};
};

class TBool: public TType{
	public:
		TBool():TType("Bool",1,true){};
};
class TFloat: public TType{
	public:
		TFloat():TType("Float",4,true,true){};
};

class TError: public TType{
	public:
		TError():TType("Error",0,true){};
};

class TChar: public TType{
	public:
		TChar():TType("Char",1,true,true){};
};

class TUndef: public TType{
	public:
		TUndef():TType("Undef",0,true){};
};

class TVoid: public TType{
	public:
		TVoid():TType("Void",0,true){};
};

//Complex types
class Field{
	public:
		TType& type;
		std::string& name;
		Field(TType& type,std::string& name):type(type),name(name){}
};

class TStructured: public TType{
	public:
		std::unordered_map<std::string,TVar*> fields;
		bool isUnion;
		TStructured(std::string name,unsigned size,bool isUnion=false):TType(name,size,false,false,true),isUnion(isUnion){}
		TType* accessType(std::string name);
		virtual void addField(TVar* type,std::string name)=0;
};

class TRegister: public TStructured{
	public:
		TRegister(std::string name):TStructured(name,0){};
		void addField(TVar* var, std::string name);
};
class TUnion: public TStructured{
	public:
		TUnion(std::string name):TStructured(name,0,true){}
		void addField(TVar* var,std::string name);
};

class TArray: public TType{
	public:
		TType* type;
		int length;
		TArray(TType* typ, int length):TType("Arrayof"+typ->name,typ->size*length,false,false,false,true),type(typ),length(length){}

		TArray(TType*, sizelist);
        bool operator==(const TArray &t2)const{
            return *type == *t2.type && length == t2.length;
        }

};


//VARS AND FUNCTIONS
class TVar: public TElement{
	public:
		TType& type;
		int offset;
		TVar(std::string name,TType& type):type(type),TElement(name){}
};

class TFunc: public TElement{
	public:
		TType* type;
		std::vector<TType*> args;
		TFunc(std::string name, TType* type, std::vector<TType*> args):TElement(name),type(type),args(args){}
};

//SYMTABLE
class Symtable {
	private:
		std::unordered_map<Tuple,TVar*> vars;
		std::unordered_map<std::string,TFunc*> functions;
		std::unordered_map<std::string,TType*> types;
		std::list<int> scopeStack;
		int nextscope;
	public:
		Symtable():nextscope(1){scopeStack.push_front(0);}
		
		void insert(TType*);
		void insert(TVar*);
		void insert(TFunc*);
		
		TVar* lookupVar(const std::string name);
        TVar* lookupLocalVar(const std::string name);
		TFunc* lookupFunc(const std::string name, const std::vector<TType*> args);
        TType* lookupType(const std::string name);

        int begScope(){
			scopeStack.push_front(nextscope);
			nextscope++;
		}

        int endScope(){
			scopeStack.pop_front();
		}

		int resetScope(){
			nextscope=1;
			scopeStack.clear();
			scopeStack.push_front(1);
		}
};
#endif
