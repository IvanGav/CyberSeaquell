#include <string>
#include <vector>
#include "../Win32.h"

typedef std::vector<std::string> file;

struct Terminal {
    std::vector<file> files;
    std::vector<std::string> names;
    file history;
};

Terminal ts[4]; //terminals

int curTerminal;
int curFile;

int curOffset; //will be -1 if none yet
int curCursorX;
int curCursorY;

int savedCursorX;
int editingHistory; //which item in terminal history is being edited; will be at most curCursorY

file& get_cur_file();
std::string get_prompt();
void create_file(std::string name);
int seek(std::string& str, int& from);
void change_file(int file);
void newCmdLine();
void up_arrow();
void down_arrow();
void left_arrow();
void right_arrow();
void backspace_key();
bool enter_key();
void insert_char(char c);
bool interpret_typed_character(Win32::Key charCode, char c);

/*
    API
*/

//open a terminal number 'terminal'
void open_terminal(int terminal) {
    curTerminal = terminal;
    curFile = 0;
    curCursorX = 2;
    curCursorY = get_cur_file().size()-1;
    curOffset = -1;
    savedCursorX = 0;
    newCmdLine();
}

//get a pointer to a vector of strings to draw
file& get_terminal() {
    return get_cur_file();
}

//return x position of cursor in an editor
int cursor_x() {
    return curCursorX;
}

//return y position of cursor in an editor
int cursor_y() {
    return curCursorY;
}

//call for every button user presses
//if returns true, exit from the terminal
bool type_char(Win32::Key char_code, char c) {
    return interpret_typed_character(char_code, c);
}

//when a terminal is clicked, call this function with coordinates of the character that has been clicked
void click_at(int x, int y) {
    if(curFile != 0 || (y == curCursorY && x > 2)) {
        curCursorX = x;
        curCursorY = y;
    }
}

//get offset based on current height
int get_offset(int height) {
    if(curOffset == -1) {
        if(curFile == 0) {
            curOffset = curCursorY-1;
        } else {
            curOffset = max(curCursorY-height+1, 0);
        }
    } else {
        if(curOffset < curCursorY && curCursorY < curOffset + height) {
            //within bounds, do nothing
        } else if(curCursorY >= curOffset + height) {
            curOffset = max(0, curCursorY - height + 1);
        } else {
            //shouldn't happen, do nothing
        }
    }
    return curOffset;
}

void terminals_init() {
    /*
    struct Terminal {
        std::vector<file> files;
        std::vector<std::string> names;
        file history;
    };
    */
    for(int i = 0; i < 4; i++) {
        std::vector<file> files;
        std::vector<std::string> names;
        std::vector<std::string> history;
        ts[i] = Terminal{files, names, history};
    }
    curFile = 0;
    curTerminal = 0;
    create_file("t0");
    get_cur_file()[0] = get_prompt();
    create_file("test.txt");
    curFile = 1;
    get_cur_file()[0] = "it's such a terrible system i made";
    get_cur_file().push_back("...but i hope it just works...");

    curFile = 0;
    curTerminal = 1;
    create_file("t1");
    get_cur_file()[0] = get_prompt();

    curFile = 0;
    curTerminal = 2;
    create_file("t2");
    get_cur_file()[0] = get_prompt();

    curFile = 0;
    curTerminal = 3;
    create_file("t3");
    get_cur_file()[0] = get_prompt();
}

/*
    Internal
*/

void create_file(std::string name) {
    std::vector<std::string> f;
    f.push_back("");
    ts[curTerminal].files.push_back(f);
    ts[curTerminal].names.push_back(name);
}

//given a command, interpret it
bool interpret_command(std::string cmd) {
    int from = 0;
    int cmdsize = seek(cmd, from);
    if(strncmp("help", cmd.c_str(), cmdsize)) {
        // int argSize = seek(cmd, from);
        // if(strncmp("-please", cmd.c_str(), cmdsize))
        file& f = get_cur_file();

        f.push_back("help <-please>");
        f.push_back("  see this message");

        f.push_back("clear");
        f.push_back("  clear all previous commands from the screen");

        f.push_back("ls <dir>");
        f.push_back("  list all files in a specified directory (this directory if not specified)");

        f.push_back("cd dir");
        f.push_back("  change directory to a specified directory");

        f.push_back("pwd");
        f.push_back("  print working directory");

        f.push_back("zgull file");
        f.push_back("  open a given file in zgull");

        f.push_back(get_prompt());
    } else if(strncmp("clear", cmd.c_str(), cmdsize)) {
        get_cur_file().clear();
    } else if(strncmp("ls", cmd.c_str(), cmdsize)) {
        get_cur_file().push_back("  .");
        get_cur_file().push_back("  ..");
        for(int i = 1; i < ts[curTerminal].files.size(); i++) {
            get_cur_file().push_back("  " + ts[curTerminal].names[i]);
        }
    // } else if(strncmp("cd", cmd.c_str(), cmdsize)) {
    //     int argSize = seek(cmd, from);
    //     if(strncmp(".", cmd.c_str(), cmdsize));
    } else if(strncmp("pwd", cmd.c_str(), cmdsize)) {
        get_cur_file().push_back("C:/cryptocom/central facility/ehtp1nfo34-terminal" + std::to_string(curTerminal));
    } else if(strncmp("zgull", cmd.c_str(), cmdsize)) {
        int argSize = seek(cmd, from);
        for(int i = 1; i < ts[curTerminal].files.size(); i++) {
            if(strncmp(ts[curTerminal].names[i].c_str(), cmd.c_str()+cmdsize+1, argSize)) {
                //open a file i
                newCmdLine();
                change_file(i);
                return false;
            }
        }
        //print error
        get_cur_file().push_back("  ERROR: file not found");
    } else if(strncmp("exit", cmd.c_str(), cmdsize)) {
        return true;
    } else {
        get_cur_file().push_back("  ERROR: command not recognized");
    }
    newCmdLine();
    return false;
}

std::string get_prompt() {
    return "> ";
}

void change_file(int file) {
    curFile = file;
    curCursorY = get_cur_file().size()-1;
    if(file == 0) {
        curCursorX = 2;
    } else {
        curCursorX = get_cur_file()[curCursorY].size()-1;
    }
    curOffset = -1;
}

file& get_cur_file() {
    return ts[curTerminal].files[curFile];
}

//return how many characters were "read" until next space
int seek(std::string& str, int& from) {
    int len = 0;
    while(from < str.size() && str[from] != ' ') { 
        from++;
        len++;
    }
    from++;
    return len;
}

//return true if cursor is currently the rightmost of the line
bool rightmost() {
    return get_cur_file().size() == curCursorX;
}

//when in a terminal, set the latest line to be the correct selected (history) command
void setCurTerminalLine() {
    get_cur_file()[curCursorY] = get_prompt() + ts[curTerminal].history[editingHistory];
}

void newCmdLine() {
    get_cur_file().push_back(get_prompt());
    ts[curTerminal].history.push_back("");
    //if the last history line is empty, don't make a new empty line, reuse
    if(ts[curTerminal].history[ts[curTerminal].history.size()-1] != "") ts[curTerminal].history.push_back("");

    curCursorY = get_cur_file().size()-1;
    curCursorX = 2;
    editingHistory = ts[curTerminal].history.size()-1;
}

/*
    Interpret typed character
*/

//typing prefixes the selected character

bool interpret_typed_character(Win32::Key charCode, char c) {
    if(charCode == Win32::KEY_UP) {
        up_arrow();
    } else if(charCode == Win32::KEY_DOWN) {
        down_arrow();
    } else if(charCode == Win32::KEY_LEFT) {
        left_arrow();
    } else if(charCode == Win32::KEY_RIGHT) {
        right_arrow();
    } else if(charCode == Win32::KEY_BACKSPACE) {
        backspace_key();
    } else if(charCode == Win32::KEY_RETURN) {
        return enter_key();
    } else if(charCode == Win32::KEY_ESC) {
        if(curFile != 0) {
            change_file(0);
        } else {
            return true;
        }
    } else if(c != '\0') {
        insert_char(c);
    }
    return false;
}

void up_arrow() {
    if(curFile == 0) {
        //in terminal
        if(editingHistory == 0) return;
        editingHistory--;
        setCurTerminalLine();
    } else {
        //in editor
        if(curCursorY == 0) return;
        curCursorY--;
        if(get_cur_file()[curCursorY].size() < curCursorX) {
            //bad, save
            savedCursorX = std::max(curCursorX, savedCursorX);
        } else {
            //good, it's fine, but check if can go back to savedCursorX
            curCursorX = std::max(curCursorX, savedCursorX);
        }
    }
}

void down_arrow() {
    if(curFile == 0) {
        //in terminal
        if(editingHistory == ts[curTerminal].history.size()-1) return;
        editingHistory++;
        setCurTerminalLine();
    } else {
        //in editor
        if(curCursorY == get_cur_file().size()-1) return;
        curCursorY++;
        if(get_cur_file()[curCursorY].size() < curCursorX) {
            //bad, save
            savedCursorX = std::max(curCursorX, savedCursorX);
        } else {
            //good, it's fine, but check if can go back to savedCursorX
            curCursorX = std::max(curCursorX, savedCursorX);
        }
    }
}

//
void left_arrow() {
    if(curCursorX > (curFile == 0 ? 2 : 0)) { //because you don't want to be able to edit "> " in the terminal
        curCursorX--;
    }
    savedCursorX = 0;
}

//go right until to the right of the rightmost character; reset savedCursorX
void right_arrow() {
    if(curCursorX < get_cur_file()[curCursorY].size()) {
        curCursorX++;
    }
    savedCursorX = 0;
}

void insert_char(char c) {
    if(curFile == 0) {
        //terminal
        if(rightmost()) {
            ts[curTerminal].history[editingHistory].push_back(c);
        } else {
            ts[curTerminal].history[editingHistory].insert(curCursorX-2, 1, c);
        }
        setCurTerminalLine();
    } else {
        //editor
        if(rightmost()) {
            get_cur_file()[curCursorY].push_back(c);
        } else {
            get_cur_file()[curCursorY].insert(curCursorX, 1, c);
        }
    }
    right_arrow();
}

void backspace_key() {
    if(curFile == 0) {
        //terminal
        file& f = get_cur_file();
        if(curCursorX > 2) {
            ts[curTerminal].history[editingHistory].erase(curCursorX-2, 1);
            left_arrow();
        }
        setCurTerminalLine();
    } else {
        //editor
        file& f = get_cur_file();
        if(curCursorX != 0) {
            f[curCursorY].erase(curCursorX, 1);
            left_arrow();
        } else {
            //merge 2 lines
        }
    }
}

bool enter_key() {
    if(curFile == 0) {
        //copy current history item to latest history line
        int last = ts[curTerminal].history.size()-1;
        ts[curTerminal].history[last] = ts[curTerminal].history[editingHistory];
        editingHistory = last;
        return interpret_command(ts[curTerminal].history[last]);
    } else {
        file& f = get_cur_file();
        if(rightmost()) {
            //newline
            f.insert(f.begin()+curCursorY+1, "");
        } else {
            //split
            f.insert(f.begin()+curCursorY+1, (*(f.begin()+curCursorY)).substr(curCursorX));
            f[curCursorY].erase(curCursorX);
        }
        down_arrow();
        curCursorX = 0;
        savedCursorX = 0;
        return false;
    }
}