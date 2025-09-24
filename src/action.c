#include <stdio.h>
#include <windows.h>

#include "action.h"
#include "task.h"
#include "utils.h"

#define DELAYS_MS 100

int moveMouseAction(ActionString ac) {
    long x, y;
    if (ac.argcount != 2 || !parse_long_strict(ac.args[0], &x) || !parse_long_strict(ac.args[1], &y)) {
        fprintf(stderr, "%s:%d:1: MoveMouse expects 2 integers\n", ac.filename, ac.line_no);
        return 1;
    }
    moveMouseImpl(x, y);
    Sleep(DELAYS_MS);
    return 0;
}

int moveMouseToAction(ActionString ac) {
    long x, y, totalMs;
    if (ac.argcount != 3 || !parse_long_strict(ac.args[0], &x) || !parse_long_strict(ac.args[1], &y) || !parse_long_strict(ac.args[2], &totalMs)) {
        fprintf(stderr, "%s:%d:1: MoveMouseTo expects 3 integers\n", ac.filename, ac.line_no);
        return 1;
    }
    if (moveMouseToImpl(x, y, totalMs) != 0) {
        fprintf(stderr, "%s:%d:1: Something went wrong!\n", ac.filename, ac.line_no);
        return 1;
    }
    Sleep(DELAYS_MS);
    return 0;
}

int sleepAction(ActionString ac) {
    long totalMs;
    if (ac.argcount != 1 || !parse_long_strict(ac.args[0], &totalMs)) {
        fprintf(stderr, "%s:%d:1: Sleep expects 1 integer\n", ac.filename, ac.line_no);
        return 1;
    }
    sleepImpl(totalMs);
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
    int max_len = 0;
    for (int i = 0; i < TOTAL_ACTION - 1; i++) {
        int len = strlen(actionMap[i].name);
        if (len > max_len) {
            max_len = len;
        }
    }
    for (int i = 0; i < TOTAL_ACTION - 1; i++) {
        printf("%-*s - %s\n", max_len, actionMap[i].name, actionMap[i].description);
    }
}
