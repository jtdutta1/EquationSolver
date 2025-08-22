#include "expression_parser.h"
#include<typeindex>
#include<queue>
#include<cmath>
#include<iostream>
using std::regex_match, std::queue, std::cout;

regex equation_regex ("^(?:\\d+|\\(\\d+\\s*[-+/*]\\s*\\d+\\))(?:\\s*[-+/*]\\s*(?:\\d+|\\(\\d+\\s*[-+/*]\\s*\\d+\\)))*$");
bool match_expression(string expression) {
    return regex_match(expression, equation_regex);
}

char Node::getData() {
    return this->data;
}

int DataNode::getData() {
    return this->value;
}

void Node::setData(char data) {
    this->data = data;
}

void DataNode::setData(int data){
    this->value = data;
}

bool isNotOperator(char item) {
    for (const char& i: operatorPrecedence) {
        if (item == i) {
            return false;
        }
    }
    return true;
}

bool leftOpIsHigher(const char& op1, const char& op2) {
    /* Finds if the precedence of operator op1 is higher than operator op2.*/
    int op1_precedence {-1}, op2_precedence {-1};
    for (int i  = 0; i < operatorPrecedence.size(); i++) {
        if (operatorPrecedence[i] == op1) {
            op1_precedence = i;
        }

        if (operatorPrecedence[i] == op2) {
            op2_precedence = i;
        }
    }

    return op1_precedence >= op2_precedence;
}


Expr parse_expression(string expression) {
    // expression += ' ';
    string temp {};
    Expr obj;
    bool decimal {false};
    for (char& i: expression) {
        int level {};
        if (isNotOperator(i)){
            if (i == '.') {
                decimal = true;
            }
            temp += i;
        } else {
            // check if the expression is empty
            // i.e. we are at the first operator
            if (obj.root == nullptr) {
                // create a new opnode and add the 
                // term to the left of the opnode

                obj.root = new TreeNode(new OpNode(i));
                if (decimal) {
                    throw ValueNotSupportedException(("Decimal value " + temp + " is not supported yet.").c_str());
                }
                // create the data node
                obj.root->left = new TreeNode(new DataNode(std::stoi(temp)));
                obj.maxLevel++;
            } else { // expression is not empty
                // check if the decimal flag was enabled
                if (decimal) {
                    throw ValueNotSupportedException(("Decimal value " + temp + " is not supported yet.").c_str());
                }
                // create a new opnode and a treenode
                TreeNode* opnode = new TreeNode(new OpNode(i));
                // create a data node
                TreeNode* dataNode = new TreeNode(new DataNode(std::stoi(temp)));
                // traverse the tree to find the suitable place to 
                // insert the new node to
                TreeNode* ptr = obj.root;
                // Repeatedly go to the right nodes until a nullptr/DataNode is reached
                while (typeid(*(ptr->obj)) == typeid(OpNode) 
                && leftOpIsHigher(ptr->obj->getData(), i) == false
                && ptr->right != nullptr) {
                    // go to the right node
                    ptr = ptr->right;
                    level++;
                }

                // check if the current node op is higher than the new op
                if (leftOpIsHigher(ptr->obj->getData(), i)) {
                    // insert operand to the right of the operator
                    if (ptr == obj.root) {
                        ptr->right = dataNode;
                        opnode->left = ptr;
                        obj.root = opnode;
                        obj.maxLevel++;
                    } else {
                        ptr->right = dataNode;
                        opnode->left = ptr;
                        obj.maxLevel++;
                    }
                } else {
                    if (ptr->right == nullptr) {
                        cout<< "Found right pointer is nullptr\n";
                        // check if the right node is empty (ideally should be)
                        ptr->right = opnode;
                        opnode->left = dataNode;
                        obj.maxLevel = level > obj.maxLevel ? level + 1 : obj.maxLevel + 1;
                    } else {
                        // this cannot happen and is impossible
                        throw ExpressionParseException(("A value already exists in the tree."));
                    }
                }
            }
            temp = "";
            decimal = false;
        }
    }

    // Insert remaining element (if any)
    if (temp.size() > 0) {
        int level {};
        // cout<< "Inserting last element: " << temp << '\n';
        TreeNode* ptr = obj.root;
        // find the right most OpNode with a right node as nullptr
        while (ptr->right != nullptr) {
            ptr = ptr->right;
        }
        TreeNode* dataNode = new TreeNode(new DataNode(std::stoi(temp)));
        ptr->right = dataNode;
        // cout<< "Success\n";
    }
    obj.maxLevel = obj.rescanMaxLevel();
    // cout<< "exited parsing loop\n";

    return obj;
}

int Expr::rescanMaxLevel() {
    TreeNode* ptr {this->root};
    queue<TreeNode*> q;
    int level {};

    do {
        if (q.size() == 0) {
            if (ptr->left != nullptr)
                q.push(ptr->left);
            if (ptr->right != nullptr) 
                q.push(ptr->right);
            level++;
        } else {
            int queueSize = static_cast<int>(q.size());
            while (queueSize > 0) {
                // get the front of the queue
                ptr = q.front();
                q.pop();
                // add its left and right nodes to the queue
                if (ptr->left != nullptr)
                    q.push(ptr->left);
                if (ptr->right != nullptr) 
                    q.push(ptr->right);
                // pop handled above and decrement queueSize
                queueSize--;
                // add internode gaps
            }
            level++;
        }
    } while(q.size() > 0);
    return level;
}

void Expr::show() {
    string tree {};
    queue<TreeNode*> q;
    TreeNode* ptr {this->root};
    int maxLevel = this->maxLevel;
    int maxGaps = static_cast<int>(std::pow(2, maxLevel - 1)) - 1;
    int lineNum = maxLevel - 2;
    string interNodeGaps = "", preGaps = "";

    cout<< "\n=================\n";
    cout<< "Show the tree\n";
    cout<< "=================\n";
    do {
        // first insert the required spaces before the ops/data nodes in the level
        for(int i = 0; i < maxGaps; i++) {
            preGaps += ' ';
        }

        // add pre-node gaps to the line first
        tree += preGaps;

        // see if the queue is empty. 
        if (q.size() <= 0) {
            // this is the root node. 
            // add the data in the root node and add the left and right nodes in the queue
            if (ptr->left == nullptr && ptr->right == nullptr) {
                tree += std::to_string(dynamic_cast<DataNode*>(ptr->obj)->getData());
            } else {
                tree += ptr->obj->getData();
            }
            // this is to check if the user has not just provided a single number
            // if yes then the loop won't move forward
            q.push(ptr->left);
            q.push(ptr->right);
        } else { // not empty means that we are in the middle of the tree
            int queueSize = static_cast<int>(q.size());
            // we iterate over the queue to get the values of the current level
            // while adding the nodes for the next level
            while (queueSize > 0) {
                // get the front of the queue
                ptr = q.front();
                q.pop();
                // if ptr is null, add spacing and continue
                if (ptr == nullptr) {
                    tree += ' ';
                    queueSize--;
                    continue;
                }
                // add the element to the tree
                if (ptr->left == nullptr && ptr->right == nullptr) {
                    tree += std::to_string(dynamic_cast<DataNode*>(ptr->obj)->getData());
                } else {
                    tree += ptr->obj->getData();
                }
                // add its left and right nodes to the queue
                q.push(ptr->left);
                q.push(ptr->right);
                // pop handled above and decrement queueSize
                queueSize--;
                // add internode gaps
                tree += interNodeGaps;
            }
        }
        tree += '\n';
        interNodeGaps = preGaps;
        preGaps = "";
        maxGaps -= static_cast<int>(std::pow(2, lineNum));
        lineNum--;

    } while(q.size() > 0);
    cout<< tree << '\n';
}