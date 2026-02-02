#include "../include/pai.h"
#include <vector>

Pai::Pai(PaiType pt) : type(pt) {}

PASS::PASS() : Pai(PaiType::PASS_T) {}
vector<Pai *> PASS::get(int *arr) {
    vector<Pai *> ret;
    ret.push_back(new PASS);
    return ret;
}
ostream& PASS::output() {
    return cout << "PASS";
}


DAN::DAN(int x) : Pai(PaiType::DAN_T), x(x) {}
ostream& DAN::output() {
    cout << "DAN " << Pai::getName(x);
    return cout;
}

DUIZI::DUIZI(int x) : Pai(PaiType::DUIZI_T), x(x) {}
ostream& DUIZI::output() {
    cout << "DUI " << Pai::getName(x);
    return cout;
}

SHUNZI::SHUNZI(int head, int length) : Pai(PaiType::SHUNZI_T), head(head), length(length) {}
vector<Pai *> SHUNZI::get(int *arr) {
    vector<Pai *> ret;
    for (int l = 5; l <= 12; l++) {
        for (int h = 3, H = 14 - l + 1; h <= H; h++) {
            bool valid = true;
            for (int k = h; k < h + l; k++) {
                if (arr[k] == 0) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                ret.push_back(new SHUNZI(h, l));
            }
        }
    }
    return ret;
}
ostream& SHUNZI::output() {
    cout << "SHUNZI :";
    for (int i = head; i < head + length; i++) cout << " " << Pai::getName(i);
    return cout;
}

SANDAI::SANDAI(int x, Pai *dai) : Pai(PaiType::SANDAI_T), x(x), dai(dai) {}
vector<Pai *> SANDAI::get(int *arr) {
    vector<Pai *> ret;
    for (int i = 3; i < MAX_N; i++) {
        if (arr[i] < 3) continue;
        ret.push_back(new SANDAI(i, new PASS));
        for (int j = 3; j < MAX_N; j++) {
            if (arr[j] == 0 || j == i) continue;
            ret.push_back(new SANDAI(i, new DAN(j)));
            if (arr[j] >= 2) ret.push_back(new SANDAI(i, new DUIZI(j)));
        }
    }
    return ret;
}
ostream& SANDAI::output() {
    cout << "SAN " << Pai::getName(x) << " && ";
    dai->output();
    return cout;
}

ZHADAN::ZHADAN(int x) : Pai(PaiType::ZHADAN_T), x(x) {}
vector<Pai *> ZHADAN::get(int *arr) {
    vector<Pai *> ret;
    for (int i = 3; i < MAX_N; i++) {
        if (arr[i] < 4) continue;
        ret.push_back(new ZHADAN(i));
    }
    return ret;
}
ostream& ZHADAN::output() {
    return cout << "ZHA " << Pai::getName(x);
}

WANGZHA::WANGZHA() : Pai(PaiType::WANGZHA_T) {}

vector<Pai *> WANGZHA::get(int *arr) {
    vector<Pai *> ret;
    if (arr[16] > 0 && arr[17] > 0) ret.push_back(new WANGZHA());
    return ret;
}
ostream& WANGZHA::output() {
    return cout << "WANGZHA";
}
 

vector<Pai *> Pai::getPai(int *arr) {
    vector<Pai *> ret = DAN::get(arr);
    
    vector<Pai *> t = DUIZI::get(arr);
    ret.insert(ret.end(), t.begin(), t.end());

    t = SHUNZI::get(arr);
    ret.insert(ret.end(), t.begin(), t.end());

    t = LIANDUI::get(arr);
    ret.insert(ret.end(), t.begin(), t.end());

    t = SANDAI::get(arr);
    ret.insert(ret.end(), t.begin(), t.end());

    t = ZHADAN::get(arr);
    ret.insert(ret.end(), t.begin(), t.end());

    t = WANGZHA::get(arr);
    ret.insert(ret.end(), t.begin(), t.end());

    t = PASS::get(arr);
    ret.insert(ret.end(), t.begin(), t.end());
    return ret;
}

string Pai::getName(int code) {
    switch (code) {
        case 3 :
        case 4 :
        case 5 :
        case 6 :
        case 7 :
        case 8 :
        case 9 :
        case 10 : return to_string(code);  break;
        case 11 : return "Jack";                break;
        case 12 : return "Queen";               break;
        case 13 : return "King";                break;
        case 14 : return "Ace";                 break;
        case 15 : return "2";                   break;
        case 16 : return "Small Jocker";        break;
        case 17 : return "Big Jocker";          break;
        default:  return "Invalid Card!";       break;
    }
}

vector<Pai *> DAN::get(int *arr) {
    vector<Pai *> ret;
    for (int i = 3; i < MAX_N; i++) {
        if (arr[i] == 0) continue;
        ret.push_back(new DAN(i));
    }
    return ret;
}

vector<Pai *> DUIZI::get(int *arr) {
    vector<Pai *> ret;
    for (int i = 3; i < MAX_N; i++) {
        if (arr[i] < 2) continue;
        ret.push_back(new DUIZI(i));
    }
    return ret;
}


bool PASS::operator>(Pai *pre) {
    return pre->type != PaiType::PASS_T;
}

bool DAN::operator>(Pai *pre) {
    switch (pre->type) {
        case PaiType::PASS_T: return true;
        case PaiType::DAN_T: return this->x > dynamic_cast<DAN *>(pre)->x;
        default: return false;
    }
}

bool DUIZI::operator>(Pai *pre) {
    switch (pre->type) {
        case PaiType::PASS_T: return true;
        case PaiType::DUIZI_T: return this->x > dynamic_cast<DUIZI *>(pre)->x;
        default: return false;
    }
}

bool SHUNZI::operator>(Pai *pre) {
    switch (pre->type) {
        case PaiType::PASS_T: return true;
        case PaiType::SHUNZI_T: {
            SHUNZI *pre_ = dynamic_cast<SHUNZI *>(pre);
            if (this->length != pre_->length) return false;
            return  this->head > pre_->head;
        }
        default: return false;
    }
}


bool SANDAI::operator>(Pai *pre) {
    switch (pre->type) {
        case PaiType::PASS_T: return true;
        case PaiType::SANDAI_T: {
            SANDAI *pre_ = dynamic_cast<SANDAI *>(pre);
            if (pre_->dai->type != this->dai->type) return false;
            return this->x > pre_->x;
        }
        default: return false;
    }
}


bool ZHADAN::operator>(Pai *pre) {
    switch (pre->type) {
        case PaiType::WANGZHA_T: return false;
        case PaiType::ZHADAN_T: {
            ZHADAN *pre_ = dynamic_cast<ZHADAN *>(pre);
            return this->x > pre_->x;
        }
        default: return true;
    }
}


bool WANGZHA::operator>(Pai *pre) {
    switch (pre->type) {
        case PaiType::WANGZHA_T: return false;
        default: return true;
    }
}

#include <algorithm>

vector<Pai *> Pai::getLegalPai(int *arr, Pai *pai) {
    vector<Pai *> t = Pai::getPai(arr);
    vector<Pai *> ret;
    for (auto x : t) {
        if (x->operator>(pai)) ret.push_back(x);
        else delete x;
    }
    
    std::sort(ret.begin(), ret.end(), [](Pai *a, Pai *b) {
        if (a->type != b->type) return a->type > b->type;
        return a->operator>(b); 
    });
    
    return ret;
}

void PASS::take(int *arr) { return ; }
void PASS::back(int *arr) { return ; }

void DAN::take(int *arr) {
    arr[this->x]--;
}
void DAN::back(int *arr) {
    arr[this->x]++;
}

void DUIZI::take(int *arr) {
    arr[this->x] -= 2;
}
void DUIZI::back(int *arr) {
    arr[this->x] += 2;
}

void SHUNZI::take(int *arr) {
    for (int i = this->head, I = this->head + this->length; i < I; i++) arr[i]--;
}
void SHUNZI::back(int *arr) {
    for (int i = this->head, I = this->head + this->length; i < I; i++) arr[i]++;
}

void SANDAI::take(int *arr) {
    arr[this->x] -= 3;
    this->dai->take(arr);
}
void SANDAI::back(int *arr) {
    arr[this->x] += 3;
    this->dai->back(arr);
}

void ZHADAN::take(int *arr) {
    arr[x] -= 4;
}
void ZHADAN::back(int *arr) {
    arr[x] += 4;
}

void WANGZHA::take(int *arr) {
    arr[16]--;
    arr[17]--;
}
void WANGZHA::back(int *arr) {
    arr[16]++;
    arr[17]++;
}

void Pai::output_arr(int *arr) {
    for (int i = 3; i < MAX_N; i++) {
        if (arr[i] == 0) continue;
        cout << Pai::getName(i) << "*" << arr[i] << " ";
    }
    cout << endl;
}

LIANDUI::LIANDUI(int head, int length) : Pai(PaiType::LIANDUI_T), head(head), length(length) {}

vector<Pai *> LIANDUI::get(int *arr) {
    vector<Pai *> ret;
    for (int l = 3; l <= 12; l++) {
        for (int h = 3, H = 14 - l + 1; h <= H; h++) {
            bool valid = true;
            for (int k = h; k < h + l; k++) {
                if (arr[k] < 2) {
                    valid = false;
                    break;
                }
            }
            if (valid) {
                ret.push_back(new LIANDUI(h, l));
            }
        }
    }
    return ret;
}

ostream& LIANDUI::output() {
    cout << "LIANDUI :";
    for (int i = head; i < head + length; i++) cout << " " << Pai::getName(i);
    return cout;
}

bool LIANDUI::operator>(Pai *pre) {
    switch (pre->type) {
        case PaiType::PASS_T: return true;
        case PaiType::LIANDUI_T: {
            LIANDUI *pre_ = dynamic_cast<LIANDUI *>(pre);
            if (this->length != pre_->length) return false;
            return this->head > pre_->head;
        }
        default: return false;
    }
}

void LIANDUI::take(int *arr) {
    for (int i = this->head, I = this->head + this->length; i < I; i++) arr[i] -= 2;
}
void LIANDUI::back(int *arr) {
    for (int i = this->head, I = this->head + this->length; i < I; i++) arr[i] += 2;
}