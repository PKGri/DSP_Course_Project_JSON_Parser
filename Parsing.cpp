#include "Parsing.h"

#include <regex>
#include <stack>
#include <queue>

char consent(const std::string question)
{
	std::cout << question << " - 'Y'/'N' ";
	char c;
	do
	{
		std::cin >> c;
	} while (c != 'Y' && c != 'y' && c != 'N' && c != 'n');
	return c;
}


bool isJSONFile(const std::string &filename) // Checks file extension
{
    if (filename.size() < 6)
    {
        return false;
    }
    if (filename.substr(filename.size() - 5, 5) == ".json")
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool areBracketsBalanced(std::string data)
{
    std::stack<char> s;
    char c;

    for (size_t i = 0; i < data.length(); i++)
    {
        if (data[i] == '[' || data[i] == '{')
        {
            s.push(data[i]);
        }

        if (s.empty())
        {
            return false;
        }

        switch (data[i])
        {
        case '}':
            c = s.top();
            s.pop();
            if (c == '[')
            {
                return false;
            }
            break;
        case ']':
            c = s.top();
            s.pop();
            if (c == '{')
            {
                return false;
            }
            break;
        }
    }

    return (s.empty());
}

bool validate()
{
    return false;
}

JSONElement parseValue(std::istream &is, const std::string key)
{
    char c;

    is >> c;

    switch (c)
    {
    case '{':
        return JSONElement('o', key, "", parseObject(is));
    case '[':
        return JSONElement('a', key, "", parseArray(is));
    case '"':
        return JSONElement('s', key, parseString(is));
    case 't':
        return JSONElement('n', key, parseBoolean(is));
    case 'f':
        return JSONElement('n', key, parseBoolean(is));
    case 'n':
        return JSONElement('n', key, parseBoolean(is));
    case '-':
        return JSONElement('n', key, parseNumber(is));
    default:
        return JSONElement('n', key, parseNumber(is));
    }
}

std::list<JSONElement> parseObject(std::istream &is)
{
    char c;
    std::list<JSONElement> result;

    do
    {
        is >> c;

        if (c == '}')
        {
            return result;
        }

        if (c != '"')
        {
            throw std::runtime_error("Expected '\"'!");
        }

        std::string key = parseString(is);

        is >> c;
        if (c != ':')
        {
            throw std::runtime_error("Expected ':'!");
        }

        JSONElement toAdd = parseValue(is, key);
        result.push_back(toAdd);
        is >> c;

    } while (c == ',');

    if (c != '}')
    {
        throw std::runtime_error("Expected '}'!");
    }

    return result;
}

std::list<JSONElement> parseArray(std::istream &is)
{
    char c;
    std::list<JSONElement> result;

    do
    {
        is >> c;

        if (c == ']')
        {
            return result;
        }
        is.unget();

        JSONElement toAdd = parseValue(is, "");
        result.push_back(toAdd);
        is >> c;

    } while (c == ',');

    if (c != ']')
    {
        throw std::runtime_error("Expected ']'!");
    }

    return result;
}

bool specialChar(const char c)
{
    return c == '"' || c == '\\' || c == '/' || c == 'b' || c == 'f' || c == 'n' || c == 'r' || c == 't' || c == 'u';
}

std::string parseString(std::istream &is)
{
    char c;
    std::string result;

    while (is.get(c) && c != '"')
    {
        if (c == '\\')
        {
            if (is.get(c) && specialChar(c))
            {
                result += '\\';
            }
            else
            {
                throw std::runtime_error("Failed to parse string!");
            }
        }
        result += c;
    }

    return result;
}

std::string parseNumber(std::istream &is)
{
    is.unget();
    char c;
    std::string result;

    while (is.get(c) && !isspace(c) && c != ',' && c != ']' && c != '}')
    {
        result += c;
    }
    is.unget();

    std::regex number("-?(?:0|[1-9][0-9]*)(?:.[0-9]+)?(?:[eE][+-]?[0-9]+)?");

    std::smatch matches;
    std::regex_search(result, matches, number);

    if (result != *matches.begin())
    {
        throw std::runtime_error("Failed to parse number!");
    }

    return result;
}

std::string parseBoolean(std::istream &is)
{
    is.unget();
    char c;
    std::string result;

    while (is.get(c) && result.length() < 5)
    {
        result += c;
        if (result == "true" || result == "null" || result == "false")
        {
            return result;
        }
    }
    is.unget();

    throw std::runtime_error("Failed to parse boolean!");
}

std::queue<std::string> parseJSONPath(std::string JSONPath) // Creates vector of strings for easier traversal one by one later
{

    JSONPath = JSONPath.substr(JSONPath.find_first_not_of(" \t\v\r\n"),
                               JSONPath.find_last_not_of(" \t\v\r\n") - JSONPath.find_first_not_of(" \t\v\r\n") + 1); // Removes leading and trailing whitespaces

    std::regex pathReg("[$]?((\\['[^\\[]*'\\])|(\\[(?:0|[1-9][0-9]*)\\]))*");

    std::smatch matches;

    std::regex_search(JSONPath, matches, pathReg);

    if (matches[0] != JSONPath)
    {
        throw std::runtime_error("Invalid JSONPath!");
    }

    std::regex node("(\\['[^\\[]*'\\])|(\\[(?:0|[1-9][0-9]*)\\])");

    std::queue<std::string> nodes;

    while (std::regex_search(JSONPath, matches, node))
    {
        nodes.push(matches[0]);
        JSONPath = matches.suffix();
    }

    return nodes;
}
