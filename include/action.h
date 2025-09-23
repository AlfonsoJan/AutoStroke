#ifndef ACTION_H_
#define ACTION_H_

#include <stdlib.h>
#include <assert.h>

typedef enum {
    MOVE_MOUSE = 0,
    MOVE_MOUSE_TO,

    UNKNOWN_ACTION,
    TOTAL_ACTION
} ActionType;

typedef struct {
    ActionType action;
    char **args;
    int argcount;
    int line_no;
    const char *filename;
} ActionString;

typedef int (*ActionFunc)(ActionString);

int moveMouse(ActionString ac);
int moveMouseTo(ActionString ac);

typedef struct {
    const char *name;
    ActionType value;
    ActionFunc func;
    const char *description;
} ActionTypeMap;

static const ActionTypeMap actionMap[TOTAL_ACTION] = {
    {"MoveMouse", MOVE_MOUSE, moveMouse, "Set the mouse position to a given location on the main screen."},
    {"MoveMouseTo", MOVE_MOUSE_TO, moveMouseTo, "Move the mouse to a given location from the current location on the main screen."},
    {"UNKNOWN_ACTION", UNKNOWN_ACTION, 0, ""},
};

static_assert(TOTAL_ACTION == 3, "update actionMap");

const ActionTypeMap* getActionFromString(ActionType action);
ActionType stringToActionType(const char *str);
const char* actionTypeToString(ActionType action);
void free_actions(ActionString *actions, size_t count);
int run_actions(ActionString *actions, size_t action_count);
void print_out_action_type_map(void);

#endif // ACTION_H_