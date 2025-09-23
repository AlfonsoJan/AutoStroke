#include <stdio.h>
#include <string.h>
#include <windows.h>

#include "action.h"

#include "utils.h"

#define DELAYS_MS 300

int moveMouse(ActionString ac) {
    long x, y;
    if (ac.argcount != 2 || !parse_long_strict(ac.args[0], &x) || !parse_long_strict(ac.args[1], &y)) {
        fprintf(stderr, "%s:%d:1: MoveMouse expects 2 integers\n", ac.filename, ac.line_no);
        return 1;
    }
    INPUT in = {0};
    in.type = INPUT_MOUSE;
    in.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    in.mi.dx = (65535 * x) / GetSystemMetrics(SM_CXSCREEN);
    in.mi.dy = (65535 * y) / GetSystemMetrics(SM_CYSCREEN);
    SendInput(1, &in, sizeof(INPUT));
    Sleep(DELAYS_MS);
    return 0;
}

const ActionTypeMap* getActionFromString(ActionType action) {
    for (int i = 0; i < TOTAL_ACTION; i++) {
        if (actionMap[i].value == action) {
            return &actionMap[i];
        }
    }
    return &actionMap[UNKNOWN_ACTION];
}

ActionType stringToActionType(const char *str) {
    for (int i = 0; i < TOTAL_ACTION; i++) {
        if (strcmp(str, actionMap[i].name) == 0) {
            return actionMap[i].value;
        }
    }
    return UNKNOWN_ACTION;
}

const char* actionTypeToString(ActionType action) {
    for (int i = 0; i < TOTAL_ACTION; i++) {
        if (actionMap[i].value == action) {
            return actionMap[i].name;
        }
    }
    return "UNKNOWN_ACTION";
}

void free_actions(ActionString *actions, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        for (int j = 0; j < actions[i].argcount; ++j) free(actions[i].args[j]);
        free(actions[i].args);
    }
}

int run_actions(ActionString *actions, size_t action_count) {
    for (size_t i = 0; i < action_count; ++i) {
        ActionString actionString = actions[i];
        const ActionTypeMap *actionTypeMap = getActionFromString(actionString.action);
        int result = actionTypeMap->func(actionString);
        if (result != 0) {
            return 1;
        }
    }

    return 0;
}

void print_out_action_type_map(void) {
    for (int i = 0; i < TOTAL_ACTION - 1; i++) {
        printf("%s\n", actionMap[i].description);
    }
}
