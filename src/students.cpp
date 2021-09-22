#include "mysql_conn.hpp"
#include "utils.hpp"
#include "students.hpp"
#include <ctype.h> 
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

  bool CheckIntegrity(MYSQL *conn, int id_student){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[400];
    sprintf(query, "select if(current_token = (select start_token - (select count(*) from logs where id_student = %d) from students where id=%d), TRUE, FALSE)  from students where id = %d", id_student, id_student, id_student);
    res = mysqlConn::Query(conn, query);
    if(!res){
      printw("Error interno; sin comprobar integridad\n");
      getch();
      return false;
    }

    row = mysql_fetch_row(res);
    if(!row){
      printw("Error interno; sin comprobar integridad\n");
      getch();
      return false;
    }
 
    bool result = row[0];
    mysql_free_result(res);
    return result;
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
    try{

      current_student->id = std::stoi(row[0]);
      current_student->code = row[1];
      current_student->full_name = row[2];
      current_student->current_token = std::stoi(row[3]);
    }catch(std::exception const &e){
      printw("Student info casting failed");
      getch();
    }
    
    if(current_student->current_token <= 0){
      printw("Parece que no tienes más tokens :(");
      getch();
      return nullptr;
    }

    char confirmation [sizeof(row[4])];
    strcpy(confirmation, row[4]);
    mysql_free_result(res);
    clear();
    printw("\n\n");
    printw("%s", current_student->full_name);
    printw("Bienvenid@ %s cuentas con %d tokens\n", current_student->full_name, current_student->current_token);
    
    char confirmation_input [sizeof(confirmation)+1];

    while(true){
    
      printw("\nPregunta de seguridad\nQué entidad elegiste para hacer tu tarea de tipos de variables? [null = No Entregada, undefined = Multiples entidades, s = Salir]: ");
      getnstr(confirmation_input, sizeof(confirmation_input));
      if(utils::GetLength(confirmation_input) == 0){
        continue;
      }

      short i{0};
      while(confirmation_input[i]){
        confirmation_input[i] = (tolower(confirmation_input[i]));  
        ++i;
      }

      printw("%s - %s\n", confirmation, confirmation_input);
      if((confirmation_input[0] == 'S' || confirmation_input[0] == 's') && confirmation_input[1] == '\0'){
        return nullptr;
      }
      i=0;
      bool flag {true};
      while(confirmation[i]){
        if(confirmation[i] != confirmation_input[i]){
          flag = false;
          break;
        }
        ++i;
      }

      if(flag){
        // Check data integrity
        if(!CheckIntegrity(conn, current_student->id)){
          printw("Datos corruptos x_x\n");
          getch();
        }

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
        //continue;
      }
    }
  }

}
