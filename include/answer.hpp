#ifndef ANSWER_H
#define ANSWER_H

#include "students.hpp"
#include <mysql.h>

namespace answer{
  void ShowAnswer(MYSQL*, Student*, int);
}

#endif
