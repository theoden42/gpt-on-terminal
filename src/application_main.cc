#include <string>

#include "CLI11.hpp"
#include "gtot_api.hpp"

using gtot_api::output_response;
using gtot_api::process_request;
using gtot_api::verify_request;

int main(int argc, char **argv) {
  CLI::App app{"GPT on Terminal"};
  std::string user_prompt;
  std::string code_file_path;
  std::string code_line_number;
  app.add_option(
         "-p,--prompt", user_prompt,
         "The user prompt sent to the model. This message is sent as is")
      ->required();
  app.add_option("-f,--file", code_file_path,
                 "The file path the code from which code snippet is extracted "
                 "and included in the prompt. If not specified, sent without "
                 "code snippet.");
  app.add_option("-l,--line", code_line_number,
                 "The line number of the code snippet to get from the code "
                 "file, if not specified first 50 lines are sent");
  try {
    app.parse(argc, argv);
  } catch (const CLI::ParseError &e) {
    return app.exit(e);
  }
  if (!verify_request(user_prompt, code_file_path, code_line_number)) {
    std::cerr << "Process request failed, see the error logs above"
              << std::endl;
    exit(1);
  }
  std::cout << "Request verified, querying the model" << std::endl;
  std::string response =
      process_request(user_prompt, code_file_path, code_line_number);
  if (response.empty()) {
    std::cerr << "Request failed. Model returned no response" << std::endl;
    exit(1);
  }
  output_response(response);
}