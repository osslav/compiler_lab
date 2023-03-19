#pragma once

#include <map>
#include <vector>
#include <deque>
#include <stack>
#include <string>


#include <cstdint>

//---------------------------------------------------------------------------------
//grammar

struct DataRule
{
    std::string right_part;
    uint32_t ind_rule = 0;
    bool is_last_alt = true;
};

using Rule = std::pair<char, DataRule>;

class ParserString;

class Grammar
{
    friend class ParserString;
private:
    std::vector<char> terminal_symb_;
    std::vector<char> non_terminal_symb_;
    std::multimap<char, DataRule> rules_;
    uint32_t ind_start_symb = 0;

public:
    void setTerminalSymb(std::initializer_list<char> list_symb); 
    void setNonTerminalSymb(std::initializer_list<char> list_symb); 
    void addRule(char left_part, std::string right_part);
    void setStartSymb(char symb);

    std::deque<uint32_t> parseString(std::string input_str);
};

//---------------------------------------------------------------
//parser

class ParserString
{
private:
    using RuleIterator = std::multimap<char, DataRule>::iterator; 
    struct ElemL1
    {
        char char_elem = '\0';
        RuleIterator rule_iter;

        ElemL1(char elem) : char_elem(elem) {}
        ElemL1(RuleIterator iter) : rule_iter(iter) {}

        bool isCharElem() { return char_elem != '\0'; }
    };

    enum class StateAlg : uint8_t { q, b, t };    //{good, bad_state, end}
   
    StateAlg curr_state_ = StateAlg::q;

    std::stack<ElemL1> l1_;
    std::stack<char> l2_;
   
    Grammar& grammar_;

    std::string input_str_;
    uint32_t curr_ind_input_ = 0;

    std::deque<uint32_t> result_;

    ParserString(std::string input_str, Grammar& grammar);
    ParserString(const ParserString&) = default;

    void stepOne();
    void stepTwo();
    void stepThree();
    void stepFour();
    void stepFive();
    void stepSix();

    void stepSixA();
    void stepSixB();
    void stepSixC();
public:
    static std::deque<uint32_t> parse(std::string input_str, Grammar& grammar);
};
