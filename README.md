# GPT on Terminal

A small terminal tool that let's you prompt GPT on your terminal. 

### Set Up

1. Build Using CMake 
 - Download [CMake](https://cmake.org/cmake/help/latest/guide/tutorial/index.html) for your OS. 
 - Naviage to the build folder using ```cd build```
 - run ```cmake ..``` to create require buildfile 
 - run ```make``` to create the executable `gpt-on-terminal`

2. If you trust me a lot. Directly download executable from [here](https://github.com/theoden42/gpt-on-terminal/releases/tag/v1.0.0).

### (Optional) Alias set up
1. Move the executable to `~/scripts`(or anywhere you want). 
2. Add a path environment to the executable by adding `export PATH="$PATH:/path/to/your/executable/directory"` to your `~/.zshrc`. Also run `source ~/.zshrc`.
3. Set up an alias like `alias gtot="gpt-on-terminal"`  

### How to Use

1. Get the API key from [OpenAI](https://openai.com/index/openai-api/). 
2. Set an Envirionment variable OPENAI_API_KEY to the API key obtained above.
```export OPENAI_API_KEY="some_value"```
3. Use ```gtot --help``` to get a short man page. 

#### Example Usage
Query: 
```bash
./gpt-on-terminal -p "suggest a concise way write the code for this" -f "../src/gtot_api.cc" -l "140-151"
```

Reponse: 
```
To make the code more concise, we can remove unnecessary brackets and shorten the error message:

Attached Code:
      uint32_t line_number_int;
      auto result = std::from_chars(line_number.data(), line_number.data() + line_number.size(), line_number_int);
      if (result.ec != std::errc()) {
        std::cerr << "Invalid line number" << std::endl;
        return false;
      }

  return true;
```


