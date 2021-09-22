#include "mysql_conn.hpp"
#include "utils.hpp"
#include "students.hpp"
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

  Student* AskVerification(MYSQL *conn, char code_input[5]){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[100];
    sprintf(query, "SELECT id, code, full_name, current_token, identity_confirmation FROM students WHERE code = '%s'", code_input);
    res = mysqlConn::Query(conn, query);
    if(!res){
      printw("Error al solicitar confirmación");
      return nullptr;  
    }

    row = mysql_fetch_row(res);
    if(!row){
      printw("Código invalido, enter para continuar\n");
      getch();
      return nullptr;
    }

    Student *current_student = new (std::nothrow) Student;
    current_student->id = row[0];
    current_student->code = row[1];
    current_student->full_name = row[2];
    current_student->current_token = row[3];
    //VALIDAR TOKENS VACIOS!!!
    char confirmation [sizeof(row[4])];
    strcpy(confirmation, row[4]);
    clear();
    printw("\n\n");
    printw("Bienvenid@ %s cuentas con %s tokens\n", current_student->full_name, current_student->current_token);
    
    mysql_free_result(res);
    char confirmation_input [sizeof(confirmation)];

    while(true){
    
      printw("\nPregunta de seguridad\nQué entidad elegiste para hacer tu tarea de tipos de variables? [S = Salir]: ");
      getnstr(confirmation_input, sizeof(confirmation_input)-1);
      if(utils::GetLength(confirmation_input) == 0){
        continue;
      }

      //printw("%s - %s\n", confirmation, confirmation_input);
      if(confirmation_input[0] == 'S' && confirmation_input[1] == '\0'){
        return nullptr;
      }
      if(strcmp(confirmation, confirmation_input) == 0){
        printw("CORRECTO!\n");
        return current_student;
      }
    }
  }

  int ConfirmQuestion(MYSQL *conn, int question_number){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[69];
    sprintf(query, "SELECT id, txt_question FROM answers WHERE num_question = %d", question_number);
    res = mysqlConn::Query(conn, query);
    if(!res){
      printw("Error al soliciar texto de pregunta :(");
      return false;
    }

    row = mysql_fetch_row(res);
    int id_question = 0;
    try {
      id_question = std::stoi(row[0]);
    }catch(std::exception const &e){
      printw("Error interno id_pregunta");
      getch();
    }
    printw("\nPregunta seleccionada:\n%s\nDeseas continuar y gastar un token? [s/n]: ", row[1]);
    mysql_free_result(res);
    char confirm_input [2];
    getnstr(confirm_input, 1);
    
    if(confirm_input[0] == 's'){
      return id_question;
    }
    return 0;
  }
  
  int AskQuestionNum(MYSQL *conn, Student *current_student){
    while(true){
      printw("Ingresa el número de pregunta que quieres [1-17] [0 = Salir]: ");
      char question_input[3];
      getnstr(question_input, 2);
      int question_number = 0;
      try {
        question_number = std::stoi(question_input);
      } catch (std::exception const &e) {
        printw("Número invalido"); 
        getch();
        // This could not be parsed into a number so an exception is thrown.
        // atoi() would return 0, which is less helpful if it could be a valid value.
      }

      if(question_number == 0){
        return 0;
      }
      if(question_number >= 1 && question_number <= 17){
        int id_question = ConfirmQuestion(conn, question_number);
        if(id_question){
          return id_question;
        }
        return 0;
      }
    }
  }

}
