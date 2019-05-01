/*
*   Inyect query in a schema of database
*   This example was based in mysql connector example:
*     -> https://dev.mysql.com/doc/connector-cpp/1.1/en/connector-cpp-examples-complete-example-1.html
*   Author: Ivan Moreno
*     April 2019
*/
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include "sqlConnector.h"

sqlConnector::sqlConnector() {
  user = pass = host = schema = NULL;
  driver = NULL;
  con = NULL;
  stmt = NULL;
}
sqlConnector::sqlConnector(const char* _u,const char* _p,const char* _h) {
  user = (char *)_u;
  pass = (char *)_p;
  host = (char *)_h;
}
sqlConnector::~sqlConnector() {
  if (con) {
    delete stmt;
    delete con;
  }
}
void sqlConnector::setUser(char* _user) {
  user = _user;
}
void sqlConnector::setPassword(char* _pass) {
  pass = _pass;
}
void sqlConnector::setHost(char* _host) {
  host = _host;
}
void sqlConnector::setSchema(char *_schema) {
  schema = _schema;
}
void sqlConnector::connect() {
  if ( user && pass && host && schema)
    try {
      /* Create a connection */
      driver = get_driver_instance();
      con = driver->connect(host,user,pass);
      
      /* Connect to the MySQL schema */
      con->setSchema(schema);
      stmt = con->createStatement();    
    } catch(sql::SQLException &e ) {
      /* manage errors*/
      std::cerr << "# ERR: SQLException in " << __FILE__;
      std::cerr << "( << __FUNCTION__ << ) on line >>" << __LINE__ << std::endl;
      std::cerr << "# ERR: " << e.what();
      std::cerr << " (MySQL error code: " << e.getErrorCode();
      std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
    }
}
void sqlConnector::excecuteQuery(char * query_str) {
  try {
    /* Execute query */
    stmt->execute(query_str);
  } catch(sql::SQLException &e) {
    /* manage errors*/
    std::cerr << "# ERR: SQLException in " << __FILE__;
    std::cerr << "( << __FUNCTION__ << ) on line >>" << __LINE__ << std::endl;
    std::cerr << "# ERR: " << e.what();
    std::cerr << " (MySQL error code: " << e.getErrorCode();
    std::cerr << ", SQLState: " << e.getSQLState() << " )" << std::endl;
  }
}
void sqlConnector::insert_in_GAS_INFO(struct db_info *val) {
  static const char *query = "INSERT INTO GAS_INFO VALUES(NULL,CURRENT_TIMESTAMP,\"%s\",%.5f,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f);";
  memset(buff,0,sizeof(buff));
  sprintf(buff,query,val->gasType,val->amb->temp,val->amb->hum,val->amb->pre,val->we,val->ae,val->ppb,val->ppb_c);
  excecuteQuery(buff);
}
void sqlConnector::insert_in_GAS_VALUES(struct db_values *val) {
  static const char *query = "INSERT INTO GAS_VALUES VALUES(NULL,CURRENT_TIMESTAMP,%.5f,%.5f,%.5f,%.5f,\"%s\",%.5f,\"%s\",%.5f,\"%s\",%.5f,\"%s\");";
  memset(buff,0,sizeof(buff));
  sprintf(buff,query,val->amb->temp,val->amb->pre,val->amb->hum,val->gas[0].ppb,val->gas[0].flag,
          val->gas[1].ppb,val->gas[1].flag,val->gas[2].ppb,val->gas[2].flag,val->gas[3].ppb,val->gas[3].flag);
  excecuteQuery(buff);
}

