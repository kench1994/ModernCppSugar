#include <iostream>
int main()
{
    if(1 and 1)
        std::cout << "and" << std::endl;

    if(0 or 1)
        std::cout << "or" << std::endl;

    if(not 0 and not 0)
        std::cout << "and not" << std::endl;

    return 0;
}