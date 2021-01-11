#include "JSON_File.h"
#include "Parsing.h"

#include <fstream>
#include <sstream>
#include <stack>

JSONFile::JSONFile() : path(), fileObject() /*, workingObject(nullptr)*/ {}

JSONFile::~JSONFile() {}

void JSONFile::updateAncestors()
{
	fileObject.updateAncestors(0);
}

void JSONFile::updateChildren()
{
	fileObject.updateChildren();
}

void JSONFile::open()
{
	std::cin >> path; // File path

	if (isJSONFile(path))
	{
		std::ifstream json_file(path.data());

		if (!json_file) // Creates new file with given path if nonexistent
		{
			json_file.close();
			std::ofstream new_file(path.data());
			new_file.close();
			std::cout << "Successfully created " << path << '\n';
			return;
		}

		std::cout << "Opening...\n";

		// READS DATA \\

		char c;
		json_file >> c;

		if (c != '{' && c != '[')
		{
			throw std::runtime_error("Expected '{' or '[' at beginning of file!");
		}

		if (c == '{')
		{
			json_file.unget();
			fileObject = parseValue(json_file, "");
		}
		else
		{
			json_file.unget();
			fileObject = parseValue(json_file, "");
		}

		updateAncestors();

		updateChildren();

		std::cout << "Successfully opened " << path << '\n';

		json_file.close();

		saveObject = fileObject;
	}
	else
	{
		std::cout << "Invalid file\n";
	}
}

void JSONFile::print()
{
	std::cout << fileObject << '\n';
}

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

void JSONFile::save()
{
	std::ofstream outputFile(path.data());

	char c = consent("Do you wish to save object compressed?");

	switch (c)
	{
	case 'Y':
		fileObject.compressedPrint(outputFile);
		break;
	case 'y':
		fileObject.compressedPrint(outputFile);
		break;
	case 'N':
		outputFile << fileObject;
		break;
	case 'n':
		outputFile << fileObject;
		break;
	}

	outputFile.close();
}

void JSONFile::saveAs()
{
	std::cout << "Please input save path: ";

	std::string _path;
	std::cin >> _path;

	std::ifstream check;
	check.open(_path);

	if (!check.fail())
	{
		check.close();
		char c = consent("File already exists. Overwrite?");

		switch (c)
		{
		case 'Y':
			path = _path;
			break;
		case 'y':
			path = _path;
			break;
		case 'N':
			return;
		case 'n':
			return;
		}
	}

	save();
}

void compressedPrint(std::ofstream &outputFile, JSONElement *toPrint)
{
	outputFile << '{';
	toPrint->compressedPrint(outputFile);
	outputFile << '}';
}

void uncompressedPrint(std::ofstream &outputFile, JSONElement *toPrint)
{
	toPrint->updateAncestors(1);
	outputFile << "{\n";
	outputFile << *toPrint;
	outputFile << "\n}";
}

void JSONFile::saveElement(const std::string JSONPath)
{
	std::queue<std::string> nodes = parseJSONPath(JSONPath);
	JSONElement *toPrint = fileObject.traverse(nodes);
	toPrint->updateAncestors(0);

	std::ofstream outputFile(path.data());

	char c = consent("Do you wish to save object compressed?");

	switch (c)
	{
	case 'Y':
		compressedPrint(outputFile, toPrint);
		break;
	case 'y':
		compressedPrint(outputFile, toPrint);
		break;
	case 'N':
		uncompressedPrint(outputFile, toPrint);
		break;
	case 'n':
		uncompressedPrint(outputFile, toPrint);
		break;
	}

	outputFile.close();

	updateAncestors();
}

void JSONFile::saveElementAs(const std::string JSONPath)
{
	std::cout << "Please input save path: ";

	std::string _path;
	std::cin >> _path;

	std::ifstream check;
	check.open(_path);

	if (!check.fail())
	{
		check.close();
		char c = consent("File already exists. Overwrite?");

		switch (c)
		{
		case 'Y':
			path = _path;
			break;
		case 'y':
			path = _path;
			break;
		case 'N':
			return;
		case 'n':
			return;
		}
	}

	saveElement(JSONPath);
}

void JSONFile::JSONPath(const std::string JSONPath)
{
	std::queue<std::string> nodes = parseJSONPath(JSONPath);
	std::cout << *fileObject.traverse(nodes) << '\n';
}

void JSONFile::findByKey(const std::string &key)
{
	fileObject = fileObject.findByKey(key);
}

void JSONFile::set(const std::string JSONPath)
{
	std::queue<std::string> nodes = parseJSONPath(JSONPath);

	char c;
	std::cin >> c;

	if (c != '{' && c != '[')
	{
		throw std::runtime_error("Expected '{' or '['!");
	}

	if (c == '{')
	{
		JSONElement &toSet = *fileObject.traverse(nodes);
		std::cout << toSet;
		toSet.setType('o');
		toSet.getChildren() = parseObject(std::cin);
	}
	else
	{
		JSONElement &toSet = *fileObject.traverse(nodes);
		std::cout << toSet;
		toSet.setType('o');
		toSet.getChildren() = parseObject(std::cin);
	}

	updateAncestors();
}

void JSONFile::create()
{
}

void JSONFile::remove(const std::string JSONPath)
{
	std::queue<std::string> nodes = parseJSONPath(JSONPath);
	JSONElement *toRemove = fileObject.traverse(nodes);
	JSONElement *parent = toRemove->getParent();

	parent->removeChild(toRemove);
}

void JSONFile::move()
{
}

void JSONFile::order()
{
}

void JSONFile::reset()
{
	fileObject = saveObject;
}

std::string JSONFile::getPath()
{
	return path;
}

void JSONFile::clear()
{
	path = "";
	saveObject.clear();
	fileObject.clear();
}
