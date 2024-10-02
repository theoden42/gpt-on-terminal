#ifndef GTOT_API_HPP
#define GTOT_API_HPP

#include <string> 

namespace gtot_api { 
    // check if the openai api keys for the open api are set in the environment variables
    bool check_api_key_env_var();
    // get the openai api keys for the open api 
    std::string get_api_key_env_var();
    // call the openai api with the given prompt
    std::string call_openai_api(std::string_view prompt);
}

#endif // GTOT_API_HPP