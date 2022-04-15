// defined std::unique_ptr
#include <memory>
// defines Var and Lit
#include "minisat/core/SolverTypes.h"
// defines Solver
#include "minisat/core/Solver.h"
// defined std::cout
#include <iostream>
// Compile with c++ ece650-a2cpp -std=c++11 -o ece650-a2
#include <sstream>
#include <vector>
#include <regex>
#include <algorithm>
#include <tuple>
#include <queue>
using namespace std;
using Minisat::mkLit;
using Minisat::lbool;

class Graph {
    
public:
    int V;
    vector<int> *adj;
    vector<int> edgeVector;


    Graph(int vertex){
        this->V = vertex;
        adj = new vector<int>[++vertex];
    }

	~Graph(){
        adj->clear();
        adj->shrink_to_fit();
    }

    void vertices(int vertex) {
		adj->clear();
        adj->shrink_to_fit();
        this->V = vertex;
        adj = new vector<int>[++vertex];
    }

    int num_of_vertices() {
        return (this->V);
    }

    int getVertexRange(){
        return (this->V);
    };

    void changeVertexRange(int newVertexRange){
        this->V=newVertexRange;
    };

    vector<int> get_vertex_vector(){
        vector<int> vertex_vector;
        for(int i = 1; i<=this->V; ++i){
            vertex_vector.push_back(i);
        }
        return vertex_vector;
    }
    vector<int> getEdgeVector(){
        return edgeVector;
    };

    void setEdgeVector(vector<int> e){
        edgeVector = e;
    }
    
    void newEdge(int v1, int v2){
        adj[v1].push_back(v2);
        adj[v2].push_back(v1);
    }

    void shortestPath(int v1, int v2){
        //uses BFS to get the shortest part between v1 and v2
        int previous[this->V];
        int distance[this->V];
		int start = v1;
		int end = v2;
        if (bredth_first_search (v1, v2, previous, distance) == false) {
            cout << "Error: No path exists" <<endl;
            return;
        }
        vector<int> s;
        int ctr = end;
        s.push_back(ctr);
        while(previous[ctr] != -1){
            if(ctr == start) {
                break;
            }
            s.push_back(previous[ctr]);
            ctr = previous[ctr];
        }
        string output = "";
        for (int i = s.size() -1; i > 0; i--){
            output += to_string(s[i]);
            output += "-";
        }
        output += to_string(end);
        cout << output<< endl;
    }

    bool bredth_first_search(int v1, int v2, int previous[], int distance[]) {
        queue<int> bfsq;
        bool visited[this->V];
        for(int i = 0; i < this->V; i++) {
            distance[i] = 2147483647; // max int value
            visited[i] = false;
            previous[i] = -1;
        }

        visited[v1] = true;
        distance[v1] = 0;
        bfsq.push(v1);

        while(!bfsq.empty()) {
            int u = bfsq.front();
            bfsq.pop();
            
            int num_of_nbrs = adj[u].size();
            for(int i = 0; i< num_of_nbrs; i++){
                if (visited[adj[u][i]] == false) {
                    visited[adj[u][i]] = true;
                    //increase distance by 1
                    distance[adj[u][i]] = distance[u] + 1;
                    previous[adj[u][i]] = u;
                    bfsq.push(adj[u][i]);

                    if (adj[u][i] == v2){
                        return true;
                    }
                }
				if (adj[u][i] == v2){
                    visited[adj[u][i]] = true;
                    distance[adj[u][i]] = distance[u] + 1;
                    previous[adj[u][i]] = u;
					bfsq.push(adj[u][i]);
                    return true;
                }
            }
        }
        return false;
    }

    void print(){
        for(int i = 1; i<=V; ++i){
            cout << "V " << i << "->";
            for (auto x: adj[i]){
                cout << x << ",";
            }
            cout << endl;
        }
    }
};


tuple<char, vector<int>> parse_line(string line){
    // Testing what was input
    // cout << "Entered: " <<line << endl;

    const char input_type = line.at(0);
    // we expect each line to contain a list of numbers
    // this vector will store the numbers.
    // they are assumed to be unsigned (i.e., positive)
    vector<int> nums;

    // values contains everything from line apart from 1st char
    string values;
    try{
        values = line.substr(2);
        if (values.length() < 0) {
            // cout << "values: " <<values << endl;
            return make_tuple('0', nums);
        }
    }
    catch (const exception& e){
        cout << "Error: " << e.what() << endl;
        return make_tuple('0', nums);
    }
    

    // search for digits in the string
    smatch m;
    regex e ("\\d+");
    while (regex_search (values,m,e)) {
        for (auto x : m) {
            int num = stoi(x);
            //add number to vector
            nums.push_back(num);
        }
        values = m.suffix().str();
    }
    return make_tuple(input_type, nums);
}

class Cover{
private:     
    Minisat::Solver solver;
    int n;
    std::vector<int> edge;

public: 
    Cover(int n1, std::vector<int> edge_vect){
        int max_it = *max_element(begin(edge_vect), end(edge_vect)); // c++11
        int min_it = *min_element(begin(edge_vect), end(edge_vect));
        if (max_it > n1 || min_it == 0 ){  
            std::cerr << "Error: vertex out of range" << std::endl;
        }
        else {
            n = n1;
            edge = edge_vect;
        }
    }
    vector<int> find_vertex_cover(){
        vector<int> vertex_cover;
        for (int k = 1 ; k < n; ++k){ 
            Minisat::Solver solver;
            Minisat::Lit x[n][k];

            for(int j = 0; j < k; j++){
                for (int i = 1; i <= n; i ++){
                    x[i][j] = Minisat::mkLit(solver.newVar());

                }
            }
            // clause 1
            Minisat::vec<Minisat::Lit> ps;
            for (int i = 0; i <k; i++){
                for (int j = 1; j <= n; j++){
                    ps.push(x[j][i]);
                }
            }
            solver.addClause(ps);

            // clause 2 
            for (int m = 1; m <= n; m++){
                for (int p = 0; p < k; p++ ){
                    for (int q = p+1; q < k; q++){
                        solver.addClause(~x[m][p], ~x[m][q]);
                    }
                }
            }
            // clause 3
            for (int m =0 ; m < k; m++){
                for (int p = 1; p <= n; p ++){
                    for (int q = p +1; q <= n; q++) {
                        solver.addClause(~x[p][m], ~x[q][m]);
                    }
                }
            }
            // clause 4
            for (std::vector<int>::const_iterator i = edge.begin(); i != edge.end(); ++i){
                Minisat::vec<Minisat::Lit> ps;
                int v1 = *i;
                std::advance (i,1);
                int v2 = *i;
                for (int j = 0; j<k; j++){
                    ps.push(x[v1][j]);
                    ps.push(x[v2][j]);
                }
                solver.addClause(ps);
            }
            // print result
            int t = 1;
            if (solver.solve()){
                for (int i =1; i <= n ; i++){
                    for (int j = 0; j < k; j++){
                        if(solver.modelValue(x[i][j])==Minisat::l_True){
                            vertex_cover.push_back(t);
                            // cout <<t<<" ";
                        }
                    }
                    t++;
                }
                // cout << endl;
                sort(vertex_cover.begin(),vertex_cover.end());
                return vertex_cover;
            }
        }
        return vertex_cover;
    }
};

int main(int argc, char** argv) {
    Graph g(1);
    char input_type;
    vector <int> nums;
    //read from stdin until EOF
    while (!cin.eof()) {
         // read a line of input until EOL and store in a string
        string line;
        getline(cin, line);
        if(line.empty()) {
            continue;
        }
        tie(input_type,nums) = parse_line(line);

        if (input_type == '0'){
            cout << "Error: " << endl;
            break;
        }
        if (input_type == 'V'){
            // Vertex with one number in nums
            auto V =  nums.back();
            g.vertices(V);
            //g.printAdjMatrix();
        }
        else if (input_type == 'E'){
            int v1, v2;
            int num_of_vertices = 0;
			num_of_vertices = g.num_of_vertices();
            // Edges with multiple numbers
			int numbers_input = nums.size();
            if (numbers_input < 1){
                cout << endl;
                continue;
            }
            g.setEdgeVector(nums);
            for(int index = 0; index < numbers_input-1; index+=2){
                v1 = nums[index];
                v2 = nums[index+1];
                if((1 <= v1 && v1 <= num_of_vertices) && (1 <= v2 && v2 <= num_of_vertices)) {
                    g.newEdge(v1, v2);
                }
                else{
                    cout << "Error: Invalid Vertex" <<endl;
                }
            }
            Cover s(g.num_of_vertices(),g.getEdgeVector());
	        vector<int> vertex_cover = s.find_vertex_cover();
            for(auto v: vertex_cover){
                cout << v << " ";
            }
            cout << endl;
        }
        else{
            cout << "Error: Invalid command" << endl;
        }
    }
}
