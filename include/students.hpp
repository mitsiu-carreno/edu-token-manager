#ifndef STUDENTS_H
#define STUDENTS_H

#include <mysql.h>

namespace students{
  bool GetCodes(MYSQL *);
  bool AskVerification(MYSQL *, char [5]);
  void AskQuestionNum(MYSQL *, char[5]);
}

#endif
