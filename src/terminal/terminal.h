#pragma once

#include <string>
#include <vector>
#include "../Win32.h"
#include "interpreter.h"

//TODO: ignore leading spaces

typedef std::vector<std::string> file;

struct Terminal {
    std::vector<file> files;
    std::vector<std::string> names;
    file history;
};

enum TerminalMode {
    Normal,
    Input,
    Camera,
};

Terminal ts[4]; //terminals

bool t2_cam_enabled = false;
int connectedCamNumber;

TerminalMode terminalMode;
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
int get_line_len();
void ask_security_question(int terminal);

/*
    API
*/

//open a terminal number 'terminal'
void open_terminal(int terminal) {
    terminalMode = TerminalMode::Normal;
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
    println_integer(x);
    println_integer(y);
    println();
    if (curFile != 0) {
        curCursorY = min(size_t(y), get_cur_file().size() - 1);
        curCursorX = min(size_t(x), get_cur_file()[curCursorY].size());
    } else if (y == curCursorY && x > 1 && x <= get_line_len()) {
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
    create_file("maintenanceNote.txt");
    curFile = 1;
    get_cur_file()[0] = "maintenance Moe here!";
    get_cur_file().push_back("");
    get_cur_file().push_back("I just set up the laser system like you wanted, it uses the laserPerms.JSON");
    get_cur_file().push_back("file to detect who should be allowed to go through the laser");
    get_cur_file().push_back("");
    get_cur_file().push_back("Its currently set up to allow humans and absolutely disallow bees and seagulls");
    get_cur_file().push_back("so as long as no one deletes those lines, the system should work perfectly");
    create_file("laserPerms.JSON");
    curFile = 2;
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
    create_file("cryptoStats.txt");
    curFile = 3;
    get_cur_file()[0] = "If Bitcoin were a country, its energy consumption would";
    get_cur_file().push_back("have ranked 27th in the world in 2020-2021, ahead of a country like Pakistan,");
    get_cur_file().push_back("with a population of over 230 million people.");
    get_cur_file().push_back("");
    get_cur_file().push_back("Equivalent to that of burning 84 billion pounds of coal");
    get_cur_file().push_back("or operating 190 natural gas-fired power plants.");
    get_cur_file().push_back("");
    get_cur_file().push_back("At the same time, Bitcoin used water equivalent to about");
    get_cur_file().push_back("660,000 Olympic-sized swimming pools or enough to meet the");
    get_cur_file().push_back("current domestic water needs of more than 300 million people");
    get_cur_file().push_back("in rural sub-Saharan Africa");
    get_cur_file().push_back("");
    get_cur_file().push_back("https://unu.edu/press-release/un-study-reveals-hidden-environmental-impacts-bitcoin-carbon-not-only-harmful-product");

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
    get_cur_file().push_back("");
    get_cur_file().push_back("lscam");
    get_cur_file().push_back("connect camera (id of disabled camera)");
    get_cur_file().push_back("...then I just need to answer the security question");
    get_cur_file().push_back("enable camera");
    create_file("beeStats.txt");
    curFile = 2;
    get_cur_file()[0] = "Of the 1437 native bee species in the US, 749 are declining in population.";
    get_cur_file().push_back("");
    get_cur_file().push_back("1 out of 4 native bee species is at increased risk of extinction");
    get_cur_file().push_back("");
    get_cur_file().push_back("The major culprits of these declines are habitat loss, pesticide use,");
    get_cur_file().push_back("monocultures, climate change, and urbanization");
    get_cur_file().push_back("");
    get_cur_file().push_back("Not just bees are at risk, more than 40 percent of insect pollinators");
    get_cur_file().push_back("globally are highly threatened");
    get_cur_file().push_back("");
    get_cur_file().push_back("Monocultures seem to be the largest cause");
    get_cur_file().push_back("Between 2008 and 2013, wild bee abundance declined across nearly quarter");
    get_cur_file().push_back("of the United States, with California's Central Valley and the Midwest's Corn Belt");
    get_cur_file().push_back("ranking among the lowest in wild bee abundance");
    get_cur_file().push_back("");
    get_cur_file().push_back("Kopec, K., Ann Burd, L., & Center for Biological Diversity. (2017).");
    get_cur_file().push_back("Pollinators in Peril:: A Systematic Status Review of North American and Hawaiian Native Bees.");
    get_cur_file().push_back("In https://www.biologicaldiversity.org/. Center for Biological Diversity. Retrieved April 27, 2024,");
    get_cur_file().push_back("from https ://www.biologicaldiversity.org/campaigns/native_pollinators/pdfs/Pollinators_in_Peril.pdf");

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
    get_cur_file().push_back("");
    get_cur_file().push_back("the encrypted password is: kcpyidroomyqgc");
    get_cur_file().push_back("");
    get_cur_file().push_back("man, this passwords so secure only a roman emperor could crack this");
    create_file("ifeltbadthatyouwontfigureoutthepassword.txt");
    curFile = 2;
    get_cur_file()[0] = "Hey, maintenance Moe here";
    get_cur_file().push_back("JUST TO BE CLEAR, i was hinting that this used a Vigenere cipher");
    get_cur_file().push_back("with a key: sustainability");
    get_cur_file().push_back("");
    get_cur_file().push_back("you can use any online decodder of choice");
    create_file("llamaStats.txt");
    curFile = 3;
    get_cur_file()[0] = "The llama population dropped from 145,000 to less than 40,000 from 2002 to 2017.";
    get_cur_file().push_back("This is mostly due to the steep decline in the popularity of llama.");
    get_cur_file().push_back("");
    get_cur_file().push_back("The llama industry once included well known celebrities and athletes.");
    get_cur_file().push_back("");
    get_cur_file().push_back("They were used to provide fiber and herd sheep.");
    get_cur_file().push_back("");
    get_cur_file().push_back("In 1986, a llama sold for $220,000.");
    get_cur_file().push_back("Llama popularity crashed with the housing market.");
    get_cur_file().push_back("");
    get_cur_file().push_back("Warmer Climates have led to increased parasites in llamas.");
    get_cur_file().push_back("");
    get_cur_file().push_back("https://www.nbcnews.com/news/us-news/llamas-disappearing-across-united-states-n994181");
    

    curFile = 0;
    curTerminal = 3;
    create_file("t3");
    get_cur_file().pop_back();
    create_file("prisonSystemSpecs.txt");
    curFile = 1;
    get_cur_file()[0] = "Hey, maintenance Moe again, just set up this prison system!";
    get_cur_file().push_back("");
    get_cur_file().push_back("tried to make a system that will just lock them in forever like you wanted");
    get_cur_file().push_back("but unfortunately thats impossible so i just made it really difficult");
    get_cur_file().push_back("");
    get_cur_file().push_back("if someone wants to let them out theyd have to write some kind of program");
    get_cur_file().push_back("in the primeNumbers.seag file that would take in an integer and print that");
    get_cur_file().push_back("many prime numbers");
    get_cur_file().push_back("");
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
    create_file("beaverStats.txt");
    curFile = 4;
    get_cur_file()[0] = "Beavers used to live in almost every stream in North America";
    get_cur_file().push_back("and numbered in the many millions");
    get_cur_file().push_back("");
    get_cur_file().push_back("Due to demand for their fur, they were hunted to near extinction");
    get_cur_file().push_back("");
    get_cur_file().push_back("Despite reintroductions and natural expansion, the beavers haven't returned");
    get_cur_file().push_back("fully which is very bad for the environments they once inhabited.");
    get_cur_file().push_back("");
    get_cur_file().push_back("Beavers are keystone species thanks to the dams they build. This means");
    get_cur_file().push_back("they are crucial to the stability and wellbeing of every other animal");
    get_cur_file().push_back("in their wetland.");
    get_cur_file().push_back("");
    get_cur_file().push_back("Entire ecosystems are still in disarray from their absence.");
    get_cur_file().push_back("");
    get_cur_file().push_back("https://defenders.org/wildlife/beaver");
}

void scroll_input(F32 scroll) {
    //assume -120 to 120
    curOffset = clamp(curOffset - signumf32(scroll), 0, curCursorY);
}

bool disallow_bees() {
    int i;
    for (i = 0; i < (int)ts[curTerminal].names.size(); i++) {
        if (strcmp(ts[curTerminal].names[i].c_str(), "laserPerms.JSON") == 0) {
            for (int j = 0; j < (int)ts[curTerminal].files[i].size(); j++) {
                if (strncmp(ts[curTerminal].files[i][j].c_str(), "disallow: bees", 14))
                    return true;
            }
        }
    }
    return false;
}

/*
    Internal
*/

int get_line_len() {
    return get_cur_file()[curCursorY].size();
}

void create_file(std::string name) {
    std::vector<std::string> f;
    f.push_back("");
    ts[curTerminal].files.push_back(f);
    ts[curTerminal].names.push_back(name);
}

void ask_security_question(int terminal) {
    connectedCamNumber = terminal;
    terminalMode = TerminalMode::Input;
    get_cur_file().push_back("  Please enter the security question:");
    get_cur_file().push_back("  Of the 1437 native bee species in the US, how many are declining in population?");
}

//given a command, interpret it
bool interpret_command(std::string cmd) {
    int from = 0;
    int cmdsize;
    seek(cmd, from, cmdsize);
    if (terminalMode == TerminalMode::Normal) {
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

                f.push_back("  exec <code_file> <log_file>");

                f.push_back("  exec kill");

                f.push_back("  exec active");

                f.push_back("  lscam");

                f.push_back("  connect camera <cameraID>");

                f.push_back("  c> enable camera");
            }
            else {
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

                f.push_back("  exec <code_file> <log_file>");
                f.push_back("    compile and execute a program in code_file, which will log (if any) to log_file");

                f.push_back("  exec kill");
                f.push_back("    kill an active program, if it's active");

                f.push_back("  exec active");
                f.push_back("    print information about a process that's currently being executed, if any");

                f.push_back("  lscam");
                f.push_back("   list all available cameras");

                f.push_back("  connect camera <cameraID>");
                f.push_back("   securely connect to a camera to change its settings");

                f.push_back("  c> enable camera");
                f.push_back("    when connected to a camera, enable it (if it's currently disabled)");
            }
        }
        else if ("exec"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
            int argSize; int argFrom = seek(cmd, from, argSize);
            if ("active"sa == StrA{ cmd.c_str() + argFrom, U64(argSize) }) {
                if (virus.active) {
                    get_cur_file().push_back("  A process is currently active. To stop, enter 'exec kill'.");
                } else if (virus.passed) {
                    get_cur_file().push_back("  A process has exited successfully and there are no active processes.");
                } else {
                    get_cur_file().push_back("  A process has failed. There are no active processes.");
                }
            } else if ("kill"sa == StrA{ cmd.c_str() + argFrom, U64(argSize) }) {
                if (virus.active) {
                    virus.active = false;
                    get_cur_file().push_back("  A process has been stopped.");
                } else {
                    get_cur_file().push_back("  No active processes.");
                }
            } else {
                //assume that a file name is given
                file* code = nullptr;
                for (int i = 0; i < ts[curTerminal].names.size(); i++) {
                    if (strncmp(ts[curTerminal].names[i].c_str(), cmd.c_str() + argFrom, argSize) == 0) {
                        code = &ts[curTerminal].files[i];
                        break;
                    }
                }
                int argFrom = seek(cmd, from, argSize);
                file* log = nullptr;
                for (int i = 0; i < ts[curTerminal].names.size(); i++) {
                    if (strncmp(ts[curTerminal].names[i].c_str(), cmd.c_str() + argFrom, argSize) == 0) {
                        log = &ts[curTerminal].files[i];
                        break;
                    }
                }
                if (code == nullptr || log == nullptr) {
                    get_cur_file().push_back("  At least one of given files is invalid");
                }
                compileAndLoad(code, log);
            }
        }
        else if ("clear"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
            get_cur_file().clear();
        }
        else if ("ls"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
            get_cur_file().push_back("  .");
            get_cur_file().push_back("  ..");
            for (int i = 1; i < ts[curTerminal].files.size(); i++) {
                get_cur_file().push_back("  " + ts[curTerminal].names[i]);
            }
        }
        else if ("cd"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
            int argSize; int startAt = seek(cmd, from, argSize);
            if (cmd[startAt] == '.' && cmd.size() == 1);
            else {
                get_cur_file().push_back("  ERROR: Permussion denied");
            }
        }
        else if ("pwd"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
            get_cur_file().push_back("  C:/cryptocom/central facility/ehtp1nfo34-terminal" + std::to_string(curTerminal));
        }
        else if ("zgull"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
            int argSize; int argFrom = seek(cmd, from, argSize);
            for (int i = 1; i < ts[curTerminal].files.size(); i++) {
                if (StrA{ ts[curTerminal].names[i].c_str(), ts[curTerminal].names[i].size() } == StrA{ cmd.c_str() + argFrom, U64(argSize) }) {
                    //open a file i
                    newCmdLine();
                    change_file(i);
                    return false;
                }
            }
            //print error
            get_cur_file().push_back("  ERROR: file not found");
        }
        else if ("exit"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
            return true;
        }
        else if ("lscam"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
            file& f = get_cur_file();
            f.push_back("  Available cameras (name, id, status)");
            f.push_back("  aim-1050     0   online");
            f.push_back("  anticoin5    1   online");
            f.push_back("  riista-11    2   online");
            f.push_back("  abx-03c      3   online");
            f.push_back("  abx-03c      4   online");
            f.push_back("  abx-03b      5   online");
            f.push_back(std::string("  abx12        6   ") + (t2_cam_enabled ? "online" : "offline"));
            f.push_back("  cryptocam    7   online");
            f.push_back("  ccam3        8   online");
            f.push_back(std::string("  Total: 9     Online: ") + (t2_cam_enabled ? "9" : "8"));
        }
        else if ("connect"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
            int argSize; int argFrom = seek(cmd, from, argSize);
            if ("camera"sa == StrA{ cmd.c_str() + argFrom, U64(argSize) }) {
                argFrom = seek(cmd, from, argSize);
                if (argSize == 1) {
                    if (cmd[argFrom] == '0') {
                        ask_security_question(0);
                    }
                    else if (cmd[argFrom] == '1') {
                        ask_security_question(1);
                    }
                    else if (cmd[argFrom] == '2') {
                        ask_security_question(2);
                    }
                    else if (cmd[argFrom] == '3') {
                        ask_security_question(3);
                    }
                    else if (cmd[argFrom] == '4') {
                        ask_security_question(4);
                    }
                    else if (cmd[argFrom] == '5') {
                        ask_security_question(5);
                    }
                    else if (cmd[argFrom] == '6') {
                        ask_security_question(6);
                    }
                    else if (cmd[argFrom] == '7') {
                        ask_security_question(7);
                    }
                    else if (cmd[argFrom] == '8') {
                        ask_security_question(8);
                    }
                    else {
                        get_cur_file().push_back("  ERROR: camera doesn't exist");
                    }
                }
                else {
                    get_cur_file().push_back("  ERROR: camera doesn't exist");
                }
            }
            else {
                get_cur_file().push_back("  ERROR: not a destination");
            }
        }
        else {
            get_cur_file().push_back("  ERROR: command not recognized");
        }
    } else if (terminalMode == TerminalMode::Input) {
        if ("749"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
            get_cur_file().push_back("  Connected to camera " + std::to_string(connectedCamNumber));
            terminalMode = TerminalMode::Camera;
        }
        else {
            get_cur_file().push_back("  Incorrect, aborting");
            terminalMode = TerminalMode::Normal;
        }
    } else if (terminalMode == TerminalMode::Camera) {
        if ("help"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
            get_cur_file().push_back("  help");
            get_cur_file().push_back("  exit");
            get_cur_file().push_back("  enable camera");
        }
        else if ("exit"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
            terminalMode = TerminalMode::Normal;
        }
        else if ("enable"sa == StrA{ cmd.c_str(), U64(cmdsize) }) {
            int argSize; int argFrom = seek(cmd, from, argSize);
            if ("camera"sa == StrA{ cmd.c_str() + argFrom, U64(argSize) }) {
                get_cur_file().push_back("  Please wait...");
                get_cur_file().push_back("  Enabled camera " + std::to_string(connectedCamNumber));
                t2_cam_enabled = true;
            }
        }
    }
    newCmdLine();
    return false;
}

std::string get_prompt() {
    if (terminalMode == TerminalMode::Normal)
        return "> ";
    else if (terminalMode == TerminalMode::Input)
        return ": ";
    else
        return std::to_string(connectedCamNumber) + ">";
}

void change_file(int file) {
    curFile = file;
    curCursorY = get_cur_file().size()-1;
    if(file == 0) {
        curCursorX = 2;
    } else {
        curCursorX = get_line_len();
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
    return get_line_len() == curCursorX;
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
        curCursorX = get_line_len();
    } else {
        //in editor
        if (curCursorY == 0) {
            savedCursorX = max(curCursorX, savedCursorX); 
            curCursorX = 0; 
            return; 
        }
        curCursorY--;
        curCursorX = max(curCursorX, savedCursorX);
        if (get_line_len() < curCursorX) {
            //bad, save
            savedCursorX = max(curCursorX, savedCursorX);
            curCursorX = get_line_len();
        }
    }
}

void down_arrow() {
    if(curFile == 0) {
        //in terminal
        if(editingHistory == ts[curTerminal].history.size()-1) return;
        editingHistory++;
        setCurTerminalLine();
        curCursorX = get_line_len();
    } else {
        //in editor
        if (curCursorY == get_cur_file().size() - 1) { curCursorX = get_line_len(); return; }
        curCursorY++;
        curCursorX = max(curCursorX, savedCursorX);
        if (get_line_len() < curCursorX) {
            //bad, save
            savedCursorX = max(curCursorX, savedCursorX);
            curCursorX = get_line_len();
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
    if(curCursorX < get_line_len()) {
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
            curCursorX = get_line_len();
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