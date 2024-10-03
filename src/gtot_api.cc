#include <charconv>
#include <cstdlib>
#include <curl/curl.h>
#include <fstream>
#include <iostream>
#include <string_view>

#include "gtot_api.hpp"
#include "json.hpp"

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
    starting_line_number = 0;
    ending_line_number = INF;
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
    std::cerr << "Unable to open the required file" << std::endl;
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
  CURL *curl;
  CURLcode res;
  std::string read_buffer;
  curl = curl_easy_init();
  if (!curl) {
    std::cerr << "Uanble to set up Curl\n" << std::endl;
    exit(1);
  }
  json request_message = {
      {"model", "gpt-3.5-turbo"},
      {"messages", {{{"role", "user"}, {"content", prompt}}}}};
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  headers =
      curl_slist_append(headers, ("Authorization: Bearer " + api_key).c_str());
  curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_message.dump().c_str());
  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &read_buffer);
  res = curl_easy_perform(curl);
  if (res != CURLE_OK) {
    std::cerr << "The API request failed with error" << curl_easy_strerror(res)
              << std::endl;
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);
    exit(1);
  }
  curl_easy_cleanup(curl);
  curl_slist_free_all(headers);
  return read_buffer;
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
      if(start_line_number > end_line_number || end_line_number - start_line_number + 1 > MAX_LINES) {
        std::cerr << "Invalid line number range. Only " << MAX_LINES << " lines are acceptable" << std::endl;
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
                             "\n" + extracted_code + "\n";
  return call_gpt_api(final_prompt);
}

void output_response(std::string_view response) {
  std::cout << response << std::endl;
}

} // namespace gtot_api