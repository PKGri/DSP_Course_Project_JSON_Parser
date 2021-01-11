#ifndef PARSING_H
#define PARSING_H

#include "JSON_File.h"
#include <iostream>
#include <sstream>

bool isJSONFile(const std::string &);

JSONElement parseValue(std::istream &, const std::string);
std::list<JSONElement> parseObject(std::istream &);
std::list<JSONElement> parseArray(std::istream &);
std::string parseString(std::istream &);
std::string parseNumber(std::istream &);
std::string parseBoolean(std::istream &);

std::queue<std::string> parseJSONPath(std::string);

#endif
