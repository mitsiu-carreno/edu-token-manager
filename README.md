apt-get install libmysqlcppconn-dev

gcc -o output-file $(mysql_config --cflags) src/main.cpp $(mysql_config --libs)

