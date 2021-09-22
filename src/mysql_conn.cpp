#include <mysql.h>
#include <string>
#include <ncurses.h>

namespace mysqlConn{
  MYSQL* Connect(){
    char server[] = "127.0.0.1";
    char user[] = "user";
    char password[] = "pass";
    char database[] = "db";

    MYSQL * conn;
    
    conn = mysql_init(NULL);
    
    /* Connect to database */ 
    if(!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0)){
      printw("Error while connecting to database\n");
      printw("%s\n", mysql_error(conn));
      return nullptr;
    }

    return conn;
  }

  MYSQL_RES* Query(MYSQL *conn, const char *q){
    MYSQL_RES *res;
    
    if(mysql_query(conn, q)){
      printw("Error while performing query %s\n", q);
      printw("%s\n", mysql_error(conn));
      return nullptr;  
    }

    res = mysql_use_result(conn);
    return res;
  }

  void Disconnect(MYSQL *conn){
    mysql_close(conn);
  }
}
