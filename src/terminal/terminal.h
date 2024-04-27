#include <string>
#include <vector>

#define CMD_RETURN_TYPE_PRINT 0
#define CMD_RETURN_TYPE_ENTER_TEXT_EDITOR 1

typedef std::vector<std::string> file;

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

//get a pointer to a vector of strings to draw
std::vector<std::string> get_terminal() {
    return std::vector<std::string>();
}

//return x position of cursor in an editor
int cursor_x() {
    return -1;
}

//return y position of cursor in an editor
int cursor_y() {
    return -1;
}

//when in editor, type a character c in the current location
void type_char(int char_code, char c) {
    return;
}

//when a terminal is clicked, call this function with coordinates of the character that has been clicked
void click_at(int x, int y) {
    return;
}

//get offset based on current height
int get_offset(int height) {
    return -1;
}

/*
    Internal
*/

std::vector<std::vector<file>> ts; //terminals
//ts[n][0] is the actual terminal with its history; other indicies after 0 are the actual files

int cur_terminal;
int cur_file;

//given a command, interpret it and return a result
//  if 'type' is CMD_RETURN_TYPE_PRINT, then 'str' to print to the terminal
//  if 'type' is CMD_RETURN_TYPE_ENTER_TEXT_EDITOR, then you need to enter the text editor, and 'str' is the current text in the file
CommandReturn interpret_command(std::string input) {
    return CommandReturn{CMD_RETURN_TYPE_PRINT, "    ERROR: Command not found"};
}

std::string get_prompt() {
    return "> ";
}