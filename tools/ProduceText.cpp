#include<stdio.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <vector>
#include <algorithm>

#define DEBUG 0

using namespace std;


std::string default_filename = "default.graph";
int default_num_v = 100;
int default_num_e = 100;

enum _mode {
    NO_SET ,
    SPARSE ,
    DENSE
};

// vertice/edge 的比率
#define DENSE_RATIO 0

//顏色處理
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define CYN "\e[0;36m"
#define REDB "\e[41m"

vector<int> random_pick_n(int n, int num_v, int avoid){ // random choose n numbers from |V| vertex (不重複)
    int arr[num_v] ;
    vector<int> rand_pick_set ;
    for (int i=0; i<num_v; i++){
        arr[i] = i;
    }
    // printf("choose %d-number from %d node, avoid:%d\n", n, num_v, avoid);、
    srand(5);
    for(int i=0; i<n; i++){
        /* 指定亂數範圍 */
        int min = i;
        int max = num_v-1;

        /* 產生 [min , max] 的整數亂數 */
        int pick = -1;
        do{
            pick = rand() % (max - min + 1) + min;
            
        }while(arr[pick]==avoid); //避免選到avoid
        
        #if (DEBUG==1)
                // printf("pick arr[%d]:%d\n",pick, arr[pick] );
        #endif
        int tmp = arr[i];
        arr[i] = arr[pick];
        arr[pick] = tmp;
        rand_pick_set.push_back(arr[i]);
    }
    return rand_pick_set;
}

void print_help(const char* binary_name) {
    printf(RED "helper\n" NC);
}

int* make_edge_start(const int num_e, const int num_v){
    int* edge_start = (int*)malloc(sizeof(int)*num_v);
    int* num_edges_of_v = (int*)malloc(sizeof(int)*num_v);
    memset(edge_start, 0, num_v);
    memset(num_edges_of_v, 0, num_v);
    srand(5); // set random seed

    int e_quota = num_e; // 還有多少quota的edge可以分配
    for (int i=0; i<num_v; i++){
        int left_v = num_v - i -1 ; //如果配給完現在這顆剩餘還沒有配給edge的vertex數量
        int max = (e_quota>num_v-1)? num_v-1 : e_quota; //每個點最多只能有 |V|-1 個 edges
        int min = 0;
        int n = rand() % (max - min + 1) + min;

        if(left_v == 0){
            n = e_quota;
        }else if(e_quota - n > (left_v * (left_v-1) + left_v)){ // 如果這個edge只分配給目前的n個，會讓接下來edge > (剩下節點的complete subgraph + 剩餘節點都有連到這顆) => edge分配不完
            n = num_v-1; // 直接給爆，給到最滿
        }
        num_edges_of_v[i] = n;
        #if (DEBUG==1)
            printf("%d-th node: has %d edges, e_quota = %d\n",i,  num_edges_of_v[i], e_quota);
        #endif

        e_quota -= num_edges_of_v[i];
    }
    assert(e_quota==0); //如果不等於0代表還有edge沒有分配完
    
    edge_start[0] = 0;
    for (int i=1; i<num_v; i++){
        edge_start[i] = edge_start[i-1] + num_edges_of_v[i-1];
        
    }
    free(num_edges_of_v);
    return edge_start;
}

int set_outgoing_edge_of_a_vertex(const int v_index,const int num_v, const int total_edges,  int* edge_start, int* outgoing_edges){

    int num_edge = 0;
    if(v_index == 0){
        num_edge = edge_start[1];
    }else if(v_index == num_v-1) { //最後一個點，沒有edge_start[v_index+1]
        
        num_edge = (total_edges) - edge_start[v_index];
    }
    else{
        num_edge = edge_start[v_index+1] - edge_start[v_index];
    }

    

    vector<int> pick_set = random_pick_n(num_edge, num_v, v_index); // choose a set of vertex
    sort(pick_set.begin(), pick_set.end());

    #if (DEBUG == 1)
        printf("I'm %d-th vertex, I have %d edges", v_index, num_edge);
        printf("  the outgoing edge size = %ld\n", pick_set.size());
    #endif

    int i=0;
    for (vector<int>::iterator it = pick_set.begin();  it != pick_set.end(); ++it) {
        outgoing_edges[i] = *it;
        i++;
    }

    return num_edge;
}

int main(int argc, char** argv){
    
   
   
    std::ofstream ofs;
    std:string file_name;
    int mode = NO_SET;

    if (argc == 1) {
        file_name = default_filename;
    }else if(argc == 2){
        file_name = std::string(argv[1]) + ".graph";
    }


    ofs.open(file_name);
    if (!ofs.is_open()) {
        cout << "Failed to open file.\n";
        exit(1); // EXIT_FAILURE
    }

    ofs << "AdjacencyGraph " <<  "\n";
    //get number of vertices
    int num_v = default_num_v;
    printf(GRN "Please input number of vertices you want\n " NC);
    scanf("%d", &num_v);
    ofs << "# num vertices\n";
    ofs << num_v <<"\n" ;

    //get mode
    printf(GRN "Please input mode you want\n \tSKIP: type \'0 \' , \tsparse: type \'1 \'  \tdense: type \'2 \' \n" NC);
    scanf("%d", &mode);

    //get number of edges
    int num_e = default_num_e;
    printf(GRN "Please input number of edges you want\n " NC);
    scanf("%d", &num_e);
    if(num_e> num_v*(num_v - 1)){
        num_e = num_v*(num_v - 1);
        printf(RED "Because the max number of edges = %d in a |V|=%d graph, so we set the edge numbers = %d\n" NC, num_e, num_v, num_e);
    }
    ofs << "# num edges\n";
    ofs << num_e <<"\n" ;
    #if (DEBUG == 1)
        printf("number of total edges = %d\n", num_e);
    #endif

    //get edge starts
    int *edge_start = (int*)malloc(sizeof(int) * num_v);
    edge_start = make_edge_start(num_e, num_v);
    ofs << "# edge starts\n";
    ofs << edge_start[0] ;
    for(int i=1; i<num_v; i++){
        ofs << " " << edge_start[i] ;
    }
    ofs << "\n";

    //make each outgoing edges
    int** outgoing_edges_2D = new int*[num_v];

    for (int i = 0; i < num_v; ++i) {
        outgoing_edges_2D [i] = new int[num_v];
    }

    ofs << "# all the outgoing edges (target vertex)" <<"\n";
    for(int i=0; i<num_v; i++){
        // printf("set %d-th node outgoing edges\n", i);
        int num_e_this_node = set_outgoing_edge_of_a_vertex(i, num_v, num_e, edge_start, outgoing_edges_2D[i]);
        if(num_e_this_node>=1) ofs << outgoing_edges_2D[i][0];
        for(int j=1; j<num_e_this_node; j++){
            ofs << " "<<outgoing_edges_2D[i][j];
        }
        ofs << "\n";
    }

    
    ofs.close();
    return 0;

    
}

/*
AdjacencyGraph
# num vertices
5
# num edges
8
# edge starts
0 4 6 7 8
# all the outgoing edges (target vertex)
1 2 3 4
2 3
0
0

*/