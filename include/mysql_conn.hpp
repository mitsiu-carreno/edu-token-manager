#ifndef MYSQL_CONN_H
#define MYSQL_CONN_H

#include <mysql.h>
#include <string>

namespace mysqlConn{
  MYSQL* Connect();
  MYSQL_RES* Query(MYSQL *, const char*);
  void Disconnect(MYSQL *);
}

#endif
