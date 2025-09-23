#include <stdio.h>
#include <string.h>

#include "parse.h"
#include "action.h"

#define shift_args(argc, argv) ((argc) > 0 ? ((argc)--, *(argv)++) : NULL)

void usage(const char *program) {
    fprintf(stderr, "Usage: %s <macro script> [OPTIONS...] \n", program);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "   -h         print this help and exit\n");
    fprintf(stderr, "   -commands  print out all the command\n");
}

int main(int argc, char **argv) {
    const char *program = shift_args(argc, argv);
    const char *macro_path = shift_args(argc, argv);

    if (macro_path == NULL) {
        usage(program);
        return 1;
    }

    while (argc > 0) {
        const char *flag = shift_args(argc, argv);
        if (strcmp(flag, "-h") == 0) {
            usage(program);
            return 1;
        } else if (strcmp(flag, "-commands") == 0) {
            print_out_action_type_map();
            return 0;
        } else {
            fprintf(stderr, "Unknown command: '%s'\n", flag);
            usage(program);
            return 1;
        }
    }

    ActionString actions[MAX_ACTIONS];
    size_t action_count = 0;
    if (parse_macro_text(macro_path, actions, &action_count) != 0) {
        return 1;
    }

    if (run_actions(actions, action_count) != 0) {
        free_actions(actions, action_count);
        return 1;
    }

    free_actions(actions, action_count);
    return 0;
}
