#include <iostream>
#include <vector>
#include <string>


struct Node
{
    std::string value;
    Node* left = nullptr;
    Node* right = nullptr;
};

void insertSpaces(std::string& expression);

void removeExtraSpaces(std::string& expression);

void connectUnaryMinuses(std::string& expression);

std::vector<std::string> tokenize(const std::string& expression);

int setPriority(char sign);

bool isOnlyDigit(const std::string& expression);

bool isCorrect(const std::string& expression);

size_t findLowPriorityOperation(const std::string& expression);

void createCalcTree(const std::string& expression, Node* node);

std::string applyOperation(char sign, const std::string& expr1, const std::string& expr2);

void calculate(Node* node);


int main(int argc, char* argv[])
{
    if (argc != 2) {
        std::cout << "error" << std::endl;
        return 1;
    }

    std::string expression = argv[1];

    insertSpaces(expression);
    removeExtraSpaces(expression);
    connectUnaryMinuses(expression);

    if (!isCorrect(expression)) {
        std::cout << "error" << std::endl;
        return 1;
    }

    Node* root = new Node;
    createCalcTree(expression, root);

    calculate(root);

    std::cout << root->value << std::endl;

    delete root;

    return 0;
}


void removeExtraSpaces(std::string& expression)
{
    for (size_t i = 1; i < expression.length(); i++)
        if (expression[i - 1] == ' ' && expression[i] == ' ')
            expression.erase(i--, 1);

    if (expression[0] == ' ')
        expression.erase(0, 1);

    if (expression[expression.length() - 1] == ' ')
        expression.erase(expression.length() - 1, 1);
}

void insertSpaces(std::string& expression)
{
    for (size_t i = 1; i < expression.length(); i++)
        if (setPriority(expression[i]))
            expression.insert(i++, " ");
}

void connectUnaryMinuses(std::string& expression)
{
    if (expression[0] == '-')
        for (size_t i = 2; i < expression.length(); i++)
            if (isOnlyDigit(expression.substr(2, i - 2))) {
                expression.erase(1, 1);
                break;
            }

    for (size_t i = 0; i < expression.length(); i++)
        if (setPriority(expression[i]) > 0 && (expression[i + 2] == '-'))
            expression.erase(i + 3, 1);
}

std::vector<std::string> tokenize(const std::string& expression)
{
    size_t last = 0;
    std::vector<std::string> tokens;

    for (size_t i = 0; i < expression.length(); i++)
        if (expression[i] == ' ') {
            tokens.push_back(expression.substr(last, i - last));
            last = i + 1;
        }

    return tokens;
}

int setPriority(char sign)
{
    switch (sign)
    {
        case '*':
            return 2;
        case '/':
            return 2;
        case '+':
            return 1;
        case '-':
            return 1;
        default:
            return -1;
    }
}

bool isOnlyDigit(const std::string& expression)
{
    if (expression[0] == '-' && expression.length() > 1) {
        for (size_t i = 1; i < expression.length(); i++)
            if (expression[i] < '0' || expression[i] > '9')
                return false;
    } else {
        for (auto i : expression)
            if (i < '0' || i > '9')
                return false;
    }

    return true;
}

bool isCorrect(const std::string& expression)
{
    std::vector<std::string> tokens = tokenize(expression + ' ');

    if (tokens.size() % 2 == 0)
        return false;

    for (size_t i = 0; i < tokens.size(); i += 2)
        if (!isOnlyDigit(tokens[i]))
            return false;

    for (size_t i = 1; i < tokens.size(); i += 2)
        if ((tokens[i].length() > 1) || (setPriority(tokens[i].c_str()[0]) < 0))
            return false;

    for (size_t i = 0; i < tokens.size() - 1; i++)
        if (tokens[i] == "/" && tokens[i + 1] == "0")
            return false;

    return true;
}

size_t findLowPriorityOperation(const std::string& expression)
{
    size_t low = 0;
    int priority = 3;
    for (size_t i = expression.length() - 1; i > 0; i--) {
        if (expression[i] == '-' && expression[i + 1] != ' ')
            continue;
        int currentPriority = setPriority(expression[i]);
        if (currentPriority != -1 && currentPriority < priority) {
            priority = currentPriority;
            low = i;
        }
    }
    return low;
}

void createCalcTree(const std::string& expression, Node* node)
{
    if (isOnlyDigit(expression)) {
        node->value = expression;
    } else {
        size_t i = findLowPriorityOperation(expression);
        node->value = expression[i];
        node->left = new Node;
        node->right = new Node;

        createCalcTree(expression.substr(0, i - 1), node->left);
        createCalcTree(expression.substr(i + 2), node->right);
    }
}

std::string applyOperation(char sign, const std::string& expr1, const std::string& expr2)
{
    int64_t var1 = 1;
    int64_t var2 = 1;
    if (expr1[0] == '-')
        var1 = -1 * std::atoi(expr1.substr(1).c_str());
    else
        var1 = std::atoi(expr1.c_str());

    if (expr2[0] == '-')
        var2 = -1 * std::atoi(expr2.substr(1).c_str());
    else
        var2 = std::atoi(expr2.c_str() );

    switch (sign)
    {
        case '*':
            return std::to_string(var1 * var2);
        case '/':
            return std::to_string(var1 / var2);
        case '+':
            return std::to_string(var1 + var2);
        case '-':
            return std::to_string(var1 - var2);
        default:
            return "";
    }
}

void calculate(Node* node)
{
    if (isOnlyDigit(node->value))
        return;

    calculate(node->left);
    calculate(node->right);

    node->value = applyOperation(node->value[0], node->left->value, node->right->value);

    delete node->left;
    delete node->right;
}
