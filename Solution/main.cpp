// Задача 10. По данному регулярному выражению в Польской записи над алфавитом {a,b,c,1,*,.,+},
// символу x из множества {a, b, c} и неотрицательному целому k вывести минимальную длину слова
// с суффиксом x^k в языке, задаваемым регулярным выражением.


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
    bool IsSymbol_abc(const char &element) {
        return element == 'a' || element == 'b' || element == 'c';
    }

    bool IsSymbol_1(const char &element) {
        return element == '1';
    }

    bool IsOperand(const char &element) {
        return element == '+' || element == '*' || element == '.';
    }

    bool IsUnaryOperand(const char &operand) {
        assert(IsOperand(operand));
        return operand == '*';
    }

    bool IsBinaryOperand(const char &operand) {
        assert(IsOperand(operand));
        return operand == '+' || operand == '.';
    }

    vector<size_t> UpdateNodeUnary(const vector<size_t> &current_node) {
        vector<size_t> new_current_node(current_node);
        new_current_node[0] = 0;

        for (size_t i = 1; i < current_node.size(); ++i) {
            for (size_t j = 1; j < i; ++j) {
                if (current_node[j] == j && new_current_node[i - j] != UINT64_MAX) {
                    new_current_node[i] = std::min(new_current_node[i], new_current_node[i - j] + j);
                }
            }
        }
        return new_current_node;
    }

    vector<size_t> UpdateNodePlus(const vector<size_t> &lhs_node, const vector<size_t> &rhs_node) {
        vector<size_t> new_current_node(rhs_node.size());
        for (size_t i = 0; i < new_current_node.size(); ++i) {
            new_current_node[i] = std::min(lhs_node[i], rhs_node[i]);
        }
        return new_current_node;
    }

    vector<size_t> UpdateNodeDot(const vector<size_t> &lhs_node, const vector<size_t> &rhs_node) {
        vector<size_t> new_current_node(lhs_node.size(), UINT64_MAX);

        for (size_t i = 0; i < rhs_node.size(); ++i) {
            if (rhs_node[i] != UINT64_MAX) {
                new_current_node[i] = lhs_node[0] + rhs_node[i];
            }
            for (size_t j = 0; j < i; ++j) {
                if (rhs_node[j] == j && lhs_node[i - j] != UINT64_MAX) {
                    new_current_node[i] = std::min(new_current_node[i], lhs_node[i - j] + j);
                }
            }
        }
        return new_current_node;
    }


    class RegExp {
        vector<char> regexp_;

    public:
        explicit RegExp(const std::string &regExp) : regexp_() {
            regexp_.reserve(regExp.size());
            for (const char &i : regExp) {
                regexp_.emplace_back(i);
            }
        }

        vector<char> RegExpInRegView() const {
            stack<vector<char>> symbols;

            for (const auto &i : regexp_) {
                assert(i != '\0');
                if (IsSymbol_abc(i) || IsSymbol_1(i)) {
                    symbols.push(std::vector<char>(1, i));
                } else if (IsOperand(i)) {
                    if (IsUnaryOperand(i)) {
                        if (symbols.empty()) {
                            throw std::runtime_error("The input is not a regexp");
                        }

                        vector<char> expr = symbols.top();
                        symbols.pop();

                        if (expr.size() == 1) { // it's a symbol
                            expr.push_back(i);
                            symbols.push(expr);
                        } else {
                            vector<char> new_expr;
                            new_expr.reserve(expr.size() + 3);
                            new_expr.push_back('(');
                            std::copy(expr.begin(), expr.end(), std::back_inserter(new_expr));
                            new_expr.push_back(')');
                            new_expr.push_back('*'); // i == '*'

                            symbols.push(new_expr);
                        }
                    } else if (IsBinaryOperand(i)) { // + or .
                        if (symbols.size() <= 1) {
                            throw std::runtime_error("The input is not a regexp");
                        }
                        vector<char> rhs = symbols.top();
                        symbols.pop();
                        vector<char> lhs = symbols.top();
                        symbols.pop();

                        std::vector<char> new_expr;
                        if (i == '+') {
                            new_expr.reserve(lhs.size() + rhs.size() + 5);
                            new_expr.push_back('(');
                            std::copy(lhs.begin(), lhs.end(), std::back_inserter(new_expr));
                            new_expr.push_back(' ');
                            new_expr.push_back(i);
                            new_expr.push_back(' ');
                            std::copy(rhs.begin(), rhs.end(), std::back_inserter(new_expr));
                            new_expr.push_back(')');
                        } else if (i == '.') {
                            new_expr.reserve(lhs.size() + rhs.size() + 1);
                            std::copy(lhs.begin(), lhs.end(), std::back_inserter(new_expr));
                            //new_expr.push_back(i); // the multiplication sign is redundant
                            std::copy(rhs.begin(), rhs.end(), std::back_inserter(new_expr));
                        }
                        symbols.push(new_expr);
                    }
                } else {
                    throw std::runtime_error("The input contains incorrect characters");
                }
            }
            if (symbols.size() != 1) {
                throw std::runtime_error("The input is not a regexp");
            }
            return symbols.top();
        }

        size_t FindMinWord() const {
            // assert(RegExpInRegView());
            stack<size_t> min_word_length;

            for (const auto &i : regexp_) {
                if (IsSymbol_abc(i)) {
                    min_word_length.push(1);
                } else if (IsSymbol_1(i)) {
                    min_word_length.push(0);
                } else if (IsOperand(i)) {
                    if (IsUnaryOperand(i)) {
                        min_word_length.top() = 0;
                    } else {
                        size_t rhs_min_len = min_word_length.top();
                        min_word_length.pop();
                        size_t lhs_min_len = min_word_length.top();
                        min_word_length.pop();

                        if (i == '+') {
                            min_word_length.push(std::min(lhs_min_len, rhs_min_len));
                        } else if (i == '.') {
                            min_word_length.push(lhs_min_len + rhs_min_len);
                        }
                    }
                }
            }
            return min_word_length.top();
        }

        size_t FindMinWordWithGivenPrefix(char x, size_t k) const {
            if (!k || IsSymbol_1(x)) {
                size_t min_word_length = FindMinWord();
                return min_word_length;
            }

            stack<vector<size_t>> nodes;
            for (const char &i : regexp_) {
                if (IsSymbol_abc(i) || IsSymbol_1(i)) {
                    vector<size_t> current_node(k + 1, UINT64_MAX); // x_0, x_1, ... , x_k
                    current_node[0] = IsSymbol_1(i) ? 0 : 1;
                    if (i == x) {
                        current_node[1] = 1;
                    }
                    nodes.push(current_node);
                } else if (IsOperand(i)) {
                    if (IsUnaryOperand(i)) {
                        vector<size_t> current_node = nodes.top();
                        nodes.pop();
                        nodes.push(UpdateNodeUnary(current_node));
                    } else if (IsBinaryOperand(i)) // + or .
                    {
                        vector<size_t> rhs_node = nodes.top();
                        nodes.pop();
                        vector<size_t> lhs_node = nodes.top();
                        nodes.pop();

                        if (i == '+') {
                            nodes.push(UpdateNodePlus(lhs_node, rhs_node));
                        } else if (i == '.') {
                            nodes.push(UpdateNodeDot(lhs_node, rhs_node));
                        }
                    }
                }
            }
            return nodes.top()[k];
        }

        void Solve(char x, size_t k) const {
            size_t answer = FindMinWordWithGivenPrefix(x, k);
            if (answer == UINT64_MAX) {
                cout << "INF" << "\n";
                return;
            }
            cout << answer << "\n";
        }
    };
}


namespace Testing {
    using RegExp::RegExp;

    bool RegExpCorrectnessRead(const std::string& s) {
        RegExp reg_exp(s);
        try {
            reg_exp.RegExpInRegView();
        } catch (std::exception& ex) {
            return false;
        }
        return true;
    }

    void RegExpCorrectnessTests() {
        std::string s_test_1 = "ab.aa.+*";
        std::string s_test_2 = "a*****b+";
        std::string s_test_3 = "1a+b*c..";
        std::string s_test_4 = "abcabc..+..*1+*";
        std::string s_test_5 = "a*b*.c*1*.+";
        std::string s_test_6 = "c1.a+c.a.aa.+*";

        assert(RegExpCorrectnessRead(s_test_1) && RegExpCorrectnessRead(s_test_2) && RegExpCorrectnessRead(s_test_3) &&
               RegExpCorrectnessRead(s_test_4) && RegExpCorrectnessRead(s_test_5) && RegExpCorrectnessRead(s_test_6));
    }

    size_t RegExpMinWordWithGivenPrefixRead(const std::string& s, char x, size_t k) {
        RegExp reg_exp(s);
        return reg_exp.FindMinWordWithGivenPrefix(x, k);
    }

    void RegExpMinWordWithGivenPrefixTest() { // First two tests are from the file
        std::string s_test_1 = "ab+c.aba.*.bac.+.+*";
        char x_1 = 'b';
        size_t k_1 = 2;

        auto res_1 = RegExpMinWordWithGivenPrefixRead(s_test_1, x_1, k_1);
        // Expected UINT64_MAX

        std::string s_test_2 = "acb..bab.c.*.ab.ba.+.+*a.";
        char x_2 = 'a';
        size_t k_2 = 2;

        auto res_2 = RegExpMinWordWithGivenPrefixRead(s_test_2, x_2, k_2);
        // Expected 4

        std::string s_test_3 = "ab.aa.+*";
        char x_3 = 'a';
        size_t k_3 = 2020;

        auto res_3 = RegExpMinWordWithGivenPrefixRead(s_test_3, x_3, k_3);
        // Expected 2020

        std::string s_test_4 = "ba.ba.aaaa...*+.a.";
        char x_4 = 'a';
        size_t k_4 = 11;

        auto res_4 = RegExpMinWordWithGivenPrefixRead(s_test_4, x_4, k_4);
        // Expected 15
        
        std::string s_test_5 = "1c+abbbb...+abb..+*.bbb..aa.+*.";
        char x_5 = 'b';
        size_t k_5 = 5;

        auto res_5 = RegExpMinWordWithGivenPrefixRead(s_test_5, x_5, k_5);
        // Expected 6 

        std::string s_test_6 = "11111c.....ab.+";
        char x_6 = 'a';
        size_t k_6 = 0;

        auto res_6 = RegExpMinWordWithGivenPrefixRead(s_test_6, x_6, k_6);
        // Expected 1

        std::string s_test_7 = "cccccccc.......ccccc....+*";
        char x_7 = 'c';
        size_t k_7 = 9;

        auto res_7 = RegExpMinWordWithGivenPrefixRead(s_test_7, x_7, k_7);
        // Expected 10

        std::string s_test_8 = "cccccccc.......ccccc....+*";
        char x_8 = 'c';
        size_t k_8 = 28;

        auto res_8 = RegExpMinWordWithGivenPrefixRead(s_test_8, x_8, k_8);
        // Expected 28

        std::string s_test_9 = "cccccccc.......ccccc....+*";
        char x_9 = 'c';
        size_t k_9 = 36;

        auto res_9 = RegExpMinWordWithGivenPrefixRead(s_test_9, x_9, k_9);
        // Expected 36

        std::string s_test_10 = "1c+1.*b.c*.";
        char x_10 = 'b';
        size_t k_10 = 1;

        auto res_10 = RegExpMinWordWithGivenPrefixRead(s_test_10, x_10, k_10);
        // Expected 1

        std::string s_test_11 = "aaa..bbb..+*a.";
        char x_11 = 'a';
        size_t k_11 = 5;

        auto res_11 = RegExpMinWordWithGivenPrefixRead(s_test_11, x_11, k_11);
        // Expected 7

        std::string s_test_12 = "aa.bb.+*c.ac+*.";
        char x_12 = 'b';
        size_t k_12 = 1;

        auto res_12 = RegExpMinWordWithGivenPrefixRead(s_test_12, x_12, k_12);
        // Expected UINT_64_MAX

        assert(RegExpCorrectnessRead(s_test_1) && RegExpCorrectnessRead(s_test_2) && RegExpCorrectnessRead(s_test_3) &&
               RegExpCorrectnessRead(s_test_4) && RegExpCorrectnessRead(s_test_5) && RegExpCorrectnessRead(s_test_6) &&
               RegExpCorrectnessRead(s_test_7) && RegExpCorrectnessRead(s_test_8) && RegExpCorrectnessRead(s_test_9) &&
               RegExpCorrectnessRead(s_test_10) && RegExpCorrectnessRead(s_test_11) && RegExpCorrectnessRead(s_test_12));

        assert(res_1 == UINT64_MAX && res_2 == 4 && res_3 == 2020 && res_4 == 15 && res_5 == 6 && res_6 == 1 && res_7 == 10
                && res_8 == 28 && res_9 == 36 && res_10 == 1 && res_11 == 7 && res_12 == UINT64_MAX);
    }

    void DoTesting() {
        RegExpCorrectnessTests();
        RegExpMinWordWithGivenPrefixTest();
    }
}


int main() {
    std::string regexp;
    char x;
    size_t k;
    cin >> regexp >> x >> k;
    RegExp::RegExp regExp(regexp);
    try {
        auto vec = regExp.RegExpInRegView();
    } catch (std::runtime_error &Er) {
        //cout << regexp << " " << Er.what() <<"\n";
        cout << "ERROR" << "\n";
        return 0;
    }

    if (!RegExp::IsSymbol_abc(x) && !RegExp::IsSymbol_1(x)) {
        cout << "ERROR" << "\n";
        return 0;
    }

    regExp.Solve(x, k);
    Testing::DoTesting();
}
