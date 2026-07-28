#ifndef _inclkaskada7_h_
#define _inclkaskada7_h_

#include "event1.h"
#include "params.h"

#include <cmath>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <chrono>
#include <array>
#include <iostream>
#include <algorithm>
#include <map>
#include <cassert>
#include <queue>

#include "generatormt.h"
#include "vect.h"
#include "pdg.h"
#include "nucleus.h"

#include <TROOT.h>
#include <TTree.h>

#include "beam.h"
//#include "Metropolis.h"
#include "Interaction.h"
#include "proctable.h"
#include "nucleusmaker.h"
#include "input_data.h"
#include "shell_sampler.h"

using namespace std;
using namespace PDG;

#define LOG(x) cout<< x <<endl;
#define LOG2(x,y) cout<< x << y <<endl;
#define ERR(x) cerr<< x <<endl;
#define ERR2(x,y) cerr<< x << y <<endl;



#define DEBUG_INCL_INTERFACE 1



class inclkaskada
{
    params par;                                   //!< Params of the simulation.
    event *e;                                     //!< Current event.
    nucleus *nucl;                                //!< Nucleus for the use of the cascade.

  public:
    inclkaskada(params &p, event &e1, input_data *input);
                                                  //!< The default constructor.
                                                  /*!< Takes the params file and the current event.
                                                       Generates a new nucleus for the cascade. */
    ~inclkaskada();                                   //!< The default destructor.
    int inclevent(bool bare_kaskada = false);  //!< Runs the cascade.
};

#endif
