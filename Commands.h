#ifndef COMMANDS_H
#define COMMANDS_H

#include <string>
#include <functional>
#include "JSON_File.h"

void open();
void save();
void saveAs();
void saveElement();
void saveElementAs();
void help();

void print();
void select();
void set();
void create();
void remove();
void move();
void order();
//void reset();

void JSONPath();

bool mainCommandList(std::string);
bool openCommandList(std::string);
void executeCommand(std::string);
bool isExit(std::string);
bool isCloseOrExit(std::string);

void enterCommand(std::function<bool(std::string)> terminationPredicate, std::function<bool(std::string)> commandList);

#endif
