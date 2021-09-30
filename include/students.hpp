#ifndef STUDENTS_H
#define STUDENTS_H

#include <mysql.h>

struct Student{
  int id;
  char *code;
  char *full_name;
  int current_token;
};

namespace students{
  bool GetCodes(MYSQL *);
  void CheckIntegrity(MYSQL *);
  Student* AskVerification(MYSQL *, char [5]);
  int AskQuestionNum(MYSQL *, Student*);
}

#endif
