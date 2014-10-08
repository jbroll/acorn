
#include <stddef.h>
#include <string.h>

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

//#include <cstring>


#define Keyword void
#define Param

#define Type_int	1
#define Type_string	2
#define Type_double	3

typedef std::string string;



std::vector<char*> split(char *str, const char* delim) {
    std::vector<char*> list;

    char *here = strtok(str, delim);

    list.push_back(here);

    while ( (here = strtok(NULL, delim)) != NULL ) {
	list.push_back(here);
    }

    return list;
}

std::vector<char> cat(const std::string& filename) 
{
    int  	ch;
    bool	skip = 0;

    std::vector<char> reply;

    int ch0, ch1;

    std::ifstream inp(filename.c_str(), std::ios::in | std::ios_base::binary);


    if ( (ch0 = inp.get()) != -1 && ch0 == 0xFE || ch0 == 0xFF ) {
	if ( (ch1 = inp.get()) != -1  && ((ch0 == 0xFF && ch1 == 0xFE) || (ch0 == 0xFE && ch1 == 0xFF)) ) {
	    skip = 1;


	    if ( ch0 == 0xFE && ch1 == 0xFF ) {
		inp.get();
	    }
	} else {
	    reply.push_back(ch0);
	    reply.push_back(ch1);
	}
    } else {
	reply.push_back(ch0);
    }

    while ( (ch = inp.get()) != -1  && !inp.eof() ) {
	if ( skip ) inp.get();

	reply.push_back(ch);
    }

    return reply;
}



typedef struct _VarMap {
    int type;
    int offset;
} VarMap;

class Scripted {
  public:
    static std::map<std::string, void (ZMX::*)(void)> mtable;
    static std::map<std::string, VarMap> vtable;


    void invoke(const char *method) {
	if ( mtable.count(method) == 1 ) {
	    (this->*mtable[method])();
	}
    }
    void setvar(const char *varname, int set, void *value) {
	if ( vtable.count(varname) == 1 ) {
	    VarMap v = vtable[varname];

	    if ( set ) {
		printf("setvar %s %d %p %p\n", varname, v.offset, ((int *)((char*)this+v.offset)), ((int *) value));
		switch ( v.type ) {
		    case Type_int:	*((int *)((char*)this+v.offset)) = *((int *) value);				break;
		    case Type_string:	*((string *)((char*)this+v.offset)) = (char *) value;				break;
		    case Type_double:	*((double *)((char*)this+v.offset)) = *((double *) value);			break;
		    default : ;
		}
	    } else {
		switch ( v.type ) {
		    case Type_int:	*((int *) value)          = *((int *)((char*)this+v.offset));			break;
		    case Type_string:	(*(const char **) value)  = (*((string *)((char*)this+v.offset))).c_str();	break;
		    case Type_double:	*((double *) value)       = *((double *)((char*)this+v.offset));		break;
		    default : ;
		}
	    }
	}

    }
}

class ZMX : public Scripted { 

    int a;
    int b;

    Param int 	 ik;
    Param string sk;
    Param double dk;

    Keyword fluff () {
	printf("Here\n");
	return;
    }
};

std::map<std::string, void (ZMX::*)(void)> ZMX::mtable = {
#	include "zmx.mtable"
};
std::map<std::string, VarMap> ZMX::vtable = {
#	include "zmx.vtable"
};




int main(int argc, char** argv) {

    ZMX zmx = ZMX();

    std::vector<char*> list = split(&cat(argv[1])[0], "\n");

    for ( auto &i : list ) {
	std::cout << i << std::endl;
    }


    zmx.fluff();
    zmx.invoke("fluff");

    int four = 4;
    int dour = 5;

    printf("%p %p %d\n", &zmx.ik, &four, four);
    zmx.setvar("ik", 1, &four);
    zmx.setvar("ik", 0, &dour);
    printf("%p %p %d\n", &zmx.ik, &dour, dour);

    return 0;
}

