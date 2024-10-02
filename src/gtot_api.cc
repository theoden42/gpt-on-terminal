#include <cstdlib>
#include <iostream>

#include "gtot_api.hpp"

namespace gtot_api {
namespace {
bool check_api_key_env_var() {
  if (std::getenv("OPENAI_API_KEY") == nullptr) {
    return false;
  } else {
    return true;
  }
}
std::string get_api_key_env_var() { return std::getenv("OPENAI_API_KEY"); }
} // namespace

bool verify_request(std::string_view prompt, std::string_view file_path,
                    std::string_view line_number) {
  if (!check_api_key_env_var()) {
    std::cerr << "OPENAI_API_KEY environment variable not set" << std::endl;
    return false;
    std::exit(1);
  }
  
} // namespace gtot_api