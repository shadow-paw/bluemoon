#include <stdio.h>
#include "arguments.h"
#include "image_maker.h"

int main(int argc, char* argv[]) {
    Arguments args;
    if (!args.parse(argc, argv) || args.help) {
        args.show_help();
        return 1;
    }
    printf("[ ] creating disk image: %s\n", args.filename.c_str());
    if (!ImageMaker::create(args.filename, args.size)) {
        printf("[!] failed\n");
        return 1;
    }
    printf("[ ] success\n");
    return 0;
}
