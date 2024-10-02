#include <string>

#include "CLI11.hpp"
#include "gtot_api.hpp"

int main(int argc, char **argv) {
  CLI::App app{"GPT on Terminal"};
  std::string user_prompt;
  std::string code_file_path;
  std::string code_line_number;
  app.add_option("-p,--prompt", user_prompt,
                 "The prompt to send to the openai api")
      ->required();
  app.add_option("-f,--file", code_file_path,
                 "The file path of the code file to get the code snippet from");
  app.add_option(
      "-l,--line", code_line_number,
      "The line number of the code snippet to get from the code file");
  if (!verify_request(user_prompt, code_file_path, code_line_number)) {
    std::cerr << "Process request failed, see the error logs above"
              << std::endl;
  }
  std::cout << "Request verified, querying the model" << std::endl;
  std::string response =
      process_request(user_prompt, code_file_path, code_line_number);
  if (response.empty()) {
    std::cerr << "Request failed. Model returned no response" << std::endl;
    exit(1);
  }
  output_response(response);
  app.parse(argc, argv);
}