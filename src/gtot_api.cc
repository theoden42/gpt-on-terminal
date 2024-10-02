#include <cstdlib> 
#include <iostream>

#include "gtot_api.hpp" 

namespace gtot_api { 
    namespace { 
            
    }

    bool check_api_key_env_var() {
        if (std::getenv("OPENAI_API_KEY") == nullptr) {
            return false;
        } else {
            return true;
        }
    }   
    std::string get_api_key_env_var() {
        return std::getenv("OPENAI_API_KEY");
    }
    std::string call_openai_api(std::string_view prompt) {
        return "This is a response from the openai api";
    }
} // namepsace gtot_api