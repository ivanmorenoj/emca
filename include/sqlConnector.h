#ifndef SQLCONNECTOR_H_
#define SQLCONNECTOR_H_

#include <cstddef>
#include <mysql_connection.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "commonConfig.h"

class sqlConnector
{
private:
    char *user;
    char *pass;
    char *host;
    char *schema;
    char buff[250];

    sql::Driver *driver;
    sql::Connection *con;
    sql::Statement *stmt;

public:
    sqlConnector();
    sqlConnector(const char* _u,const char* _p,const char* _h);
    ~sqlConnector();
    void setUser(char* _user);
    void setPassword(char* _pass);
    void setHost(char* _host);
    void setSchema(char *_schema);
    void connect();
    void excecuteQuery(char * query_str);
    void insert_in_GAS_INFO(struct db_info *val);
    void insert_in_GAS_VALUES(struct db_values *val);
};

#endif //SQLCONNECTOR_H_