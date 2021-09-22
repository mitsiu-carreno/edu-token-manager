#include <string.h>

namespace utils{
  short GetLength(char * input){
    short i{0};
    while(input[i]){
      ++i;
    }
    return i;
  }

  /*
  int CharToInt(char * char_value){
    int int_value = 0;
    try{
      int_value = std::stoi(char_value);
    }catch(std::exception const &e){
      return 0;
    }
    return int_value;
  }
  */

}
