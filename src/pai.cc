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

    t = FEIJI::get(arr);
    ret.insert(ret.end(), t.begin(), t.end());

    t = SIDAIER::get(arr);
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

FEIJI::FEIJI(int head, int length, int wingType, const vector<int>& wings) 
    : Pai(PaiType::FEIJI_T), head(head), length(length), wingType(wingType), wings(wings) {}

// Move DFS struct outside of function or make it a helper function
// Since local classes cannot have static member functions in C++11 (it's allowed but scope is tricky)
// Let's just make a recursive helper function outside.

static void findWingsRecursive(int startVal, int countNeeded, int type, int *arr, int h, int len, 
                vector<int>& current, vector<vector<int>>& out) {
    if (countNeeded == 0) {
        out.push_back(current);
        return;
    }
    if (startVal > 17) return;
    
    // Try taking k instances of startVal
    int avail = arr[startVal];
    if (startVal >= h && startVal < h + len) avail -= 3;
    int unit = (type == 2 ? 2 : 1);
    
    if (avail < 0) avail = 0; // Safety
    int maxTake = avail / unit;
    
    // Iterate k from 0 to maxTake
    // But we need exactly countNeeded.
    // Optimization: Don't take more than countNeeded
    for (int k = 0; k <= maxTake && k <= countNeeded; k++) {
        // Add k instances
        for (int x = 0; x < k; x++) current.push_back(startVal);
        
        findWingsRecursive(startVal + 1, countNeeded - k, type, arr, h, len, current, out);
        
        // Backtrack
        for (int x = 0; x < k; x++) current.pop_back();
    }
}

vector<Pai *> FEIJI::get(int *arr) {
    vector<Pai *> ret;
    
    // Body length: at least 2
    // Max length: 12 (3 to A is 12 cards)
    for (int len = 2; len <= 6; len++) { // Max 6*3=18 cards
        for (int h = 3, H = 14 - len + 1; h <= H; h++) {
            // Check body validity
            bool validBody = true;
            for (int k = h; k < h + len; k++) {
                if (arr[k] < 3) {
                    validBody = false;
                    break;
                }
            }
            if (!validBody) continue;
            
            // 1. No wings
            ret.push_back(new FEIJI(h, len, 0, {}));
            
            // 2. Wings: Singles (type 1)
            vector<vector<int>> wingsList;
            vector<int> current;
            findWingsRecursive(3, len, 1, arr, h, len, current, wingsList);
            
            for (auto& w : wingsList) {
                ret.push_back(new FEIJI(h, len, 1, w));
            }
            
            // 3. Wings: Pairs (type 2)
            vector<vector<int>> wingsListDui;
            vector<int> currentDui;
            findWingsRecursive(3, len, 2, arr, h, len, currentDui, wingsListDui);
            
            for (auto& w : wingsListDui) {
                ret.push_back(new FEIJI(h, len, 2, w));
            }
        }
    }
    return ret;
}

ostream& FEIJI::output() {
    cout << "FEIJI " << Pai::getName(head) << "-" << length;
    if (wingType == 1) {
        cout << " WITH DAN";
        for (int w : wings) cout << " " << Pai::getName(w);
    } else if (wingType == 2) {
        cout << " WITH DUI";
        for (int w : wings) cout << " " << Pai::getName(w);
    }
    return cout;
}

bool FEIJI::operator>(Pai *pre) {
    switch (pre->type) {
        case PaiType::PASS_T: return true;
        case PaiType::FEIJI_T: {
            FEIJI *pre_ = dynamic_cast<FEIJI *>(pre);
            if (this->length != pre_->length) return false;
            if (this->wingType != pre_->wingType) return false; // Strict type match
            return this->head > pre_->head;
        }
        default: return false;
    }
}

void FEIJI::take(int *arr) {
    // Body
    for (int i = head; i < head + length; i++) arr[i] -= 3;
    // Wings
    int unit = (wingType == 2 ? 2 : (wingType == 1 ? 1 : 0));
    if (unit > 0) {
        for (int w : wings) arr[w] -= unit;
    }
}

void FEIJI::back(int *arr) {
    // Body
    for (int i = head; i < head + length; i++) arr[i] += 3;
    // Wings
    int unit = (wingType == 2 ? 2 : (wingType == 1 ? 1 : 0));
    if (unit > 0) {
        for (int w : wings) arr[w] += unit;
    }
}

SIDAIER::SIDAIER(int x, int d1, int d2, bool isDui) : Pai(PaiType::SIDAIER_T), x(x), d1(d1), d2(d2), isDui(isDui) {}

vector<Pai *> SIDAIER::get(int *arr) {
    vector<Pai *> ret;
    for (int i = 3; i <= 15; i++) { // Main card: 3 to 2. No Jokers for main card of 4-of-a-kind typically
        if (arr[i] < 4) continue;
        
        // With 2 single cards
        for (int j = 3; j <= 17; j++) {
            // Cannot take Joker as attachment according to user requirement
            // "Note: cannot take Big/Small Joker"
            if (j >= 16) continue; 
            
            if (j == i) continue;
            if (arr[j] < 1) continue;
            
            for (int k = j; k <= 17; k++) {
                if (k >= 16) continue; // No Jokers
                if (k == i) continue;
                if (arr[k] < 1) continue;
                if (j == k && arr[k] < 2) continue;
                
                ret.push_back(new SIDAIER(i, j, k, false));
            }
        }
        
        // With 2 pairs
        for (int j = 3; j <= 17; j++) {
            if (j >= 16) continue; // No Jokers
            if (j == i) continue;
            if (arr[j] < 2) continue;
            
            for (int k = j; k <= 17; k++) { // Pairs can be same? Typically not. "Two pairs".
                // If pairs are same, it's 4 cards. 4帶4? 
                // Usually "Two Pairs" implies distinct pairs.
                // Let's assume distinct pairs for now to be safe, or allow same if arr has 4.
                // Let's start k from j to allow same pairs if available (e.g. 4444 + 33 + 33).
                if (k >= 16) continue; // No Jokers
                if (k == i) continue;
                if (arr[k] < 2) continue;
                if (j == k && arr[k] < 4) continue; 
                
                ret.push_back(new SIDAIER(i, j, k, true));
            }
        }
    }
    return ret;
}

ostream& SIDAIER::output() {
    cout << "SIDAI " << Pai::getName(x) << " WITH " << (isDui ? "DUI " : "DAN ") << Pai::getName(d1) << " " << Pai::getName(d2);
    return cout;
}

bool SIDAIER::operator>(Pai *pre) {
    switch (pre->type) {
        case PaiType::PASS_T: return true;
        case PaiType::SIDAIER_T: {
            SIDAIER *pre_ = dynamic_cast<SIDAIER *>(pre);
            if (this->isDui != pre_->isDui) return false;
            return this->x > pre_->x;
        }
        default: return false;
    }
}

void SIDAIER::take(int *arr) {
    arr[x] -= 4;
    if (isDui) {
        arr[d1] -= 2;
        arr[d2] -= 2;
    } else {
        arr[d1]--;
        arr[d2]--;
    }
}

void SIDAIER::back(int *arr) {
    arr[x] += 4;
    if (isDui) {
        arr[d1] += 2;
        arr[d2] += 2;
    } else {
        arr[d1]++;
        arr[d2]++;
    }
}