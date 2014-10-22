
#define INVOKER_TABLE(type)							\
    static std::map<std::string, void (type::*)(std::vector<char*>)> mtable;

#define INVOKER_METHOD(type)							\
    void invoke(const char *method, std::vector<char*> argv) {			\
	if ( mtable.count(method) == 1 ) {					\
	    (this->*mtable[method])(argv);					\
	} else {								\
	    if ( mtable.count("unknown") == 1 ) {				\
		(this->*mtable["unknown"])(argv);				\
	    } else {								\
	        fprintf(stderr, "unknown method " #type "::%s\n", method);	\
	    }									\
	}									\
    }

#define INVOKER(type)		\
    INVOKER_TABLE(type)		\
  public:			\
    INVOKER_METHOD(AGF)
