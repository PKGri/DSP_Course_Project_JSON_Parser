#ifndef JSON_ELEMENT_H
#define JSON_ELEMENT_H

#include <iostream>
#include <string>
#include <list>
#include <queue>

class JSONElement
{
private:
    char type;       //four types: object (o), array (a), string (s), number/boolean/null (n)
    std::string key; //for elements inside objects
    JSONElement *parent;                //parent element
    std::list<JSONElement> children; //objects and arrays have this
    std::string data;                //strings and numbers have this
    size_t ancestors;                //number of ancestors

    void copy(const JSONElement *);
    void parse(std::istream &);

public:
    JSONElement();
    JSONElement(const char);
    JSONElement(const char, const std::string);
    JSONElement(const char, const std::string, const std::list<JSONElement>);
    JSONElement(const char, const std::string, const std::string);
    JSONElement(const char, const std::string, const std::string, const std::list<JSONElement>);
    JSONElement(const JSONElement *);
    ~JSONElement();

    void setType(const char &);
    void setKey(const std::string &);
    void setParent(JSONElement *);
    void setData(const std::string &);
    void setAncestors(const size_t &);

    void updateAncestors(const size_t &);
    void updateChildren();

    void addChild(JSONElement);
    void removeChild(JSONElement *);

    JSONElement &takeByKey(const std::string,JSONElement &);
    JSONElement findByKey(const std::string);

    const char getType() const;
    const std::string &getKey() const;
    const JSONElement *getParent() const;
    JSONElement *getParent();
    const std::list<JSONElement> &getChildren() const;
    std::list<JSONElement> &getChildren();
    const std::string &getData() const;
    const size_t &getAncestors() const;

    void clear();

    JSONElement *traverse(std::queue<std::string> &);

    void operator =(const JSONElement &);
    bool operator ==(const JSONElement &) const;

    void printKey(std::ostream &) const;

    void compressedPrintKey(std::ostream &) const;
    void compressedPrintParent(std::ostream &, const char) const;
    void compressedPrint(std::ostream &) const;
};

void nesting(std::ostream &, const size_t);

std::ostream &operator<<(std::ostream &, const JSONElement &);

std::ostream &operator<<(std::ostream &, const std::list<JSONElement> &);

#endif
