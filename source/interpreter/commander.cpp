#include "source/flow_controller/flow_controller.hpp"
#include "source/lexer/lexer.hpp"
#include "source/type_checker/type_checker.hpp"
#include <fstream>
#include <iostream>
#include <ncurses.h>

void interpretFile(std::string& fileName, std::vector<std::string>& arguments, Parser::Parser& parser,
                   TypeChecker::TypeChecker& typeChecker) {
    Lexer::TokenList tokens;
    Lexer::tokenize(tokens, fileName);
    if (std::find(arguments.begin(), arguments.end(), "-l") != arguments.end()) {
        for (const Lexer::TokenPtr& token : tokens) Util::println(token->toString());
        return;
    }
    Parser::ASTNodeList nodes = parser.parse(tokens);
    if (std::find(arguments.begin(), arguments.end(), "-p") != arguments.end()) {
        for (const Parser::ASTNodePtr& node : nodes) Util::println(node->sExpression());
        return;
    }
    typeChecker.typeCheck(nodes);
    if (std::find(arguments.begin(), arguments.end(), "-t") != arguments.end()) {
        for (const Parser::ASTNodePtr& node : nodes) Util::println(node->sExpression());
        return;
    }
    FlowController::FlowController controller(nodes);
    controller.runtime();
}

int main(int argc, char** argv) {
    std::vector<std::string> arguments;
    for (int i = 1; i < argc; i++) { arguments.emplace_back(argv[i]); }
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
        Util::usingNCurses = true;
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        curs_set(2);
        Util::println("Commander Language Version Beta");
        Util::println("Basic REPL for Commander scripting language");
        Util::print(">> ");
        refresh();
        std::string currentLine;
        std::vector<std::string> lines;
        int line = 0;
        int idx = 0;
        while (true) {
            int chr = getch();
            switch (chr) {
                case KEY_ENTER:
                case '\n':
                    move(getcury(stdscr), 3);
                    Util::println(currentLine);
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
                        } catch (const Util::CommanderException& err) { Util::println(err.what()); }
                    }
                    idx = 0;
                    Util::print(">> ");
                    if (lines.empty() || lines.back() != currentLine) { lines.push_back(currentLine); }
                    line = (int)lines.size();
                    currentLine.clear();
                    break;
                case KEY_BACKSPACE:
                    if (getcurx(stdscr) > 3) {
                        move(getcury(stdscr), getcurx(stdscr) - 1);  // Move cursor back
                        delch();                                     // Delete character
                        currentLine.erase(currentLine.begin() + --idx);
                    }
                    break;
                case KEY_UP:
                    if (line == 0) { break; }
                    currentLine = lines[--line];
                    move(getcury(stdscr), 3);
                    clrtoeol();
                    Util::print(currentLine);
                    idx = (int)currentLine.size();
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
                    Util::print(currentLine);
                    idx = (int)currentLine.size();
                    break;
                case KEY_LEFT:
                    if (getcurx(stdscr) > 3) {
                        move(getcury(stdscr), getcurx(stdscr) - 1);
                        idx--;
                    }
                    break;
                case KEY_RIGHT:
                    if (getcurx(stdscr) - 3 < currentLine.size()) {
                        move(getcury(stdscr), getcurx(stdscr) + 1);
                        idx++;
                    }
                    break;
                default:
                    if (getcurx(stdscr) < getmaxx(stdscr)) {
                        insch(chr);
                        currentLine.insert(idx++, std::string(1, (char)chr));
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