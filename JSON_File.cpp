#include "JSON_File.h"
#include "Parsing.h"

#include <fstream>
#include <sstream>
#include <stack>

JSONFile::JSONFile() : path(), fileObject() , workingObject(nullptr) {}

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
			std::cout << "Successfully created " << path << "\nPlease close!\n";
			return;
		}

		std::cout << "Opening...\n";

		// READS DATA \\

		char c;
		json_file >> c;

		if (c != '{' && c != '[') // Main element must be object or array
		{
			throw std::runtime_error("Expected '{' or '[' at beginning of file!");
		}

		if (c == '{') // If object
		{
			json_file.unget();
			fileObject = parseValue(json_file, "");
		}
		else // If array
		{
			json_file.unget();
			fileObject = parseValue(json_file, "");
		}

		updateAncestors(); // For proper printing

		updateChildren(); // Removes duplicates

		std::cout << "Successfully opened " << path << '\n';

		json_file.close();

		workingObject = &fileObject;
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

void JSONFile::printSelected()
{
	std::cout << selectedObject << '\n';
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

void JSONFile::saveElement(const std::string JSONPath) // Saves element at <JSONPath> in the context of the working object
{
	std::queue<std::string> nodes = parseJSONPath(JSONPath);
	JSONElement *toPrint = workingObject->traverse(nodes);
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

void JSONFile::saveSelected() // Saves selected object
{
	std::ofstream outputFile(path.data());

	char c = consent("Do you wish to save object compressed?");

	switch (c)
	{
	case 'Y':
		selectedObject.compressedPrint(outputFile);
		break;
	case 'y':
		selectedObject.compressedPrint(outputFile);
		break;
	case 'N':
		outputFile << selectedObject;
		break;
	case 'n':
		outputFile << selectedObject;
		break;
	}

	outputFile.close();
}

void JSONFile::saveSelectedAs()
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

	saveSelected();
}

void JSONFile::JSONPath(const std::string JSONPath) // Prints element at <JSONPath> in the context of the working object 
{
	std::queue<std::string> nodes = parseJSONPath(JSONPath);
	std::cout << *workingObject->traverse(nodes) << '\n';
}

void JSONFile::findByKey(const std::string key) // Used by select function
{
	if (key == "$")
	{
		selectedObject = fileObject;
	}
	else
	{
		selectedObject = workingObject->findByKey(key);
	}
	
}

void JSONFile::set(const std::string JSONPath) // Used by set function
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
		toSet.setType('o');
		toSet.getChildren() = parseObject(std::cin);
	}
	else
	{
		JSONElement &toSet = *fileObject.traverse(nodes);
		toSet.setType('a');
		toSet.getChildren() = parseArray(std::cin);
	}

	updateAncestors();
}

void JSONFile::create(const std::string JSONPath) //(work in progress)
{
	std::queue<std::string> toCreate = parseJSONPath(JSONPath);
	fileObject.createPath(toCreate);


}

void JSONFile::remove(const std::string JSONPath) // Removes element at <JSONPath>
{
	std::queue<std::string> nodes = parseJSONPath(JSONPath);
	JSONElement *toRemove = fileObject.traverse(nodes);
	JSONElement *parent = toRemove->getParent();

	parent->removeChild(toRemove);
}

void JSONFile::move(const std::string JSONPath1, const std::string JSONPath2) //(work in progress)
{
	std::queue<std::string> toFind = parseJSONPath(JSONPath1);
	JSONElement *found = fileObject.traverse(toFind);

	std::queue<std::string> toMove = parseJSONPath(JSONPath2);
	fileObject.createPath(toMove);
}

void JSONFile::order(const std::string JSONPath) // Used by order
{
	std::string key;
	std::cout << "Please input key:\n";
	std::cin >> key;

	std::queue<std::string> nodes = parseJSONPath(JSONPath);
	JSONElement *toOrder = fileObject.traverse(nodes);

	toOrder->order(key);

}

void JSONFile::selectedAsWorking() // Change working object to selected object
{
	workingObject = &selectedObject;
	std::cout << "Working on selected object!\n";
}

void JSONFile::reset() // Change working object to file object
{
	workingObject = &fileObject;
	std::cout << "Working on original object!\n";
}

std::string JSONFile::getPath()
{
	return path;
}

void JSONFile::clear()
{
	path = "";
	workingObject = nullptr;
	selectedObject.clear();
	fileObject.clear();
}
