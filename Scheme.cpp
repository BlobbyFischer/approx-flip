#include <iostream>
#include <fstream>
#include <sstream>
#include <regex>

#include "Scheme.h"

Scheme::Scheme() {
    //ctor
}

Scheme::~Scheme() {
    //dtor
}

bool Scheme::update() {
    move_list.clear();
    // first we update each tensor
    for (int i=0; i<tensors.size(); i++) {
        if (tensors[i].update()) {
            tensors.erase(tensors.begin()+i);
            update();
            return true; // because there was a reduction
        }
    }
    // then we go through all pairs and check for flips
    for (int i=0; i<tensors.size()-1; i++) {
        Rank1Tensor& tensor1 = tensors[i];
        for (int j=i+1; j<tensors.size(); j++) {
            Rank1Tensor& tensor2 = tensors[j];
            // aeq, beq, ceq to help with future things
            int aeq = 0;
            while (tensor1.a[aeq] == tensor2.a[aeq]) {
                aeq++;
            }
            int beq = 0;
            while (tensor1.b[beq] == tensor2.b[beq]) {
                beq++;
            }
            int ceq = 0;
            while (tensor1.c[ceq] == tensor2.c[ceq]) {
                ceq++;
            }
            // is there a flip?
            if (aeq >= MAX_ORDER - tensor1.coeff or aeq >= MAX_ORDER - tensor2.coeff) {
                move_list.push_back(tuple<int,int,int,char>(i,j,-1,'a')); // -1 means its a normal flip
            }
            if (beq >= MAX_ORDER - tensor1.coeff or beq >= MAX_ORDER - tensor2.coeff) {
                move_list.push_back(tuple<int,int,int,char>(i,j,-1,'b'));
            }
            if (ceq >= MAX_ORDER - tensor1.coeff or ceq >= MAX_ORDER - tensor2.coeff) {
                move_list.push_back(tuple<int,int,int,char>(i,j,-1,'c'));
            }
            // a SENSIBLE eflip? i.e an eflip that can be followed by a NEW flip (this tells us what our choice of Gamma' should be)
            if (beq>0 && ceq>0) {
                // does this lead to a reduction?
                int min_eq = min(beq,ceq);
                int min_coeff = min(tensor1.coeff,tensor2.coeff);
                if (MAX_ORDER - min_eq <= min_coeff) { // is this the most general? or are there other cases as well? Maybe max_coeff instead?
                    int powdiff = tensor1.coeff - tensor2.coeff;
                    if (powdiff < 0) {
                        for (int ii=0;ii-powdiff<MAX_ORDER-tensor1.coeff;ii++) {
                            tensor1.a[ii-powdiff] ^= tensor2.a[ii];
                        }
                        tensors.erase(tensors.begin()+j);
                    } else {
                        for (int ii=0;ii+powdiff<MAX_ORDER-tensor2.coeff;ii++) {
                            tensor2.a[ii+powdiff] ^= tensor1.a[ii];
                        }
                        tensors.erase(tensors.begin()+i);
                    }
                    update();
                    return true;
                }
                // is there a third tensor which might go well with either of them?
                int k = 0;
                while (k<tensors.size()) {
                    while (k==i or k==j) {
                        k++;
                        if (k>=tensors.size()) {
                            break; // we are done in the while loop
                        }
                    }
                    Rank1Tensor& tensor3 = tensors[k];
                    // so is tensor3 a good choice?
                    int aeq1 = 0;
                    while (tensor1.a[aeq1] == tensor3.a[aeq1]) {
                        aeq1++;
                    }
                    int aeq2 = 0;
                    while (tensor2.a[aeq2] == tensor3.a[aeq2]) {
                        aeq2++;
                    }
                    if (aeq1 >= MAX_ORDER - min_eq - min_coeff) {
                        move_list.push_back(tuple<int,int,int,char>(i,j,k,'a'));
                    }
                    if (aeq2 >= MAX_ORDER - min_eq - min_coeff) {
                        move_list.push_back(tuple<int,int,int,char>(j,i,k,'a'));
                    }
                    k++;
                }
            }
            // DO SAME FOR b AND c
            if (aeq>0 && ceq>0) {
                // does this lead to a reduction?
                int min_eq = min(aeq,ceq);
                int min_coeff = min(tensor1.coeff,tensor2.coeff);
                if (MAX_ORDER - min_eq <= min_coeff) { // is this the most general? or are there other cases as well? Maybe max_coeff instead?
                    int powdiff = tensor1.coeff - tensor2.coeff;
                    if (powdiff < 0) {
                        for (int ii=0;ii-powdiff<MAX_ORDER-tensor1.coeff;ii++) {
                            tensor1.b[ii-powdiff] ^= tensor2.b[ii];
                        }
                        tensors.erase(tensors.begin()+j);
                    } else {
                        for (int ii=0;ii+powdiff<MAX_ORDER-tensor2.coeff;ii++) {
                            tensor2.b[ii+powdiff] ^= tensor1.b[ii];
                        }
                        tensors.erase(tensors.begin()+i);
                    }
                    update();
                    return true;
                }
                // is there a third tensor which might go well with either of them?
                int k = 0;
                while (k<tensors.size()) {
                    while (k==i or k==j) {
                        k++;
                        if (k>=tensors.size()) {
                            break; // we are done in the while loop
                        }
                    }
                    Rank1Tensor& tensor3 = tensors[k];
                    // so is tensor3 a good choice?
                    int beq1 = 0;
                    while (tensor1.b[beq1] == tensor3.b[beq1]) {
                        beq1++;
                    }
                    int beq2 = 0;
                    while (tensor2.b[beq2] == tensor3.b[beq2]) {
                        beq2++;
                    }
                    if (beq1 >= MAX_ORDER - min_eq - min_coeff) {
                        move_list.push_back(tuple<int,int,int,char>(i,j,k,'b'));
                    }
                    if (beq2 >= MAX_ORDER - min_eq - min_coeff) {
                        move_list.push_back(tuple<int,int,int,char>(j,i,k,'b'));
                    }
                    k++;
                }
            }
            if (aeq>0 && beq>0) {
                // does this lead to a reduction?
                int min_eq = min(aeq,beq);
                int min_coeff = min(tensor1.coeff,tensor2.coeff);
                if (MAX_ORDER - min_eq <= min_coeff) { // is this the most general? or are there other cases as well? Maybe max_coeff instead?
                    int powdiff = tensor1.coeff - tensor2.coeff;
                    if (powdiff < 0) {
                        for (int ii=0;ii-powdiff<MAX_ORDER-tensor1.coeff;ii++) {
                            tensor1.c[ii-powdiff] ^= tensor2.c[ii];
                        }
                        tensors.erase(tensors.begin()+j);
                    } else {
                        for (int ii=0;ii+powdiff<MAX_ORDER-tensor2.coeff;ii++) {
                            tensor2.c[ii+powdiff] ^= tensor1.c[ii];
                        }
                        tensors.erase(tensors.begin()+i);
                    }
                    update();
                    cout << "eflip reduction found" << endl;
                    return true;
                }
                // is there a third tensor which might go well with either of them?
                int k = 0;
                while (k<tensors.size()) {
                    while (k==i or k==j) {
                        k++;
                        if (k>=tensors.size()) {
                            break; // we are done in the while loop
                        }
                    }
                    Rank1Tensor& tensor3 = tensors[k];
                    // so is tensor3 a good choice?
                    int ceq1 = 0;
                    while (tensor1.c[ceq1] == tensor3.c[ceq1]) {
                        ceq1++;
                    }
                    int ceq2 = 0;
                    while (tensor2.c[ceq2] == tensor3.c[ceq2]) {
                        ceq2++;
                    }
                    if (ceq1 >= MAX_ORDER - min_eq - min_coeff) {
                        move_list.push_back(tuple<int,int,int,char>(i,j,k,'c'));
                    }
                    if (ceq2 >= MAX_ORDER - min_eq - min_coeff) {
                        move_list.push_back(tuple<int,int,int,char>(j,i,k,'c'));
                    }
                    k++;
                }
            }
        }
    }
    cout << endl;
    return false;
}

//REMEMBER TO WRITE IN FORM a0*e^2 ELSE IT WON'T WORK

void Scheme::from_file(string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open file for reading: " << filename << std::endl;
        return;
    }
    tensors.clear();
    std::string line;
    while (std::getline(file, line)) {
        Rank1Tensor tensor;
        tensor.coeff = 0; // default
        std::smatch match;
        std::regex coeff_regex(R"(e(\^(\d+))?\*)");
        if (std::regex_search(line, match, coeff_regex)) {
            tensor.coeff = match[2].matched ? std::stoi(match[2]) : 1;
            line = line.substr(match[0].length());
        }
        std::regex group_regex(R"(\((.*?)\))");
        auto it = std::sregex_iterator(line.begin(), line.end(), group_regex);
        int group_idx = 0;
        for (; it != std::sregex_iterator() && group_idx < 3; ++it, ++group_idx) {
            std::string group = (*it)[1];
            std::istringstream terms(group);
            std::string term;
            while (std::getline(terms, term, '+')) {
                std::smatch tm;
                std::regex term_regex(R"(([abc])(\d+)(\*e(\^(\d+))?)?)");
                if (std::regex_match(term, tm, term_regex)) {
                    char var = tm[1].str()[0];
                    int index = std::stoi(tm[2]);
                    int pow = tm[5].matched ? std::stoi(tm[5]) : (tm[3].matched ? 1 : 0);
                    if (index < 0 || index >= N || pow < 0 || pow >= MAX_ORDER) continue;
                    if (var == 'a') tensor.a[pow][index] = 1;
                    else if (var == 'b') tensor.b[pow][index] = 1;
                    else if (var == 'c') tensor.c[pow][index] = 1;
                }
            }
        }
        tensors.push_back(tensor);
    }
    file.close();
    update();
}

void Scheme::write_to_file() {
    string filename = "k" + to_string(rand())+".exp";
    ostringstream output;
    for (int i=0;i<tensors.size();i++) {
        Rank1Tensor& tensor = tensors[i];
        if (i>0) {
            output << "\n";
        }
        if (tensor.coeff > 0) {
            output << "e";
            if (tensor.coeff > 1) {
                output << "^" << to_string(tensor.coeff);
            }
            output << "*";
        }
        bool first = true;
        output << "(";
        for (int pow=0;pow<MAX_ORDER;pow++) {
            for (int j=0;j<N;j++) {
                if (tensor.a[pow][j]==1) {
                    if (not first) {
                        output << "+";
                    } else {
                        first = false;
                    }
                    output << "a" + to_string(j);
                    if (pow > 0) {
                        output << "*e";
                        if (pow > 1) {
                            output << "*e^" << to_string(pow);
                        }
                    }
                }
            }
        }
        first = true;
        output << ")(";
        for (int pow=0;pow<MAX_ORDER;pow++) {
            for (int j=0;j<N;j++) {
                if (tensor.b[pow][j]==1) {
                    if (not first) {
                        output << "+";
                    } else {
                        first = false;
                    }
                    output << "b" << to_string(j);
                    if (pow > 0) {
                        output << "*e";
                        if (pow > 1) {
                            output << "*e^" << to_string(pow);
                        }
                    }
                }
            }
        }
        first = true;
        output << ")(";
        for (int pow=0;pow<MAX_ORDER;pow++) {
            for (int j=0;j<N;j++) {
                if (tensor.c[pow][j]==1) {
                    if (not first) {
                        output << "+";
                    } else {
                        first = false;
                    }
                    output << "c" << to_string(j);
                    if (pow > 0) {
                        output << "*e";
                        if (pow > 1) {
                            output << "*e^" << to_string(pow);
                        }
                    }
                }
            }
        }
        output << ")";
    }
    ofstream file(filename);
    if (file.is_open()) {
        file << output.str();
        file.close();
        cout << filename << "," << tensors.size() << endl;
    } else {
        cerr << "Failed to open file" << endl;
    }
}

bool Scheme::flip(int ind1, int ind2, char flip_around) { // I THINK JUST REWRITE THIS...
    Rank1Tensor& tensor1 = tensors[ind1];
    Rank1Tensor& tensor2 = tensors[ind2];
    int beta1 = rand() % (1 + tensor1.coeff);
    int beta2 = rand() % (1 + tensor2.coeff);
    int random_var_for_choosing_gamma = rand() % ((1 << MAX_ORDER) - 1);
    int gamma = MAX_ORDER-1;
    while (random_var_for_choosing_gamma > 1) {
        random_var_for_choosing_gamma /= 2;
        gamma--;
    }
    bool change_coeff1 = false;
    bool change_coeff2 = false;
    //gamma is weighted to be most likely to be 0, but could be anything more as well
    switch(flip_around) {
        case 'a':
            //we know we can change one of them to make them equal at flip_around, so lets do that...
            if (tensor1.coeff >= tensor2.coeff) {
                for (int i=MAX_ORDER-tensor1.coeff;i<MAX_ORDER-tensor2.coeff;i++) {
                    tensor1.a[i] = tensor2.a[i];
                }
            } else {
                for (int i=MAX_ORDER-tensor2.coeff;i<MAX_ORDER-tensor1.coeff;i++) {
                    tensor2.a[i] = tensor1.a[i];
                }
            }
            //first, lets change tensor1:
            //which case are we in?
            if (beta2 + gamma >= beta1) {
                for (int i=0;i+beta2-beta1+gamma<MAX_ORDER;i++) {
                    tensor1.b[i+beta2-beta1+gamma] ^= tensor2.b[i];
                }
            } else {
                change_coeff1 = true;
                //shift t1.b by beta1-beta2-gamma then add t2.b to it
                int ii=MAX_ORDER-1;
                while (ii-beta1+beta2+gamma >= 0) {
                    tensor1.b[ii] = tensor1.b[ii-beta1+beta2+gamma] ^ tensor2.b[ii];
                    ii--;
                }
                while (ii >= 0) {
                    tensor1.b[ii] = tensor2.b[ii];
                    ii--;
                }
            }

            //next, we change tensor2:
            //which case are we in this time?
            if (tensor1.coeff + beta2 - tensor2.coeff - beta1 + gamma >= 0) {
                for (int i=0;i+beta2+tensor1.coeff+gamma-beta1<MAX_ORDER;i++) {
                    tensor2.c[i+tensor1.coeff+beta2+gamma-beta1-tensor2.coeff] ^= tensor1.c[i];
                }
            } else {
                change_coeff2 = true;
                //shift t2.c by t2.coeff+beta1-t1.coeff-beta2-gamma then add t1.c to it
                int i=MAX_ORDER-1;
                while (i+tensor1.coeff+beta2+gamma-beta1-tensor2.coeff >= 0) {
                    tensor2.c[i] = tensor2.c[i+tensor1.coeff+beta2+gamma-beta1-tensor2.coeff] ^ tensor1.c[i];
                    i--;
                }
                while (i >= 0) {
                    tensor2.c[i] = tensor1.c[i];
                    i--;
                }
            }
            break;
        case 'b':
            //we know we can change one of them to make them equal at flip_around, so lets do that...
            if (tensor1.coeff >= tensor2.coeff) {
                for (int i=MAX_ORDER-tensor1.coeff;i<MAX_ORDER-tensor2.coeff;i++) {
                    tensor1.b[i] = tensor2.b[i];
                }
            } else {
                for (int i=MAX_ORDER-tensor2.coeff;i<MAX_ORDER-tensor1.coeff;i++) {
                    tensor2.b[i] = tensor1.b[i];
                }
            }
            //first, lets change tensor1:
            //which case are we in?
            if (beta2 + gamma >= beta1) {
                for (int i=0;i+beta2-beta1+gamma<MAX_ORDER;i++) {
                    tensor1.c[i+beta2-beta1+gamma] ^= tensor2.c[i];
                }
            } else {
                change_coeff1 = true;
                //shift t1.b by beta1-beta2-gamma then add t2.b to it
                int ii=MAX_ORDER-1;
                while (ii-beta1+beta2+gamma >= 0) {
                    tensor1.c[ii] = tensor1.c[ii-beta1+beta2+gamma] ^ tensor2.c[ii];
                    ii--;
                }
                while (ii >= 0) {
                    tensor1.c[ii] = tensor2.c[ii];
                    ii--;
                }
            }

            //next, we change tensor2:
            //which case are we in this time?
            if (tensor1.coeff + beta2 - tensor2.coeff - beta1 + gamma >= 0) {
                for (int i=0;i+beta2+tensor1.coeff+gamma-beta1<MAX_ORDER;i++) {
                    tensor2.a[i+tensor1.coeff+beta2+gamma-beta1-tensor2.coeff] ^= tensor1.a[i];
                }
            } else {
                change_coeff2 = true;
                //shift t2.c by t2.coeff+beta1-t1.coeff-beta2-gamma then add t1.c to it
                int i=MAX_ORDER-1;
                while (i+tensor1.coeff+beta2+gamma-beta1-tensor2.coeff >= 0) {
                    tensor2.a[i] = tensor2.a[i+tensor1.coeff+beta2+gamma-beta1-tensor2.coeff] ^ tensor1.a[i];
                    i--;
                }
                while (i >= 0) {
                    tensor2.a[i] = tensor1.a[i];
                    i--;
                }
            }
            break;
        case 'c':
            //we know we can change one of them to make them equal at flip_around, so lets do that...
            if (tensor1.coeff >= tensor2.coeff) {
                for (int i=MAX_ORDER-tensor1.coeff;i<MAX_ORDER-tensor2.coeff;i++) {
                    tensor1.c[i] = tensor2.c[i];
                }
            } else {
                for (int i=MAX_ORDER-tensor2.coeff;i<MAX_ORDER-tensor1.coeff;i++) {
                    tensor2.c[i] = tensor1.c[i];
                }
            }
            //first, lets change tensor1:
            //which case are we in?
            if (beta2 + gamma >= beta1) {
                for (int i=0;i+beta2-beta1+gamma<MAX_ORDER;i++) {
                    tensor1.a[i+beta2-beta1+gamma] ^= tensor2.a[i];
                }
            } else {
                change_coeff1 = true;
                //shift t1.b by beta1-beta2-gamma then add t2.b to it
                int ii=MAX_ORDER-1;
                while (ii-beta1+beta2+gamma >= 0) {
                    tensor1.a[ii] = tensor1.b[ii-beta1+beta2+gamma] ^ tensor2.a[ii];
                    ii--;
                }
                while (ii >= 0) {
                    tensor1.a[ii] = tensor2.a[ii];
                    ii--;
                }
            }

            //next, we change tensor2:
            //which case are we in this time?
            if (tensor1.coeff + beta2 - tensor2.coeff - beta1 + gamma >= 0) {
                for (int i=0;i+beta2+tensor1.coeff+gamma-beta1<MAX_ORDER;i++) {
                    tensor2.b[i+tensor1.coeff+beta2+gamma-beta1-tensor2.coeff] ^= tensor1.b[i];
                }
            } else {
                change_coeff2 = true;
                //shift t2.c by t2.coeff+beta1-t1.coeff-beta2-gamma then add t1.c to it
                int i=MAX_ORDER-1;
                while (i+tensor1.coeff+beta2+gamma-beta1-tensor2.coeff >= 0) {
                    tensor2.b[i] = tensor2.b[i+tensor1.coeff+beta2+gamma-beta1-tensor2.coeff] ^ tensor1.b[i];
                    i--;
                }
                while (i >= 0) {
                    tensor2.b[i] = tensor1.b[i];
                    i--;
                }
            }
            break;
    }
    if (change_coeff2) {
        tensor2.coeff = tensor1.coeff - beta1 + beta2 + gamma;
    }
    if (change_coeff1) {
        tensor1.coeff = tensor1.coeff - beta1 + beta2 + gamma;
    }
    return update();
}

bool Scheme::eflip(int ind1, int ind2, int ind3, char flip_around) {
    // SOME COMPLICATED STUFF WILL GO HERE LATER
    return update();
}

void Scheme::random_walk(int pathlength) {
    for (int i=0;i<pathlength;i++) {
        if (move_list.size() == 0) break;
        tuple<int,int,int,char> next_flip = move_list[rand() % move_list.size()];
        //for now we suppose it will be a normal flip
        if (get<2>(next_flip) == -1) {
            if (rand() % 2) flip(get<0>(next_flip),get<1>(next_flip),get<3>(next_flip));
            else flip(get<1>(next_flip),get<0>(next_flip),get<3>(next_flip));
        } else {
            //eflip(get<0>(next_flip),get<1>(next_flip),get<2>(next_flip),get<3>(next_flip));
        }
    }
}

void Scheme::check()
{
    for (int pow=0;pow<MAX_ORDER;pow++) {
        for (int i=0;i<N;i++) {
            for (int j=0;j<N;j++) {
                for (int k=0;k<N;k++) {
                    int counter = 0;
                    for (int ind=0;ind<tensors.size();ind++) {
                        Rank1Tensor tensor = tensors[ind];
                        for (int pow1=0;pow1<=pow;pow1++) {
                            for (int pow2=0;pow1+pow2+tensor.coeff<=pow;pow2++){
                                if (tensor.a[pow1][i] and tensor.b[pow2][j] and tensor.c[pow-pow1-pow2-tensor.coeff][k]){
                                    counter = 1-counter;
                                }
                            }
                        }
                    }
                    if (counter == 1) {
                        cout << "a" << i << " b" << j << " c" << k << " * e^" << pow << endl;
                    }
                }
            }
        }
    }
}
