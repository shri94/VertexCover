#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
using namespace std;


int main (int argc, char **argv) {
    const set<char> VALID_CMD = {'s','n','l','c'}; 
    map<char, int> input_char_int;
    /*  
        Get command line args starting 1.
        Iterates over 2 at a time so we have s 5, l 3 etc.
        Stores valid inputs in map  
    */
    for(int i = 1; i < argc-1; i+=2)
    {
        //cout << argv[i] << " " << argv[i+1] <<endl;
        string a = argv[i];
        char input_type = a.back();
        if(VALID_CMD.find(input_type)!= VALID_CMD.end()){
            string b = argv[i+1];
            int num = stoi(b);
            input_char_int.insert(pair<char, int>(input_type, (int)num));
        }
    }

    // Default values
    int s = 10, n = 5, l = 5, c = 20;

    // update arg values if passed
    for(const auto& elem : input_char_int){
        if(elem.first == 's'){
            s = elem.second;
            if(elem.second < 2){
                cerr << "Error: Invalid value for arg s" << endl;
                exit(1);
            }
        }
        else if(elem.first == 'n'){
            n = elem.second;
            if(elem.second < 1){
                cerr << "Error: Invalid value for arg n" << endl;
                exit(1);
            }
        }
        else if(elem.first == 'l'){
            l = elem.second;
            if(elem.second < 5){
                cerr << "Error: Invalid value for arg l" << endl;
                exit(1);
            }
        }
        else if(elem.first == 'c'){
            c = elem.second;
            if(elem.second < 1){
                cerr << "Error: Invalid value for arg c" << endl;
                exit(1);
            }
        }
        else{
            // do nothing
        }
    }

    vector<pid_t> kids;

    int rgen_to_a1[2];
    int p1_status = pipe(rgen_to_a1);

    if(p1_status == -1)
  	{
	  cerr<<"Error:creating the rgen_to_a1"<<endl;
	  exit(1);  		
  	}

  
    pid_t rgen;
    rgen = fork();

    if(rgen == 0) {
        //cout << "[child] exec rgen.." << std::endl;
        dup2(rgen_to_a1[1], STDOUT_FILENO);
        close(rgen_to_a1[0]);
        close(rgen_to_a1[1]);
        //close(rgen_to_a1[1]);
        execv("./rgen", argv);
        perror("Error from arie");
        exit(1);
    } else if (rgen < 0) {
        cout << "Error: could not fork rgen\n";
        exit(1);
    }
    kids.push_back(rgen);
  
    int a1_to_a2[2];
    int p2_status = pipe(a1_to_a2);

    if(p2_status == -1)
  	{
	  cerr<<"Error:creating the a1_to_a2"<<endl;
	  exit(1);  		
  	}

    pid_t a1;
    a1 = fork();
    if(a1 == 0) {
        //cout << "[child] exec py.." << std::endl;
        dup2(rgen_to_a1[0], STDIN_FILENO);
        dup2(a1_to_a2[1], STDOUT_FILENO);
        close(rgen_to_a1[0]);
        close(rgen_to_a1[1]);
        close(a1_to_a2[0]);
        close(a1_to_a2[1]);
        char* pyArg[3];
        pyArg[0]=(char*)"/usr/bin/python3";
        pyArg[1]=(char*)"ece650-a1.py";
        pyArg[2]= nullptr;
        execv("/usr/bin/python3",pyArg);
        cerr<<"Error:Exec of a1!\n";
        // execv("./ece650-a1.py", {});
        perror("Error: from arie");
        exit(1);
    } else if (a1 < 0) {
        cout << "Error: could not fork py\n";
        exit(1);
    }
    kids.push_back(a1);

    close(rgen_to_a1[0]);
	close(rgen_to_a1[1]);

    pid_t a2;
    a2 = fork();
    if(a2 == 0) {
        //cout << "[child] exec A2.." << std::endl;
        dup2(a1_to_a2[0], STDIN_FILENO);
        close(a1_to_a2[0]);
        close(a1_to_a2[1]);
        execv("./ece650-a2", {});
        
        perror("Error from arie");
        exit(1);
    } else if (a1 < 0) {
        cout << "Error: could not fork py\n";
        exit(1);
    }

    dup2(a1_to_a2[1],STDOUT_FILENO);
  	close(a1_to_a2[0]);
  	close(a1_to_a2[1]);

    while (!std::cin.eof()) {
        // read a line of input until EOL and store in a string
        std::string line;
        std::getline(std::cin, line);
        if (line.size () > 0)
            std::cout << line << std::endl;
    }

    kids.push_back(a2);
    // int res1;
    // std::cout << "[exec] waiting for child process to terminate" << std::endl;
    // waitpid(child_pid, &res1, 0);
    // kids.push_back(child_pid);
    // child_pid = 0;

    // //redirect stdout to the pipe
    // dup2(rgen_to_a1[1], STDOUT_FILENO);
    // close(rgen_to_a1[0]);
    // close(rgen_to_a1[1]);
    // waitpid(-1, NULL, 0);

    // send kill signal to all children
    for (pid_t k : kids) {
        int status;
        kill(k, SIGTERM);
        waitpid(k, &status, 0);
    }

    return 0;
}
