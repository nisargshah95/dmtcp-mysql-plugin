#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF 100

// code snippets taken from http://zetcode.com/db/mysqlc/

void finish_with_error(MYSQL *con)
{
  fprintf(stderr, "%s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

int main(int argc, char **argv)
{  
  if(argc < 6)
  {
    printf("USAGE: %s [host] [user] [pass] [database] [port]\n", argv[0]);
    return 0;
  }


  int dummy;
  char *query = malloc(sizeof(char) * BUF);
  MYSQL *con = mysql_init(NULL);

  if (con == NULL) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }

  // using "localhost" creats a connection via unix socket
  // use 127.0.0.1 instead
  if (mysql_real_connect(con, argv[1], argv[2], argv[3], 
          argv[4], (unsigned int)atoi(argv[5]), NULL, 0) == NULL) 
  {
      finish_with_error(con);
  }
  printf("connected to database\n");

  while(1)
  {
    printf("Enter SQL command (q to quit): ");
    scanf(" %[^\n]s", query);
    if(strcmp(query, "q") == 0)
      break;

    if (mysql_query(con, query)) 
    {
        fprintf(stderr, "%s\n", mysql_error(con));
        continue;
    }

    MYSQL_RES *result = mysql_store_result(con);
  
    if (result == NULL)
    {
        if(mysql_errno(con) == 0)
          continue;
        finish_with_error(con);
    }

    int num_fields = mysql_num_fields(result);
    MYSQL_ROW row;
    while ((row = mysql_fetch_row(result))) 
    { 
        for(int i = 0; i < num_fields; i++) 
        { 
            printf("%s ", row[i] ? row[i] : "NULL"); 
        } 
            printf("\n"); 
    }
    mysql_free_result(result);
  }
  
  mysql_close(con);
  
  return 0;
}