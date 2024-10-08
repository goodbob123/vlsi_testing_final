#include <time.h>
#include <algorithm>
#include <unordered_map>
#include <climits>
#include <chrono>
#include <cmath>
#include <cassert>
#include "atpg.h"


void ATPG::data_compress() {
    cerr << "-----------------------" << endl;
    cerr << "[ Data compress start ]" << endl;
    int DC_choice = 1;
    bool sort_flag = true;
    int no_improve_threshold = 5;
    srand(time(NULL));
    auto start = chrono::high_resolution_clock::now();

    // reset fault list
    for (auto &f : flist) {
        f->detect = false;
        f->activate = false;
        f->detected_time = 0;
    }
    flist_undetect.clear();
    for (auto &f : flist) {
        flist_undetect.push_front(f.get());
    }


    // print settings
    cerr << "DC_choice: " << DC_choice << endl;
    cerr << "sort_flag: " << sort_flag << endl;
    cerr << "no_improve_threshold: " << no_improve_threshold << endl;

    // Calc the origin fault coverage
    int current_detect_num = 0;
    int total_detect_num = 0;

    for (int i = vectors.size() - 1; i >= 0; i--) {
        bool is_redundant_vector = true;
        tdfault_sim_a_vector(vectors[i], current_detect_num, is_redundant_vector);
        total_detect_num += current_detect_num;
    }

    int origin_vector_size = vectors.size();
    double origin_fault_coverage = (double)total_detect_num / (double)num_of_tdf_fault * 100;
    int origin_detect_num = total_detect_num;

    auto find_list_size = [](forward_list<ATPG::FAULT *> &list) {
        return distance(list.begin(), list.end());
    };

    // heuristic method (I): if the fault is not be detected in the end.
    // then it is a redundant fault, we can remove it from flist_undetect.
    for (auto &f : flist) {
        f->atpg_detected = (f->detect == true) ? true : false;
    }

    // (0) (optional) sort the fault list by detected faults num
    if (sort_flag) {
        cerr << "---- sort fault list -----" << endl;
        unordered_map<int, int> pattern_score;
        int max_score = 0;
        for (int i = 0; i < vectors.size(); ++i) { 
            get_pattern_score(vectors[i], pattern_score[i]);
            reset_flist_undetect();
            max_score = max(max_score, pattern_score[i]);
        }
        cerr << "max score: " << max_score << endl;
        vector<vector<string>> new_vectors(max_score + 1);
        for (auto &p : pattern_score) {
            new_vectors[p.second].push_back(vectors[p.first]);
        }
        vectors.clear();
        for (int i = 0; i <= max_score; ++i) {
            for (auto &v : new_vectors[i]) {
                vectors.push_back(v);
            }
        }
        cerr<<"sort fault list finish, time = "<<(chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count())/1000.0<<endl;
    }
    // (1) ROFS
    if (DC_choice == 1) {
        int no_improve = 0;
        bool improved = false;
        cerr << "---- ROFS start -----" << endl;
        // for multiple times with shuffling vectors
        for (int ite = 0; ite < 100; ++ite) {
            improved = false;
            // cerr << "ROFS round " << ite << " vectors size: " << vectors.size() << endl;
            reset_flist_undetect();

            current_detect_num = 0;
            total_detect_num = 0;

            /* for every vector */
            for (int i = vectors.size() - 1; i >= 0; i--) {
                bool is_redundant_vector = true;
                tdfault_sim_a_vector(vectors[i], current_detect_num, is_redundant_vector);
                if (is_redundant_vector) {
                    vectors.erase(vectors.begin() + i);
                    improved = true;
                    continue;
                }
                total_detect_num += current_detect_num;
            }

            // terminate condition
            no_improve = improved ? 0 : no_improve + 1;
            if (no_improve >= no_improve_threshold) {
                break;
            }
            random_shuffle(vectors.begin(), vectors.end());
        }
    cerr<<"ROFS finish, time = "<<(chrono::duration_cast<std::chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count())/1000.0<<endl;
    }

    if (DC_choice == 2) {
        cerr << "---- RVE start -----" << endl;
        // (2) Redundant Vector Elimination (RVE)
        int no_improve = 0;
        bool improved = false;
        for (int ite = 0; ite < 100; ++ite) {
            improved = false;
            reset_flist_undetect();
            vector<string> _double_comfirm_vectors;

            // (2-1) collect the double comfirm vectors
            // there are 2 traversing way, 0 -> size-1, size-1 ->0
            total_detect_num = 0;
            for (int i = 0; i < vectors.size(); i++) {
                // if the vector detects redundant fault, then it is a double comfirm vector, temp remove it
                tdfault_RVE_sim_a_vector(vectors[i], current_detect_num);
                if (current_detect_num == 0) {
                    _double_comfirm_vectors.push_back(vectors[i]);
                    vectors.erase(vectors.begin() + i);
                    --i;
                }
                total_detect_num += current_detect_num;
            }
            // cerr << "double comfirm vectors size: " << _double_comfirm_vectors.size() << endl;

            // (2-2) check the double comfirm vectors, let fault coverage increase
            for (int i = _double_comfirm_vectors.size() - 1; i >= 0; --i) {
                bool is_redundant_vector = true;
                tdfault_sim_a_vector(_double_comfirm_vectors[i], current_detect_num, is_redundant_vector);
                if (is_redundant_vector == true) {
                    continue;
                }
                vectors.push_back(_double_comfirm_vectors[i]);
                total_detect_num += current_detect_num;
                if (total_detect_num == origin_detect_num) {
                    // cerr << "double comfirm vectors check finish" << endl;
                    break;
                }
            }
            // terminate condition
            no_improve = improved ? 0 : no_improve + 1;
            if (no_improve >= no_improve_threshold) {
                break;
            }
            random_shuffle(vectors.begin(), vectors.end());
        }
    }
    // for print result
    cerr << "total fault: " << num_of_tdf_fault << endl;
    cerr << "origin vectors size: " << origin_vector_size << endl;
    cerr << "origin fault coverage: " << origin_fault_coverage << "%" << endl;
    cerr << "after compress vectors size: " << vectors.size() << endl;
    cerr << "total_detect_num: " << total_detect_num << endl;
    cerr << "after compress coverage: " << (double)total_detect_num / (double)num_of_tdf_fault * 100 << "%" << endl;
    cerr << "compress ratio:" << (double)origin_vector_size / (double)vectors.size() << endl;

    // for check
    // cerr << "---- check correctness -----" << endl;
    // reset_flist_undetect();
    // current_detect_num = 0;
    // total_detect_num = 0;
    // for (int i = vectors.size() - 1; i >= 0; i--) {
    //     bool is_redundant_vector = true;
    //     tdfault_sim_a_vector(vectors[i], current_detect_num, is_redundant_vector);
    //     total_detect_num += current_detect_num;
    // }
    // if (total_detect_num != origin_detect_num) {
    //     cerr << "error: compress fault coverage is WRONG" << endl;
    // }else{
    //     cerr << "compress fault coverage is CORRECT (not FC dropping)" << endl;
    // }

    cerr << "-----------------------" << endl;
}

// reset flist_undetect to sim fault again
void ATPG::reset_flist_undetect() {
    flist_undetect.clear();
    for (auto &f : flist) {
        if (f->atpg_detected == true)
            flist_undetect.push_front(f.get());
    }
    for (auto &f : flist) {
        f->activate = false;
        f->detect = false;
        f->detected_time = 0;
    }
}

void ATPG::get_pattern_score(const string &vec, int &score) {
    score = 0;
    tdfault_sim_a_vector_for_detect_once(vec, score);
}
