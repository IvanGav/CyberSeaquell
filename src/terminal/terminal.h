#include <string>

#define CMD_RETURN_TYPE_PRINT 0
#define CMD_RETURN_TYPE_ENTER_TEXT_EDITOR 1

#define ARROW_KEY_U 0
#define ARROW_KEY_D 1
#define ARROW_KEY_L 2
#define ARROW_KEY_R 3

struct CommandReturn {
    int type;
    std::string str;
};

/*
    API
*/

//open a terminal number 'terminal'
void open_terminal(int terminal) {
    return;
}

//get a prompt to print in the terminal
std::string get_prompt() {
    return "> ";
}

//given a command, interpret it and return a result
//  if 'type' is CMD_RETURN_TYPE_PRINT, then 'str' to print to the terminal
//  if 'type' is CMD_RETURN_TYPE_ENTER_TEXT_EDITOR, then you need to enter the text editor, and 'str' is the current text in the file
CommandReturn interpret_command(std::string input) {
    return CommandReturn{CMD_RETURN_TYPE_PRINT, "    ERROR: Command not found"};
}

//when in editor, type a character c in the current location
void editor_type_char(char c) {
    return;
}

//when in editor, if arrow keys are pressed, pass it to this function ARROW_KEY_U/D/L/R
void editor_arrow_keys(int arrow_key) {
    return;
}

//return x position of cursor in an editor
int editor_at_x() {
    return -1;
}

//return y position of cursor in an editor
int editor_at_y() {
    return -1;
}

void editor_go_to_pos(int x, int y) {
    return;
}

/*
    Internal
*/