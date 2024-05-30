/**********************************************************************/
/*           Constructors of the classes defined in atpg.h            */
/*           ATPG top-level functions                                 */
/*           Author: Bing-Chen (Benson) Wu                            */
/*           last update : 01/21/2018                                 */
/**********************************************************************/

#include "atpg.h"
#include <climits>

void ATPG::test() {
    string vec;
    int current_detect_num = 0;
    int total_detect_num = 0;
    int total_no_of_backtracks = 0;  // accumulative number of backtracks
    int current_backtracks = 0;
    int no_of_aborted_faults = 0;
    int no_of_redundant_faults = 0;
    int no_of_calls = 0;

    fptr fault_under_test = flist_undetect.front();

    /* stuck-at fault sim mode */
    if (fsim_only) {
        fault_simulate_vectors(total_detect_num);
        in_vector_no += vectors.size();
        display_undetect();
        fprintf(stdout, "\n");
        return;
    }// if fsim only

    /* transition fault sim mode */
    if (tdfsim_only) {
        return;
    }  // if fsim only

    /* test generation mode */
    /* Figure 5 in the PODEM paper */
    int fail_count;
    forward_list<ATPG::fptr>::iterator it;
    forward_list<ATPG::fptr>::iterator prev;
    SCOAP();
    while (fault_under_test != nullptr) {
        string vec = "";
        switch (podem(fault_under_test, current_backtracks, 0)) {
            case TRUE:
                /* form a vector */
                // vec.clear();
                // for (int i : pattern) {
                //     vec.push_back(itoc(i));
                // }
                fault_under_test->detect = TRUE;
                flist_undetect.remove(fault_under_test);
                in_vector_no++;

                prev = flist_undetect.before_begin();
                it = flist_undetect.begin();
                fail_count = 0;
                
                while((it != flist_undetect.end()) && redundant_input() && (fail_count < 2000)){
                    //find secondary fault
                    if(podem(*it, current_backtracks, 1) == 1){
                        fail_count = 0;
                        (*it)->detect = TRUE;
                        it = flist_undetect.erase_after(prev);
                    }
                    else{
                        prev = it;
                        it++;
                        fail_count++;
                    }
                }

                // fprintf(stdout, "T\'");
                for (int i = 0; i < pattern.size(); i++) {
                    vec += itoc(pattern[i]);
                }
                vectors.push_back(vec);
                // fprintf(stdout, "'");
                // fprintf(stdout, "\n");

                /*by defect, we want only one pattern per fault */
                /*run a fault simulation, drop ALL detected faults */
                // if (total_attempt_num == 1) {
                //     tdfault_sim_a_vector(vec, current_detect_num);
                //     total_detect_num += current_detect_num;
                // }
                    /* If we want mutiple petterns per fault,
                     * NO fault simulation.  drop ONLY the fault under test */
                // else {
                //     fault_under_test->detect = TRUE;
                //     /* drop fault_under_test */
                //     flist_undetect.remove(fault_under_test);
                // }
                break;

            case FALSE:
                fault_under_test->detect = REDUNDANT;
                flist_undetect.remove(fault_under_test);
                no_of_redundant_faults++;
                break;

            case MAYBE:
                flist_undetect.remove(fault_under_test);
                no_of_aborted_faults++;
                break;
        }
        fault_under_test->test_tried = true;
        fault_under_test = nullptr;
        for (fptr fptr_ele: flist_undetect) {
            if (!fptr_ele->test_tried) {
                fault_under_test = fptr_ele;
                break;
            }
        }
        total_no_of_backtracks += current_backtracks; // accumulate number of backtracks
        no_of_calls++;
    }

    data_compress();
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
    this->backtrack_limit = 50;     /* default value */
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

bool ATPG::redundant_input() {
    for(int i = 0; i < pattern.size(); i++){
        if(pattern[i] == 2) return 1;
    }
    return 0;
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
            sort_wlist[i]->co = 0;
        }
        else{
            sort_wlist[i]->co = INT_MAX;
            int temp;
            for (int j = 0; j < sort_wlist[i]->onode.size(); j++) {
                ATPG::nptr n = sort_wlist[i]->onode[j];
                switch (n->type) {
                    case AND:
                    case NAND:
                        for(int i = 0; i < n->iwire.size(); i++){
                            if(sort_wlist[i] != n->iwire[i])
                                temp = n->owire[0]->co + n->iwire[i]->cc[1] + 1;
                        }
                        break;
                    case BUF:
                    case NOT:
                        temp = n->owire[0]->co + 1;
                        break;
                    case OR:
                    case NOR:
                        for(int i = 0; i < n->iwire.size(); i++){
                            if(sort_wlist[i] != n->iwire[i])
                                temp = n->owire[0]->co + n->iwire[i]->cc[0] + 1;
                        }
                        break;
                    case XOR:
                    case EQV:
                        for(int i = 0; i < n->iwire.size(); i++){
                            if(sort_wlist[i] != n->iwire[i])
                                temp = ((n->owire[0]->co + n->iwire[i]->cc[0] + 1) > (n->owire[0]->co + n->iwire[i]->cc[1] + 1))? (n->owire[0]->co + n->iwire[i]->cc[1] + 1) : (n->owire[0]->co + n->iwire[i]->cc[0] + 1);
                        }
                        break;
                }
                sort_wlist[i]->co = (sort_wlist[i]->co > temp)?temp : sort_wlist[i]->co;
            }
        }
    }

    // for(int i = 0; i < sort_wlist.size(); i++){
    //     cout<<"wire "<<i<<" : "<<sort_wlist[i]->cc[0]<<", "<<sort_wlist[i]->cc[1]<<", "<<sort_wlist[i]->co<<endl;
    // }
}