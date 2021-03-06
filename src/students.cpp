#include "constants.hpp"
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
      mysql_free_result(res);
      return false;
    }

    while((row = mysql_fetch_row(res)) != NULL){
      printw("%s\n", row[0]);
    }

    mysql_free_result(res);
    return true;
  }

  void CheckIntegrity(MYSQL *conn){
    clear();
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[400];
    sprintf(query, "SELECT code, IF( IFNULL(current_token_u%d,0) = (IFNULL(start_token_u%d,0) - IFNULL(token_used,0) ), 1, 0) AS checked FROM students LEFT JOIN (SELECT id_student, COUNT(*) AS token_used FROM logs WHERE unit = %d GROUP BY id_student) AS logs on id_student = id HAVING checked = 0", constants::KUnit, constants::KUnit, constants::KUnit);
    res = mysqlConn::Query(conn, query);
    if(!res){
      mysql_free_result(res);
      printw("Error interno; sin comprobar integridad total\n");
      getch();
      return;
    }
    
    printw("Verificando integridad de registros\n\n");

    while((row = mysql_fetch_row(res)) != NULL){
      printw("Error de integridad -> %s\n", row[0]);
    }

    mysql_free_result(res);
    return;
  }

  bool CheckIntegrity(MYSQL *conn, int id_student){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[400];
    sprintf(query, "select if(current_token_u%d = (select start_token_u%d - (select count(*) from logs where id_student = %d AND unit = %d) from students where id=%d), 1, 0) AS res  from students where id = %d", constants::KUnit, constants::KUnit, id_student, constants::KUnit, id_student, id_student);
    res = mysqlConn::Query(conn, query);
    if(!res){
      mysql_free_result(res);
      printw("Error interno; sin comprobar integridad\n");
      getch();
      return false;
    }

    row = mysql_fetch_row(res);
    if(!row){
      mysql_free_result(res);
      printw("Error interno; sin comprobar integridad\n");
      getch();
      return false;
    }

    bool result {false};
    try{
       result = std::stoi(row[0]);
    }catch(std::exception const &e){
      printw("Error interno integridad");
      getch();
    }
    mysql_free_result(res);
    return result;
  }



  Student* AskVerification(MYSQL *conn, char code_input[5], bool sudo_mode){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[120];
    sprintf(query, "SELECT id, code, full_name, IFNULL(current_token_u%d,0), identity_confirmation FROM students WHERE code = '%s'", constants::KUnit, code_input);
    res = mysqlConn::Query(conn, query);
    if(!res){
      mysql_free_result(res);
      printw("Error al solicitar confirmaci??n");
      return nullptr;  
    }

    row = mysql_fetch_row(res);
    if(!row){
      mysql_free_result(res);
      printw("C??digo invalido, enter para continuar\n");
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
      mysql_free_result(res);
      printw("Parece que no tienes m??s tokens :(");
      getch();
      return nullptr;
    }

    char confirmation [sizeof(row[4])];
    strcpy(confirmation, row[4]);
    mysql_free_result(res);
    clear();
    printw("\n\n");
    //printw("%s", current_student->full_name);
    printw("Bienvenid@ %s cuentas con %d tokens\n", current_student->full_name, current_student->current_token);
    
    // Check data integrity
    if(!CheckIntegrity(conn, current_student->id)){
      printw("Datos corruptos x_x\n");
      getch();
      return nullptr;
    }

    if(sudo_mode){
      return current_student;
    }

    char confirmation_input [25];

    while(true){
    
      printw("\nPregunta de seguridad\nQu?? entidad elegiste para hacer tu tarea de tipos de variables? [null = No Entregada, undefined = Multiples entidades, s = Salir]: ");
      getnstr(confirmation_input, sizeof(confirmation_input));
      if(utils::GetLength(confirmation_input) == 0){
        continue;
      }

      short i{0};
      while(confirmation_input[i]){
        confirmation_input[i] = (tolower(confirmation_input[i]));  
        ++i;
      }

      //printw("%s - %s\n", confirmation, confirmation_input);
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

        printw("CORRECTO!\n");
        return current_student;
      }
    }
  }

  int ConfirmQuestion(MYSQL *conn, int question_number){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[80];
    sprintf(query, "SELECT id, txt_question FROM answers WHERE num_question = %d AND unit = %d", question_number, constants::KUnit);
    res = mysqlConn::Query(conn, query);
    if(!res){
      mysql_free_result(res);
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
  
  int AskQuestionNum(MYSQL *conn, Student *current_student, bool sudo_mode){
    MYSQL_RES *res;
    MYSQL_ROW row;
    char query[90];
    if(sudo_mode){
      sprintf(query, "SELECT MIN(num_question), MAX(num_question) FROM answers WHERE unit = %d AND answer = \"\"", constants::KUnit);
    }else{
      sprintf(query, "SELECT MIN(num_question), MAX(num_question) FROM answers WHERE unit = %d AND answer <> \"\"", constants::KUnit);
    }
    res = mysqlConn::Query(conn, query);
    if(!res){
      mysql_free_result(res);
      printw("Error al solicitar n??mero de preguntas\n");
      getch();
      return 0;  
    }

    row = mysql_fetch_row(res);
    if(!row){
      mysql_free_result(res);
      printw("Error al obtener respuesta de n??mero de preguntas");
      getch();
      return 0;
    }
    
    int min_question_num;
    int max_question_num;

    try{
      min_question_num = std::stoi(row[0]);
      max_question_num = std::stoi(row[1]);
    }catch(std::exception const &e){
      mysql_free_result(res);
      printw("Error al castear n??mero de preguntas\n");
      getch();
      return 0;
    }

    mysql_free_result(res);
    while(true){
      printw("Ingresa el n??mero de pregunta que quieres [%d-%d] [0 = Salir]: ", min_question_num, max_question_num);
      char question_input[3];
      getnstr(question_input, 2);
      int question_number = 0;
      try {
        question_number = std::stoi(question_input);
      } catch (std::exception const &e) {
        printw("N??mero invalido"); 
        getch();
        // This could not be parsed into a number so an exception is thrown.
        // atoi() would return 0, which is less helpful if it could be a valid value.
      }

      if(question_number == 0){
        return 0;
      }
      if(question_number >= min_question_num && question_number <= max_question_num){
        int id_question = ConfirmQuestion(conn, question_number);
        if(id_question){
          return id_question;
        }
        //continue;
      }
    }
  }

}
