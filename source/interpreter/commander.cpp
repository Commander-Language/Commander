#include "source/flow_controller/flow_controller.hpp"
#include "source/lexer/lexer.hpp"
#include "source/parser/parser.hpp"
#include "source/type_checker/type_checker.hpp"
#include "source/util/commander_exception.hpp"
#include <fstream>
#include <iostream>
#include <ncurses.h>

void interpretFile(std::string& fileName, std::vector<std::string>& arguments, Parser::Parser& parser,
                   TypeChecker::TypeChecker& typeChecker) {
    Lexer::TokenList tokens;
    Lexer::tokenize(tokens, fileName);
    if (std::find(arguments.begin(), arguments.end(), "-l") != arguments.end()) {
        for (const Lexer::TokenPtr& token : tokens) printw("%s\n", token->toString().c_str());
        return;
    }
    Parser::ASTNodeList nodes = parser.parse(tokens);
    if (std::find(arguments.begin(), arguments.end(), "-p") != arguments.end()) {
        for (const Parser::ASTNodePtr& node : nodes) printw("%s\n", node->sExpression().c_str());
        return;
    }
    typeChecker.typeCheck(nodes);
    if (std::find(arguments.begin(), arguments.end(), "-t") != arguments.end()) {
        for (const Parser::ASTNodePtr& node : nodes) printw("%s\n", node->sExpression().c_str());
        return;
    }
    FlowController::FlowController controller(nodes);
    controller.runtime();
}

int main(int argc, char** argv) {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    curs_set(2);
    printw("Commander Language Version Beta\n");
    printw("Basic REPL for Commander scripting language\n");
    printw(">> ");
    refresh();
    std::vector<std::string> arguments;
    for (int i = 1; i < argc; i++) { arguments.push_back(argv[i]); }
    try {
        Parser::Parser parser;
        TypeChecker::TypeChecker typeChecker;
        auto fileIterator = std::find(arguments.begin(), arguments.end(), "-f");
        if (fileIterator++ != arguments.end()) {
            if (fileIterator == arguments.end()) { throw Util::CommanderException("No file name provided."); }
            std::string file = *fileIterator;
            interpretFile(file, arguments, parser, typeChecker);
            return 0;
        }
        std::string currentLine;
        std::vector<std::string> lines;
        int line = 0;
        int i = 0;
        while (true) {
            int ch = getch();
            switch (ch) {
                case KEY_ENTER:
                case '\n':
                    move(getcury(stdscr), 3);
                    printw("%s\n", currentLine.c_str());
                    if (currentLine == "exit") {
                        endwin();
                        return 0;
                    }
                    if (currentLine == "clear") {
                        clear();
                    } else {
                        try {
                            // Make a temporary file for the lexer
                            std::string tmpFileName = std::tmpnam(nullptr);  // Not thread safe!!
                            std::ofstream tmp(tmpFileName);
                            tmp << currentLine;
                            tmp.close();  // close file here to save changes
                            interpretFile(tmpFileName, arguments, parser, typeChecker);
                            std::remove(tmpFileName.c_str());
                        } catch (const Util::CommanderException& err) { printw("%s\n", err.what()); }
                    }
                    i = 0;
                    printw(">> ");
                    if (lines.size() == 0 || lines.back() != currentLine) { lines.push_back(currentLine); }
                    line = lines.size();
                    currentLine.clear();
                    break;
                case KEY_BACKSPACE:
                    if (getcurx(stdscr) > 3) {
                        move(getcury(stdscr), getcurx(stdscr) - 1);  // Move cursor back
                        delch();                                     // Delete character
                        currentLine.erase(currentLine.begin() + --i);
                    }
                    break;
                case KEY_UP:
                    if (line == 0) { break; }
                    currentLine = lines[--line];
                    move(getcury(stdscr), 3);
                    clrtoeol();
                    printw("%s", currentLine.c_str());
                    i = currentLine.size();
                    break;
                case KEY_DOWN:
                    if (line == lines.size()) { break; }
                    if (++line == lines.size()) {
                        currentLine = "";
                    } else {
                        currentLine = lines[line];
                    }
                    move(getcury(stdscr), 3);
                    clrtoeol();
                    printw("%s", currentLine.c_str());
                    i = currentLine.size();
                    break;
                case KEY_LEFT:
                    if (getcurx(stdscr) > 3) {
                        move(getcury(stdscr), getcurx(stdscr) - 1);
                        i--;
                    }
                    break;
                case KEY_RIGHT:
                    if (getcurx(stdscr) - 3 < currentLine.size()) {
                        move(getcury(stdscr), getcurx(stdscr) + 1);
                        i++;
                    }
                    break;
                default:
                    if (getcurx(stdscr) < getmaxx(stdscr)) {
                        insch(ch);
                        currentLine.insert(i++, std::string(1, (char)ch));
                        move(getcury(stdscr), getcurx(stdscr) + 1);
                    }
            }
            refresh();
        }
        endwin();
        return 0;
    } catch (const Util::CommanderException& e) {
        endwin();
        std::cout << e.what() << '\n';
        return 1;
    }
}