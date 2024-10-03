#include <charconv>
#include <cstdlib>
#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <string_view>

#include "gtot_api.hpp"
#include "json.hpp"
#include "openai.hpp"

using json = nlohmann::json;

const uint32_t INF = 1000000;
const uint32_t MAX_LINES = 50;
constexpr const char *INITIAL_PROMPT =
    "Suppose a user is giving you the following prompt and after the prompt "
    "ends, the code for the issue is attached.\n";
const std::string url = "https://api.openai.com/v1/chat/completions";

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

std::string extract_code(std::string_view file_path,
                         std::string_view line_number) {
  uint32_t starting_line_number, ending_line_number;
  if (line_number.empty()) {
    starting_line_number = 1;
    ending_line_number = MAX_LINES;
  } else if (line_number.find("-") == std::string::npos) {
    auto result = std::from_chars(line_number.data(),
                                  line_number.data() + line_number.size(),
                                  starting_line_number);
    ending_line_number = starting_line_number;
  } else {
    std::string_view start_line = line_number.substr(0, line_number.find('-'));
    std::string_view end_line = line_number.substr(line_number.find('-') + 1);
    auto result = std::from_chars(start_line.data(),
                                  start_line.data() + start_line.size(),
                                  starting_line_number);
    result = std::from_chars(end_line.data(), end_line.data() + end_line.size(),
                             ending_line_number);
  }
  std::string extracted_code;
  std::ifstream file(file_path.data());
  if (!file.is_open()) {
    std::cerr << "Unable to open the required code file" << std::endl;
    exit(1);
  }
  std::string extracted_line;
  uint32_t current_line_number = 1;
  while (std::getline(file, extracted_line)) {
    if (current_line_number >= starting_line_number &&
        current_line_number <= ending_line_number) {
      extracted_code += extracted_line + "\n";
    }
  }
  return extracted_code;
}

static size_t WriteCallback(void *contents, size_t size, size_t nmemb,
                            std::string *s) {
  size_t total_size = size * nmemb;
  s->append((char *)contents, total_size);
  return total_size;
}

std::string call_gpt_api(std::string_view prompt) {
  std::string api_key = get_api_key_env_var();
  openai::start(api_key);
  auto completion = openai::completion::create(
      R"({
        "model":"gpt-3.5-turbo",
        "prompt": ")" +
      std::string(prompt) + R"("
    })"_json);
  return completion.dump();
}

} // namespace

bool verify_request(std::string_view prompt, std::string_view file_path,
                    std::string_view line_number) {
  if (!check_api_key_env_var()) {
    std::cerr << "OPENAI_API_KEY environment variable is not set" << std::endl;
    return false;
  }
  if (file_path.empty() && !line_number.empty()) {
    std::cerr << "Ambiguous options File path is empty, but line number is "
                 "specified"
              << std::endl;
    return false;
  }
  // check if the lines numbers are specified in the correct format of either
  // a single line number or a range of line numbers
  if (!line_number.empty()) {
    if (line_number.find('-') != std::string::npos) {
      std::string_view start_line =
          line_number.substr(0, line_number.find('-'));
      std::string_view end_line = line_number.substr(line_number.find('-') + 1);
      if (start_line.empty() || end_line.empty()) {
        std::cerr << "Invalid line number range" << std::endl;
        return false;
      }
      uint32_t start_line_number, end_line_number;
      auto result = std::from_chars(start_line.data(),
                                    start_line.data() + start_line.size(),
                                    start_line_number);
      if (result.ec == std::errc()) {
        std::cerr << "Invalid start line number" << std::endl;
      }
      result = std::from_chars(
          end_line.data(), end_line.data() + end_line.size(), end_line_number);
      if (result.ec == std::errc()) {
        std::cerr << "Invalid end line number" << std::endl;
        return false;
      }
      if (start_line_number > end_line_number ||
          end_line_number - start_line_number + 1 > MAX_LINES) {
        std::cerr << "Invalid line number range. Only " << MAX_LINES
                  << " lines are acceptable" << std::endl;
        return false;
      }
    } else {
      uint32_t line_number_int;
      auto result = std::from_chars(line_number.data(),
                                    line_number.data() + line_number.size(),
                                    line_number_int);
      if (result.ec == std::errc()) {
        std::cerr << "Invalid line number" << std::endl;
        return false;
      }
    }
  }
  return true;
}

std::string process_request(std::string_view prompt, std::string_view file_path,
                            std::string_view line_number) {
  std::string extracted_code =
      !file_path.empty() ? extract_code(file_path, line_number) : "";
  std::string final_prompt = std::string(INITIAL_PROMPT) + std::string(prompt) +
                             "\n" + "Attached Code:\n" + extracted_code + "\n";
  return call_gpt_api(final_prompt);
}

void output_response(std::string_view response) {
  // std::cout << response << std::endl;
  json response_json;
  try {
    response_json = json::parse(response);
  } catch (json::parse_error &e) {
    std::cerr << "Error parsing the response from the API" << std::endl;
    exit(1);
  }
  if (response_json["error"] != nullptr) {
    std::cout << "Model API Erred!" << std::endl;
    std::cerr << "Query failed due to following error\n"
              << response_json["error"]["message"] << std::endl;
    exit(1);
  }
  std::cout << response << std::endl;
  std::string generated_text =
      response_json["choices"][0]["message"]["content"];
  std::cout << generated_text << std::endl;
}

} // namespace gtot_api