#include "atpg.h"
#include <cassert>
void ATPG::init_reach() {
    size_t num_in = cktin.size();

    for (size_t i = 0; i < num_in; i++) {
        // pi : ---, have rc0, rc1, ro same
        wptr pi = cktin[i];
        pi->_rc0.push_back(new REACH(num_in));
        pi->_rc0[0]->set(i, 0);
        pi->_rc1.push_back(new REACH(num_in));
        pi->_rc1[0]->set(i, 1);
        pi->backup_rc();
    }

    for (size_t i = 0; i < sort_wlist.size(); i++) {
        wptr w = sort_wlist[i];
        nptr n = w->inode[0];
        if (n->type == INPUT) continue; // input rc is done

        rptr w_rc0 = new REACH(num_in);
        rptr w_rc1 = new REACH(num_in);
        w->_rc0.push_back(w_rc0);
        w->_rc1.push_back(w_rc1);

        wptr w_fi1 = n->iwire[0];
        rptr rc0_fi1 = w_fi1->_rc0[w_fi1->get_reach_id(n)];
        rptr rc1_fi1 = w_fi1->_rc1[w_fi1->get_reach_id(n)];
        wptr w_fi2 = n->iwire.size() > 1 ? n->iwire[1] : 0;
        rptr rc0_fi2 = w_fi2 != 0 ? w_fi2->_rc0[w_fi2->get_reach_id(n)] : 0;
        rptr rc1_fi2 = w_fi2 != 0 ? w_fi2->_rc1[w_fi2->get_reach_id(n)] : 0;

        switch (n->type) {
            case AND:
                *w_rc0 |= (w_fi2 == 0 || rc0_fi1->size() < rc0_fi2->size() ? rc0_fi1->getV1() : rc0_fi2->getV1());
                *w_rc1 |= rc1_fi1->getV1();
                if (w_fi2 != 0) *w_rc1 |= rc1_fi2->getV1();
                break;
            case BUF:
                *w_rc0 |= rc0_fi1->getV1();
                *w_rc1 |= rc1_fi1->getV1();
                break;
            case NAND:
                *w_rc1 |= (w_fi2 == 0 || rc0_fi1->size() < rc0_fi2->size() ? rc0_fi1->getV1() : rc0_fi2->getV1());
                *w_rc0 |= rc1_fi1->getV1();
                if (w_fi2 != 0) *w_rc0 |= rc1_fi2->getV1();
                break;
            case OR:
                *w_rc0 |= rc0_fi1->getV1();
                if (w_fi2 != 0) *w_rc0 |= rc0_fi2->getV1();
                *w_rc1 |= (w_fi2 == 0 || rc1_fi1->size() < rc1_fi2->size() ? rc1_fi1->getV1() : rc1_fi2->getV1());
                break;
            case NOR:
                *w_rc1 |= rc0_fi1->getV1();
                if (w_fi2 != 0) *w_rc1 |= rc0_fi2->getV1();
                *w_rc0 |= (w_fi2 == 0 || rc1_fi1->size() < rc1_fi2->size() ? rc1_fi1->getV1() : rc1_fi2->getV1());
                break;
            case NOT:
                *w_rc1 |= rc0_fi1->getV1();
                *w_rc0 |= rc1_fi1->getV1();
                break;
            case XOR:
                if (w_fi2 == 0) {
                    *w_rc0 |= (rc0_fi1->size() < rc1_fi1->size() ? rc0_fi1->getV1() : rc1_fi1->getV1());
                    *w_rc1 |= (rc0_fi1->size() < rc1_fi1->size() ? rc0_fi1->getV1() : rc1_fi1->getV1());
                } else {
                    if (rc0_fi1->size() + rc0_fi2->size() < rc1_fi1->size() + rc1_fi2->size()) {
                        *w_rc0 |= rc0_fi1->getV1();
                        *w_rc0 |= rc0_fi2->getV1();
                    } else {
                        *w_rc0 |= rc1_fi1->getV1();
                        *w_rc0 |= rc1_fi2->getV1();
                    }
                    if (rc0_fi1->size() + rc1_fi2->size() < rc1_fi1->size() + rc0_fi2->size()) {
                        *w_rc1 |= rc0_fi1->getV1();
                        *w_rc1 |= rc1_fi2->getV1();
                    } else {
                        *w_rc1 |= rc1_fi1->getV1();
                        *w_rc1 |= rc0_fi2->getV1();
                    }
                }
                break;
            case EQV:
                if (w_fi2 == 0) {
                    *w_rc0 |= (rc0_fi1->size() < rc1_fi1->size() ? rc0_fi1->getV1() : rc1_fi1->getV1());
                    *w_rc1 |= (rc0_fi1->size() < rc1_fi1->size() ? rc0_fi1->getV1() : rc1_fi1->getV1());
                } else {
                    if (rc0_fi1->size() + rc0_fi2->size() < rc1_fi1->size() + rc1_fi2->size()) {
                        *w_rc1 |= rc0_fi1->getV1();
                        *w_rc1 |= rc0_fi2->getV1();
                    } else {
                        *w_rc1 |= rc1_fi1->getV1();
                        *w_rc1 |= rc1_fi2->getV1();
                    }
                    if (rc0_fi1->size() + rc1_fi2->size() < rc1_fi1->size() + rc0_fi2->size()) {
                        *w_rc0 |= rc0_fi1->getV1();
                        *w_rc0 |= rc1_fi2->getV1();
                    } else {
                        *w_rc0 |= rc1_fi1->getV1();
                        *w_rc0 |= rc0_fi2->getV1();
                    }
                }
                break;
            case OUTPUT: // can't be input of some node
            case INPUT: // done
                assert(false);
                // cout << "sth wrong" << endl;
                break;
        }
        w->backup_rc(); // branch has same rc as stem
    }

    for (int i = sort_wlist.size() - 1; i >= 0; i--) {
        wptr& w = sort_wlist[i];
        w->_ro.push_back(0); // remain a place for in-wire RO
        rptr best_ro = 0;

        for (nptr& n : w->onode) {
            rptr cur_ro = new REACH(num_in);
            w->_ro.push_back(cur_ro);
            if (n->type != OUTPUT) { // ro of po is empty set
                wptr w_fic = n->iwire.size() < 2 ? 0 : n->iwire[0] == w ? n->iwire[1] : n->iwire[0];
                rptr rc0_fic = w_fic == 0 ? 0 : w_fic->_rc0[w_fic->get_reach_id(n)];
                rptr rc1_fic = w_fic == 0 ? 0 : w_fic->_rc1[w_fic->get_reach_id(n)];

                wptr w_fo = n->owire[0];
                rptr ro_fo = w_fo->_ro[w_fo->get_reach_id(n)];
                switch (n->type) {
                    case NAND:
                    case AND:
                        *cur_ro |= ro_fo->getV1();
                        if (w_fic != 0) *cur_ro |= rc1_fic->getV1();
                        break;
                    case NOR:
                    case OR:
                        *cur_ro |= ro_fo->getV1();
                        if (w_fic != 0) *cur_ro |= rc0_fic->getV1();
                        break;
                    case BUF:
                    case NOT:
                        *cur_ro |= ro_fo->getV1();
                        break;
                    case XOR:
                    case EQV:
                        *cur_ro |= ro_fo->getV1();
                        if (w_fic != 0) *cur_ro |= (rc0_fic->size() < rc1_fic->size() ? rc0_fic->getV1() : rc1_fic->getV1());
                        break;
                    case OUTPUT: // done
                    case INPUT: // should not be some node's output
                        assert(false);
                        break; // done in previous
                }
            }
            if (best_ro == 0 || cur_ro->size() < best_ro->size()) best_ro = cur_ro;
        }
        // if (best_ro == 0)
        w->_ro[0] = best_ro;
    }

    for (auto& f: flist) {
        f->get_det();
    }
}

    // vector<ATPG::wptr> scoap_wlist(sort_wlist);
    // sort(scoap_wlist.begin(), scoap_wlist.end(), [](wptr a, wptr b) {
    //     return (a->_ro[0]->size() < b->_ro[0]->size()) || ((a->_ro[0]->size() == b->_ro[0]->size()) && (a->level > b->level));
    // });

    // for(fptr f : sorted_flist){
    //     int f_cc, f_co;
    //     if(f->fault_type == 0) f_cc = sort_wlist[f->to_swlist]->_rc1[0]->size();
    //     else f_cc = sort_wlist[f->to_swlist]->_rc0[0]->size();

        
    //     for(int i = 0; i < sort_wlist[f->to_swlist]->onode.size(); i++){
    //         if(f->node == sort_wlist[f->to_swlist]->onode[i])
    //             f_co = sort_wlist[f->to_swlist]->_ro[i]->size();
    //     }

    //     // f->scoap = f_cc * f_co;
    //     f->scoap = f->det->size();
    // }

// string v = "";
//             auto f = sorted_flist[i];
//             auto fr = sorted_flist[i]->det;
//             for (int i = 0; i < cktin.size(); i++) {
//                 v += fr->get_val(i);
//                 // pattern[i] = ctoi(itoc(pattern[i]));
//             }

//             for (i = 0; i < cktin.size(); i++) {
//                 cktin[i]->value = ctoi(v[i]);
//             }

//             for (i = 0; i < sort_wlist.size(); i++) {
//                 if (i < cktin.size()) {
//                     sort_wlist[i]->set_changed();
//                 } else {
//                     sort_wlist[i]->value = U;
//                 }
//             }

//             sim();
//             assert(f->fault_type == sort_wlist[f->to_swlist]->value);

//             v = "";
//             v += fr->get_val(cktin.size());
//             for (int i = 1; i < cktin.size(); i++) {
//                 v += fr->get_val(i - 1);
//                 // pattern[i] = ctoi(itoc(pattern[i]));
//             }
//             for (i = 0; i < cktin.size(); i++) {
//                 cktin[i]->value = ctoi(v[i]);
//             }
//             for (i = 0; i < sort_wlist.size(); i++) {
//                 if (i < cktin.size()) {
//                     sort_wlist[i]->set_changed();
//                 } else {
//                     sort_wlist[i]->value = U;
//                 }
//             }

//             sim();
//             assert(f->fault_type != sort_wlist[f->to_swlist]->value);

//             for (i = 0; i < sort_wlist.size(); i++) {
//                 sort_wlist[i]->value = U;
//                 sort_wlist[i]->remove_changed();
//             }