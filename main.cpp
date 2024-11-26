#include <iostream>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <stdlib.h>
#include <string>
#include <sys/wait.h>
#include <vector>
#include <bits/stdc++.h>
#include <dirent.h>
#include <sys/types.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#define PARTSCSV "Parts.csv"
#define MAX 10000
#define READ 0
#define WRITE 1
#define PRICE 2
#define STATE 3
using namespace std;


vector<string>
get_store_names(string path)
{
    vector <string> files;
    vector <string> store_names;
    DIR *dir;
    struct dirent *en;
    char* temp[MAX];
    dir = opendir(path.c_str());
    if (dir) {
        while ((en = readdir(dir)) != NULL) {
            files.push_back(en->d_name);
        }
        closedir(dir);
    }
    int count = 0;
    for (int i = 0; i< files.size(); i++)
    {
        if ( files[i] != "." && files[i] != ".." && files[i] != PARTSCSV)
        {
            store_names.push_back(files[i]);
        }
    }
    return store_names;
}
vector<string>
seperate(string row,char delimiter)
{
    stringstream sStream(row); 
    string productName; 
    vector <string> str;
    while (getline(sStream, productName, delimiter)) {    
        str.push_back(productName);
    }
    return str; 
}
vector<string>
read_parts_csv(string path)
{
    fstream fin;
    string line;

    fin.open(path + "/" + PARTSCSV, ios::in);

    getline(fin, line);
    return seperate(line,',');
}
void show_products(vector<string>products){
    int size = products.size();
    cout<<"Welcome dear customer! Please select the product that you want from the list above:"<< endl ;
    for (int i = 0 ; i < size ; i++){
        cout<< i+1 << ". " << products.at(i) << endl;
    }
    cout<< "do it with this format:"<<endl<<"\033[34m" << " <product_name> <amount>" << endl;
    return;
}
void make_paths(vector <string>& stores,string path)
{
    for(int i=0; i< stores.size(); i++)
    {
        stores[i] = path + "/" + stores[i];
    }
    return; 
}
vector<string> get_chosen_product(){
    string line;
    getline(cin,line);
    vector<string> product = seperate(line,' ');
    return product;
}
int create_process(int& write_pipe, int& read_pipe, string executable)
{
    int pipe_fd[2];
    int pipe_fd2[2];
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
    }
    if (pipe(pipe_fd2) == -1) {
        perror("pipe");
    }
    cout << "\033[31m";
    cout << "Unnamed pipe created!" << endl;
    fflush(stdout);
    int pid = fork();
    if (pid == -1) {
        perror("fork");
    }
    cout << "\033[34m";
    cout << "Child process created!" << endl;
    fflush(stdout);
    if (pid == 0) {
        // Child process
        //dup2(pipe_fd[READ], STDIN_FILENO);
        //cout << executable;
        //fflush(stdout);
        close(pipe_fd[WRITE]);
        close(pipe_fd2[READ]);
        //close(pipe_fd[READ]);
        char read_fd[20];
        char write_fd[20];
        sprintf(read_fd , "%d" , pipe_fd[READ]);
        sprintf(write_fd , "%d" , pipe_fd2[WRITE]);
        execl(executable.c_str(), read_fd , write_fd, NULL);
        perror("execl");
    } else if (pid > 0) {
        // Parent process
        close(pipe_fd[READ]);
        close(pipe_fd2[WRITE]);
        read_pipe = pipe_fd2[READ] ;
        write_pipe = pipe_fd[WRITE];
    }else{
        perror("fork");
    }
    return pid;
}
string store_data(string store, string product,int id)
{
    store = store + "-" + product + "-" + to_string(id);
    return store;
}
void create_buildings_process(const vector<string>& stores, string product,vector <int>& child_pids)
{
    for (int i = 0; i < stores.size(); i++)
    {
        int write_pipe;
        int read_pipe;
        int pid = create_process(write_pipe,read_pipe,"./stores.out");
        string data = store_data(stores[i],product,i);
        write(write_pipe, data.c_str(), data.length());
        cout << "store data sent" << endl;
        fflush(stdout);
        child_pids.push_back(pid);
        close(write_pipe);
        char buf[10240];
        read(read_pipe, buf, 10240);
        cout << "Results sent to main!" << endl;
        cout << "\033[0m";
        fflush(stdout);
        close(read_pipe);
        printf("%s\n",buf);
        fflush(stdout);
        
    }
}
int
main(int argc, char* argv[]){
    vector <int> children_pids;
    //string path = argv[1];
    string path = "./stores";
    vector<string> products = read_parts_csv(path);
    show_products(products);
    vector<string>stores = get_store_names(path);
    vector<string> chosen_product = get_chosen_product();
    string product_name = chosen_product[0];
    int product_amount = stoi(chosen_product[1]);
    make_paths(stores,path);
    int i = 0;
    return 0;
}
