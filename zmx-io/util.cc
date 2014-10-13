
#include <stddef.h>
#include <string.h>

#include <cstdlib>
#include <iostream>
#include <fstream>

#include <string>
#include <vector>

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


    if ( (ch0 = inp.get()) != -1 && (ch0 == 0xFE || ch0 == 0xFF) ) {
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
