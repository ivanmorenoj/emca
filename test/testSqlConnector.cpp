#include <stdio.h>

#include "sqlConnector.h"

struct ambVariables ambS = {
    .temp = 20,
    .hum = 20,
    .pre = 200
};
struct db_info info = {
    .gasType = (char *)"TST",
    .amb = &ambS,
    .we = 0.200,
    .ae = 0.100,
    .ppb = 0.50,
    .ppb_c = 998.5
};
struct db_values values = {
    .amb = &ambS,
    .gas = {
        {500.5 , (char *)"ND"},
        {600.6 , (char *)"ND"},
        {700.7 , (char *)"ND"},
        {800.8 , (char *)"ND"}            
    }
};

int main(int argc, char const *argv[])
{    
    sqlConnector _db("insert_user","inserp@ss","tcp://127.0.0.1:3306");
    _db.setSchema((char *)"EMCA");
    _db.connect();
    printf("Insert in tables\n");
    _db.insert_in_GAS_INFO(&info);
    _db.insert_in_GAS_VALUES(&values);

    printf("End of the program\n");
    
    return 0;
}
