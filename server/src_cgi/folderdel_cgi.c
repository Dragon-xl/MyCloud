#include "fcgi_config.h"
#include "fcgi_stdio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "make_log.h" //日志头文件
#include "util_cgi.h"
#include "deal_mysql.h"
#include "cfg.h"
#include "cJSON.h"
#include <sys/time.h>

#define MYFILES_LOG_MODULE       "cgi"
#define MYFILES_LOG_PROC         "folderdel"

static char mysql_user[128] = {0};
static char mysql_pwd[128] = {0};
static char mysql_db[128] = {0};

//redis 服务器ip、端口
//static char redis_ip[30] = {0};
//static char redis_port[10] = {0};

//读取配置信息
void read_cfg()
{
    //读取mysql数据库配置信息
    get_cfg_value(CFG_PATH, "mysql", "user", mysql_user);
    get_cfg_value(CFG_PATH, "mysql", "password", mysql_pwd);
    get_cfg_value(CFG_PATH, "mysql", "database", mysql_db);
    LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "mysql:[user=%s,pwd=%s,database=%s]", mysql_user, mysql_pwd, mysql_db);

    //读取redis配置信息
    //get_cfg_value(CFG_PATH, "redis", "ip", redis_ip);
    //get_cfg_value(CFG_PATH, "redis", "port", redis_port);
    //LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "redis:[ip=%s,port=%s]\n", redis_ip, redis_port);
}
int  get_user_move_json(char* buf ,int* folder_id)
{
    int ret =0;
    

    cJSON* root = cJSON_Parse(buf);
    if(NULL == root)
    {
        LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "cJSON_Parse err\n");
        ret = -1;
        goto END;
    }
    cJSON* parentId = cJSON_GetObjectItem(root,"folder_id");
    if(parentId ==NULL)
    {
        LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "cJSON_GetObjectItem(root,\"folder_id\")\n");
        ret = -1;
        goto END;
    }
    (*folder_id) = parentId->valueint;
    
END:
    if(root != NULL)
    {
        cJSON_Delete(root);//删除json对象
        root = NULL;
    }

    return ret;
}
int put_user_move_mysql(int folder_id)
{
    int ret =0;
    MYSQL_RES* res_set=NULL;
    MYSQL* conn=NULL;
    char sql_cmd[SQL_MAX_LEN]={0};
    conn =  msql_conn(mysql_user,mysql_pwd,mysql_db);
    if(conn == NULL)
    {
        LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "msql_conn connect err\n");
        ret = -1;
        goto END;
    }
    mysql_query(conn, "set names utf8");
    sprintf(sql_cmd,"DELETE FROM user_folder WHERE folder_id = %d;",folder_id);
    if(mysql_query(conn, sql_cmd) != 0)
    {
        LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "%s 操作失败: %s\n", sql_cmd, mysql_error(conn));
        ret = -1;
        goto END;
    }

END:
    if (conn != NULL)
    {
        mysql_close(conn); //断开数据库连接
    }

    return ret;

}

int main()
{
    read_cfg();
    
  
   
    int folder_id=0;
    while(FCGI_Accept()>=0)
    {
        
        
        char* contentLength = getenv("CONTENT_LENGTH");
        int len;
        printf("Content-type: text/html\r\n\r\n");
        if(contentLength ==NULL)
        {
            len=0;
        }
        else
        {
             len = atoi(contentLength);
        }
        if(len<=0)
        {
            printf("No data from standard input.<p>\n");
            LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "len = 0, No data from standard input\n");
        }
        else
        {
            char buf[4*1024];
            int ret= 0;
            ret =fread(buf,1,len,stdin);
            if(ret == 0)
            {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "fread(buf, 1, len, stdin) err\n");
                continue;
            }
            LOG(MYFILES_LOG_MODULE,MYFILES_LOG_PROC,"buf:%s\n",buf);
            ret = get_user_move_json(buf,&folder_id);
            if(ret !=0)
            {
                LOG(MYFILES_LOG_MODULE, MYFILES_LOG_PROC, "get_user_folder failed\n");
                ret =-1;
                goto END;
            }
            ret = put_user_move_mysql(folder_id);
            if(ret<0)
            {
                ret=-1;
                goto END;
            }
END:            
            
            folder_id=0;

              char *out = NULL;
            //给前端返回，上传情况
            /*
               上传文件：
               成功：{"code":"008"}
               失败：{"code":"009"}
               */
            if(ret == 0) //成功上传
            {
                out = return_status("008");//common.h
            }
            else//上传失败
            {
                out = return_status("009");//common.h
            }

            if(out != NULL)
            {
                printf(out); //给前端反馈信息
                free(out);   //记得释放
            }
            
        }
    }




    return 0;
}