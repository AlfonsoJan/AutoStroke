#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "parse.h"
#include "action.h"

#define shift_args(argc, argv) ((argc) > 0 ? ((argc)--, *(argv)++) : NULL)

void usage(const char *program) {
    fprintf(stderr, "Usage: %s <macro script> [OPTIONS...]\n", program);
    fprintf(stderr, "Options:\n");
    fprintf(stderr, "   -h                      print this help and exit\n");
    fprintf(stderr, "   -c | --commands         print out all the commands\n");
    fprintf(stderr, "   -i | --immediate        start immediately (do not wait for Alt+F12)\n");
    fprintf(stderr, "\n");
    fprintf(stderr, "Default behavior is to arm and wait for Alt+F12, then run the macro.\n");
}

int main(int argc, char **argv) {
    const char *program = shift_args(argc, argv);
    const char *macro_path = NULL;
    int wait_for_hotkey = 1;

    while (argc > 0) {
        const char *arg = shift_args(argc, argv);
        if (strcmp(arg, "-h") == 0) {
            usage(program);
            return 0;
        } else if (strcmp(arg, "-c") == 0 || strcmp(arg, "--commands") == 0) {
            print_out_action_type_map();
            return 0;
        } else if (strcmp(arg, "-i") == 0 || strcmp(arg, "--immediate") == 0) {
            wait_for_hotkey = 0;
        } else if (arg[0] == '-') {
            fprintf(stderr, "Unknown option: '%s'\n", arg);
            usage(program);
            return 1;
        } else {
            if (!macro_path) {
                macro_path = arg;
            } else {
                fprintf(stderr, "Unexpected extra argument: '%s'\n", arg);
                usage(program);
                return 1;
            }
        }
    }

    if (!macro_path) {
        usage(program);
        return 1;
    }

    ActionString actions[MAX_ACTIONS];
    size_t action_count = 0;
    if (parse_macro_text(macro_path, actions, &action_count) != 0) {
        return 1;
    }

    if (wait_for_hotkey) {
        // ALT + F12
        if (!RegisterHotKey(NULL, 1, MOD_ALT, VK_F12)) {
            DWORD err = GetLastError();
            if (err == ERROR_HOTKEY_ALREADY_REGISTERED /*1409*/) {
                fprintf(stderr, "Failed to register hotkey: Alt+F12 is already in use.\n");
            } else {
                fprintf(stderr, "Failed to register hotkey (error %lu).\n", err);
            }
            free_actions(actions, action_count);
            return 1;
        }

        printf("AutoStroke armed. Press Alt+F12 to start...\n");
        MSG msg;
        while (GetMessage(&msg, NULL, 0, 0) != 0) {
            if (msg.message == WM_HOTKEY && msg.wParam == 1) {
                break;
            }
        }
        UnregisterHotKey(NULL, 1);
    }

    if (run_actions(actions, action_count) != 0) {
        free_actions(actions, action_count);
        return 1;
    }

    free_actions(actions, action_count);
    return 0;
}
