/* Simple C program that connects to MySQL Database server*/
#include "mysql_conn.hpp"
#include "students.hpp"
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

  while(true){
    clear();
    
    if(!students::GetCodes(conn)){
      printw("Error al solicitar códigos");
      return 0;
    }
    
    printw("Ingresa tu clave:");
    const short kMaxInput = 5;
    char code_input[kMaxInput];
    getnstr(code_input, kMaxInput-1);

    if(strcmp(code_input, "oliv\0") == 0){
      break;
    }
  
    bool test = students::AskVerification(conn, code_input);
    printw("%d",test);
    getch();

    //if(students::AskVerification(conn, code_input)){
    if(true){
      continue;
    }
    printw("unreachable");
    getch();
  }

  mysqlConn::Disconnect(conn);
  /*
   MYSQL *conn;
   MYSQL_RES *res;
   MYSQL_ROW row;

   char server[] = "ip";
   char user[] = "user";
   char password[] = "pass"; * set me first *
   char database[] = "db";

   conn = mysql_init(NULL);

   * Connect to database *
   if (!mysql_real_connect(conn, server,
         user, password, database, 0, NULL, 0)) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }

   * send SQL query *
   if (mysql_query(conn, "show tables")) {
      fprintf(stderr, "%s\n", mysql_error(conn));
      exit(1);
   }

   res = mysql_use_result(conn);

   * output table name *
   printf("MySQL Tables in mysql database:\n");
   while ((row = mysql_fetch_row(res)) != NULL)
      printf("%s \n", row[0]);

   * close connection *
   mysql_free_result(res);
   mysql_close(conn);
*/
   endwin();  // end curses mode
   return 0;
}
