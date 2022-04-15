#include <iostream>
#include <random>
#include <map>
#include <climits>
#include <set>
#include <tuple>
#include <unistd.h>
#include <chrono>
#include <thread>
#include <tuple>
using namespace std;

struct Point {
    int x;
    int y;
    Point(){
        // default values that dont matter
        this->x = INT_MAX;
        this->y = INT_MAX;
    }
    Point(int x, int y) {
        this->x = x;
        this->y = y;
    }
    tuple<int,int> get_points(){
        return make_tuple(this->x, this->y);
    }
    int get_x(){
        return this->x;
    }
    int get_y(){
        return this->y;
    }
    string get_string(){
        string s = "(" + to_string(this->x) +","+ to_string(this->y) + ")";
        return s;
    }
};
// Point p(1, 3);
// int x, y;
// tie(x, y) = p.get_points();
// cout << x << " " << y << endl;

struct Line{
    Point start;
    Point end;

    Line(){
        // default values that dont matter
        Point st(INT_MAX,INT_MAX);
        Point en(INT_MAX,INT_MAX);
        this->start = st;
        this->end = en;
    }
    Line(Point st, Point en){
        this->start = st;
        this->end = en;
    }
    tuple<Point,Point> get_start_end(){
        return make_tuple(this->start, this->end);
    }
    Point get_start(){
        return this->start;
    }
    Point get_end(){
        return this->end;
    }
    string get_string(){
        // '['+ str(self.src) + '-->' + str(self.dst) + ']'
        string s = "[" + this->start.get_string() + "-->" + this->end.get_string() + "]";
        return s;
    }
};
// Point st(0, 0);
// Point en(1,1);
// Line l(st, en);
// Point p1, p2;
// tie(p1, p2) = l.get_start_end();
// int x, y;
// tie(x, y) = p1.get_points();
// cout << x << " " << y << endl;
// tie(x, y) = p2.get_points();
// cout << x << " " << y << endl;

struct Street {
    string name;
    vector<Line> lines;
    vector<Point> points;
    
    Street(){
        this->name = "unnamed street";
        this->lines = {};
        this->points = {};
    }

    Street(string name){
        this->name = name;
        this->lines = {};
        this->points = {};
    }

    Street(string name, vector<Line> lines){
        this->name = name;
        this->lines = lines;
    }

    Street(string name, vector<Line> lines, vector<Point> points){
        this->name = name;
        this->lines = lines;
        this->points = points;
    }

    ~Street(){
        this->lines.clear();
        this->lines.shrink_to_fit();

        this->points.clear();
        this->points.shrink_to_fit();
    }

    string get_name(){
        return this->name;
    }

    vector<Line> get_lines(){
        return this->lines;
    }

    void add_line(Line l){
        this->lines.push_back(l);
    }

    void add_point(Point p){
        this->points.push_back(p);
    }

    string get_string(){
        string s = "\"" + this->name + "\" " ;
        for (auto a: this->points){
            s += a.get_string() + " ";
        }
        s.pop_back();
        return s;
    }
};

int ran(int start, int end){
    /*
        Get random number between start and end
    */
    uniform_int_distribution<int> d(start, end);
    random_device rd1("/dev/urandom");
    return d(rd1);
}

string get_randm_st_name(int n){
    /*
        n - len of st name ie. Street "1...n"
    */
    char letters[26] = { 'a', 'b', 'c', 'd', 'e', 'f', 'g',
                            'h', 'i', 'j', 'k', 'l', 'm', 'n',
                            'o', 'p', 'q', 'r', 's', 't', 'u',
                            'v', 'w', 'x', 'y', 'z' };
    
    string st_name = "Street ";
    for (int i = 1; i <= n; i++) {
        st_name = st_name + letters[rand() % 26];
    }
    return st_name;
}



bool overlap(Line l1, Line l2){
    // lets get the formula of the line
    int x1 = l1.start.x;
    int y1 = l1.start.y;
    int x2 = l1.end.x;
    int y2 = l1.end.y;

    int x3 = l2.start.x;
    int y3 = l2.start.y;
    int x4 = l2.end.x;
    int y4 = l2.end.y;

    Point p1 = l1.start;
    Point p2 = l1.end;
    Point p3 = l2.start;
    Point p4 = l2.end;

    if(p1.get_string() == p3.get_string() || p1.get_string() == p4.get_string()){
        return false;
    }
    if(p2.get_string() == p3.get_string() || p2.get_string() == p4.get_string()){
        return false;
    }
    
    float m1_num = y2-y1;
    float m1_den = x2-x1;
    float m2_num = y4-y3;
    float m2_den = x4-x3; 

    // m -> slope
    if((m1_den == 0) || (m2_den == 0)){
        return false;
    }
    float m1 = m1_num/m1_den;
    float m2 = m2_num/m2_den;

    // for lines to overlap, their slopes must be the same
    if(m1 != m2) {
        return false;
    }

    // c -> y intercept
    float c1 = y1 - m1*(x1);
    float c2 = y3 - m2*(x3);

    if (c1 != c2) {
        return false;
    }

    // mx_n + c - y_n == 0
    // does either of the point from l2 satisfy the l1's equation?
    if (((m1*x3) + c1 - y3) == 0) {
        //x3 and y3 satisfy L1's equation
        if(((x3 > x1) && (x3 < x2)) && ((y3 > y1) && (y3 < y2)) || ((x3 < x1) && (x3 > x2)) && ((y3 < y1) && (y3 > y2))){
            //cout << "overlap found: " << l1.get_string() << " and " << l2.get_string() << endl;
            return true;
        }
    }
    if (((m1*x4) + c1 - y4) == 0) {
        //x4 and y4 satisfy L1's equation
        if(((x4 > x1) && (x4 < x2)) && ((y4 > y1) && (y4 < y2)) || ((x4 < x1) && (x4 > x2)) && ((y4 < y1) && (y4 > y2))){
            //cout << "overlap found: " << l1.get_string() << " and " << l2.get_string() << endl;
            return true;
        }
    }
    if (((m2*x1) + c2 - y1) == 0) {
        //x1 and y2 satisfy L2's equation
        if(((x1 > x3) && (x1 < x4)) && ((y1 > y3) && (y1 < y4)) || ((x1 < x3) && (x1 > x4)) && ((y1 < y3) && (y1 > y4))){
            //cout << "overlap found: " << l1.get_string() << " and " << l2.get_string() << endl;
            return true;
        }
    }
    if (((m2*x2) + c1 - y2) == 0) {
        //x2 and y2 satisfy L2's equation
        if(((x2 > x3) && (x2 < x4)) && ((y2 > y3) && (y2 < y4)) || ((x2 < x3) && (x2 > x4)) && ((y2 < y3) && (y2 > y4))){
            //cout << "overlap found: " << l1.get_string() << " and " << l2.get_string() << endl;
            return true;
        }
    }
    return false;
}

bool intersect(Line l1, Line l2){
    int x1 = l1.start.x;
    int y1 = l1.start.y;
    int x2 = l1.end.x;
    int y2 = l1.end.y;

    int x3 = l2.start.x;
    int y3 = l2.start.y;
    int x4 = l2.end.x;
    int y4 = l2.end.y;

    double xnum = (((x1*y2)-(y1*x2))*(x3-x4) - (x1-x2)*((x3*y4)-(y3*x4)));
    double xden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4));

    double ynum = ((x1*y2) - (y1*x2))*(y3-y4) - (y1-y2)*((x3*y4)-(y3*x4));
    double yden = (x1-x2)*(y3-y4) - (y1-y2)*(x3-x4);

    double tnum = ((x1-x3)*(y3-y4) - (y1-y3)*(x3-x4));
    double tden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4));

    double unum = ((x1-x3)*(y1-y2) - (y1-y3)*(x1-x2));
    double uden = ((x1-x2)*(y3-y4) - (y1-y2)*(x3-x4));

    double xcoor, ycoor, t, u;
    try{
        if(xden == 0){
            return false;
        }
        xcoor = xnum/xden;
    }
    catch(const exception& e){
        //cout << &e << endl;
        return false;
    }
    try{
        if(yden == 0){
            return false;
        }
        ycoor = ynum/yden;
    }
    catch(const exception& e){
        //cout << &e << endl;
        return false;
    }
    try{
        if(tden == 0){
            return false;
        }
        t = tnum / tden;
    }
    catch(const exception& e){
        //cout << &e << endl;
        return false;
    }
    try{
        if(uden == 0){
            return false;
        }
        u = unum / uden;
    }
    catch(const exception& e){
        //cout << &e << endl;
        return false;
    }
  
    // cout << "xcoor: " << xcoor <<endl;
    // cout << "ycoor: " << ycoor <<endl;
    // cout << "t: " << t <<endl;
    // cout << "u: " << u <<endl;

    if ((t >= 0.0 && t <= 1.0) && (u >= 0.0 && u <= 1.0)){
        //cout << to_string(xcoor) << " , " << to_string(ycoor) << endl;
        //cout << "t, u: " << t << ", " << u << endl;
        //cout << l1.end.get_string() << endl;
        double cmn_x = l1.end.x;
        double cmn_y = l1.end.y;
        //cout << to_string(cmn_x) << " , " << to_string(cmn_y) << endl;
        if ((cmn_x == xcoor) && (cmn_y == ycoor)){
            // cout << "intersect is on common point" << endl;
            return false;
        }
        // if(int_point.get_string() == l1.end.get_string()){
        //     cout << "Comparing  " << int_point.get_string() << " and " << l1.end.get_string() << endl;
        //     return false;
        // }
        return true;
    }
    else{
        return false;
    }
}

bool is_valid_line(vector<Street> valid_streets, vector<Line> valid_lines, vector<Point> points, Line l){
    /*
        The following critera makes it a valid street:
        1. no overlapping line segments (with any other valid_street)
        2. no zero length lines
        3. not forming a loop
        4. not intersecting itself
    */
    //cout << "checking line: " << l.get_string() << endl;
    Point src = l.start;
    Point dst = l.end;

    // check if the length is > 1
    int diff_x = src.x - dst.x;
    int diff_y = src.y - dst.y;
    if(diff_x == 0 && diff_y == 0){
        return false;
    }
    //cout << "line's len is gt 0" << endl;
    /*
        Checking duplicate points, also, no loops
    */
    //cout << "comparing point " << dst.get_string() << " to ";
    for (int i = points.size() -1; i >= 0; i--){
        if (dst.get_string() == points[i].get_string()){
            return false;
        }
        //cout << points[i].get_string();
    }
    //cout << "New point is unique" << endl;

    // cout << "Current points: ";
    // for(auto pt: points){
    //     cout << pt.get_string();
    // }
    // cout << endl;
    /*
        check if there is an intersection between new line and existing lines 
    */
    for(Line val_li: valid_lines){
        //cout << "comparing " << val_li.get_string() << " and " << l.get_string() << endl;
        if (intersect(val_li, l)){
            //cout << "Found an intersection" << endl;
            return false;
        }
        if (overlap(val_li, l)){
            return false;
        }
    }
    //cout << "Done looking for intersections" <<endl;

    /*
        compare line to each line in each street, if exact line exists, return false
        then it checks for overlapping lines
    */
    for (auto st: valid_streets){
        for(auto st_l: st.lines){
            if (l.get_string() == st_l.get_string()){
                //cout << "line " << l.get_string() << " exists in street " << st.name <<endl;
                return false;
            }
            if (overlap(l, st_l)){
                return false;
            }
        }
    }
    //cout << "No exact line exists in st database" << endl;

    return true;
}

bool is_valid_street(vector<Street> valid_streets, Street st){
   return true;
}

Line create_line(int c){
    int x1 = ran(-c, c);
    int y1 = ran(-c, c);
    int x2 = ran(-c, c);
    int y2 = ran(-c, c);
    Point p1(x1, y1);
    Point p2(x2, y2);
    Line l(p1, p2);
    return l;
}

Line create_line(Point src, int c){
    int x1 = src.x;
    int y1 = src.y;
    int x2 = ran(-c, c);
    int y2 = ran(-c, c);
    Point dst(x2, y2);
    Line l(src, dst);
    return l;
}

Street create_street(vector<Street> valid_streets, string st_name, int n, int c){
    int num_of_lines = ran(1, n);
    //cout << "Number of lines: " << num_of_lines << endl;
    vector<Line> lines;
    vector<Point> points;
    int attempts;
    bool is_first_line = true;
    //number of lines we need to generate
    for(int i = 1; i <= num_of_lines; ++i){
        attempts = 0;
        while (attempts <= 25){
            //try to create a valid line in 25 attempts
            attempts++;
            Line l;
            if(lines.size()>0){
                Point src = lines.back().end;
                l = create_line(src, c);
                is_first_line = false;
            }
            else{
                is_first_line = true;
                l = create_line(c);
            }
            if(is_valid_line(valid_streets, lines, points, l)){
                lines.push_back(l);
                if(is_first_line){
                    points.push_back(l.start);
                    points.push_back(l.end);
                }
                else{
                    points.push_back(l.end);
                }
                break;
            }
            if(attempts >= 25) {
                cout << "Error: failed to generate valid input for 25 simultaneous attempts" <<endl;
                exit(0);
            }
        }
    }
    if(lines.size() == num_of_lines){
        Street s(st_name, lines, points);
        // cout << "add " << s.get_string() << endl;
        return s;
    }
    else{
        Street s;
        return s;
    }
}

tuple<vector<Street>, int> generate(map<char, int> input_char_int, vector<Street> valid_streets){
    /* 
        Returns: l so main can use it to sleep for l sec
        Default values:
        s: no. of streets ran[2,s]
        n: no. of line segments per street ran[1, n]
        l: no. of seconds to wait ran[5, l]
        c: x,y coordinates both in ran[-c, c]
    */
    int s = 10, n = 5, l = 5, c = 20;

    // Print values in input_char_int map
    // cout << elem.first << " " << elem.second << "\n";

    // update arg values if passed
     for(const auto& elem : input_char_int){
        char first = elem.first;
        if(elem.first == 's'){
            s = elem.second;
            if(elem.second < 2){
                cout << "Error: Invalid value for arg s" << endl;
                exit(1);
            }
        }
        else if(elem.first == 'n'){
            n = elem.second;
            if(elem.second < 1){
                cout << "Error: Invalid value for arg n" << endl;
                exit(1);
            }
        }
        else if(elem.first == 'l'){
            l = elem.second;
            if(elem.second < 5){
                cout << "Error: Invalid value for arg l" << endl;
                exit(1);
            }
        }
        else if(elem.first == 'c'){
            c = elem.second;
            if(elem.second < 1){
                cout << "Error: Invalid value for arg c" << endl;
                exit(1);
            }
        }
        else{
            // do nothing
        }
    }
    // cout << "s: " << s << endl;
    // cout << "n: " << n << endl;
    // cout << "l: " << l << endl;
    // cout << "c: " << c << endl;
    // first get a random number of streets
    int num_of_st = ran(2, s);

    // vector of valid streets
    // vector<Street> valid_streets;


    //cout << "number of streets " << num_of_st <<endl;

    string st_name;
    int attempt = 0;
    // this vector will store valid streets
    while(valid_streets.size() < num_of_st){
        attempt++;
        st_name = get_randm_st_name(valid_streets.size()+1);
        //cout << "Adding new st: " << st_name << endl;
        Street st = create_street(valid_streets, st_name, n, c);
        if(st.name != "unnamed street"){
            if(is_valid_street(valid_streets, st)){
                valid_streets.push_back(st);
                attempt = 0;
                continue;
            }
        }
        if(attempt >= 25){
            cout << "Error: failed to generate valid input for 25 simultaneous attempts" <<endl;
            exit(1);
        }
        //this_thread::sleep_for(chrono::seconds(l));
    }

    for(auto st: valid_streets) {
        cout << "add " << st.get_string() << endl;
    }
    cout << "gg" << endl;
    return make_tuple(valid_streets, l);
}

int main(int argc, char **argv)
{
    const set<char> VALID_CMD = {'s','n','l','c'}; 
    map<char, int> input_char_int;
    vector<Street> valid_streets;

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
    int l = 5;
    while (!cin.eof()) {
        for(auto va: valid_streets){
            cout << "rm \"" << va.name << "\"" << endl;
        }
        valid_streets.clear();
        tie(valid_streets, l) = generate(input_char_int, valid_streets);
        sleep(l);
    }
    //tie(valid_streets, l) = generate(input_char_int, valid_streets);
}
