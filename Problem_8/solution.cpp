// Задача 8. Дано регулярное выражение в обратной польской записи, а также символ x и число k. Вывести длину кратчафшего слова
// из языка L, содержащего префикс x^k.

// Тесты:
//
// Ввод: ab+c.aba.*.bac.+.+* b 2
// Вывод: INF
//
// Ввод: acb..bab.c.*.ab.ba.+.+*a. a 2
// Вывод: 4

#include <iostream>
#include <cassert>
#include <vector>
#include <stack>

using std::cin;
using std::cout;
using std::stack;
using std::string;
using std::vector;

namespace RegExp {
    bool IsSymbol_abc(const char& element) {
        return element == 'a' || element == 'b' || element == 'c';
    }

    bool IsOperand(const char& element) {
        return element == '+' || element == '*' || element == '.';
    }

    bool IsUnaryOperand(const char& operand) {
        assert(IsOperand(operand));
        return operand == '*';
    }

    bool IsBinaryOperand(const char& operand) {
        assert(IsOperand(operand));
        return operand == '+' || operand == '.';
    }


    vector<size_t> UpdateNodeUnary(const vector<size_t>& current_node, char x, size_t k) {
        vector<size_t> new_current_node(current_node);
        new_current_node.front() = 0;

        for (size_t i = 1; i <= k; ++i)
        {
            if (current_node[i] != UINT32_MAX) {
                new_current_node[i] = current_node[i];
            }

            for (size_t j = 1; j < i; ++j)
            {
                if (current_node[j] == j) {
                    size_t new_min_length = ((k + 1) / j) * j;
                    new_current_node[i] = std::min(new_current_node[i], new_min_length);
                }
            }
        }
        return new_current_node;
    }

    vector<size_t> UpdateNodePlus(const vector<size_t>& lhs_node, const vector<size_t>& rhs_node, char x, size_t k) {
        vector<size_t> new_current_node(rhs_node);
        for (size_t i = 0; i <= k; ++i)
        {
            new_current_node[i] = std::min(lhs_node[i], rhs_node[i]);
        }
        return new_current_node;
    }

    vector<size_t> UpdateNodeDot(const vector<size_t>& lhs_node, const vector<size_t>& rhs_node, char x, size_t k) {
        vector<size_t> new_current_node(k + 1, UINT32_MAX);
        for (size_t i = 0; i <= k; ++i)
        {
            if (rhs_node[i] != UINT32_MAX) {
                    new_current_node[i] = rhs_node[i] + lhs_node[0];
            }
            for (size_t j = 0; j <= i; ++j)
            {
                if (rhs_node[j] == j) {
                    if (lhs_node[i - j] != UINT32_MAX) {
                        new_current_node[i] = std::min(new_current_node[i], lhs_node[i - j] + j);
                    }
                }
            }
        }
        return new_current_node;
    }


    class RegExp{
        vector<char> regexp_;

    public:
        explicit RegExp(const std::string& regExp) {
            regexp_.reserve(regExp.size());
            for (const char& i : regExp)
            {
                regexp_.emplace_back(i);
            }
        }

        vector<char> RegExpInRegView() const {
            stack<vector<char>> symbols;

            for (const auto& i : regexp_) {
                assert(i != '\0');
                if (IsSymbol_abc(i)) {
                    symbols.push(std::vector<char>(1, i));
                }
                else if (IsOperand(i))
                {
                    if (IsUnaryOperand(i))
                    {
                        if (symbols.empty() || (symbols.top().size() == 1 && !IsSymbol_abc(symbols.top().back())))
                        {
                            throw std::runtime_error("The input is not a regexp");
                        }

                        vector<char> expr = symbols.top();
                        symbols.pop();

                        if (expr.size() == 1) { // it's a symbol
                            expr.push_back(i);
                            symbols.push(expr);
                        }
                        else {
                            vector<char> new_expr;
                            new_expr.reserve(expr.size() + 3);
                            new_expr.push_back('(');
                            std::copy(expr.begin(), expr.end(), std::back_inserter(new_expr));
                            new_expr.push_back(')');
                            new_expr.push_back(i);
                            symbols.push(new_expr);
                        }
                    }
                    else if (IsBinaryOperand(i)) // + or .
                    {
                        if (symbols.size() <= 1)
                        {
                            throw std::runtime_error("The input is not a regexp");
                        }
                        vector<char> rhs = symbols.top();
                        symbols.pop();
                        vector<char> lhs = symbols.top();
                        symbols.pop();

                        if (lhs.size() == 1 && !IsSymbol_abc(lhs.back()) || rhs.size() == 1 && !IsSymbol_abc(rhs.back()))
                        {
                            throw std::runtime_error("The input is not a regexp");
                        }

                        std::vector<char> new_expr;
                        if (i == '+') {
                            new_expr.reserve(lhs.size() + rhs.size() + 3);
                            new_expr.push_back('(');
                            std::copy(lhs.begin(), lhs.end(), std::back_inserter(new_expr));
                            new_expr.push_back(i);
                            std::copy(rhs.begin(), rhs.end(), std::back_inserter(new_expr));
                            new_expr.push_back(')');
                        } else if (i == '.'){
                            new_expr.reserve(lhs.size() + rhs.size() + 1);
                            std::copy(lhs.begin(), lhs.end(), std::back_inserter(new_expr));
                            //new_expr.push_back(i);
                            std::copy(rhs.begin(), rhs.end(), std::back_inserter(new_expr));
                        }
                        symbols.push(new_expr);
                    }
                }
                else {
                    throw std::runtime_error("The input contains incorrect characters");
                }
            }
            if (symbols.size() != 1) {
                throw std::runtime_error("The input is not a regexp");
            }
            return symbols.top();
        }
        size_t FindMinWord() const noexcept {
            stack<size_t> min_word_length;

            for (const auto& i : regexp_) {
                if (IsSymbol_abc(i)) {
                    min_word_length.push( 1);
                }
                else if (IsOperand(i))
                {
                    if (IsUnaryOperand(i))
                    {
                        min_word_length.pop();
                        min_word_length.push(0);
                    }
                    else if (IsBinaryOperand(i)) // + or .
                    {
                        size_t rhs_min_len = min_word_length.top();
                        min_word_length.pop();
                        size_t lhs_min_len = min_word_length.top();
                        min_word_length.pop();

                        if (i == '+') {
                            min_word_length.push(std::min(rhs_min_len, lhs_min_len));
                        } else if (i == '.') {
                            min_word_length.push(rhs_min_len + lhs_min_len);
                        }
                    }
                }
            }
            return min_word_length.top();
        }

        size_t FindMinWordWithGivenPrefix(char x, size_t k) {
            if (!k) {
                return FindMinWord();
            }

            stack<vector<size_t>> nodes;

            for (const char& i : regexp_) {
                if (IsSymbol_abc(i)) {
                    vector<size_t> current_node(k + 1, UINT32_MAX);
                    current_node.front() = 1; // x_0, x_1, \ldots , x_k

                    if (i == x) {
                        current_node[1] = 1;
                    }
                    nodes.push(current_node);

                    /*cout << i << " : " << "\n";
                    for (const auto& d : current_node) {
                        cout << d << "\n";
                    }
                    cout << "\n";*/
                }
                else if (IsOperand(i))
                {
                    if (IsUnaryOperand(i))
                    {
                        vector<size_t> current_node = nodes.top();
                        nodes.pop();
                        nodes.push(UpdateNodeUnary(current_node, x, k));

                        /*cout << i << " : " << "\n";
                        for (const auto& d : nodes.top()) {
                            cout << d << "\n";
                        }
                        cout << "\n";*/
                    }
                    else if (IsBinaryOperand(i)) // + or .
                    {
                        vector<size_t> rhs_node = nodes.top();
                        nodes.pop();
                        vector<size_t> lhs_node = nodes.top();
                        nodes.pop();

                        if (i == '+') {
                            nodes.push(UpdateNodePlus(lhs_node, rhs_node, x, k));
                        } else if (i == '.') {
                            nodes.push(UpdateNodeDot(lhs_node, rhs_node, x, k));
                        }
                        /*cout << i << ": " << "\n";
                        for (const auto& d : nodes.top()) {
                            cout << d << "\n";
                        }
                        cout << "\n";*/
                    }
                }
            }
            return nodes.top().back();
        }

        void Solve(char x, size_t k) {
            size_t answer = FindMinWordWithGivenPrefix(x, k);
            if (answer == UINT32_MAX) {
                cout << "INF";
                return;
            }
            cout << answer;
        }
    };

}



int main()
{
    std::string regexp;
    char x;
    size_t k;
    cin >> regexp >> x >> k;
    RegExp::RegExp regExp(regexp);
    try {
        auto vec = regExp.RegExpInRegView();
        /*for (const auto& i : vec) {
            cout << i;
        }
        cout << "\n";*/
    } catch (std::runtime_error& Er) {
        //cout << regexp << " " << Er.what() <<"\n";
        cout << "ERROR" << "\n";
        return 0;
    }

    if (!RegExp::IsSymbol_abc(x)) {
        cout << "ERROR" << "\n";
        return 0;
    }

    regExp.Solve(x, k);
}

