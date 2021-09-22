#ifndef STUDENTS_H
#define STUDENTS_H

#include <mysql.h>

struct Student{
  char * id;
  char *code;
  char *full_name;
  char *current_token = nullptr;
};

namespace students{
  bool GetCodes(MYSQL *);
  Student* AskVerification(MYSQL *, char [5]);
  int AskQuestionNum(MYSQL *, Student*);
}

#endif
