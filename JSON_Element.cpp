#include <exception>
#include <sstream>
#include <functional>

#include "JSON_Element.h"
#include "Parsing.h"

JSONElement::JSONElement() {}

JSONElement::JSONElement(const char t) : type(t), ancestors(0) {}

JSONElement::JSONElement(const char t, const std::string _data) : type(t), parent(nullptr), data(_data) {}

JSONElement::JSONElement(const char t, const std::string _data, const std::list<JSONElement> _children) : type(t), parent(nullptr), data(_data), children(_children) {}

JSONElement::JSONElement(const char t, const std::string _key, const std::string _data) : type(t), parent(nullptr), key(_key), data(_data) {}

JSONElement::JSONElement(const char t, const std::string _key, const std::string _data, const std::list<JSONElement> _children) : type(t), parent(nullptr), key(_key), data(_data), children(_children) {}

JSONElement::JSONElement(const JSONElement *other)
{
    copy(other);
}

JSONElement::~JSONElement() {}

void JSONElement::copy(const JSONElement *other)
{
    type = other->type;
    parent = other->parent;
    children = other->children;
    data = other->data;
    ancestors = other->ancestors;
}

void JSONElement::setType(const char &t)
{
    if (t != 'o' && t != 'a' && t != 's' && t != 'n')
    {
        throw std::runtime_error("Invalid Type");
    }
    type = t;
}

void JSONElement::setKey(const std::string &_key)
{
    key = _key;
}

void JSONElement::setData(const std::string &_data)
{
    data = _data;
}

void JSONElement::setParent(JSONElement *_parent)
{
    parent = _parent;
}

void JSONElement::setAncestors(const size_t &_ancestors)
{
    ancestors = _ancestors;
}

void JSONElement::updateAncestors(const size_t &ancestors)
{
    setAncestors(ancestors);

    for (JSONElement &child : children)
    {
        child.setParent(this);
        child.updateAncestors(ancestors + 1);
    }
}

void JSONElement::updateChildren() // Removes duplicate children in objects
{
    if (type == 'o')
    {
        for (std::list<JSONElement>::reverse_iterator i = children.rbegin(); i != children.rend(); i++)
        {
            std::list<JSONElement>::reverse_iterator j = i;
            j++;
            for (j; j != children.rend(); j++)
            {
                if (j->key == i->key)
                {
                    std::list<JSONElement>::reverse_iterator toErase = j;
                    toErase++;
                    j--;
                    if (toErase == children.rend())
                    {
                        children.erase((toErase).base());
                        break;
                    }
                    else
                    {
                        children.erase((toErase).base());
                    }
                }
            }
            i->updateChildren();
        }
    }
    else if (type == 'a')
    {
        for (JSONElement &child : children)
        {
            child.updateChildren();
        }
    }
}

void JSONElement::createPath(std::queue<std::string> &nodes)
{
    std::queue<std::string> toCreate = nodes; //(work in progress) Creates objects

    try
    {
        traverse(nodes);
    }
    catch (const std::exception &e)
    {
        char c = consent("Path does not exist! Create?");
        switch (c)
        {
        case 'Y':
            break;
        case 'y':
            break;
        case 'N':
            return;
        case 'n':
            return;
        }
    }
}

void JSONElement::addChild(JSONElement child) // Adds child to this->children
{
    child.updateAncestors(ancestors + 1);
    children.push_back(child);
}

void JSONElement::removeChild(JSONElement *toRemove) // Removes child from this-children
{
    std::list<JSONElement>::iterator child = children.begin();
    while (child != children.end())
    {
        std::cout << toRemove << " TO REMOVE " << *toRemove << '\n';
        std::cout << &*child << " CHILD " << child->data << '\n';

        if (&*child == toRemove)
        {
            children.erase(child);
            return;
        }
        child++;
    }
}

const std::string &JSONElement::getDataByKey(const std::string _key) // Returns data if it's key is equal to _key
{
    for (JSONElement &child : children)
    {
        if (child.key == _key)
        {
            return child.data;
        }
    }

    throw std::runtime_error("Element has no such child!");
}

bool JSONElement::hasChildWithKey(const std::string _key) // Checks if object has child with key equal to _key
{
    for (JSONElement &child : children)
    {
        if (child.key == _key && (child.type == 'n' || child.type == 's'))
        {
            return true;
        }
    }
    return false;
}

bool JSONElement::checkChildren(const std::string _key) // Checks if all children in array are objects and contain a child with key equal to _key
{
    for (JSONElement &child : children)
    {
        if (child.type != 'o' || !child.hasChildWithKey(_key))
        {
            return false;
        }
    }
    return true;
}

bool compare(JSONElement &first, JSONElement&second, std::string _key) // Comp function to use for sorting array
{
    return first.getDataByKey(_key).compare(second.getDataByKey(_key)) == -1;
}

void JSONElement::order(const std::string _key) // Order array elements by data corresponding to _key
{
    if (type != 'a')
    {
        throw std::runtime_error("Element is not array!");
    }

    if (checkChildren(_key)) // Can order children only if they are all objects and have a child with key equal to key
    {
        auto compareCurry = std::bind(compare, std::placeholders::_1, std::placeholders::_2, _key);
        children.sort(compareCurry);
    }
    else
    {
        throw std::runtime_error("Cannot order elements!");
    }
}

JSONElement &JSONElement::takeByKey(const std::string _key, JSONElement &result) // Goes through children and their children searching for elements with key equal to _key
{
    for (JSONElement child : children)
    {
        if (child.key == _key)
        {
            result.addChild(JSONElement(child));
            result.children.back().setKey("");
        }
        child.takeByKey(_key, result);
    }

    return result;
}

JSONElement JSONElement::findByKey(const std::string _key) // Creates an array of all elements with key equal to _key
{
    JSONElement result('a');

    takeByKey(_key, result);

    result.updateAncestors(0);

    return result;
}

const char JSONElement::getType() const
{
    return type;
}

const std::string &JSONElement::getKey() const
{
    return key;
}

const JSONElement *JSONElement::getParent() const
{
    return parent;
}

JSONElement *JSONElement::getParent()
{
    return parent;
}

const std::list<JSONElement> &JSONElement::getChildren() const
{
    return children;
}

std::list<JSONElement> &JSONElement::getChildren()
{
    return children;
}

const std::string &JSONElement::getData() const
{
    return data;
}

const size_t &JSONElement::getAncestors() const
{
    return ancestors;
}

void JSONElement::clear()
{
    type = 0;
    key = "";
    parent = nullptr;
    children.clear();
    data = "";
    ancestors = 0;
}

JSONElement *JSONElement::traverse(std::queue<std::string> &nodes) // Recursively finds element for JSONPath evaluation
{
    if (nodes.empty()) //If this is the sought after element
    {
        return this;
    }
    else if (type == 'n' || type == 's') // If nodes isn't empty, but *this is neither object, nor array
    {
        throw std::runtime_error("No match!");
    }
    else
    {
        std::string content = nodes.front().substr(1, nodes.front().length() - 2); // Removes [ ] brackets from node string

        if (content[0] == '\'' && type == 'o') //If current node is a key and current element is an object
        {
            content = content.substr(1, content.length() - 2); // Removes '' from both ends of the string to compare

            for (JSONElement &child : children)
            {
                if (child.key == content)
                {
                    nodes.pop(); // Readies next node
                    return child.traverse(nodes); // Continues on down
                }
            }

            throw std::runtime_error("No match!");
        }
        else if (content[0] != '\'' && type == 'a') // If current node is an index and current element is an array
        {
            std::stringstream numberStream(content); // To convert to number
            size_t index;
            numberStream >> index;

            if (index >= children.size())
            {
                throw std::runtime_error("No match!");
            }

            std::list<JSONElement>::iterator it = children.begin();

            std::advance(it, index);

            nodes.pop(); // Readies next node
            return it->traverse(nodes); // Continues on down
        }
        else
        {
            throw std::runtime_error("No match!");
        }
    }
}

void JSONElement::operator=(const JSONElement &other)
{
    copy(&other);
}

bool JSONElement::operator==(const JSONElement &other) const
{
    return type == other.type && key == other.key && parent == other.parent &&
           data == other.data && children == other.children &&
           ancestors == other.ancestors;
}

void JSONElement::printKey(std::ostream &os) const
{
    if (!key.empty())
    {
        nesting(os, ancestors);
        os << '"' << key << "\" : ";
    }
    else
    {
        nesting(os, ancestors);
    }
}

void nesting(std::ostream &os, const size_t ancestors) // Adds necessary indentation when printing
{
    for (size_t i = 0; i < ancestors; i++)
    {
        os << "  ";
    }
}

void printParent(std::ostream &os, const JSONElement &element, const char bracket) // Function to print objects and arrays (because they are parents)
{
    element.printKey(os);
    os << bracket;
    if (!element.getChildren().empty())
    {
        os << '\n';
        os << element.getChildren();
        nesting(os, element.getAncestors());
    }
    os << (char)(bracket + 2);
}

std::ostream &operator<<(std::ostream &os, const JSONElement &element)
{
    switch (element.getType())
    {
    case 's':
        element.printKey(os);
        os << '"' << element.getData() << '"';
        break;
    case 'n':
        element.printKey(os);
        os << element.getData();
        break;
    case 'o':
        printParent(os, element, '{');
        break;
    case 'a':
        printParent(os, element, '[');
        break;
    default:
        throw std::runtime_error("Missing Type!");
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, const std::list<JSONElement> &children) // To make printing children easier
{
    for (const JSONElement &child : children)
    {
        os << child;
        if (&child != &children.back())
        {
            os << ',';
        }
        os << '\n';
    }
    return os;
}

//////////////////////////// COMPRESSED PRINTING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
**************** FOR WHEN USER WISHES TO SAVE 1KB OF WHITESPACES ******************

void JSONElement::compressedPrintKey(std::ostream &os) const
{
    if (!key.empty())
    {
        os << '"' << key << "\":";
    }
}

void compressedPrintChildren(std::ostream &os, const std::list<JSONElement> &children)
{
    for (const JSONElement &child : children)
    {
        child.compressedPrint(os);
        if (&child != &children.back())
        {
            os << ',';
        }
    }
}

void JSONElement::compressedPrintParent(std::ostream &os, const char bracket) const
{
    compressedPrintKey(os);
    os << bracket;
    if (!children.empty())
    {
        compressedPrintChildren(os, children);
    }
    os << (char)(bracket + 2);
}

void JSONElement::compressedPrint(std::ostream &os) const
{
    switch (type)
    {
    case 's':
        compressedPrintKey(os);
        os << '"' << data << '"';
        break;
    case 'n':
        compressedPrintKey(os);
        os << data;
        break;
    case 'o':
        compressedPrintParent(os, '{');
        break;
    case 'a':
        compressedPrintParent(os, '[');
        break;
    default:
        throw std::runtime_error("Missing Type!");
    }
}
