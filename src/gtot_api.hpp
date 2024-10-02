#ifndef GTOT_API_HPP
#define GTOT_API_HPP

#include <string>

namespace gtot_api {

bool verify_request(std::string_view prompt, std::string_view file_path,
                    std::string_view line_number);
std::string process_request(std::string_view prompt, std::string_view file_path,
                            std::string_view line_number);
void output_response(std::string_view response);

} // namespace gtot_api

#endif // GTOT_API_HPP