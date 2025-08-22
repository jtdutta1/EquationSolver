#include<string>
#include<regex>
#include<vector>
#include<exception>
using std::string, std::regex, std::vector, std::exception;

const vector<char> operatorPrecedence = {'-', '+', '*', '/', ')', '('};

class Node{
    private:
        char data;
    public:
        Node() : data('\0') {}
        Node(char data): data(data) {}
        char getData();
        void setData(char data);
        virtual void foo() {};
};

class OpNode : public Node {
    public:
        OpNode(char data): Node(data) {}
};

class DataNode : public Node {
    private:
        int value;
    public:
        DataNode(int data): Node('\0'), value(data) {}
        int getData();
        void setData(int data);
};

struct TreeNode {
    Node* obj;
    TreeNode* left;
    TreeNode* right;

    TreeNode(): obj{nullptr}, left(nullptr), right(nullptr) {}
    TreeNode(Node* obj): obj(obj), left(nullptr), right(nullptr) {}
};

struct Expr{
    TreeNode* root;
    int maxLevel;

    Expr(): root (nullptr), maxLevel (0) {}

    void show();
};

bool match_expression(string expression);

Expr parse_expression(string expression);


class ValueNotSupportedException : public exception {
    private:
        string message;
    
    public:
        ValueNotSupportedException(const char* msg) : message(msg) {}

        const char* what() const noexcept{
            return message.c_str();
        }
};

class ExpressionParseException : public exception {
    private:
        string message;
    
    public:
        ExpressionParseException(const char* msg) : message(msg) {}

        const char* what() const noexcept{
            return message.c_str();
        }
};
