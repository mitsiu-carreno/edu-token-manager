/* Simple C program that connects to MySQL Database server*/
#include "mysql_conn.hpp"
#include "students.hpp"
#include "answer.hpp"
#include "utils.hpp"
#include <ncurses.h>
#include <string.h>

int main(int argc, char *argv[]) {
  // argv[0] is program name

  initscr();  // start curses mode
/*
  if(argv[1] == NULL){
    printw("Agregue un string con la dirección IP de la base de datos ./token_manager \"127.0.0.1\" \"my_user\" \"my_password\" \"my_database\"")
    endwin();
    exit(EXIT_FAILURE);
  }

  if(argv[2] == NULL){
    printw("Agregue un string con el usuario de la base de datos ./token_manager \"127.0.0.1\" \"my_user\" \"my_password\" \"my_database\"")
    endwin();
    exit(EXIT_FAILURE);
  }
  if(argv[3] == NULL){
    printw("Agregue un string con la contraseña de la base de datos ./token_manager \"127.0.0.1\" \"my_user\" \"my_password\" \"my_database\"")
    endwin();
    exit(EXIT_FAILURE);
  }
  if(argv[4] == NULL){
    printw("Agregue un string con el nombre  de la base de datos ./token_manager \"127.0.0.1\" \"my_user\" \"my_password\" \"my_database\"")
    endwin();
    exit(EXIT_FAILURE);
  }
  */
  
  MYSQL* conn = mysqlConn::Connect();  
  if(!conn){
    return 0;
  }

  const short kMaxInput = 5;
  char mode_input[kMaxInput];
  getnstr(mode_input, kMaxInput-1);
  
  const bool sudo_mode = strcmp(mode_input, "prof\0") == 0 ? 1 : 0;

  while(true){
    clear();
  
    if(sudo_mode){
      printw("PROF MODE\n");
    }  
    if(!students::GetCodes(conn)){
      printw("Error al solicitar códigos");
      getch();
      return 0;
    }
    
    printw("Ingresa tu clave:");
    char code_input[kMaxInput];
    getnstr(code_input, kMaxInput-1);

    if(utils::GetLength(code_input) < 3){
      printw("Código no valido, enter para continuar");
      getch();
      continue;
    }

    if(strcmp(code_input, "gtfo\0") == 0){
      break;
    }

    if(strcmp(code_input, "chek\0") == 0){
      students::CheckIntegrity(conn);
      printw("\nEnter para continuar");
      getch();
      continue;
    }

    Student* current_student = students::AskVerification(conn, code_input, sudo_mode);
    if(!current_student){
      continue;
    }

    int question_number = students::AskQuestionNum(conn, current_student, sudo_mode);
    if(!question_number){
      continue;
    }

    answer::ShowAnswer(conn, current_student, question_number);
     

    delete current_student;
  }

  mysqlConn::Disconnect(conn);

  endwin();  // end curses mode
  return 0;
}
