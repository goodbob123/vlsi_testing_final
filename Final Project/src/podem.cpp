/**********************************************************************/
/*           This is the podem test pattern generator for atpg        */
/*                                                                    */
/*           Author: Bing-Chen (Benson) Wu                            */
/*           last update : 01/21/2018                                 */
/**********************************************************************/

#include "atpg.h"

#define CONFLICT 2

/* generates a single pattern for a single fault */


int ATPG::podem(const fptr fault, int &current_backtracks, int flag) {

  int i, ncktwire, ncktin;
  wptr wpi; // points to the PI currently being assigned
  forward_list<wptr> decision_tree; // design_tree (a LIFO stack)
  forward_list<wptr> decision_tree_V1; // design_tree (a LIFO stack)
  wptr wfault;

  /* initialize all circuit wires to unknown */
  ncktwire = sort_wlist.size();
  ncktin = cktin.size();

  find_test = false;
  no_test = false;


  for (i = 0; i < ncktwire; i++){
    if(i < ncktin) cktin[i]->value = pattern[i];
    else sort_wlist[i]->value = U;
  }

  bool backward_return = 0;
  if((backward_return = backward_imply(sort_wlist[fault->to_swlist], fault->fault_type)) == TRUE){
    for (i = 0; i < ncktin; i++){
      pattern[i] = cktin[i]->value;
    } 
  }
  else if(backward_return == CONFLICT){
    no_test = true;
  }

  cktin[0]->value = pattern[pattern.size()-1];
  cktin[0]->set_changed();
  for (i = 1; i < ncktin; i++){
    cktin[i]->set_changed();
    cktin[i]->value = pattern[i-1];
  } 

  bool backtrack_V1 = false;
  bool valid_V2 = false;

  mark_propagate_tree(fault->node);

  /* Fig 7 starts here */
  /* set the initial objective, assign the first PI.  Fig 7.P1 */
  
  switch (set_uniquely_implied_value(fault)) {
    case TRUE: // if a  PI is assigned
      sim();  // Fig 7.3
      wfault = fault_evaluate(fault);
      if (wfault != nullptr) forward_imply(wfault);// propagate fault effect
      if (check_test()){
        pattern[ncktin] = cktin[0]->value;
        for(int i = 0; i < ncktin; i++){
          if(i != 0) pattern[i-1] = cktin[i]->value;
          if(i != ncktin-1) cktin[i]->value = cktin[i+1]->value;
          cktin[i]->set_changed();
        }
        cktin[ncktin-1]->value = pattern[ncktin-1];

        sim();
        if((sort_wlist[fault->to_swlist]->value ^ 1) == fault->fault_type){
          no_test = true;
        }
        else{
          if(sort_wlist[fault->to_swlist]->value == fault->fault_type) find_test = true;
          else valid_V2 = true;
        }
      } 
      break;
    case CONFLICT:
      no_test = true; // cannot achieve initial objective, no test
      break;
    case FALSE:
      if(flag){
        sim();  // Fig 7.3
        wfault = fault_evaluate(fault);
        if (wfault != nullptr) forward_imply(wfault);// propagate fault effect
        if (check_test()){
          pattern[ncktin] = cktin[0]->value;
          for(int i = 0; i < ncktin; i++){
            if(i != 0) pattern[i-1] = cktin[i]->value;
            if(i != ncktin-1) cktin[i]->value = cktin[i+1]->value;
            cktin[i]->set_changed();
          }
          cktin[ncktin-1]->value = pattern[ncktin-1];

          sim();
          if((sort_wlist[fault->to_swlist]->value ^ 1) == fault->fault_type){
            no_test = true;
          }
          else{
            if(sort_wlist[fault->to_swlist]->value == fault->fault_type) find_test = true;
            else valid_V2 = true;
          }
        } 
      }
      break;
  }

  /* loop in Fig 7.ABC
   * quit the loop when either one of the three conditions is met:
   * 1. number of backtracks is equal to or larger than limit
   * 2. no_test
   * 3. already find a test pattern AND no_of_patterns meets required total_attempt_num */
  vector<bool> assigned_cache(ncktin);
  int V2_find = 0, total_backtracks = 0;
  while((V2_find < find_limit) && !no_test && !find_test){
    if(!valid_V2){
      int V2_backtracks = 0;
      while ((V2_backtracks < backtrack_limit_V2) && !no_test && !valid_V2 && !find_test) {
        //find V2
        if ((!backtrack_V1) && (wpi = test_possible(fault))) {
          wpi->set_changed();
          /* insert a new PI into decision_tree */
          decision_tree.push_front(wpi);
        } 
        else { // no test possible using this assignment, backtrack.
          wpi = nullptr;
          backtrack_V1 = false;
          while (!decision_tree.empty() && (wpi == nullptr)) {
            /* if both 01 already tried, backtrack. Fig.7.7 */
            if (decision_tree.front()->is_all_assigned()) {
              decision_tree.front()->remove_all_assigned();  // clear the ALL_ASSIGNED flag
              decision_tree.front()->value = U; // do not assign 0 or 1
              decision_tree.front()->set_changed(); // this PI has been changed
              /* remove this PI in decision tree.  see dashed nodes in Fig 6 */
              decision_tree.pop_front();
            }
              /* else, flip last decision, flag ALL_ASSIGNED. Fig. 7.8 */
            else {
              decision_tree.front()->value = decision_tree.front()->value ^ 1; // flip last decision
              decision_tree.front()->set_changed(); // this PI has been changed
              decision_tree.front()->set_all_assigned();
              V2_backtracks++;
              wpi = decision_tree.front();
            }
          } // while decision tree && ! wpi
          if (wpi == nullptr) no_test = true; //decision tree empty,  Fig 7.9
        } // no test possible
        // string temp = "";
        // for(int i = 0; i < ncktin; i++){
        //   temp+=itoc(cktin[i]->value);
        //   cktin[i]->set_changed();
        // }
        // if(temp == "111111111111111111111111111111111112210121"){
        //   cerr<<"good"<<endl;
        // }
        if (wpi) {
          sim();
          if (wfault = fault_evaluate(fault)) forward_imply(wfault);
          if (check_test()) {
            V2_find++;
            pattern[ncktin] = cktin[0]->value;
            for(int i = 0; i < ncktin; i++){
              if(i != 0) pattern[i-1] = cktin[i]->value;
              if(i != ncktin-1) cktin[i]->value = cktin[i+1]->value;
              cktin[i]->set_changed();
            }
            cktin[ncktin-1]->value = pattern[ncktin-1];

            sim();
            if((sort_wlist[fault->to_swlist]->value ^ 1) == fault->fault_type){
              backtrack_V1 = true;
              for(int i = ncktin-1; i > 0; i--){
                cktin[i]->value = pattern[i-1];
                cktin[i]->set_changed();
              }
              cktin[0]->value = pattern[ncktin];
              cktin[0]->set_changed();
            }
            else{
              if(sort_wlist[fault->to_swlist]->value == fault->fault_type) find_test = true;
              else{
                for(int i = 0; i < ncktin; i++){
                  assigned_cache[i] = cktin[i]->is_all_assigned();
                  cktin[i]->remove_all_assigned();
                }
                valid_V2 = true;
              } 
            }
          }
        }
      }
      total_backtracks += V2_backtracks;
      if((!valid_V2) && (!find_test)) no_test = true;
    }
    else{
      int V1_backtracks = 0;
      while ((V1_backtracks < backtrack_limit_V1) && valid_V2 && !find_test) {
        //find_V1
        if (wpi = find_pi_assignment(sort_wlist[fault->to_swlist], fault->fault_type)) {
          wpi->set_changed();
          /* insert a new PI into decision_tree */
          decision_tree_V1.push_front(wpi);
        } 
        else { // no test possible using this assignment, backtrack.
          while (!decision_tree_V1.empty() && (wpi == nullptr)) {
            /* if both 01 already tried, backtrack. Fig.7.7 */
            if (decision_tree_V1.front()->is_all_assigned()) {
              decision_tree_V1.front()->remove_all_assigned();  // clear the ALL_ASSIGNED flag
              decision_tree_V1.front()->value = U; // do not assign 0 or 1
              decision_tree_V1.front()->set_changed(); // this PI has been changed
              /* remove this PI in decision tree.  see dashed nodes in Fig 6 */
              decision_tree_V1.pop_front();
            }
              /* else, flip last decision, flag ALL_ASSIGNED. Fig. 7.8 */
            else {
              decision_tree_V1.front()->value = decision_tree_V1.front()->value ^ 1; // flip last decision
              decision_tree_V1.front()->set_changed(); // this PI has been changed
              decision_tree_V1.front()->set_all_assigned();
              V1_backtracks++;
              wpi = decision_tree_V1.front();
            }
          } // while decision tree && ! wpi
          if (wpi == nullptr){//can't find V1, backtrack V2
            valid_V2 = false;
          } 
        }

        if (wpi) {
          sim();
          if (sort_wlist[fault->to_swlist]->value == fault->fault_type) {
            find_test = true;
            for(int i = 0; i < ncktin; i++){
              pattern[i] = cktin[i]->value;
            }
          }
        }
      }
      cktin[0]->value = pattern[ncktin];
      for(int i = 0; i < ncktin; i++){
        if(i != 0) cktin[i]->value = pattern[i-1];
        cktin[i]->set_changed();
        
        cktin[i]->remove_all_assigned();
        if(assigned_cache[i])
          cktin[i]->set_all_assigned();
      }
      total_backtracks += V1_backtracks;
      valid_V2 = false;
      backtrack_V1 = true;
    } 
  }// while (three conditions)

  /* clear everything */
  for (wptr wptr_ele: cktin) {
    wptr_ele->remove_all_assigned();
  }

  if(flag)
    current_backtracks += total_backtracks;
  else
    current_backtracks = total_backtracks;

  unmark_propagate_tree(fault->node);

  if (find_test) {
    return (TRUE);
  } else if (no_test) {
    /*fprintf(stdout,"redundant fault...\n");*/
    return (FALSE);
  } else {
    /*fprintf(stdout,"test aborted due to backtrack limit...\n");*/
    return (MAYBE);
  }
}/* end of podem */

/* drive D or D_bar to the faulty gate (aka. GUT) output
 * insert D or D_bar into the circuit.
 * returns w (the faulty gate output) if GUT output is set to D or D_bar successfully.
 * returns NULL if if faulty gate output still remains unknown  */
ATPG::wptr ATPG::fault_evaluate(const fptr fault) {
  int temp1;
  wptr w;

  if (fault->io == GO) { // if fault is on GUT gate output
    w = fault->node->owire.front(); // w is GUT output wire
    if (w->value == U) return (nullptr);
    if (fault->fault_type == STUCK0 && w->value == 1) w->value = D; // D means 1/0
    if (fault->fault_type == STUCK1 && w->value == 0) w->value = D_bar; // D_bar 0/1
    return (w);
  } else { // if fault is GUT gate input
    w = fault->node->iwire[fault->index];
    if (w->value == U) return (nullptr);
    else {
      temp1 = w->value;
      if (fault->fault_type == STUCK0 && w->value == 1) w->value = D;
      if (fault->fault_type == STUCK1 && w->value == 0) w->value = D_bar;
      if (fault->node->type == OUTPUT) return (nullptr);
      evaluate(fault->node);  // five-valued, evaluate one gate only, sim.c
      w->value = temp1;
      /* if GUT gate output changed */
      if (fault->node->owire.front()->is_changed()) {
        fault->node->owire.front()->remove_changed(); // stop GUT output change propagation
        return (fault->node->owire.front()); // returns the output wire of GUT
      } else return (nullptr); // faulty gate output does not change
    }
  }
}/* end of fault_evaluate */


/* iteratively foward implication
 * in a depth first search manner*/
void ATPG::forward_imply(const wptr w) {
  int i, nout;

  for (i = 0, nout = w->onode.size(); i < nout; i++) {
    if (w->onode[i]->type != OUTPUT) {
      evaluate(w->onode[i]);
      if (w->onode[i]->owire.front()->is_changed())
        forward_imply(w->onode[i]->owire.front()); // go one level further
      w->onode[i]->owire.front()->remove_changed();
    }
  }
}/* end of forward_imply */


/* Fig 8 
 * this function determines objective_wire and objective_level. 
 * it returns the newly assigned PI if test is possible. 
 * it returns NULL if no test is possible. */
ATPG::wptr ATPG::test_possible(const fptr fault) {
  nptr n;
  wptr object_wire;
  int object_level;

  /* if the fault is not on primary output */
  if (fault->node->type != OUTPUT) {

    /* if the faulty gate (aka. gate under test, G.U.T.) output is not U,  Fig. 8.1 */
    if (fault->node->owire.front()->value ^ U) {

      /* if GUT output is not D or D_bar, no test possible */
      if (!((fault->node->owire.front()->value == D_bar) ||
            (fault->node->owire.front()->value == D)))
        return (nullptr);

      /* find the next gate n to propagate, Fig 8.5*/
    //   if(scoap)
    //     n = find_propagate_gate(fault->node->owire.front()->min_co);
    //   else
      n = find_propagate_gate(fault->node->owire.front()->level);
      if (!n)
        return (nullptr);

      /*determine objective level according to the type of n.   Fig 8.8*/
      switch (n->type) {
        case AND:
        case NOR:
          object_level = 1;
          break;
        case NAND:
        case OR:
          object_level = 0;
          break;
        default:
          // to be improved
          /*---- comment out because C2670 has XOR ---------
          fprintf(stderr,
              "Internal Error(1st bp. in test_possible)!\n");
          exit(-1);
          -------------------------------------------------------*/
          return (nullptr);
      }
      /* object_wire is the gate n output. */
      object_wire = n->owire.front();
    }  // if faulty gate output is not U.   (fault->node->owire.front()->value ^ U)

    else { // if faulty gate output is U

      /* if X path disappear, no test possible  */
      if (!(trace_unknown_path(fault->node->owire.front())))
        return (nullptr);

      /* if fault is on GUT otuput,  Fig 8.2*/
      if (fault->io) {
        /* objective_level is opposite to stuck fault  Fig 8.3 */
        if (fault->fault_type) object_level = 0;
        else object_level = 1;
        /* objective_wire is on faulty gate output */
        object_wire = fault->node->owire.front();
      }

        /* if fault is on GUT input, Fig 8.2*/
      else {
        /* if faulted input is not U  Fig 8.4 */
        if (fault->node->iwire[fault->index]->value ^ U) {
          /* determine objective value according to GUT type. Fig 8.9*/
          switch (fault->node->type) {
            case AND:
            case NOR:
              object_level = 1;
              break;
            case NAND:
            case OR:
              object_level = 0;
              break;
            default:
              // to be improved
              /*---- comment out because C2670.sim has XOR ---------
                 fprintf(stderr,
                    "Internal Error(2nd bp. in test_possible)!\n");
                 exit(-1);
              -------------------------------------------------------*/
              return (nullptr);
          }
          /*objective wire is GUT output. */
          object_wire = fault->node->owire.front();
        }  // if faulted input is not U

        else { // if faulted input is U
          /*objective level is opposite to stuck fault.  Fig 8.10*/
          if (fault->fault_type) object_level = 0;
          else object_level = 1;
          /* objective wire is faulty wire itself */
          object_wire = fault->node->iwire[fault->index];
        }
      }
    }
  } // if fault not on PO

  else { // else if fault on PO
    /* if faulty PO is still unknown */
    if (fault->node->iwire.front()->value == U) {
      /*objective level is opposite to the stuck fault */
      if (fault->fault_type) object_level = 0;
      else object_level = 1;
      /* objective wire is the faulty wire itself */
      object_wire = fault->node->iwire.front();
    } else {
      /*--- comment out due to error for C2670.sim ---
      fprintf(stderr,"Internal Error(1st bp. in test_possible)!\n");
      exit(-1);
        */
      return (nullptr);
    }
  }// else if fault on PO

  /* find a pi to achieve the objective_level on objective_wire.
   * returns nullptr if no PI is found.  */
  return (find_pi_assignment(object_wire, object_level));
}/* end of test_possible */


/* backtrace to PI, assign a PI to achieve the objective.  Fig 9
 * returns the wire pointer to PI if succeed.
 * returns NULL if no such PI found. */
ATPG::wptr ATPG::find_pi_assignment(const wptr object_wire, const int &object_level) {
  wptr new_object_wire;
  int new_object_level;

  /* if PI, assign the same value as objective Fig 9.1, 9.2 */
  if (object_wire->is_input()) {
    object_wire->value = object_level;
    return (object_wire);
  }

    /* if not PI, backtrace to PI  Fig 9.3, 9.4, 9.5*/
  else {
    switch (object_wire->inode.front()->type) {
      case BUF:
      case AND:
      case OR:
        new_object_level = object_level;
        break;
        /* flip objective value  Fig 9.6 */
      case NOT:
      case NOR:
      case NAND:
        new_object_level = object_level ^ 1;
        break;
    }

    switch (object_wire->inode.front()->type) {
      case OR:
      case NAND:
        if (object_level) new_object_wire = find_easiest_control(object_wire->inode.front(), new_object_level);  // decision gate
        else new_object_wire = find_hardest_control(object_wire->inode.front(), new_object_level); // imply gate
        break;
      case NOR:
      case AND:
        // TODO  similar to OR and NAND but different polarity
        if (object_level) new_object_wire = find_hardest_control(object_wire->inode.front(), new_object_level);
        else new_object_wire = find_easiest_control(object_wire->inode.front(), new_object_level);
        break;
        //  TODO END
      case NOT:
      case BUF:
        new_object_wire = object_wire->inode.front()->iwire.front();
        break;
    }

    if (new_object_wire) return (find_pi_assignment(new_object_wire, new_object_level));
    else return (nullptr);
  }
}/* end of find_pi_assignment */


/* Fig 9.4 */
ATPG::wptr ATPG::find_hardest_control(const nptr n, int object_level) {
  int i;
  if(scoap){
    wptr target = nullptr;
    for (i = n->iwire.size() - 1; i >= 0; i--) {
      if (n->iwire[i]->value == U && ((target == nullptr) || (n->iwire[i]->cc[object_level] > target->cc[object_level]))) 
        target = n->iwire[i];
    }
    return target;
  }
  else{
    for (i = n->iwire.size() - 1; i >= 0; i--) {
      if (n->iwire[i]->value == U) return (n->iwire[i]);
    }
    return (nullptr);
  }
  /* because gate inputs are arranged in a increasing level order,
   * larger input index means harder to control */
  
}/* end of find_hardest_control */


/* Fig 9.5 */
ATPG::wptr ATPG::find_easiest_control(const nptr n, int object_level) {
  int i, nin;
  // TODO  similar to hardiest_control but increasing level order
  if(scoap){
    wptr target = nullptr;
    for (i = 0, nin = n->iwire.size(); i < nin; i++) {
      if (n->iwire[i]->value == U && ((target == nullptr) || (n->iwire[i]->cc[object_level] < target->cc[object_level]))) 
        target = n->iwire[i];
    }
    return target;
  }
  else{
    for (i = 0, nin = n->iwire.size(); i < nin; i++) {
      if (n->iwire[i]->value == U) return (n->iwire[i]);
    }
    //  TODO END
    return (nullptr);
  }
}/* end of find_easiest_control */


/* Find the eastiest propagation gate.   Fig 8.5, Fig 8.6 
 * returns the next gate with D or D_bar on inputs, U on output, nearest to PO
 * returns NULL if no such gate found. */
ATPG::nptr ATPG::find_propagate_gate(const int &level) {
  int i, j, nin;
  wptr w;
//   if(scoap){
//     for (i = 0; i < scoap_wlist.size(); i++) {
//       /* if reach the same level as the fault, then no propagation path exists */
//       if (scoap_wlist[i]->min_co == level) return (nullptr);
//       /* gate outptu is U */
//       /* a marked gate means it is on the path to PO */
//       if ((scoap_wlist[i]->value == U) &&
//           (scoap_wlist[i]->inode.front()->is_marked())) {    // Check if marked
//         /*  cehck all gate intputs */
//         for (j = 0, nin = scoap_wlist[i]->inode.front()->iwire.size(); j < nin; j++) {
//           w = scoap_wlist[i]->inode.front()->iwire[j];
//           /* if there is ont gate intput is D or D_bar */
//           if ((w->value == D) || (w->value == D_bar)) {
//             if (trace_unknown_path(scoap_wlist[i])) // check X path  Fig 8.6
//               return (scoap_wlist[i]->inode.front()); // succeed.  returns this gate
//             break;
//           }
//         }
//       }
//     }
//   }
//   else{
    for (i = sort_wlist.size() - 1; i >= 0; i--) {
      /* if reach the same level as the fault, then no propagation path exists */
      if (sort_wlist[i]->level == level) return (nullptr);
      /* gate outptu is U */
      /* a marked gate means it is on the path to PO */
      if ((sort_wlist[i]->value == U) &&
          (sort_wlist[i]->inode.front()->is_marked())) {    // Check if marked
        /*  cehck all gate intputs */
        for (j = 0, nin = sort_wlist[i]->inode.front()->iwire.size(); j < nin; j++) {
          w = sort_wlist[i]->inode.front()->iwire[j];
          /* if there is ont gate intput is D or D_bar */
          if ((w->value == D) || (w->value == D_bar)) {
            if (trace_unknown_path(sort_wlist[i])) // check X path  Fig 8.6
              return (sort_wlist[i]->inode.front()); // succeed.  returns this gate
            break;
          }
        }
      }
    }
  }
//}/* end of find_propagate_gate */


/* DFS search for X-path , Fig 8.6
 * returns TRUE if X pth exists
 * returns NULL if no X path exists*/
bool ATPG::trace_unknown_path(const wptr w) {
  int i, nout;
  //TODO search X-path
  //HINT if w is PO, return TRUE, if not, check all its fanout
  //------------------------------------- hole ---------------------------------------
  if (w->is_output()) // if w is PO
    return true; // a X path has been found
  else // check all its fanout
  {
    for (i = 0, nout = w->onode.size(); i < nout; i++) {
      if (w->onode[i]->owire.front()->value == U) // if unknown value exists
      {
        // down trace. if X-path exists, return TRUE.
        if (trace_unknown_path(w->onode[i]->owire.front()))
          return true;
      }
    } // end of search
    return false; // if program runs out the for loop, there is no X-path
  }
  //----------------------------------------------------------------------------------
  //TODO END
}/* end of trace_unknown_path */


/* Check if any D or D_bar reaches PO. Fig 7.4 */
bool ATPG::check_test() {
  int i, ncktout;
  bool is_tested;

  is_tested = false;
  //TODO check if any fault effect reach PO
  //HINT check every PO for their value
  //cktout[] contains all output wire
  //--------------------------------- hole -------------------------------------------
  for (i = 0, ncktout = cktout.size(); i < ncktout; i++) {
    if ((cktout[i]->value == D) || (cktout[i]->value == D_bar)) // if there is D or D-bar on the PO
    {
      is_tested = true;
    }
  }
  //----------------------------------------------------------------------------------
  //TODO END
  return is_tested;
}/* end of check_test */

/* exhaustive search of all nodes on the path from n to PO */
void ATPG::mark_propagate_tree(const nptr n) {
  int i, j, wnout, nnout;

  if (n->is_marked()) return;
  n->set_marked(); // MARKED means this node is on the path to PO
  /* depth first search */
  for (i = 0, wnout = n->owire.size(); i < wnout; i++) {
    for (j = 0, nnout = n->owire[i]->onode.size(); j < nnout; j++) {
      mark_propagate_tree(n->owire[i]->onode[j]);
    }
  }
}/* end of mark_propagate_tree */

/* clear all the MARKS */
void ATPG::unmark_propagate_tree(const nptr n) {
  int i, j, wnout, nnout;

  //TO BE CHANGED
  if (n->is_marked()) {
    n->remove_marked();
    for (i = 0, wnout = n->owire.size(); i < wnout; i++) {
      for (j = 0, nnout = n->owire[i]->onode.size(); j < nnout; j++) {
        unmark_propagate_tree(n->owire[i]->onode[j]);
      }
    }
  }
}/* end of unmark_propagate_tree */

/* set the initial objective.  
 * returns TRUE if we can backtrace to a PI to assign
 * returns CONFLICT if it is impossible to achieve or set the initial objective*/
int ATPG::set_uniquely_implied_value(const fptr fault) {
  wptr w;
  int pi_is_reach = FALSE;
  int i, nin;

  nin = fault->node->iwire.size();
  if (fault->io) w = fault->node->owire.front();  //  gate output fault, Fig.8.3
  else { // gate input fault.  Fig. 8.4
    w = fault->node->iwire[fault->index];

    switch (fault->node->type) {
      case NOT:
      case BUF:
        break;

        /* assign all side inputs to non-controlling values */
      case AND:
      case NAND:
        for (i = 0; i < nin; i++) {
          if (fault->node->iwire[i] != w) {
            switch (backward_imply(fault->node->iwire[i], 1)) {
              case TRUE:
                pi_is_reach = TRUE;
                break;
              case CONFLICT:
                return (CONFLICT);
              case FALSE:
                break;
            }
          }
        }
        break;

      case OR:
      case NOR:
        for (i = 0; i < nin; i++) {
          if (fault->node->iwire[i] != w) {
            switch (backward_imply(fault->node->iwire[i], 0)) {
              case TRUE:
                pi_is_reach = TRUE;
                break;
              case CONFLICT:
                return (CONFLICT);
              case FALSE:
                break;
            }
          }
        }
        break;
    }
  } // else , gate input fault


  /* fault excitation */
  //TODO fault excitation
  //HINT use backward_imply function to check if fault can excite or not
  /*
  switch(  )
  {
      case TRUE:
      case CONFLICT:
      case FALSE:
  }
  */
  //TODO END


  //ANSWER
  // set output of GUT opposite to stuck fault direction(Fig.8.3),
  // and do backward implication
  switch (backward_imply(w, !(fault->fault_type))) {
    case TRUE:
      pi_is_reach = TRUE;
      break; // if the backward implication reaches any PI
    case CONFLICT:
      return CONFLICT;
      break; // if it is impossible to achieve or set the initial objective
    case FALSE:
      break; // if it has not reached any PI, let pi_is_reach remain FALSE
  }
  //----------------------------------------------------------------------------------

  return (pi_is_reach);
}/* end of set_uniquely_implied_value */

/*do a backward implication of the objective: set current_wire to desired_logic_value
 *implication means a natural consequence of the desired objective. 
 *returns TRUE if the backward implication reaches at least one PI 
 *returns FALSE if the backward implication reaches no PI */
int ATPG::backward_imply(const wptr current_wire, const int &desired_logic_value) {
  int pi_is_reach = FALSE;
  int i, nin;

  nin = current_wire->inode.front()->iwire.size();
  if (current_wire->is_input()) { // if PI
    if (current_wire->value != U &&
        current_wire->value != desired_logic_value) {
      return (CONFLICT); // conlict with previous assignment
    }
    current_wire->value = desired_logic_value; // assign PI to the objective value
    current_wire->set_changed();
    // CHANGED means the logic value on this wire has recently been changed
    return (TRUE);
  } else { // if not PI
    switch (current_wire->inode.front()->type) {
      /* assign NOT input opposite to its objective ouput */
      /* go backward iteratively.  depth first search */
      case NOT:
        switch (backward_imply(current_wire->inode.front()->iwire.front(), (desired_logic_value ^ 1))) {
          case TRUE:
            pi_is_reach = TRUE;
            break;
          case CONFLICT:
            return (CONFLICT);
            break;
          case FALSE:
            break;
        }
        break;

        /* if objective is NAND output=zero, then NAND inputs are all ones
         * keep doing this back implication iteratively  */
      case NAND:
        if (desired_logic_value == 0) {
          for (i = 0; i < nin; i++) {
            switch (backward_imply(current_wire->inode.front()->iwire[i], 1)) {
              case TRUE:
                pi_is_reach = TRUE;
                break;
              case CONFLICT:
                return (CONFLICT);
                break;
              case FALSE:
                break;
            }
          }
        }
        break;

      case AND:
        if (desired_logic_value == 1) {
          for (i = 0; i < nin; i++) {
            switch (backward_imply(current_wire->inode.front()->iwire[i], 1)) {
              case TRUE:
                pi_is_reach = TRUE;
                break;
              case CONFLICT:
                return (CONFLICT);
                break;
              case FALSE:
                break;
            }
          }
        }
        break;

      case OR:
        if (desired_logic_value == 0) {
          for (i = 0; i < nin; i++) {
            switch (backward_imply(current_wire->inode.front()->iwire[i], 0)) {
              case TRUE:
                pi_is_reach = TRUE;
                break;
              case CONFLICT:
                return (CONFLICT);
                break;
              case FALSE:
                break;
            }
          }
        }
        break;

      case NOR:
        if (desired_logic_value == 1) {
          for (i = 0; i < nin; i++) {
            switch (backward_imply(current_wire->inode.front()->iwire[i], 0)) {
              case TRUE:
                pi_is_reach = TRUE;
                break;
              case CONFLICT:
                return (CONFLICT);
                break;
              case FALSE:
                break;
            }
          }
        }
        break;

      case BUF:
        switch (backward_imply(current_wire->inode.front()->iwire.front(), desired_logic_value)) {
          case TRUE:
            pi_is_reach = TRUE;
            break;
          case CONFLICT:
            return (CONFLICT);
            break;
          case FALSE:
            break;
        }
        break;
    }

    return (pi_is_reach);
  }
}/* end of backward_imply */
