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
        // if (w->inode.size() == 0)
        //     cerr << "???" << endl;
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
                cout << "sth wrong" << endl;
                break;
        }
        w->backup_rc(); // branch has same rc as stem
    }

    for (size_t i = 0; i < cktout.size(); i++) {
        wptr po = cktout[i];
        po->_ro.push_back(new REACH(num_in)); // ro of po is empty set
        po->_ro.push_back(po->_ro[0]); // for output node (though probably no use)
    }

    for (int i = sort_wlist.size() - 1; i >= 0; i--) {
        cout << i << endl;
        wptr& w = sort_wlist[i];
        // if (w->onode.size() == 0)
        //     cerr << "???" << endl;
        if (w->onode[0]->type == OUTPUT) continue;
        w->_ro.push_back(0); // remain a place for in-wire RO
        rptr best_ro = 0;
        // w ------ n 
        // w_fic -/
        for (nptr& n : w->onode) {
            // if (n->type == OUTPUT) continue; // done
            rptr cur_ro = new REACH(num_in);
            w->_ro.push_back(cur_ro);
            // if (n->iwire.size() == 0 || n->owire.size() == 0)
            //     cerr << "???" << endl;

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
                    cout << "sth wrong" << endl;
                    break; // done in previous
            }
            // smallest ro picked for stem
            if (best_ro == 0 || cur_ro->size() < best_ro->size()) best_ro = cur_ro;
        }
        // if (best_ro == 0)
        w->_ro[0] = best_ro;
    }

    for (auto& f: flist) {
        f->get_det();
    }

    for (auto& w: sort_wlist) {
        cout << w->name << endl;
        cout << "rc0 ";
        w->_rc0[0]->print();
        cout << "rc1 ";
        w->_rc1[0]->print();
        cout << "ro: "<< endl;
        for (auto ro: w->_ro) ro->print();
    }
    // compute rc, ro step by step
}