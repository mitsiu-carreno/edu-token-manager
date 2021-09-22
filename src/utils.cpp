namespace utils{
  short GetLength(char * input){
    short i{0};
    while(input[i]){
      ++i;
    }
    return i;
  }

}
