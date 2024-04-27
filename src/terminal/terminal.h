#include <string>
#include <vector>
#include "../Win32.h"

//TODO: ignore leading spaces

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
int seek(std::string& str, int& from, int& len);
void change_file(int file);
void newCmdLine();
void up_arrow();
void down_arrow();
void left_arrow();
void right_arrow();
void backspace_key();
void delete_key();
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
            curOffset = curCursorY;
        } else {
            curOffset = max(curCursorY-height+1, 0);
        }
    } else {
        if(curOffset < curCursorY && curCursorY < curOffset + height) {
            //within bounds, do nothing
        } else if(curCursorY >= curOffset + height) {
            curOffset = max(0, curCursorY - height + 1);
        } else {
            curOffset = curCursorY;
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
    get_cur_file().pop_back();
    create_file("laserPerms.JSON");
    curFile = 1;
    get_cur_file()[0] = "name:Laser Permissions";
    get_cur_file().push_back("id: 58496");
    get_cur_file().push_back("allow: humans");
    get_cur_file().push_back("disallow: bees, seagulls");
    get_cur_file().push_back("hate: llamas");
    get_cur_file().push_back("mine: crypto");
    get_cur_file().push_back("favorite employee: Patrick");
    get_cur_file().push_back("birthday: 04/24/1982");
    get_cur_file().push_back("first love: vending machine");
    get_cur_file().push_back("beavers: keystone species should be protected");
    get_cur_file().push_back("native US bees: endangered");

    curFile = 0;
    curTerminal = 1;
    create_file("t1");
    get_cur_file().pop_back();
    create_file("cameraMaintenance.txt");
    curFile = 1;
    get_cur_file()[0] = "author: Maintenance Moe";
    get_cur_file().push_back("note to self: still need to do the final setup on this camera");
    get_cur_file().push_back("only a few last commands left but im so forgetful");
    get_cur_file().push_back("ill just write them down here for now:");
    get_cur_file().push_back("lscam");
    get_cur_file().push_back("connect camera (id of disabled camera)");
    get_cur_file().push_back("...then I just need to answer the security question");
    get_cur_file().push_back("enable camera (id of disabled camera)");

    curFile = 0;
    curTerminal = 2;
    create_file("t2");
    get_cur_file().pop_back();
    create_file("passcode.txt");
    curFile = 1;
    get_cur_file()[0] = "some idiot just left our password completely unprotected";
    get_cur_file().push_back("luckily i, Maintenance Moe, was here to encrypt the password");
    get_cur_file().push_back("but just to make sure everyone can still get in I used the key: sustainability");
    get_cur_file().push_back("(as a little joke because we are not sustainable)");
    get_cur_file().push_back("the encrypted password is: kcpyidroomyqgc");
    get_cur_file().push_back("man, this passwords so secure only a roman emperor could crack this");
    create_file("ifeltbadthatyouwontfigureoutthepassword.txt");
    curFile = 2;
    get_cur_file()[0] = "Hey, maintenance Moe here";
    get_cur_file().push_back("JUST TO BE CLEAR, i was hinting that this used a Vigenere cipher");
    get_cur_file().push_back("with a key: sustainability");
    get_cur_file().push_back("you can use any online decodder of choice");
    

    curFile = 0;
    curTerminal = 3;
    create_file("t3");
    get_cur_file().pop_back();
    create_file("prisonSystemSpecs.txt");
    curFile = 1;
    get_cur_file()[0] = "Hey, maintenance Moe again, just set up this prison system!";
    get_cur_file().push_back("tried to make a system that will just lock them in forever like you wanted");
    get_cur_file().push_back("but unfortunately thats impossible so i just amde it really difficult");
    get_cur_file().push_back("if someone wants to let them out theyd have to write some kind of program");
    get_cur_file().push_back("that would take in an integer and print that many prime numbers");
    get_cur_file().push_back("luckily theres no hacker advanced enough to do something like that");
    create_file("seagullLanguageBasics.txt");
    curFile = 2;
    get_cur_file()[0] = "REM Rd, Rx, Ry";
    get_cur_file().push_back("INN get input");
    get_cur_file().push_back("OUT send output");
    get_cur_file().push_back("SUB Rd, Rx, Ry");
    get_cur_file().push_back("ADD Rd, Rx, Ry");
    get_cur_file().push_back("JMP label");
    get_cur_file().push_back("CEQ Rd, Rx, Ry (=)");
    get_cur_file().push_back("IZE Rx, label (if 0, jump)");
    get_cur_file().push_back("CNE Rd, Rx, Ry (!=)");
    get_cur_file().push_back("CGT >");
    get_cur_file().push_back("CLT <");
    get_cur_file().push_back("CGE >=");
    get_cur_file().push_back("CLE <=");
    get_cur_file().push_back("MOV Rd, num");
    get_cur_file().push_back("PSH push to stack");
    get_cur_file().push_back("POP pop from stack");
    create_file("primeNumbers.seag");
    curFile = 3;
}

void scroll_input(F32 scroll) {
    //assume -120 to 120
    curOffset = clamp(curOffset - signumf32(scroll), 0, curCursorY);
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
    int cmdsize;
    seek(cmd, from, cmdsize);
    if ("help"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
        file& f = get_cur_file();
        int argSize; int argAt = seek(cmd, from, argSize);
        if ("-please"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
            f.push_back("  help [-please]");

            f.push_back("  exit");

            f.push_back("  clear");

            f.push_back("  ls [dir]");

            f.push_back("  cd <dir>");

            f.push_back("  pwd");

            f.push_back("  zgull <file>");

            f.push_back("  lscam");

            f.push_back("  connect camera <cameraID>");

            f.push_back("  c> enable camera");
        } else {
            f.push_back("  help [-please]");
            f.push_back("    see this message");

            f.push_back("  exit");
            f.push_back("    terminate current terminal session");

            f.push_back("  clear");
            f.push_back("    clear all previous commands from the screen");

            f.push_back("  ls [dir]");
            f.push_back("    list all files in a specified directory (this directory if not specified)");

            f.push_back("  cd <dir>");
            f.push_back("    change directory to a specified directory");

            f.push_back("  pwd");
            f.push_back("    print working directory");

            f.push_back("  zgull <file>");
            f.push_back("    open a given file in zgull, the best text editor");

            f.push_back("  lscam");
            f.push_back("   list all available cameras");

            f.push_back("  connect camera <cameraID>");
            f.push_back("   securely connect to a camera to change its settings");

            f.push_back("  c> enable camera");
            f.push_back("    when connected to a camera, enable it (if it's currently disabled)");
        }
    } else if ("clear"sa == StrA{ cmd.c_str(), U64(cmdsize) } ) {
        get_cur_file().clear();
    }
    else if ("ls"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
        get_cur_file().push_back("  .");
        get_cur_file().push_back("  ..");
        for (int i = 1; i < ts[curTerminal].files.size(); i++) {
            get_cur_file().push_back("  " + ts[curTerminal].names[i]);
        }
    } else if ("cd"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
        int argSize; int startAt = seek(cmd, from, argSize);
        if (cmd[startAt] == '.' && cmd.size() == 1);
        else {
            get_cur_file().push_back("  ERROR: Permussion denied");
        }
    } else if ("pwd"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
        get_cur_file().push_back("  C:/cryptocom/central facility/ehtp1nfo34-terminal" + std::to_string(curTerminal));
    } else if ("zgull"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
        int argSize; int argFrom = seek(cmd, from, argSize);
        for(int i = 1; i < ts[curTerminal].files.size(); i++) {
            if (StrA{ ts[curTerminal].names[i].c_str(), ts[curTerminal].names[i].size() } == StrA{ cmd.c_str() + argFrom, U64(argSize) }) {
                //open a file i
                newCmdLine();
                change_file(i);
                return false;
            }
        }
        //print error
        get_cur_file().push_back("  ERROR: file not found");
    } else if ("exit"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
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
        curCursorX = get_cur_file()[curCursorY].size();
    }
    curOffset = -1;
}

file& get_cur_file() {
    return ts[curTerminal].files[curFile];
}

//set 'len' to how many characters were "read" until next space; return original 'from' value
int seek(std::string& str, int& from, int& len) {
    int from_old = from;
    len = 0;
    while(from < str.size() && str[from] != ' ') { 
        from++;
        len++;
    }
    while(from < str.size() && str[from] == ' ') from++;
    return from_old;
}

//return true if cursor is currently the rightmost of the line
bool rightmost() {
    return get_cur_file()[curCursorY].size() == curCursorX;
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
    } else if (charCode == Win32::KEY_DELETE) {
        delete_key();
    } else if (charCode == Win32::KEY_RETURN) {
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
        curCursorX = get_cur_file()[curCursorY].size();
    } else {
        //in editor
        if (curCursorY == 0) {
            savedCursorX = max(curCursorX, savedCursorX); 
            curCursorX = 0; 
            return; 
        }
        curCursorY--;
        curCursorX = max(curCursorX, savedCursorX);
        if (get_cur_file()[curCursorY].size() < curCursorX) {
            //bad, save
            savedCursorX = max(curCursorX, savedCursorX);
            curCursorX = get_cur_file()[curCursorY].size();
        }
    }
}

void down_arrow() {
    if(curFile == 0) {
        //in terminal
        if(editingHistory == ts[curTerminal].history.size()-1) return;
        editingHistory++;
        setCurTerminalLine();
        curCursorX = get_cur_file()[curCursorY].size();
    } else {
        //in editor
        if (curCursorY == get_cur_file().size() - 1) { curCursorX = get_cur_file()[curCursorY].size(); return; }
        curCursorY++;
        curCursorX = max(curCursorX, savedCursorX);
        if (get_cur_file()[curCursorY].size() < curCursorX) {
            //bad, save
            savedCursorX = max(curCursorX, savedCursorX);
            curCursorX = get_cur_file()[curCursorY].size();
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
            ts[curTerminal].history[editingHistory].erase(curCursorX-3, 1);
            left_arrow();
        }
        setCurTerminalLine();
    } else {
        //editor
        file& f = get_cur_file();
        if(curCursorX != 0) {
            f[curCursorY].erase(curCursorX-1, 1);
            left_arrow();
        } else {
            if (curCursorY == 0) return;
            curCursorY--;
            curCursorX = f[curCursorY].size();
            f[curCursorY].append(f[curCursorY+1]);
            f.erase(f.begin() + curCursorY + 1);
        }
    }
}

void delete_key() {
    if (curFile == 0) {
        //terminal
        file& f = get_cur_file();
        if (rightmost()) return;
        ts[curTerminal].history[editingHistory].erase(curCursorX - 2, 1);
        setCurTerminalLine();
    }
    else {
        //editor
        file& f = get_cur_file();
        if (!rightmost()) {
            f[curCursorY].erase(curCursorX, 1);
        } else {
            if (curCursorY == get_cur_file().size()-1) return;
            f[curCursorY].append(f[curCursorY + 1]);
            f.erase(f.begin() + curCursorY + 1);
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