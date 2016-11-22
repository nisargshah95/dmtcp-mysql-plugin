#include "dmtcp.h"
#include <stdio.h>
#include <mysql.h>

MYSQL *pmysql;
const char *phost, *puser, *ppasswd, *pdb, *punix_socket;
unsigned int pport;
unsigned long pclientflag;

void connect_err(MYSQL *con)
{
  fprintf(stderr, "PLUGIN: %s\n", mysql_error(con));
  mysql_close(con);
  exit(1);
}

MYSQL* STDCALL mysql_real_connect(MYSQL *mysql, const char *host,
             const char *user,
             const char *passwd,
             const char *db,
             unsigned int port,
             const char *unix_socket,
             unsigned long clientflag)
{
  printf("Calling connect wrapper...\n");

  pmysql = mysql, phost = host, puser = user, ppasswd = passwd, pdb = db, punix_socket = unix_socket;
  pport = port;
  pclientflag = clientflag;

  return NEXT_FNC(mysql_real_connect)(mysql, host, user, passwd, db, port, unix_socket, clientflag);
}

static void preCkpt()
{
  // Code to execute on ckpt phase.
  // You might want to update the criterion for dmtcp_must_ckpt_file.
  printf("\n*** The plugin %s is being called before checkpointing.***\n",
     __FILE__);

  if(pmysql)
  {
    pmysql->free_me = 0; // do not free memory for MYSQL struct
    mysql_close(pmysql);
  }

  // pmysql->reconnect = 1;

  //printf("CLOSE: mysql = %p\n", (void *)pmysql);
}

static void restart()
{
  // Code to execute on restart phase.
  // You might want to update the criterion for dmtcp_get_new_file_path.
  printf("*** The plugin %s has now been checkpointed. ***\n", __FILE__);

  if(pmysql)
  {
    if((pmysql = mysql_init(pmysql)) == 0)
    {
        connect_err(pmysql);
    }
    if ((NEXT_FNC(mysql_real_connect)(pmysql, phost, puser, ppasswd, 
            pdb, 0, NULL, 0)) == NULL) 
    {
        connect_err(pmysql);
    }

    //printf("RESTART: pmysql = %p\n", (void *)pmysql);

    pmysql->free_me = 1;
  }
}

void dmtcp_event_hook(DmtcpEvent_t event, DmtcpEventData_t *data)
{
  switch (event) {
    case DMTCP_EVENT_THREADS_SUSPEND:
      preCkpt();
      break;

    case DMTCP_EVENT_RESUME:
      restart();
      break;

    case DMTCP_EVENT_RESTART:
      restart();
      break;

    default:
      break;
  }

  DMTCP_NEXT_EVENT_HOOK(event, data);
  return;
}
