# GPT on Terminal

A small terminal tool that let's you prompt GPT on your terminal. 

### Set Up

1. Build Using CMake 
 - Download [CMake](https://cmake.org/cmake/help/latest/guide/tutorial/index.html) for your OS. 
 - Naviage to the build folder using ```cd build```
 - run ```cmake ..``` to create require buildfile 
 - run ```make``` to create the executable `gpt-on-terminal`

2. If you trust me a lot. Directly download executable from [here]().

### (Optional) Alias set up
1. Move the executable to `~/scripts`(or anywhere you want). 
2. Add a path environment to the executable by adding `export PATH="$PATH:/path/to/your/executable/directory"` to your `~/.zshrc`. Also run `source ~/.zshrc`.
3. Set up an alias like `alias gtot="gpt-on-terminal"`  

### How to Use

1. Get the API key from [OpenAI](https://openai.com/index/openai-api/). 
2. Set an Envirionment variable OPENAI_API_KEY to the API key obtained above.
3. Use gptot --help to get a short man page. 