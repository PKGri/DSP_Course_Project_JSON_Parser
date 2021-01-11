#ifndef JSON_FILE_H
#define JSON_FILE_H

#include "JSON_Element.h"
/*
Създаване на обект.
При подаване на пълен път към даден елемент и символен низ, Вашата програма
трябва да добави такъв елемент и да му присвои прочетената според синтаксиса
на JSON стойност на низа. Ако такъв елемент съществува, трябва да се обяви
това като проблем. В противен случай да се добави елемента, евентуално с
рекурсивно създаване на целия път. Ако символният низ не е коректен, да
се съобщи с подходящо описание на грешката.

Преместване. 
При подаване на два пътя, всички елементи, намиращи се на на първия път,
да бъдат преместени на втория.

Подреждане. 
По зададен път до елемент и име на ключ, ако елементът е масив, елементите
му да бъдат подредени според стойностите на дадения ключ, ако това е възможно
(има такъв ключ и стойностите му поддържат оператора <=). В противен случай,
проблемът да бъде съобщен на потребителя по подходящ начин.

*/

class JSONFile
{
private:
    std::string path;

    JSONElement fileObject;
    JSONElement selectedObject;
    JSONElement *workingObject;
    
	JSONElement& listNth(const size_t n);
    
    void updateAncestors();
    void updateChildren();

public:
    JSONFile();
    ~JSONFile();

    void createElement(const std::string);

    void open();

    void print();
    void printSelected();

    void save();
    void saveAs();
    void saveElement(const std::string);
    void saveElementAs(const std::string);
    void saveSelected();
    void saveSelectedAs();

    void JSONPath(const std::string);

    void findByKey(const std::string);
    void set(const std::string);
    void create();
    void remove(const std::string);
    void move();
    void order();
    void selectedAsWorking();
    void reset();

    std::string getPath();

    void clear();
};

#endif
