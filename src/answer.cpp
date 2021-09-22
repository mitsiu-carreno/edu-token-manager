#include "constants.hpp"
#include "mysql_conn.hpp"
#include "students.hpp"
#include <mysql.h>
#include <ncurses.h>

namespace answer{
  void ShowAnswer(MYSQL *conn, Student *current_student, int id_answer){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[100];

    sprintf(query, "INSERT INTO logs (id_student, id_answer, unit) VALUES (%d, %d, %d)", current_student->id, id_answer, constants::KUnit);
    mysqlConn::Query(conn, query);
    sprintf(query, "UPDATE students SET current_token_u%d = %d WHERE id = %d", constants::KUnit, current_student->current_token -1, current_student->id);
    mysqlConn::Query(conn, query);
    /*
    res = mysqlConn::Query(conn, query);
    if(!res){
      printw("Error al guardar logs\n");
      printw("%s\n", mysql_error(conn));
      getch();
      //return;
    }
    row = mysql_fetch_row(res);
    if(!row){
      printw("Error?");
      getch();
    }
    printw("REached");
    getch();

    mysql_free_result(res);
    */

    sprintf(query, "select answer from answers WHERE id = %d", id_answer);
    res = mysqlConn::Query(conn, query);
    if(!res){
      printw("Error al solicitar respuesta\n");
      getch();
    }

    row = mysql_fetch_row(res);
    if(!row){
      printw("Hmmm se me perdió esta respuesta o.0\n");
      getch();
    }

    

    printw("\n\nTienes %d tokens restantes\nRespuesta: %s", current_student->current_token-1, row[0]);
    printw("\nCopia esta respuesta y presiona cualquier tecla dos veces para finalizar\n");
    getch();
    getch();
        
    mysql_free_result(res);
  }
}
