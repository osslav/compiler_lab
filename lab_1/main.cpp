#include "grammar.hpp"
#include <iostream>

int main()
{
    try
    {
        //std::string input_str("!a+b!");
        //std::string input_str("!(a+b*a)*(b*b+a*(a+b+a))!");
        //std::string input_str("!a+!");

        Grammar grammar;
        grammar.setTerminalSymb({'!', '+', '*','(', ')', 'a', 'b'});
        grammar.setNonTerminalSymb({'A', 'B', 'T', 'M'});
        grammar.setStartSymb('A');

        grammar.addRule('A', "!B!");    //1
        grammar.addRule('B', "T");      //2
        grammar.addRule('B', "T+B");    //3
        grammar.addRule('T', "M");      //4
        grammar.addRule('T', "M*T");    //5
        grammar.addRule('M', "a");      //6
        grammar.addRule('M', "b");      //7
        grammar.addRule('M', "(B)");    //8
        
        
        std::string input_str;
        std::string quit_str("close");
        
        std::cout << "Start program. Enter \"" << quit_str << "\" for quit" << std::endl;

        std::cout << std::endl << "Enter string: ";
        std::cin >> input_str;
        
        while (input_str != quit_str)
        {
            std::deque<uint32_t> res = ParserString::parse(input_str, grammar);
        
            if (res.empty())
                std::cout << "Invalid string" << std::endl;
            else
            {
                std::cout << "Output: ";
                for (uint32_t elem: res)
                    std::cout << elem << " ";
            
                std::cout << std::endl;
            }
 
            std::cout << std::endl << "Enter string: ";
            std::cin >> input_str;   
        }
       
        std::cout << std::endl << "Program shutdown normally" << std::endl;
        return EXIT_SUCCESS;
    }
    catch(const std::exception& exc)
    {
        std::cout << std::endl << "Error: " << exc.what() << std::endl;
        return EXIT_FAILURE;
    }
    catch(...)
    {
        std::cout << std::endl << "Unknown error..." << std::endl;
        return EXIT_FAILURE;
    }
}
