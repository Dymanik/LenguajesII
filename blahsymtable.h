#ifndef BLAHSYMTABLE
#define BLAHSYMTABLE
#include <string>
#include <vector>
#include <list>
#include <unordered_map>
#include <iostream>
class TVar;

typedef std::list<int> Sizelist; 
typedef std::vector<std::pair<std::string,TVar*>> Fields;

class Tuple{
	public:
		std::string id;
		int scope;
		Tuple(std::string id,int scope):id(id),scope(scope){}
		bool operator==(const Tuple a) const{
			return id==a.id && scope==a.scope;
		}
		void print(std::ostream& os,int depth=0) const;
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
		virtual void print(std::ostream&,int d=0)=0;
		virtual ~TElement(){}
};

//TYPES
class TType: public TElement {
	public:
		unsigned size;
		unsigned alignment;
		bool isArr;
		bool isNumeric;
		bool isStruct;
		bool basic;

		TType(std::string name,unsigned size,unsigned alignment,bool basic=false, bool numeric=false, bool struc=false,bool isArr=false):TElement(name,true),size(size),alignment(alignment),basic(basic),isNumeric(numeric),isStruct(struc),isArr(isArr){};

        bool operator==(const TType &t2)const{
            return name == t2.name;
        }

		void print(std::ostream&,int d=0);
};


//Basic Types
class TInteger: public TType{
	public:
		TInteger():TType("Integer",4,4,true,true){};
};

class TBool: public TType{
	public:
		TBool():TType("Bool",1,1,true){};
};
class TFloat: public TType{
	public:
		TFloat():TType("Float",4,4,true,true){};
};

class TError: public TType{
	public:
		TError():TType("error",0,1,true){};
};

class TChar: public TType{
	public:
		TChar():TType("Char",1,1,true,true){};
};

class TUndef: public TType{
	public:
		TUndef():TType("undef",0,1,true){};
};

class TVoid: public TType{
	public:
		TVoid():TType("Void",0,1,true){};
};

//Complex types
class Field{
	public:
		TType& type;
		std::string& name;
		Field(TType& type,std::string& name):type(type),name(name){}
};

class TStructured: public TType{
	protected:
	virtual void addField(TVar* type,std::string name)=0;
	public:
		std::unordered_map<std::string,TVar*> fields;
		bool isUnion;
		TStructured(std::string name,bool isUnion=false):TType(name,0,0,false,false,true),isUnion(isUnion){}
		TType* accessType(std::string name);
		void print(std::ostream&,int d=0);
};

class TRegister: public TStructured{
	public:
		TRegister(std::string name,Fields fieldl);
		void addField(TVar* var, std::string name);
};
class TUnion: public TStructured{
	public:
		TUnion(std::string name,Fields fieldl);
		void addField(TVar* var,std::string name);
};

class TArray: public TType{
	public:
		TType* type;
		int length;
		TArray(TType* typ, int length):TType("Arrayof"+typ->name,typ->size*length,false,false,false,true),type(typ),length(length){}

		TArray(TType*, Sizelist);
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
		void print(std::ostream&,int d=0);
};

class TFunc: public TElement{
	public:
		TType& type;
		std::vector<TType*> args;
		TFunc(std::string name, TType& type, std::vector<TType*> args):TElement(name),type(type),args(args){}
		void print(std::ostream&,int d=0);
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


		void printVars(std::ostream& os);
		void printFuncs(std::ostream& os);
		void printTypes(std::ostream& os);

		void print(std::ostream& os);
};
#endif
