#include <exception>
#include <regex>
#include <sstream>
#include <functional>

#include "JSON_Element.h"

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

void JSONElement::updateChildren()
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

void JSONElement::addChild(JSONElement child)
{
    child.updateAncestors(ancestors + 1);
    children.push_back(child);
}

void JSONElement::removeChild(JSONElement *toRemove)
{   
    std::list<JSONElement>::iterator child = children.begin();
    while (child != children.end())
    {   
        if (&*child == toRemove)
        {
            children.erase(child);
            return;
        }
        child++;
    }
}

JSONElement &takeByKey(const std::string& key, JSONElement &range,JSONElement &result)
{
    for(JSONElement child : range.getChildren())
    {
        if(child.getKey() == key)
        {
            result.addChild(JSONElement(child));
            result.getChildren().back().setKey("");
        }
        takeByKey(key, child, result);
    }

    return result;
}

JSONElement JSONElement::findByKey(const std::string &key)
{
    JSONElement result('a');

    takeByKey(key, *this, result);

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
    children.clear();
    data = "";
}

JSONElement *JSONElement::traverse(std::queue<std::string> &nodes)
{
    if (nodes.empty())
    {
        return this;
    }
    else if (type == 'n' || type == 's')
    {
        throw std::runtime_error("No match!");
    }
    else
    {
        std::string content = nodes.front().substr(1, nodes.front().length() - 2);

        if (content[0] == '\'' && type == 'o')
        {
            content = content.substr(1, content.length() - 2);

            for (JSONElement &child : children)
            {
                if (child.key == content)
                {
                    nodes.pop();
                    return child.traverse(nodes);
                }
            }

            throw std::runtime_error("No match!");
        }
        else if (content[0] != '\'' && type == 'a')
        {
            std::stringstream numberStream(content);
            size_t index;
            numberStream >> index;

            if (index >= children.size())
            {
                throw std::runtime_error("No match!");
            }

            std::list<JSONElement>::iterator it = children.begin();

            std::advance(it, index);

            nodes.pop();
            return it->traverse(nodes);
        }
        else
        {
            throw std::runtime_error("No match!");
        }
    }
}

void JSONElement::operator =(const JSONElement &other)
{
    copy(&other);
}

bool JSONElement::operator ==(const JSONElement &other) const
{
    return type == other.type && key == other.key && 
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

void nesting(std::ostream &os, const size_t ancestors)
{
    for (size_t i = 0; i < ancestors; i++)
    {
        os << "  ";
    }
}

void printParent(std::ostream &os, const JSONElement &element, const char bracket)
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

std::ostream &operator<<(std::ostream &os, const std::list<JSONElement> &children)
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

/////////////////////////// COMPRESSED PRINTING \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

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
