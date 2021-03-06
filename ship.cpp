#include "ship.h"
using namespace std;



Ship::Ship(){
    grid =
    {                                   // ij (row, column)
        {new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED")},      // 00 01 02 03 04 05
        {new Container(0, "UNUSED"),new Container(5, "UNUSED"),new Container(0, "UNUSED"),new Container(8, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(9, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED"),new Container(0, "UNUSED")}      // 10 11 12 13 14 15
        // {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},      // 20 21 22 23 24 25
        // {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},      // 30 31 32 33 34 35
        // {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},      // 40 41 42 43 44 45
        // {10,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},      // 50 51 52 53 54 55
        // {20,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1},      // 60 61 62 63 64 65
        // {32,12,-1,-1,10,6,4,-1,18,13,65,-1}    // 70 71 72 73 74 75
    };
    setUniqueKey();
    fN = 1;
    parent = NULL;
    //calculate_hn();
}

Ship::Ship(vector<vector<Container*>> p){
    grid = p;
    parent = NULL;
    setUniqueKey();
    calculate_hn();
}

Ship::Ship(Ship* p){
    grid = p->grid;
    parent = p;
    gN = p->gN + 1;
    calculate_hn();
    setUniqueKey();
}

Ship::Ship(Ship* p, int fyou){
    grid = p->grid;
    gN = p->gN;
    setUniqueKey();
    calculate_hn();
    parent = NULL;
}

void Ship::print(){
    for(int i =0; i < grid.size(); ++i ){
        for(int j = 0; j < grid.at(0).size(); j++){
            cout << grid.at(i).at(j)->weight << '\t';
        }
        cout << endl;
    }
    cout << endl;
}

double Ship::find_mass_left(){
    double sum = 0.0;
    for(int i = 0; i < grid.size(); i++){
        for(int j = 0; j < grid.at(0).size()/2; ++j){
            if(grid.at(i).at(j)->weight > 0){
                sum = sum + grid.at(i).at(j)->weight;
            }
        }
    }
    return sum;
}

double Ship::find_mass_right(){
    double sum = 0.0;
    for(int i = 0; i < grid.size(); i++){
        for(int j = grid.at(0).size()/2; j < grid.at(0).size(); ++j){
            if(grid.at(i).at(j)->weight > 0){
                sum = sum + grid.at(i).at(j)->weight;
            }
        }
    }
    return sum;
}

double Ship::balance_mass(){
    return (find_mass_left() + find_mass_right())/2.0;
}

double Ship::deficit(){ // Balance Mass - smaller mass side
    double left = find_mass_left();
    double right = find_mass_right();
    double bal_mass = balance_mass();
    double def = 0.0;
    if(left > right){
        def = bal_mass - right;
    }
    else{
        def = bal_mass - left;
    }

    return def;
}

void Ship::setUniqueKey(){
    double lock = 1.0;
    double key = 0.0;

    for(int i = 0; i < grid.size(); ++i){
        vector<Container*> row = grid.at(i);
        for(int j = 0; j < row.size(); ++j){
            if(row.at(j)->weight > 0){
                key += sqrt(abs(row.at(j)->weight * lock));
                lock *= (lock < 500) ? key*3 : key/2.0;
            }
        }
    }
    uniqueKey = key;
}


vector<pair<int,int>> Ship::pickUp(){
    vector<int> filled(grid[0].size(), -1);
    vector<pair<int, int>> pickupIdxs(grid[0].size(), make_pair(-1, -1));

    for(int i = 0; i < grid.size(); ++i){
        vector<Container*> row = grid[i];
        for(int j = 0; j < row.size(); ++j){
            if(grid[i][j]->weight > 0 && filled[j] == -1){
                pickupIdxs[j] = make_pair(i, j);
                filled[j] = 1;
            }
        }
    }
    for(int i = 0; i < pickupIdxs.size(); ++i){
        pickupIdxs.erase(remove(pickupIdxs.begin(), pickupIdxs.end(), make_pair(-1,-1)), pickupIdxs.end());
        // cout << pickupIdxs[i].first << ' ' << pickupIdxs[i].second << ' ' << grid[pickupIdxs[i].first][pickupIdxs[i].second] << endl;
    }
    return pickupIdxs;
}

vector<Ship*> Ship::dropDown(pair<int, int> idx){
    vector<Ship*> children;
    Ship *move = new Ship(grid);
    pair<int, int> left = idx;
    pair<int, int> right = idx;
    int size = 12;
    int i = 0;

    while(size > 0){
        move = move_right(move, right);
        // cout << "done" << endl;
        if(move == NULL){
            size--;
            continue;
        }
        children.push_back(new Ship(move, 0));
        size--;
    }
    size = 12;
    move = new Ship(grid);
    while(size > 0){
        move = move_left(move, left);
        if(move == NULL){
            size--;
            continue;
        }
        children.push_back(new Ship(move, 0));
        size--;
    }

    for(int i = 0; i < children.size(); ++i){
        children.at(i)->parent = this;
    }
    return children;
}

vector<pair<Ship*,Container*>> Ship::unloadContainer(vector<pair<int, int>> allCont, int index){
    vector<pair<Ship*,Container*>> children;
    pair<int,int> idx = allCont[index];
    Ship *move = new Ship(grid);
    int above = containersAbove(this, idx);
    bool isAboveContainerToUnload = true;
    Container* nullCont = NULL;
    Ship* nullShip = NULL;

    while(above > 0){
        pair<int, int> idxOfTopContainer = make_pair(idx.first-above,idx.second);
        Container* cont = move->grid[idxOfTopContainer.first][idxOfTopContainer.second];

        // cout << "idxoftop: " << idxOfTopContainer.first << idxOfTopContainer.second << endl;
        move = move_left(move, idxOfTopContainer);
        isAboveContainerToUnload = true;

        if(move == NULL){
            children.push_back(make_pair(nullShip, new Container(cont->weight, cont->contents)));
            above--;
            continue;
        }
        while(isAboveContainerToUnload){
            isAboveContainerToUnload = false;
            for(int i = 0; i < allCont.size(); ++i){
                if(allCont[i].second == idxOfTopContainer.second){
                    isAboveContainerToUnload = true;
                    break;
                }
            }
            if(isAboveContainerToUnload){
                cont = move->grid[idxOfTopContainer.first][idxOfTopContainer.second];
                move = move_left(move, idxOfTopContainer);
                if(move == NULL){
                    children.push_back(make_pair(nullShip, new Container(cont->weight, cont->contents)));
                    break;
                }
            }
        }
        children.push_back(make_pair(new Ship(move, 0), nullCont));
        above--;
    }
    children.push_back(make_pair(nullShip, nullCont));
    for(int i = 0; i < children.size(); ++i){
        if(children.at(i).first != NULL){
            children.at(i).first->parent = this;
            children.at(i).first->setUniqueKey();
        }
    }
    return children;
}

int Ship::containersAbove(Ship* p, pair<int, int> idx){
    int cnt = 0;
    for(int i = 0; i < idx.first; ++i){
        if(p->grid[i][idx.second]->weight != -1){
            ++cnt;
        }
    }
    return cnt;
}

Ship* Ship::move_right(Ship *p, pair<int, int> &idx){
    if(idx.second + 1 >= 12){
        return NULL;
    }
    vector<vector<Container*>> g = p->grid;
    int row = idx.first;
    int column = idx.second + 1;

    for(int i = 0; i < g.size(); ++i){
        if(g[row][column]->weight == -1){
            if(row + 1 >= g.size()){
                swap(p->grid[row][column], p->grid[idx.first][idx.second]);
                idx.first = row;
                idx.second = column;
                return p;
            }
            else if(g[row + 1][column]->weight > 0){ //check to make sure about row+1 in OR, otherwise g[row+1] is out of bounds
                swap(p->grid[row][column], p->grid[idx.first][idx.second]);
                idx.first = row;
                idx.second = column;
                return p;
            }
            if(row < g.size()){row++;}
            while(row < p->grid.size()){ //moves crate to the very bottom or right before first occupied space in the column
                if(g[row][column]->weight > 0){
                    swap(p->grid[row-1][column], p->grid[idx.first][idx.second]);
                    idx.first = row-1;
                    idx.second = column;
                    return p;
                }
                if(row < g.size()){row++;}
                else{break;}
            }
        }
        if(row > 0){row--;}
        while(row > -1){ //edge case is if column is filled to the top
                if(g[row][column]->weight == -1){
                    swap(p->grid[row][column], p->grid[idx.first][idx.second]);
                    idx.first = row;
                    idx.second = column;
                    return p;
                }
                if(row > 0){row--;}
                else{break;}
        }
    }
    return p;
}

Ship* Ship::move_left(Ship *p, pair<int, int> &idx){
    if(idx.second - 1 < 0){
        return NULL;
    }
    vector<vector<Container*>> g = p->grid;
    int row = idx.first;
    int column = (idx.second > 0) ? idx.second - 1 : 0;

    for(int i = 0; i < idx.second; ++i){
        if(g[row][column]->weight == -1){
            if(row + 1 >= g.size()){
                swap(p->grid[row][column], p->grid[idx.first][idx.second]);
                idx.first = row;
                idx.second = column;
                return p;
            }
            else if(g[row + 1][column]->weight > 0){ //check to make sure about row+1 in OR, otherwise g[row+1] is out of bounds
                swap(p->grid[row][column], p->grid[idx.first][idx.second]);
                idx.first = row;
                idx.second = column;
                return p;
            }
            if(row < g.size()){row++;}
            while(row < p->grid.size()){ //moves crate to the very bottom or right before first occupied space in the column
                if(g[row][column]->weight > 0){
                    swap(p->grid[row-1][column], p->grid[idx.first][idx.second]);
                    idx.first = row-1;
                    idx.second = column;
                    return p;
                }
                if(row < g.size()){row++;}
                else{break;}
            }
        }
        if(row > 0){row--;}
        while(row > -1){ //edge case is if column is filled to the top
                if(g[row][column]->weight == -1){
                    swap(p->grid[row][column], p->grid[idx.first][idx.second]);
                    idx.first = row;
                    idx.second = column;
                    return p;
                }
                if(row > 0){row--;}
                else{break;}
        }
    }
    return p;
}

string Ship::ret_larger_side(){
    double left = find_mass_left();
    double right = find_mass_right();

    string tmp = "";

    if(left > right){
        tmp = "left";
    }
    else{
        tmp = "right";
    }
    return tmp;
}

vector<int> Ship::sort_larger_mass(){
    string tmp = ret_larger_side();

    vector<vector<Container*>> p = grid;
    vector<int> vec1;

    if(tmp == "left"){
        for(int i = 0; i < p.size(); i++){
            for(int j = 0; j < p.at(0).size()/2; ++j){
                if(p.at(i).at(j)->weight > 0){
                    vec1.push_back(p.at(i).at(j)->weight);
                }
            }
        }
    }
    else if(tmp == "right"){
        for(int i = 0; i < p.size(); i++){
            for(int j = p.at(0).size()/2; j < p.at(0).size(); ++j){
                if(p.at(i).at(j)->weight > 0){
                    vec1.push_back(p.at(i).at(j)->weight);
                }
            }
        }
    }

    sort(vec1.begin(), vec1.end());
    reverse(vec1.begin(), vec1.end());

    return vec1;
}

vector<int> Ship::find_num_containers(){
    vector<int> temp = sort_larger_mass();
    string tmp = ret_larger_side();

    double def = deficit();
    double high_def = def/1.0 + (def * .1);
    double low_def = def/1.0 - (def * .1);
    vector<int> values;
    bool bal = false;
    int i = 0;

    double left = find_mass_left();
    double right = find_mass_right();
    double balance = 0;

    while(!(bal)){
        if((temp.at(i)/1.0 >= low_def && temp.at(i)/1.0 <= high_def) || temp.at(i)/1.0 <= def){
            low_def = low_def - temp.at(i);
            high_def = high_def - temp.at(i);
            def = def - temp.at(i);
            values.push_back(temp.at(i));
            if(tmp == "left"){
                left = left - temp.at(i);
            }
            if(tmp == "right"){
                right = right - temp.at(i);
            }

            if(tmp == "left"){
                right = right + temp.at(i);
            }
            if(tmp == "right"){
                left = left + temp.at(i);
            }

            balance = min(left,right)/max(left,right) ;
            if(balance >= .9){
                bal = true;
            }
        }
        if(i == temp.size()-1){
            bal = true;
        }
        i++;
    }

    values.push_back(values.size());
    return values;
}

vector<pair<int,int>> Ship::find_container_location(){
    vector<int> temp = sort_larger_mass();

    double def = deficit();
    double high_def = def/1.0 + (def * .1);
    double low_def = def/1.0 - (def * .1);
    vector<pair<int,int>> values;
    bool bal = false;
    int i = 0;

    double left = find_mass_left();
    double right = find_mass_right();
    double balance = 0;

    while(!(bal)){
        if((temp.at(i)/1.0 >= low_def && temp.at(i)/1.0 <= high_def) || temp.at(i)/1.0 <= def){
            def = def - temp.at(i);

            string tmp = ret_larger_side();
            vector<vector<Container*>> p = grid;
            if(tmp == "left"){
                for(int i = 0; i < p.size(); i++){
                    for(int j = 0; j < p.at(0).size()/2; ++j){
                        if(p.at(i).at(j)->weight > 0){
                            values.push_back(make_pair(i,j));
                        }
                    }
                }
            }
            else if(tmp == "right"){
                for(int i = 0; i < p.size(); i++){
                    for(int j = p.at(0).size()/2; j < p.at(0).size(); ++j){
                        if(p.at(i).at(j)->weight > 0){
                            values.push_back(make_pair(i,j));
                        }
                    }
                }
            }
            if(tmp == "left"){
                left = left - temp.at(i);
            }
            if(tmp == "right"){
                right = right - temp.at(i);
            }
             if(tmp == "left"){
                right = right + temp.at(i);
            }
            if(tmp == "right"){
                left = left + temp.at(i);
            }
            balance = min(left,right)/max(left,right);
            if(balance >= .9){
                bal = true;
            }
        }
        if(i == temp.size()-1){
            bal = true;
        }
        i++;
    }
    return values;
}

int Ship::find_nearest_col(){
    // for each container, find the distance to next avalible column
    //probably retrun an int (the column number)

    int column = 0;
    string tmp = ret_larger_side();
    vector<vector<Container*>> p = grid;
    if(tmp == "right"){
        for(int j = (p.at(0).size()/2)-1; j > 0; j--){
            for(int i = 0; i < p.size(); ++i){
                if(p.at(i).at(j)->weight == -1){
                    return j;
                }
            }
        }
    }
    else if(tmp == "left"){
        for(int j = p.at(0).size()/2; j < p.at(0).size(); ++j){
            for(int i = 0; i < p.size(); ++i){
                if(p.at(i).at(j)->weight == -1){
                    return j;
                }
            }
        }
    }
    return column;
}



void Ship::calculate_hn(){
    // What is needed to calculate Hn:
    // 1) You need the number of moves that are needed to balance and the containers that need to be moved
    // 2) You need to know what the first avalible column is
    // *3) need to know what the first avalible row is in that column
    int empty_check = 0;
    empty_check = empty_check + find_mass_left();
    empty_check = empty_check + find_mass_right();
    if(empty_check == 0){
        hN = 0;
        fN = hN + gN;
        return;
    }
    vector<int> values = find_num_containers(); // remeber the last number is the number we need to move

    vector<pair<int,int>> loc = find_container_location();

    int nearest = find_nearest_col();

    int sum = 0;

    for(int i = 0; i < values.size(); i++){
        for(int j = 0; j < loc.size(); j++){
            if(values.at(i) == grid.at(loc.at(j).first).at(loc.at(j).second)->weight){
                sum = sum + abs(nearest - loc.at(j).second);
            }
        }
    }

    hN = sum;
    fN = hN + gN;
}

void Ship::trickleDown(){
    bool leave = false;
    pair<int,int> swapIdx;
    for(int i = 0; i < 12; ++i){
        for(int j = 7; j > -1; --j){
            if(grid[j][i]->weight > -1 && j != 7){
                int k = j + 1;
                int row = j, column = i;
                if(grid[k][column]->weight > -1){continue;}
                while(k < 8){
                    if(grid[k][column]->weight > -1 || grid[k][column]->weight == -2){break;}
                    swap(grid[row][column], grid[k][column]);
                    row = k;
                    k++;
                }
            }
        }
    }
}

void Ship::removeContainer(Container* c){
    for(int i = 0; i < 8; ++i){
        for(int j = 0; j < 12; ++j){
            if(grid[i][j]->weight == c->weight && grid[i][j]->contents == c->contents){
                grid[i][j] = new Container(0, "UNUSED");
                setUniqueKey();
                return;
            }
        }
    }
}

void Ship::addContainer(Container* c, int column){
    if(column < 0){
        for(int i = 0; i < 8; ++i){
            for(int j = 0; j < 12; ++j){
                if(grid[i][j]->weight == -1){
                    grid[i][j] = new Container(c->weight, c->contents);
                    setUniqueKey();
                    return;
                }
            }
        }
    }
    else{
        for(int i = 0; i < 8; ++i){
            if(grid[i][column]->weight == -1){
                grid[i][column] = new Container(c->weight, c->contents);
                setUniqueKey();
                return;
            }
        }
    }
}

vector<vector<Container*>> Ship::getGrid(){
    return this->grid;
}

void Ship::setGrid(vector<vector<Container*>> g){
    this->grid = g;
}

double Ship::getUniqueKey(){
    return this->uniqueKey;
}

string Ship::outputMoves(Ship* parent, Ship* child) {
    string result = "";
    bool ifFound = false;
    bool b = true;

    for(int i = 0; i < parent->grid.size(); i++) {
        for(int j = 0; j < parent->grid.at(i).size(); j++) {
            if (parent->grid.at(i).at(j) != child->grid.at(i).at(j) && parent->grid.at(i).at(j)->weight > -1) {
                result += "Move container from row " + to_string(i+1) + " and column " + to_string(j+1) + "\n";
                ifFound = true;
                break;
            }
        }
        if(ifFound == true) {
            break;
        }
    }
    if(!ifFound){
        result += "Container from buffer/truck \n";
    }
    ifFound = false;
    for(int i = 0; i < child->grid.size(); i++) {
        for(int j = 0; j < child->grid.at(i).size(); j++) {
            if (parent->grid.at(i).at(j) != child->grid.at(i).at(j) && child->grid.at(i).at(j)->weight > -1) {
                result += "to row " + to_string(i+1) + " and column " + to_string(j+1) + "\n";
                ifFound = true;
                break;
            }
        }
        if(ifFound == true) {
            break;
        }
    }
    if(!ifFound){
        if (b) {
            result += " to a truck.\n";
            b = !b;
        } else {
            result += " to the buffer.\n";
        }
    }
    return result;
}

bool Ship::check_SIFT(){
    bool check = false;
    double sum = 0;
    double def = deficit();
    double high_def = def/1.0 + (def * .1);
    double low_def = def/1.0 - (def * .1);

    int sz = find_num_containers().size();
    vector<int> contain = find_num_containers();
    for(int i = 0; i < sz-1; i++){
        sum = sum + contain.at(i);
    }

    if(sum >= low_def && sum <= high_def){
        check = false;
    }
    else{
        check = true;
    }

    return check;
}
