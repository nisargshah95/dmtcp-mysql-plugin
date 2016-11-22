#include <mysql.h>
#include <stdio.h>
#include <unistd.h>

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "APP: %s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

int main(int argc, char **argv)
{  
  int dummy;

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }

  // using "localhost" creats a connection via unix socket
  // use 127.0.0.1 instead
  if (mysql_real_connect(con, "127.0.0.1", "root", "root", 
          "dmtcp", 3306, NULL, 0) == NULL) 
  {
      finish_with_error(con);
  }

  if (mysql_query(con, "INSERT INTO t VALUES(1)")) 
  {
      finish_with_error(con);
  }

  printf("APPLICATION: mysql = %p\n", (void *)con);
  scanf(" %d", &dummy);
  printf("APPLICATION: mysql = %p\n", (void *)con);
  //sleep(5);

  if (mysql_query(con, "INSERT INTO t VALUES(2)")) 
  {
      finish_with_error(con);
  }

  // if (mysql_query(con, "INSERT INTO t VALUES(3)")) 
  // {
  //     finish_with_error(con);
  // }

  printf("mysql_close: mysql = %p\n", (void *)con);

  mysql_close(con);
  exit(0);
}