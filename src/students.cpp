#include "mysql_conn.hpp"
#include <mysql.h>
#include <string.h>
#include <ncurses.h>

namespace students{

  bool GetCodes(MYSQL *conn){    
    MYSQL_RES *res;
    MYSQL_ROW row;

    res = mysqlConn::Query(conn, "SELECT code FROM students");
    if(!res){
      return false;
    }

    while((row = mysql_fetch_row(res)) != NULL){
      printw("%s\n", row[0]);
    }

    mysql_free_result(res);
    return true;
  }

  bool AskVerification(MYSQL *conn, char code_input[5]){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[69];
    sprintf(query, "SELECT identity_confirmation FROM students WHERE code = '%s'", code_input);
    res = mysqlConn::Query(conn, query);
    if(!res){
      printw("Error al solicitar confirmación");
      return 0;  
    }

    row = mysql_fetch_row(res);
    mysql_free_result(res);

    char confirmation [sizeof(row[0])];
    strcpy(confirmation, row[0]);
    printw("\n\n");
    printw("Bienvenido %s", row[0]);
    
    char confirmation_input [sizeof(row[0])];

    while(true){
    
      printw("Pregunta de seguridad\nQué entidad elegiste para hacer tu tarea de tipos de variables? [A=Abortar]");
      getnstr(confirmation_input, sizeof(confirmation_input)-1);

      printw("%s - %s\n", confirmation, confirmation_input);
      if(confirmation_input[0] == 'A'){
        return false;
      }
      if(strcmp(confirmation, confirmation_input) == 0){
        return true;
      }
    }
  }

  void AskQuestionNum(MYSQL *conn, char code_input[5]){
    printw("Ingresa el número de respuesta que quieres:");
    char question_input[3];
    getnstr(question_input, 2);
    printw(question_input);
    getch();
  }

}
