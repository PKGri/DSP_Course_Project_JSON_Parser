#include <iostream>
#include <sstream>

#include "Commands.h"
#include "JSON_File.h"

std::string command;
JSONFile file;

std::string inputJSONPath()
{
	std::string JSONPath;
	do
	{
		getline(std::cin, JSONPath);
	} while (JSONPath.find_first_not_of(" \t\v\r\n") == std::string::npos);

	return JSONPath;
}

//**********************************************************************************************************\\
|-----------------------------------------> COMMAND FUNCTIONS <----------------------------------------------|

void open() // Opens file
{
	file.open();

	enterCommand(&isCloseOrExit, &openCommandList);

	std::cout << "Closing...\n";
	file.clear();
	std::cout << "Closed\n";
}

void save() // Saves file to original path
{
	file.save();
	std::cout << "Succesfully saved " << file.getPath() << '\n';
}

void saveAs() // Saves file to specified path
{
	file.saveAs();
	std::cout << "Succesfully saved as " << file.getPath() << '\n';
}

void saveElement() // Saves file to original path
{
	std::cout << "Please input JSONPath: ";
	std::string JSONPath = inputJSONPath();

	file.saveElement(JSONPath);
	std::cout << "Succesfully saved " << file.getPath() << '\n';
}

void saveElementAs() // Saves file to specified path
{
	std::cout << "Please input JSONPath: ";
	std::string JSONPath = inputJSONPath();

	file.saveElementAs(JSONPath);
	std::cout << "Succesfully saved as " << file.getPath() << '\n';
}

void saveSelected() // Saves file to original path
{
	file.saveSelected();
	std::cout << "Succesfully saved selected in" << file.getPath() << '\n';
}

void saveSelectedAs() // Saves file to specified path
{
	file.saveSelectedAs();
	std::cout << "Succesfully saved selected as " << file.getPath() << '\n';
}


void help() // Prints supported functionalities
{
	std::cout << "\nThe following commands are supported:\n\n"
				 "open <file>\n"
				 " -reads information from <file>\n"
				 "close\n"
				 " -frees memory\n"
				 "save\n"
				 " -saves information to origin file\n"
				 "saveas <file>\n"
				 " -saves information to <file>\n"
				 "saveelement <JSONPath>\n"
				 " -saves result of <JSONPath> evaluation to origin file\n"
				 "saveelementas <JSONPath>\n<file>\n"
				 " -saves result of <JSONPath> evaluation to <file>\n"
				 "select <key>\n"
				 " -creates an array of all elements with <key> as their key\n"
				 " -and sets that as selected object\n"
				 "set <JSONPath>\n<JSON Object>\n"
				 " -sets the result of the evaluated <JSONPath> to the evaluation of <JSON Object>"
				 "(work in progress)create <JSONPath>\n<JSON Object>\n"
				 " -creates object at given location"
				 "remove <JSONPath>\n"
				 " -removes the result of the evaluated <JSONPath>"
				 "(work in progress)move <JSONPath 1>\n<JSONPath 2>\n"
				 " -moves object found at <JSONPath 1> to <JSONPath 2>"
				 "(work in progress)order <JSONPath>"
				 " -orders result of <JSONPath> evaluation if result is array"
				 "JSONPath <JSONPath>\n"
				 " -prints the result from evaluating simple <JSONPath> expressions\n"
				 "print\n"
				 " -prints out current content\n"
				 "printselected\n"
				 " -prints out selected content\n"
				 "help\n"
				 " -prints this information\n"
				 "exit\n"
				 " -exits the program\n";
}

void print() // Prints loaded file data
{
	file.print();
}

void printSelected() // Prints loaded file data
{
	file.printSelected();
}

void select()
{
	std::string key;
	std::cin >> key;

	file.findByKey(key);
}

void set()
{
	std::string JSONPath = inputJSONPath();

	file.set(JSONPath);
}

void create()
{
}

void remove()
{
	std::string JSONPath = inputJSONPath();

	file.remove(JSONPath);
}

void move()
{
}

void order()
{
}

void selectedAsWorking()
{
	file.selectedAsWorking();
}

void reset()
{
	file.reset();
}

void JSONPath()
{
	std::string JSONPath = inputJSONPath();

	file.JSONPath(JSONPath);
}

//**********************************************************************************************************\\
|----------------------------------------> COMMANDLINE CODE <------------------------------------------------|

bool mainCommandList(std::string command)
{
	return command == "open" || command == "help" || command == "exit";
}

bool openCommandList(std::string command)
{
	return command == "close" || command == "save" || command == "saveas" || command == "saveelement" 
	|| command == "saveelementas" || command == "saveselected" || command == "saveselectedas" 
	|| command == "help" || command == "exit" || command == "print" || command == "printselected" 
	|| command == "select" || command == "set" || command == "create" || command == "remove" 
	|| command == "swap" || command == "order" || command == "JSONPath" || command == "selectedasworking" 
	|| command == "reset";
}

void executeCommand(std::string command)
{
	if (command == "help")
		help();
	else if (command == "open")
		open();
	else if (command == "save")
		save();
	else if (command == "saveas")
		saveAs();
	else if (command == "saveelement")
		saveElement();
	else if (command == "saveelementas")
		saveElementAs();
	else if (command == "saveselected")
		saveSelected();
	else if (command == "saveselectedas")
		saveSelectedAs();
	else if (command == "print")
		print();
	else if (command == "printselected")
		printSelected();
	else if (command == "select")
		select();
	else if (command == "set")
		set();
	else if (command == "create")
		create();
	else if (command == "remove")
		remove();
	else if (command == "move")
		move();
	else if (command == "order")
		order();
	else if (command == "selectedasworking")
		selectedAsWorking();
	else if (command == "reset")
		reset();

	else if (command == "JSONPath")
		JSONPath();
}

bool isExit(std::string command)
{
	return command == "exit";
}

bool isCloseOrExit(std::string command)
{
	return command == "close" || command == "exit";
}

void enterCommand(std::function<bool(std::string)> terminationPredicate,
				  std::function<bool(std::string)> commandList)
{
	do
	{
		do
		{
			std::cout << '>';
			std::cin >> command;

		} while (!commandList(command));

		executeCommand(command);

	} while (!terminationPredicate(command));
}
