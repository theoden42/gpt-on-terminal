#include <string> 

#include "CLI11.hpp" 
#include "gtot_api.hpp"

int main(int argc, char** argv){ 
    CLI::App app{"GPT on Terminal"};
    std::string user_prompt;
    std::string code_file_path;
    std::string code_line_number;
    app.add_opt
    app.parse(argc, argv);
}