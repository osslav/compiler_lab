#include "grammar.hpp"

#include <algorithm>
#include <iostream>

template <typename TypeElem>
bool vectorContainsIt(std::vector<TypeElem> vec, const TypeElem& elem)
{
    return std::find(vec.begin(), vec.end(), elem) != vec.end();
}


//grammar methods

void Grammar::setTerminalSymb(std::initializer_list<char> list_symb) 
{
    terminal_symb_.clear();
    terminal_symb_.reserve(list_symb.size());
    for (char elem: list_symb)
        terminal_symb_.push_back(elem);
}
 
void Grammar::setNonTerminalSymb(std::initializer_list<char> list_symb) 
{
    non_terminal_symb_.clear();
    non_terminal_symb_.reserve(list_symb.size());
    for (char elem: list_symb)
        non_terminal_symb_.push_back(elem);
}  

void Grammar::setStartSymb(char start_symb)
{
    auto iter_start_symb = std::find(non_terminal_symb_.begin(), non_terminal_symb_.end(), start_symb);
    if (iter_start_symb == non_terminal_symb_.end())
        throw std::logic_error("Invalid start symbol");
    else
        ind_start_symb = std::distance(non_terminal_symb_.begin(), iter_start_symb);
}

void Grammar::addRule(char left_part, std::string right_part) 
{
    DataRule data_new_rule;
    data_new_rule.right_part = std::move(right_part);
    data_new_rule.ind_rule = rules_.size() + 1;

    auto temp_iter = rules_.upper_bound(left_part);
    if (temp_iter != rules_.begin())
    {
        temp_iter--;
        if (temp_iter->first == left_part)
        {
            temp_iter->second.is_last_alt = false;
            //std::cout << temp_iter->first << " - " << temp_iter->second.right_part << " not last alt" << std::endl;
        }
    }

    rules_.insert(std::make_pair(left_part, std::move(data_new_rule))); 
}

std::deque<uint32_t> Grammar::parseString(std::string input_str)
{
    return std::deque<uint32_t>{};
}

//-------------------------------------------------------------------------------------------
//parser methods

ParserString::ParserString(std::string input_str, Grammar& grammar)
    : input_str_(std::move(input_str)), grammar_(grammar)
{
    l2_.push(grammar_.non_terminal_symb_[grammar_.ind_start_symb]);
}

void ParserString::stepOne()
{
    std::cout << "1";    
    char curr_symb = l2_.top();
    l2_.pop();

    auto iter_to_first_alt = grammar_.rules_.find(curr_symb);
   
    //for (char elem: iter_to_first_alt->second.right_part)
        //l2_.push(elem);

    std::string& str_first_alt = iter_to_first_alt->second.right_part;
    for (auto iter = str_first_alt.rbegin(); iter != str_first_alt.rend(); ++iter)
        l2_.push(*iter);

    l1_.push(ElemL1(iter_to_first_alt));
}

void ParserString::stepTwo()
{
    std::cout << "2";    
    char curr_symb = l2_.top();

    if (curr_symb == input_str_[curr_ind_input_])
    {
        l2_.pop();
        l1_.push(ElemL1(curr_symb));

        ++curr_ind_input_;
    }
    else
        stepFour();
}

void ParserString::stepThree()
{
    std::cout << "3";    

    if (l2_.empty() && curr_ind_input_ == input_str_.size())
    {
        while(!l1_.empty())
        {
            ElemL1 curr_elem = l1_.top();
            l1_.pop();

            if (!curr_elem.isCharElem())
                result_.push_front(curr_elem.rule_iter->second.ind_rule);
        }

        curr_state_ = StateAlg::t; 
    }
    else
        curr_state_ = StateAlg::b;
}

void ParserString::stepFour()
{
    std::cout << "4";    
    curr_state_ = StateAlg::b; 
}

void ParserString::stepFive()
{
    std::cout << "5";
    ElemL1 curr_elem = l1_.top();
    
    if (!curr_elem.isCharElem() || curr_state_ != StateAlg::b)
        throw std::logic_error("Error in parser");

    --curr_ind_input_;
    l1_.pop();
    l2_.push(curr_elem.char_elem);
}


void ParserString::stepSix()
{
    std::cout << "6";    

    ElemL1 curr_elem = l1_.top();
    
    if (curr_elem.isCharElem() || curr_state_ != StateAlg::b)
        throw std::logic_error("Error in parser");

    if (curr_elem.rule_iter->second.is_last_alt)
    {
        if (curr_ind_input_ == 0 && curr_elem.rule_iter->first == grammar_.non_terminal_symb_[curr_ind_input_])
            stepSixB();
        else
            stepSixC();
    }
    else
        stepSixA();
}


void ParserString::stepSixA()
{
    std::cout << "a";    
    ElemL1 curr_elem_l1 = l1_.top();
    l1_.pop();

    auto next_rule = curr_elem_l1.rule_iter;
    ++next_rule;
    
    for (const char& elem: curr_elem_l1.rule_iter->second.right_part)
        l2_.pop();

    std::string& str_curr_elem = next_rule->second.right_part;
    for (auto iter = str_curr_elem.rbegin(); iter != str_curr_elem.rend(); ++iter)
        l2_.push(*iter);


    l1_.push(ElemL1(next_rule));

    curr_state_ = StateAlg::q;
}


void ParserString::stepSixB()
{
    std::cout << "b";    
    curr_state_ = StateAlg::t;
}


void ParserString::stepSixC()
{
    std::cout << "c";    
    ElemL1 curr_elem_l1 = l1_.top();
    l1_.pop();

    for (const char& elem: curr_elem_l1.rule_iter->second.right_part)
        l2_.pop();

    l2_.push(curr_elem_l1.rule_iter->first);
}

std::deque<uint32_t> ParserString::parse(std::string input_str, Grammar& grammar)
{
    ParserString parser(input_str, grammar);

    std::cout << "Steps: ";
    while (parser.curr_state_ != StateAlg::t)
    {
        if (parser.curr_state_ == StateAlg::q)
        {
            if (parser.l2_.empty())
                parser.stepThree();
            else
            {
                if (vectorContainsIt(grammar.non_terminal_symb_, parser.l2_.top()))
                    parser.stepOne();
                else
                {
                    if (vectorContainsIt(grammar.terminal_symb_, parser.l2_.top()))
                        parser.stepTwo();
                    else
                        throw std::logic_error("Unknown symbol");
                }
            }
        }

        if (parser.curr_state_ == StateAlg::b)
        {
            if (parser.l1_.empty())
                throw std::logic_error("l1 empty");

            if (parser.l1_.top().isCharElem())
                parser.stepFive();
            else
                parser.stepSix();
        }
    }

    std::cout << std::endl;
    return parser.result_;
}
