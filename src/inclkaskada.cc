#include "inclkaskada.h"
// #include "NEUTINCLCXXInterface.h"

// #define G4cout std::cout
// #include "G4INCLGeant4Compat.hh"
#include "G4INCLCascade.hh"
#include "DatafilePaths.hh"
#include "G4INCLCascade.hh"
#include "G4INCLConfig.hh"
#include "G4INCLAbla07Interface.hh"
#include "G4INCLFSICascadeSeed.hh"
    
#include <algorithm>

#include "fsi.h"


G4INCL::Config aConfig;
G4INCL::Config* theConfig = 0;
G4INCL::INCL* theINCLModel = 0;
bool gINCLFirstSetup = false;

G4INCL::Nucleus* targetNucleus = NULL;
G4INCL::FSICascadeSeed* cascadeSeed = NULL;

int n_prefsi_particles = 0;

ABLA07CXX::Abla07Interface* theABLAModel;


void inclcxx_setup_model() {
    if (gINCLFirstSetup) return;

    aConfig.init();
    
    theConfig = new G4INCL::Config(aConfig);

    theConfig->setINCLXXDataFilePath(std::string(std::getenv("INCL_DATA"))); 
    theConfig->setABLAXXDataFilePath(std::string(std::getenv("ABLA_DATA"))); 
    theConfig->setABLA07DataFilePath(std::string(std::getenv("ABLA_DATA"))); 
    
    gINCLFirstSetup = true;
    theINCLModel = new G4INCL::INCL(theConfig);
    theABLAModel = new ABLA07CXX::Abla07Interface(theConfig);
}


void inclcxx_initialize_default_nucleus_and_seed(){

    if (targetNucleus) delete targetNucleus;
    if (cascadeSeed) delete cascadeSeed;
    
    // // Create blank nucleus
    // int targetA = neuttarget.numatom; 
    // int targetZ = neuttarget.numbndp;
    // int targetL = 0;
    // theINCLModel->initializeTarget(targetA, targetZ, targetL);

    // Update the target
    targetNucleus = theINCLModel->GetNucleus();

    cascadeSeed = new G4INCL::FSICascadeSeed();    
}

// Constructor: set up cascade state from params, event and input_data
inclkaskada::inclkaskada(params &p, event &e1, input_data *input)
{
  par = p;
  if (par.nucleus_p + par.nucleus_n < 3) par.kaskada_w = 0;  //for free nucleons and deuteron there is no extra binding energy

  e = &e1;                        // Pointer to current event
  int max_step = par.step * fermi;    // set maximum step defined in params
  // nucl = make_nucleus(par);       // create nucleus defined in params
  // radius = nucl->radius();        // calculate radius of the nucleus

}




void inclcxx_add_struck_nucleons() {

    // for (int i = 0; i < n_prefsi_particles; i++) {        
    //     if (!vcwork.iflgvc[i] == kStateInitial) continue;

    //     G4INCL::ParticleType particleSpecies =\
    //         ConvertPDGtoINCL(vcwork.ipvc[i]);

    //     if (particleSpecies == G4INCL::UnknownParticle) {
    //         continue;
    //     }

    //     G4INCL::Particle *p = \
    //     new G4INCL::Particle(particleSpecies,
    //             G4INCL::ThreeVector(vcwork.pvc[i][0],
    //                     vcwork.pvc[i][1],
    //                     vcwork.pvc[i][2]),
    //             G4INCL::ThreeVector(posinnuc.posnuc[i][0],
    //                     posinnuc.posnuc[i][1],
    //                     posinnuc.posnuc[i][2])); 

    //     cascadeSeed->struckNucleons.push_back(p);
    // }
    
}

void inclcxx_add_outgoing_particles() {

    // for (int i = 0; i < n_prefsi_particles; i++) {        
    //      if (!((vcwork.iflgvc[i] == kStateDetermineLater) &&
    //         vcwork.icrnvc[i] == kFlagChase)) continue;

    //     G4INCL::ParticleType particleSpecies =\
    //         ConvertPDGtoINCL(vcwork.ipvc[i]);

    //     if (particleSpecies == G4INCL::UnknownParticle) {
    //         continue;
    //     }

    //     G4INCL::Particle *p = \
    //     new G4INCL::Particle(particleSpecies,
    //             G4INCL::ThreeVector(vcwork.pvc[i][0],
    //                     vcwork.pvc[i][1],
    //                     vcwork.pvc[i][2]),
    //             G4INCL::ThreeVector(posinnuc.posnuc[i][0],
    //                     posinnuc.posnuc[i][1],
    //                     posinnuc.posnuc[i][2])); 

    //     cascadeSeed->cascadeParticles.push_back(p);
    // }
}

void inclcxx_modify_neut_stack_with_result(G4INCL::EventInfo& result) {

//     // If event transparent just let particle leave
//     if (result.transparent) {
        
//     #ifdef DEBUG_INCL_INTERFACE
//             std::cout << "[INCL] : EVENT WAS TRANSPARENT" << std::endl;
//     #endif

//         for (int i = 0; i < n_prefsi_particles; i++) {
//             if (!((vcwork.iflgvc[i] == kStateDetermineLater) &&
//             vcwork.icrnvc[i] == kFlagChase)) continue;

//             vcwork.iflgvc[i] =  kStateDetermineLater;
//             vcwork.icrnvc[i] =  kFlagChase;
//         }

//     // If non transparent we need to de-excite and add particles
//     // to the stack as FS partices.
//     } else {
//         std::cout << "[INCL] : EVENT NOT TRANSPARENT !!!!!!!!!!!!!!!!!" << std::endl;

//         for (int i = 0; i < n_prefsi_particles; i++) {
            
//             if (!((vcwork.iflgvc[i] == kStateDetermineLater) &&
//             vcwork.icrnvc[i] == kFlagChase)) continue;

//             // Drop the primary that produced this out of the stack
//             vcwork.iflgvc[i] =  kStateStop; //particle state flag, seen below
//             vcwork.icrnvc[i] =  kFlagDoNotChase; //chase flag
//         }
        
//         theABLAModel->deExcite(&result);

//         for (int i_particle = 0;
//             i_particle < result.nParticles;
//             i_particle++) {

//             int i_np_postfsi = vcwork.nvc; 
            
//             /*
//                 this index is across the initial, middle 
//                 and final state particles. we are starting 
//                 at the last index from when we loaded in the particles.
//             */

// #ifdef DEBUG_INCL_INTERFACE
//             std::cout << "[INCL] : Adding new postfsi "
//                 << i_np_postfsi << " "
//                 << result.PDGCode[i_particle] << std::endl;
// #endif
                
//             // Assuming here NEUT doesn't fall over being passed a remnant
//             // we are now updating neuts output struct with info from incl
//             vcwork.ipvc[i_np_postfsi] = result.PDGCode[i_particle]; //type
//             vcwork.pvc[i_np_postfsi][0] = result.px[i_particle]; //momenta
//             vcwork.pvc[i_np_postfsi][1] = result.py[i_particle];
//             vcwork.pvc[i_np_postfsi][2] = result.pz[i_particle];

//             double pmod = sqrt(
//                 vcwork.pvc[i_np_postfsi][0]*vcwork.pvc[i_np_postfsi][0] +
//                 vcwork.pvc[i_np_postfsi][1]*vcwork.pvc[i_np_postfsi][1] +
//                 vcwork.pvc[i_np_postfsi][2]*vcwork.pvc[i_np_postfsi][2]); //

//             double ke = result.EKin[i_particle];
//             vcwork.amasvc[i_np_postfsi] = (pmod*pmod - ke*ke) / (2*ke); //mass

// #ifdef DEBUG_INCL_INTERFACE
//             std::cout << "[INCL] : Adding New POST FSI Part : " 
//                 << vcwork.ipvc[i_np_postfsi] << " : " 
//                 << vcwork.amasvc[i_np_postfsi]
//                 << " " << pmod << std::endl;
// #endif

//             //I'm not sure why this is set like this
//             vcwork.iflgvc[i_np_postfsi] = kStateDetermineLater; 
//             vcwork.icrnvc[i_np_postfsi] = kFlagChase; 
//             vcwork.iorgvc[i_np_postfsi] = vcwork.nvc; 

//             vcwork.nvc += 1;
//         }
//     }
}





// Destructor: free heap-allocated nuclear and interaction objects
inclkaskada::~inclkaskada()
{
  delete nucl;
}

// Main cascade driver for a single event
int inclkaskada::inclevent(bool bare_kaskada)
{

    // // Skip unbound
    // if (!posinnuc.ibound) return;

    // // First setup if required
    // inclcxx_setup_model();

    // // Set some pre-fsi globals needed for easier access
    // n_prefsi_particles = vcwork.nvc;

    // // Initialize Struck Nucleus
    // inclcxx_initialize_default_nucleus_and_seed();

    // // Add the NEUT Incoming Particles
    // inclcxx_add_struck_nucleons();

    // // Add the NEUT outgoing particles 
    // inclcxx_add_outgoing_particles();

    // // Run internal position swapper
    // cascadeSeed->correctNucleusStruckNucleons(targetNucleus);

    // // Update INCLs energy calculations
    // cascadeSeed->updateNucleusMissingEnergyAndCharge(targetNucleus);

    // /// START OF FSI CASCADE RUNNING
    // G4INCL::EventInfo result;
    // result = theINCLModel->processFSICascadeEvent(targetNucleus, cascadeSeed);
    
    // // Update stack with event info
    // inclcxx_modify_neut_stack_with_result(result);

  
  return 0;
}