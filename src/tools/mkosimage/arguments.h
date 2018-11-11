#ifndef __ARGUMENT_H__
#define __ARGUMENT_H__

#include <string>

class Arguments {
public:
    bool help;
    bool verbose;
    std::string filename;
    size_t size;

    Arguments();
    bool parse(int argc, char* argv[]);
    void show_help();
};

#endif
