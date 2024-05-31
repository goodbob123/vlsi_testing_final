/**********************************************************************/
/*           main function for atpg                */
/*                                                                    */
/*           Author: Bing-Chen (Benson) Wu                            */
/*           last update : 01/21/2018                                 */
/**********************************************************************/

#include "atpg.h"

void usage();

int main(int argc, char *argv[]) {
  string inpFile, vetFile;
  int i, j;
  ATPG atpg; // create an ATPG obj, named atpg
  bool DC_flag = false;
  atpg.timer(stdout, "START");
  atpg.detected_num = 1;
  i = 1;

  /* parse the input switches & arguments */
  while (i < argc) {
    if (strcmp(argv[i], "-tdfatpg") == 0) {
      i++;
    } else if (strcmp(argv[i], "-DC") == 0) {
      DC_flag = true;
      i++;
    }
      // for N-detect fault simulation
    else if (strcmp(argv[i], "-ndet") == 0) {
      atpg.detected_num = atoi(argv[i + 1]);
      i += 2;
    } else if (argv[i][0] == '-') {
      j = 1;
      while (argv[i][j] != '\0') {
        if (argv[i][j] == 'd') {
          j++;
        } else {
          fprintf(stderr, "atpg: unknown option\n");
          usage();
        }
      }
      i++;
    } else {
      inpFile = string(argv[i]);
      i++;
    }
  }

/* an input file was not specified, so describe the proper usage */
  if (inpFile.empty()) { usage(); }

/* read in and parse the input file */
  atpg.input(inpFile); // input.cpp

/* if vector file is provided, read it */
  if (!vetFile.empty()) { atpg.read_vectors(vetFile); }
  atpg.timer(stdout, "for reading in circuit");

  atpg.level_circuit();  // level.cpp
  atpg.timer(stdout, "for levelling circuit");

  atpg.rearrange_gate_inputs();  //level.cpp
  atpg.timer(stdout, "for rearranging gate inputs");

  atpg.create_dummy_gate(); //init_flist.cpp
  atpg.timer(stdout, "for creating dummy nodes");

  atpg.generate_tdfault_list(); //init_flist.cpp
  atpg.timer(stdout, "for generating fault list");

  // atpg.init_reach();
  // atpg.timer(stdout, "for compute reachability");

  atpg.test(); //atpg.cpp
  atpg.timer(stdout, "for test pattern generation");
  exit(EXIT_SUCCESS);
}

void usage() {

  fprintf(stderr, "usage: atpg [options] infile\n");
  fprintf(stderr, "Options\n");
  fprintf(stderr, "    -fsim <filename>: fault simulation only; filename provides vectors\n");
  fprintf(stderr, "    -anum <num>: <num> specifies number of vectors per fault\n");
  fprintf(stderr, "    -bt <num>: <num> specifies number of backtracks\n");
  exit(EXIT_FAILURE);

} /* end of usage() */
