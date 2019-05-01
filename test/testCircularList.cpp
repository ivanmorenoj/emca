#include <stdio.h>
#include "CircularList.h"
#include "CircularList.cpp"
#include "MCP3424.h"

struct MCP3424Config tst[10] = {
    {0,0,0,0,0},
    {1,1,1,1,1},
    {2,2,2,2,2},
    {3,3,3,3,3},
    {4,4,4,4,4},
    {5,5,5,5,5},
    {6,6,6,6,6},
    {7,7,7,7,7},
    {8,8,8,8,8},
    {9,9,9,9,9}
};
int main() {
    CircularList<MCP3424Config> list;
    
    for(int i = 0; i<10 ;++i)
        list.addNode(tst+i);

    printf("Length of <list> %d: \n",list.getLength());
    
    MCP3424Config *tmp;
    for (int i = 0; i < 25 ; ++i) {
        tmp = list.getCurrentData();
        printf("%d - %d - %d - %d - %d\n",
            tmp->address,tmp->bitrate,tmp->gain,
            tmp->conv_mode,tmp->channel);
        list.next();
    }

    return 0;
}
