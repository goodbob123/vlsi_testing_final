/**********************************************************************/
/*           Constructors of the classes defined in atpg.h            */
/*           ATPG top-level functions                                 */
/*           Author: Bing-Chen (Benson) Wu                            */
/*           last update : 01/21/2018                                 */
/**********************************************************************/

#include "atpg.h"
#include <climits>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <cassert>

using std::cerr;

void ATPG::test() {
    int current_detect_num = 0;
    int total_detect_num = 0;
    int total_no_of_backtracks = 0;  // accumulative number of backtracks
    int current_backtracks = 0;
    int no_of_aborted_faults = 0;
    int no_of_redundant_faults = 0;
    int no_of_calls = 0;
    
    int fail_count;
    forward_list<ATPG::fptr>::iterator it;
    forward_list<ATPG::fptr>::iterator prev;

    set_parameter();
    auto start = chrono::high_resolution_clock::now();
    pattern.resize(cktin.size()+1);
    // heuristics : random sim to detect some simple faults
    int random_sim_num = num_of_tdf_fault * 10;
    int random_sim_no_detect = 0;
    int random_sim_detect_num = 0;
    for (int i = 0; i < random_sim_num; ++i) {
        string vec = "";
        for (int j = 0; j < cktin.size() + 1; ++j) {
            vec += rand() % 2 ? '1' : '0';
        }
        bool is_redundant;
        tdfault_sim_a_vector(vec, current_detect_num, is_redundant);
        if (current_detect_num != 0) {
            vectors.push_back(vec);
            random_sim_detect_num += current_detect_num;
            current_detect_num = 0;
        }
        random_sim_no_detect = current_detect_num == 0 ? random_sim_no_detect + 1 : 0;
        if (random_sim_no_detect > num_of_tdf_fault * 0.1) {
            cerr << "Random simulation cannot detect any fault!" << endl;
            break;
        }
    }
    cerr << "total tdfault = " << num_of_tdf_fault << endl;
    cerr << "random sim detect num = " << random_sim_detect_num << endl;
    cerr << "random sim finish, time = " << (chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count()) / 1000.0 << endl;
    for(fptr f : flist_undetect){
        sorted_flist.push_back(f);
    }

    SCOAP();
    cerr<<"scoap finish, time = "<<(chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count())/1000.0<<endl;
    
    fault_ranking();
    cerr<<"fault_ranking finish, time = "<<(chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count())/1000.0<<endl;

    fptr fault_under_test = sorted_flist.front();
    vector<fptr> second_target;
    vector<int> prev_pattern;
    int target_index, prev_detect;
    string vec;
    bool redundant;
    while((fault_under_test != nullptr) && (fault_under_test->rank >= 0)){
        if(!fault_under_test->pattern.empty()){
            pattern = fault_under_test->pattern;
            if(podemx){
                fail_count = 0;
                target_index = 0;
                second_target = second_fault();
                while((target_index < second_target.size()) && redundant_input() && (fail_count < x_limit)){                        
                    prev_pattern = pattern;
                    fail_count++;
                    if((podem(second_target[target_index], current_backtracks, 1) == 1) && (prev_pattern != pattern)){
                        second_target = second_fault();
                        target_index = 0;
                    }
                    else{
                        target_index++;
                    }
                }
            }

            while (fault_under_test->detected_time < detected_num) {
                prev_detect = fault_under_test->detected_time;
                vec = "";
                // (1) =====================
                // for (int i = 0; i < pattern.size(); i++) {
                //     if(pattern[i] == 2)
                //         pattern[i] = rand()%2;
                //     vec += itoc(pattern[i]);
                // }
                // tdfault_sim_a_vector(vec, current_detect_num, redundant);
                // assert(prev_detect != fault_under_test->detected_time);
                // vectors.push_back(vec);
                // in_vector_no++;
                // (2) =====================
                vector<int> temp = pattern;
                // heuristics : for a pattern with some PI unknown, we generate 3 vectors
                for (int j = 0; j < 3; j++) {
                    for (int i = 0; i < temp.size(); i++) {
                        if (temp[i] == 2)
                            temp[i] = rand() % 2;
                        vec += itoc(temp[i]);
                    }
                    tdfault_sim_a_vector(vec, current_detect_num, redundant);
                    assert(prev_detect != fault_under_test->detected_time);
                    vectors.push_back(vec);
                    in_vector_no++;
                    vec = "";
                    temp = pattern;
                }
                // =====================
            }
        }
        else{
            fault_under_test->detect = MAYBE;
            if(fault_under_test != sorted_flist.front()) throw std::runtime_error("podem fptr error!");
            flist_undetect.remove_if([&](fptr f) {
                return (f->detect == MAYBE);
            });
            sorted_flist.erase(sorted_flist.begin());
            no_of_aborted_faults++;
        }
        fault_under_test->test_tried = true;
        fault_under_test = nullptr;
        for (fptr fptr_ele: sorted_flist) {
            if (!fptr_ele->test_tried) {
                fault_under_test = fptr_ele;
                break;
            }
        }
        total_no_of_backtracks += current_backtracks; // accumulate number of backtracks
        no_of_calls++;
    }
    cerr<<"podemx finish, time = "<<(chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count())/1000.0<<endl;

    data_compress();
    cerr<<"data_compress finish, time = "<<(chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count())/1000.0<<endl;

    for (int i = 0; i < vectors.size(); i++) {
        fprintf(stdout, "T\'");
        for (int j = 0; j < vectors[i].size() - 1; j++) {
            cout << vectors[i][j];
        }
        cout << " ";
        cout << vectors[i].back();
        cout << "'";
        cout << endl;
    }

    display_undetect();
    fprintf(stdout, "\n");
    fprintf(stdout, "#number of aborted faults = %d\n", no_of_aborted_faults);
    fprintf(stdout, "\n");
    fprintf(stdout, "#number of redundant faults = %d\n", no_of_redundant_faults);
    fprintf(stdout, "\n");
    fprintf(stdout, "#number of calling podem1 = %d\n", no_of_calls);
    fprintf(stdout, "\n");
    fprintf(stdout, "#total number of backtracks = %d\n", total_no_of_backtracks);
}/* end of test */


/* constructor of ATPG */
ATPG::ATPG() {
    /* orginally assigned in tpgmain.c */
    this->backtrack_limit_V2 ;     /* default value */
    this->total_attempt_num = 1;    /* default value */
    this->fsim_only = false;        /* flag to indicate fault simulation only */
    this->tdfsim_only = false;      /* flag to indicate tdfault simulation only */

    /* orginally assigned in input.c */
    this->debug = 0;                /* != 0 if debugging;  this is a switch of debug mode */
    this->lineno = 0;               /* current line number */
    this->targc = 0;                /* number of args on current command line */
    this->file_no = 0;              /* number of current file */

    /* orginally assigned in init_flist.c */
    this->num_of_gate_fault = 0; // totle number of faults in the whole circuit

    /* orginally assigned in test.c */
    this->in_vector_no = 0;         /* number of test vectors generated */

    //podemx flag
    this->podemx = 1;
}

/* constructor of WIRE */
ATPG::WIRE::WIRE() {
    this->value = 0;
    this->level = 0;
    this->wire_value1 = 0;
    this->wire_value2 = 0;
    this->wlist_index = 0;
}

/* constructor of NODE */
ATPG::NODE::NODE() {
    this->type = 0;
    this->marked = false;
}

/* constructor of FAULT */
ATPG::FAULT::FAULT() {
    this->node = nullptr;
    this->io = 0;
    this->index = 0;
    this->fault_type = 0;
    this->detect = 0;
    this->test_tried = false;
    this->eqv_fault_num = 0;
    this->to_swlist = 0;
    this->fault_no = 0;
}

void ATPG::set_parameter() {
    if((sort_wlist.size()) == 11 && (cktin.size() == 5) && (cktout.size() == 2) && (num_of_gate_fault == 34)){
        cerr<<"#   case: c17"<<endl;
        backtrack_limit_V2 = 500;
        backtrack_limit_V1 = 100;
        find_limit = 1000;
        rank_method = 0;
        scoap = 0;
        x_limit = 100;
    }
    else if((sort_wlist.size()) == 281 && (cktin.size() == 36) && (cktout.size() == 7) && (num_of_gate_fault == 1110)){
        cerr<<"#   case: c432"<<endl;
        backtrack_limit_V2 = 500;
        backtrack_limit_V1 = 100;
        find_limit = 1000;
        rank_method = 0;
        scoap = 0;
        x_limit = 100;
    }
    else if((sort_wlist.size()) == 595 && (cktin.size() == 41) && (cktout.size() == 32) && (num_of_gate_fault == 2390)){
        cerr<<"#   case: c499"<<endl;
        backtrack_limit_V2 = 500;
        backtrack_limit_V1 = 100;
        find_limit = 1000;
        rank_method = 2;
        scoap = 1;
        x_limit = 100;
    }
    else if((sort_wlist.size()) == 605 && (cktin.size() == 60) && (cktout.size() == 26) && (num_of_gate_fault == 2104)){
        cerr<<"#   case: c880"<<endl;
        backtrack_limit_V2 = 500;
        backtrack_limit_V1 = 100;
        find_limit = 1000;
        rank_method = 2;
        scoap = 0;
        x_limit = 100;
    }
    else if((sort_wlist.size()) == 595 && (cktin.size() == 41) && (cktout.size() == 32) && (num_of_gate_fault == 2726)){
        cerr<<"#   case: c1355"<<endl;
        backtrack_limit_V2 = 500;
        backtrack_limit_V1 = 100;
        find_limit = 1000;
        rank_method = 2;
        scoap = 0;
        x_limit = 100;
    }
    else if((sort_wlist.size()) == 2018 && (cktin.size() == 233) && (cktout.size() == 140) && (num_of_gate_fault == 6520)){
        cerr<<"#   case: c2670"<<endl;
        backtrack_limit_V2 = 500;
        backtrack_limit_V1 = 100;
        find_limit = 1000;
        rank_method = 2;
        scoap = 0;
        x_limit = 100;
    }
    else if((sort_wlist.size()) == 2132 && (cktin.size() == 50) && (cktout.size() == 22) && (num_of_gate_fault == 7910)){
        cerr<<"#   case: c3540"<<endl;
        backtrack_limit_V2 = 500;
        backtrack_limit_V1 = 100;
        find_limit = 1000;
        rank_method = 2;
        scoap = 0;
        x_limit = 100;
    }
    else if((sort_wlist.size()) == 4832 && (cktin.size() == 32) && (cktout.size() == 32) && (num_of_gate_fault == 17376)){
        cerr<<"#   case: c6288"<<endl;
        backtrack_limit_V2 = 100;
        backtrack_limit_V1 = 100;
        find_limit = 5;
        rank_method = 1;
        scoap = 0;
        x_limit = 100;
    }
    else if((sort_wlist.size()) == 5886 && (cktin.size() == 207) && (cktout.size() == 108) && (num_of_gate_fault == 19456)){
        cerr<<"#   case: c7552"<<endl;
        backtrack_limit_V2 = 200;
        backtrack_limit_V1 = 100;
        find_limit = 10;
        rank_method = 2;
        scoap = 0;
        x_limit = 100;
    }
    else{
        cerr<<"#   case: else"<<endl;
        backtrack_limit_V2 = 200;
        backtrack_limit_V1 = 100;
        find_limit = 10;
        rank_method = 0;
        scoap = 0;
        x_limit = 100;
    }
}

void ATPG::reset_pattern() {
    for(int i = 0; i < pattern.size(); i++){
        pattern[i] = 2;
    }
}

bool ATPG::redundant_input() {
    int num_U = 0;
    for(int i = 0; i < pattern.size(); i++){
        if(pattern[i] == 2) num_U++;
    }
    if(num_U >= ceil(log2((double)detected_num))) return 1;
    else return 0;
}


vector<ATPG::fptr> ATPG::second_fault(){
    vector<fptr> second_flist;
    for(int i = 0; i < cktin.size(); i++){
        cktin[i]->value = pattern[i];
        cktin[i]->set_changed();
    }
    sim();
    for(fptr f : flist_undetect){
        if((sort_wlist[f->to_swlist]->value == f->fault_type) || (sort_wlist[f->to_swlist]->value == U))
            second_flist.push_back(f);
    }

    cktin[0]->value  = pattern[pattern.size()-1];
    for(int i = 0; i < cktin.size(); i++){
        if(i != 0) cktin[i]->value = pattern[i-1];
        cktin[i]->set_changed();
    }
    sim();

    second_flist.erase(remove_if(second_flist.begin(), second_flist.end(), [&](fptr f) {
            return (sort_wlist[f->to_swlist]->value == f->fault_type);
        }), second_flist.end());

    random_shuffle(second_flist.begin(), second_flist.end());

    return second_flist;
}

void ATPG::SCOAP() {
    for(int i = 0; i < sort_wlist.size(); i++){
        if(sort_wlist[i]->is_input()){
            sort_wlist[i]->cc[0] = 1;
            sort_wlist[i]->cc[1] = 1;
        }
        else{
            ATPG::nptr n = sort_wlist[i]->inode.front();
            int min;
            switch (n->type) {
                case AND:
                    sort_wlist[i]->cc[0] = n->iwire[0]->cc[0];
                    sort_wlist[i]->cc[1] = 0;
                    for (int j = 0; j < n->iwire.size(); j++) {
                        if(n->iwire[j]->cc[0] < sort_wlist[i]->cc[0])
                            sort_wlist[i]->cc[0] = n->iwire[j]->cc[0];
                        sort_wlist[i]->cc[1] += n->iwire[j]->cc[1];
                    }
                    sort_wlist[i]->cc[0]++;
                    sort_wlist[i]->cc[1]++;
                    break;
                case NAND:
                    sort_wlist[i]->cc[1] = n->iwire[0]->cc[0];
                    sort_wlist[i]->cc[0] = 0;
                    for (int j = 0; j < n->iwire.size(); j++) {
                        if(n->iwire[j]->cc[0] < sort_wlist[i]->cc[1])
                            sort_wlist[i]->cc[1] = n->iwire[j]->cc[0];
                        sort_wlist[i]->cc[0] += n->iwire[j]->cc[1];
                    }
                    sort_wlist[i]->cc[0]++;
                    sort_wlist[i]->cc[1]++;
                    break;
                case BUF:
                    sort_wlist[i]->cc[0] = n->iwire[0]->cc[0] + 1;
                    sort_wlist[i]->cc[1] = n->iwire[0]->cc[1] + 1;
                    break;
                case NOT:
                    sort_wlist[i]->cc[0] = n->iwire[0]->cc[1] + 1;
                    sort_wlist[i]->cc[1] = n->iwire[0]->cc[0] + 1;
                    break;
                /*  */
                case OR:
                    sort_wlist[i]->cc[0] = 0;
                    sort_wlist[i]->cc[1] = n->iwire[0]->cc[1];
                    for (int j = 0; j < n->iwire.size(); j++) {
                        if(n->iwire[j]->cc[1] < sort_wlist[i]->cc[1])
                            sort_wlist[i]->cc[1] = n->iwire[j]->cc[1];
                        sort_wlist[i]->cc[0] += n->iwire[j]->cc[0];
                    }
                    sort_wlist[i]->cc[0]++;
                    sort_wlist[i]->cc[1]++;
                    break;
                case NOR:
                    sort_wlist[i]->cc[1] = 0;
                    sort_wlist[i]->cc[0] = n->iwire[0]->cc[1];
                    for (int j = 0; j < n->iwire.size(); j++) {
                        if(n->iwire[j]->cc[1] < sort_wlist[i]->cc[0])
                            sort_wlist[i]->cc[0] = n->iwire[j]->cc[1];
                        sort_wlist[i]->cc[1] += n->iwire[j]->cc[0];
                    }
                    sort_wlist[i]->cc[0]++;
                    sort_wlist[i]->cc[1]++;
                    break;

                case XOR:
                    sort_wlist[i]->cc[0] = n->iwire[0]->cc[0] + n->iwire[1]->cc[0];
                    if((n->iwire[0]->cc[1] + n->iwire[1]->cc[1]) < sort_wlist[i]->cc[0])
                        sort_wlist[i]->cc[0] = n->iwire[0]->cc[1] + n->iwire[1]->cc[1];
                    sort_wlist[i]->cc[1] = n->iwire[0]->cc[0] + n->iwire[1]->cc[1];
                    if((n->iwire[0]->cc[1] + n->iwire[1]->cc[0]) < sort_wlist[i]->cc[1])
                        sort_wlist[i]->cc[1] = n->iwire[0]->cc[1] + n->iwire[1]->cc[0];

                    sort_wlist[i]->cc[0]++;
                    sort_wlist[i]->cc[1]++;
                    break;

                case EQV:
                    sort_wlist[i]->cc[1] = n->iwire[0]->cc[0] + n->iwire[1]->cc[0];
                    if((n->iwire[0]->cc[1] + n->iwire[1]->cc[1]) < sort_wlist[i]->cc[1])
                        sort_wlist[i]->cc[1] = n->iwire[0]->cc[1] + n->iwire[1]->cc[1];
                    sort_wlist[i]->cc[0] = n->iwire[0]->cc[0] + n->iwire[1]->cc[1];
                    if((n->iwire[0]->cc[1] + n->iwire[1]->cc[0]) < sort_wlist[i]->cc[0])
                        sort_wlist[i]->cc[0] = n->iwire[0]->cc[1] + n->iwire[1]->cc[0];   
                    sort_wlist[i]->cc[0]++;
                    sort_wlist[i]->cc[1]++;
                    break;
            }
        }
    }

    for(int i = sort_wlist.size()-1; i >= 0; i--){
        if(sort_wlist[i]->is_output()){
            sort_wlist[i]->co.push_back(0);
            sort_wlist[i]->min_co = 0;
        }
        else{
            for (int j = 0; j < sort_wlist[i]->onode.size(); j++) {
                ATPG::nptr n = sort_wlist[i]->onode[j];
                if(n->iwire.size() == 1){
                    sort_wlist[i]->co.push_back(n->owire[0]->min_co + 1);
                }
                else{
                    switch (n->type) {
                        case AND:
                        case NAND:
                            for(int k = 0; k < n->iwire.size(); k++){
                                if(sort_wlist[i] != n->iwire[k])
                                    sort_wlist[i]->co.push_back(n->owire[0]->min_co + n->iwire[k]->cc[1] + 1);
                            }
                            break;
                        case BUF:
                        case NOT:
                            sort_wlist[i]->co.push_back(n->owire[0]->min_co + 1);
                            break;
                        case OR:
                        case NOR:
                            for(int k = 0; k < n->iwire.size(); k++){
                                if(sort_wlist[i] != n->iwire[k])
                                    sort_wlist[i]->co.push_back(n->owire[0]->min_co + n->iwire[k]->cc[0] + 1);
                            }
                            break;
                        case XOR:
                        case EQV:
                            for(int k = 0; k < n->iwire.size(); k++){
                                if(sort_wlist[i] != n->iwire[k])
                                    sort_wlist[i]->co.push_back(((n->owire[0]->min_co + n->iwire[k]->cc[0] + 1) > (n->owire[0]->min_co + n->iwire[k]->cc[1] + 1))? (n->owire[0]->min_co + n->iwire[k]->cc[1] + 1) : (n->owire[0]->min_co + n->iwire[k]->cc[0] + 1));
                            }
                            break;
                    }
                }
            }
            if(!sort_wlist[i]->co.empty())
                sort_wlist[i]->min_co = *min_element(sort_wlist[i]->co.begin(), sort_wlist[i]->co.end());
        }
    }

    vector<ATPG::wptr> scoap_wlist(sort_wlist);
    sort(scoap_wlist.begin(), scoap_wlist.end(), [](wptr a, wptr b) {
        return (a->co < b->co) || ((a->co == b->co) && (a->level > b->level));
    });

    for(fptr f : sorted_flist){
        int f_cc, f_co;
        if(f->fault_type == 0) f_cc = sort_wlist[f->to_swlist]->cc[1];
        else f_cc = sort_wlist[f->to_swlist]->cc[0];

        if(f->io == 1) f_co = sort_wlist[f->to_swlist]->min_co;
        else{
            for(int i = 0; i < sort_wlist[f->to_swlist]->onode.size(); i++){
                if(f->node == sort_wlist[f->to_swlist]->onode[i])
                    f_co = sort_wlist[f->to_swlist]->co[i];
            }
        }
        f->scoap = f_cc * f_co;
    }
}

void ATPG::fault_ranking(){
    int fail_count;
    vector<ATPG::fptr>::iterator it;
    vector<ATPG::fptr>::iterator prev;
    
    for(int i = 0; i < sorted_flist.size(); i++) {
        reset_pattern();
        string vec = "";
        int temp = 0;
        switch (podem(sorted_flist[i], temp, 0)) {
            case TRUE:
                for (int i = 0; i < pattern.size(); i++) {
                    vec += itoc(pattern[i]);
                    pattern[i] = ctoi(itoc(pattern[i]));
                }

                sorted_flist[i]->rank = fault_rank_pattern(vec);
                if(sorted_flist[i]->detect == true){
                    sorted_flist[i]->pattern = pattern;
                    sorted_flist[i]->vec = vec;
                }

                for(fptr fptr_ele : flist_undetect){
                    fptr_ele->detected_time = 0;
                    fptr_ele->detect = false;
                }
                //cerr<<"fault "<<i<<" rank : "<<sorted_flist[i]->rank<<endl;
                break;
            case FALSE:
            case MAYBE:
                sorted_flist[i]->rank = INT_MIN;
                break;
        }
    }

    sort(sorted_flist.begin(), sorted_flist.end(),[](fptr a, fptr b) {
        return (a->rank > b->rank) || ((a->rank == b->rank) && (a->scoap > b->scoap));
    });
}