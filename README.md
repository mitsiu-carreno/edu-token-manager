# Description
This program is meant to handle my students tokens during the evaluation, counting their remaining tokens as well as logging when and at which question the token was spent.
A professor mode is added to access all students tokens, and use them in their pre-due date revisions.

# Instalation
```bash
$ apt-get install libmysqlcppconn-dev
```

Run the following commands to ensure the mysql driver is setted up correclty
```bash
$ mysql_config --cflags
$ mysql_config --libs
```

# Compilation & execution
```bash
$ make  // Compile and run the program
```

```bash
$ make run  // Run last compiled executable
```

```bash
$ make clean  // Remove executable
```


```bash
$ make hardclean  // Remove executable & object files
```
