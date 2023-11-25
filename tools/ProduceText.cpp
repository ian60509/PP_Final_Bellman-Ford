#include<stdio.h>
#include <string>
#include <iostream>
#include <fstream>
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
#define DENSE_RATIO 1

//顏色處理
#define NC "\e[0m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define CYN "\e[0;36m"
#define REDB "\e[41m"


void print_help(const char* binary_name) {
    printf(RED "helper\n" NC);
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
    ofs << "# num edges\n";
    ofs << num_e <<"\n" ;
    
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