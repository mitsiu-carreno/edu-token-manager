#include "mysql_conn.hpp"
#include <mysql.h>
#include <ncurses.h>

namespace answer{
  void ShowAnswer(MYSQL *conn, int id_student, int id_answer){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[50];
    sprintf(query, "select answer from answers WHERE id = %d", id_answer);
    res = mysqlConn::Query(conn, query);
    if(!res){
      printw("Error al solicitar respuesta");
      getch();
    }

    row = mysql_fetch_row(res);
    if(!row){
      printw("Hmmm se me perdió esta respuesta o.0\n");
      getch();
    }

    printw("Respuesta: %s", row[0]);
    printw("\nCopia esta respuesta y presiona cualquier tecla para finalizar\n");
    getch();
        
    mysql_free_result(res);
  }
}
