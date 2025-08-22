#include "expression_parser.h"
#include<typeindex>
#include<queue>
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

    cout << "Starting expression parsing" << '\n';
    for (char& i: expression) {
        if (isNotOperator(i)){
            cout << "Character detected not as an operator: " << i << '\n';
            if (i == '.') {
                decimal = true;
                cout << "Decimal was set to true" << '\n';
            }
            temp += i;
        } else {
            // check if the expression is empty
            // i.e. we are at the first operator
            cout << "Character detected as an operator: " << i << '\n';
            cout << "Current temp: " << temp << '\n';
            if (obj.root == nullptr) {
                // create a new opnode and add the 
                // term to the left of the opnode
                cout << "Root obj detected as nullptr. Successfully detected root" << '\n';

                obj.root = new TreeNode(new OpNode(i));
                if (decimal) {
                    throw ValueNotSupportedException(("Decimal value " + temp + " is not supported yet.").c_str());
                }
                // create the data node
                obj.root->left = new TreeNode(new DataNode(std::stoi(temp)));
                obj.maxLevel++;
            } else { // expression is not empty
                // check if the decimal flag was enabled
                cout << "Detected not a root object\n";
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
                cout << "Traversing to the required node\n";
                // cout << typeid(*(ptr->obj)).name() << '\n';
                // cout << (leftOpIsHigher(ptr->obj->getData(), i) == false) << '\n';
                // cout << (typeid(*(ptr->obj)) == typeid(OpNode)) << '\n';
                // cout << (ptr->right != nullptr) << '\n';
                while (typeid(*(ptr->obj)) == typeid(OpNode) 
                && leftOpIsHigher(ptr->obj->getData(), i) == false
                && ptr->right != nullptr) {
                    // go to the right node
                    ptr = ptr->right;
                }

                cout << "Currently ptr at " << ptr->obj->getData() << '\n';

                // check if the current node op is higher than the new op
                if (leftOpIsHigher(ptr->obj->getData(), i)) {
                    cout << "Detected that left op is higher. Left op is " << obj.root->obj->getData() << '\n';
                    // insert operand to the right of the operator
                    if (ptr == obj.root) {
                        cout << "ptr at root as well" << '\n';
                        ptr->right = dataNode;
                        opnode->left = ptr;
                        cout << "interchanging root" << '\n';
                        obj.root = opnode;
                        obj.maxLevel++;
                    } else {
                        cout << "ptr not at root\n";
                        ptr->right = dataNode;
                        opnode->left = ptr;
                        obj.maxLevel++;
                    }
                } else {
                    cout << "Detected right op is higher. Left op is " << obj.root->obj->getData() << '\n';
                    if (ptr->right == nullptr) {
                        cout << "Found right pointer is nullptr\n";
                        // check if the right node is empty (ideally should be)
                        ptr->right = opnode;
                        opnode->left = dataNode;
                        obj.maxLevel++;
                    } else {
                        // this cannot happen and is impossible
                        throw ExpressionParseException(("A value already exists in the tree."));
                    }
                }
            }
            cout << "Resetting temp and decimal flag\n";
            temp = "";
            decimal = false;
        }
    }

    // Insert remaining element (if any)
    if (temp.size() > 0) {
        cout << "Inserting last element: " << temp << '\n';
        TreeNode* ptr = obj.root;
        // find the right most OpNode with a right node as nullptr
        while (ptr->right != nullptr) {
            ptr = ptr->right;
        }
        TreeNode* dataNode = new TreeNode(new DataNode(std::stoi(temp)));
        ptr->right = dataNode;
        cout << "Success\n";
    }
    cout << "exited parsing loop\n";

    return obj;
}

void Expr::show() {
    string tree {};
    queue<TreeNode*> q;
    TreeNode* ptr {this->root};
    int maxLevel = this->maxLevel;
    int maxGaps = 2 * (maxLevel - 1);

    cout << "\n=================\n";
    cout << "Show the tree\n";
    cout << "=================\n";
    do {
        // first insert the required spaces before the ops/data nodes in the level
        // cout << "Currently tree:-\n" << tree << '\n';
        // cout << "Adding spaces\n";
        for(int i = 0; i < maxGaps; i++) {
            tree += ' ';
        }


        // see if the queue is empty. 
        if (q.size() <= 0) {
            // cout << "Queue size is empty\n";
            // this is the root node. 
            // add the data in the root node and add the left and right nodes in the queue
            if (ptr->left == nullptr && ptr->right == nullptr) {
                // cout << "========\nFound DataNode\n========\n";
                // cout << "Pushing type(" << typeid(dynamic_cast<DataNode*>(ptr->obj)->getData()).name() << ") and value (" << std::to_string(dynamic_cast<DataNode*>(ptr->obj)->getData()) << ") to the tree\n";
                tree += std::to_string(dynamic_cast<DataNode*>(ptr->obj)->getData());
            } else {
                // cout << "========\nFound OpNode\n========\n";
                // cout << "Pushing type(" << typeid(ptr->obj->getData()).name() << ") and value (" << ptr->obj->getData() << ") to the tree\n";
                tree += ptr->obj->getData();
            }
            // cout << "Added root object to tree\n";
            // this is to check if the user has not just provided a single number
            // if yes then the loop won't move forward
            if (ptr->left != nullptr)
                q.push(ptr->left);
            if (ptr->right != nullptr) 
                q.push(ptr->right);
            // cout << "Pushed left and right nodes to the queue\n";
        } else { // not empty means that we are in the middle of the tree
            // cout << "Queue size is not empty\n";
            int queueSize = static_cast<int>(q.size());
            // we iterate over the queue to get the values of the current level
            // while adding the nodes for the next level
            // cout << "Traversing queue\n";
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
                    // cout << "========\nFound DataNode\n========\n";
                    // cout << "Pushing type(" << typeid(dynamic_cast<DataNode*>(ptr->obj)->getData()).name() << ") and value (" << std::to_string(dynamic_cast<DataNode*>(ptr->obj)->getData()) << ") to the tree\n";
                    tree += std::to_string(dynamic_cast<DataNode*>(ptr->obj)->getData());
                    tree += " ";
                } else {
                    // cout << "========\nFound OpNode\n========\n";
                    // cout << "Pushing type(" << typeid(ptr->obj->getData()).name() << ") and value (" << ptr->obj->getData() << ") to the tree\n";
                    tree += ptr->obj->getData();
                    tree += " ";
                }
                // add its left and right nodes to the queue
                if (ptr->left != nullptr)
                    q.push(ptr->left);
                if (ptr->right != nullptr) 
                    q.push(ptr->right);
                // pop handled above and decrement queueSize
                queueSize--;
            }
            // cout << "Ended queue traversal\n";
        }
        // cout << "==========\nCurrently in the queue\n==========\n[ ";
        // queue<TreeNode*> copy_q = q;
        // while (!copy_q.empty()) {
        //     // check if the element is a DataNode or a OpNode
        //     if (copy_q.front()->left == nullptr && copy_q.front()->right == nullptr) {
        //         cout << dynamic_cast<DataNode*>(copy_q.front()->obj)->getData() << ' ';
        //     } else {
        //         cout << copy_q.front()->obj->getData() << ' ';
        //     }
        //     copy_q.pop();
        // }
        // cout << "]\n==========\n\n";
        tree += '\n';
        maxGaps--;
    } while(q.size() > 0);
    cout << tree << '\n';
}