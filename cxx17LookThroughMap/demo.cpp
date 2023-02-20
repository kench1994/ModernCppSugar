#include <map>
#include <string>
#include <iostream>
int main()
{
    std::map<std::string, int> m{{"CPU", 10}, {"GPU", 15}, {"RAM", 20}};

    // iterate using C++17 facilities
    for (const auto& [key, value] : m)
        std::cout << '[' << key << "] = " << value << "; ";

    std::cout << '\n';
    return 0;
}