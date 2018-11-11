#include <stddef.h>
#include <getopt.h>
#include <string.h>
#include <stdio.h>
#include "arguments.h"

Arguments::Arguments() {
    verbose = false;
    help = false;
    size = 0;
}
bool Arguments::parse(int argc, char* argv[]) {
    static struct option long_options[] = {
        {"verbose", no_argument,       0, 'v'},
        {"help",    no_argument,       0, 'h'},
        {"size",    required_argument, 0, 's'},
        {"file",    required_argument, 0, 'f'},
        {0, 0, 0, 0}
    };
    int option_index = 0;
    while (true) {
        int c = getopt_long(argc, argv, "vhs:f:",
                            long_options, &option_index);
        if (c == -1) break;
        switch (c) {
        case 'v':
            this->verbose = true;
            break;
        case 'h':
            this->help = true;
            break;
        case 's':
            this->size = static_cast<size_t>(atoi(optarg));
            break;
        case 'f':
            this->filename = std::string(optarg);
            break;
        default:
            return false;
        }
    }
    if (this->size == 0 || this->filename.empty()) {
        return false;
    }
    this->size *= 1024 * 1024;  // convert to mbytes
    return true;
}
void Arguments::show_help() {
    printf("USAGE: mkosimage [-v] [-h] -s 10 -f filename.img\n");
    printf("       mkosimage [--verbose] [--help] --size=10 --file=filename.img\n");
    printf("  size: disk size, in MB\n");
    printf("  file: output filename\n");
}
