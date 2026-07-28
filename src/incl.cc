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

G4INCL::ParticleType ConvertPDGtoINCL(const int pdg) {
    switch (pdg) {
        case 2212: return G4INCL::Proton;
        case 2112: return G4INCL::Neutron;
        case 211:  return G4INCL::PiPlus;
        case 221:  return G4INCL::Eta;
        case -211: return G4INCL::PiMinus;
        case 111:  return G4INCL::PiZero;
        case 321:  return G4INCL::KPlus;
        case 311:  return G4INCL::KZero;
        case -311: return G4INCL::KZeroBar;
        case -321:  return G4INCL::KMinus;
        case 22:  return G4INCL::Photon;
        case 13:  return G4INCL::Muon;
        case 3122: return G4INCL::Lambda;
        case 3222: return G4INCL::SigmaPlus;
        case 3212: return G4INCL::SigmaZero;
        case 3112: return G4INCL::SigmaMinus;
        case 130: return G4INCL::KLong;
        case 310: return G4INCL::KShort;
        case 2224: return G4INCL::DeltaPlusPlus;
        case 2214: return G4INCL::DeltaPlus;
        case 2114: return G4INCL::DeltaZero;
        case 1114: return G4INCL::DeltaMinus;
        case 223: return G4INCL::Omega;
        case 331: return G4INCL::EtaPrime;

    }
    return G4INCL::UnknownParticle;
}

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

void inclcxx_initialize_default_nucleus_and_seed(params &p){

    par = p;

    if (targetNucleus) delete targetNucleus;
    if (cascadeSeed) delete cascadeSeed;

    // Create blank nucleus
    int targetA = par.nucleus_p + par.nucleus_n;
    int targetZ = par.nucleus_p;
    int targetL = 0;
    theINCLModel->initializeTarget(targetA, targetZ, targetL);

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

    for (int j = 0; j < e->in.size(); ++j)
    {
      if(in[j].pdg.lepton()) nu_id = j;
      else{
        G4INCL::Particle *p = new G4INCL::Particle(ConvertPDGtoINCL(e->in[j].pdg),e->in[j].p(),e->in[j].r*inverseMeV_to_fm);

        cascadeSeed->struckNucleons.push_back(p);
      }
    }

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

    for (int j = 0; j < e->out_corrected.size(); ++j)
    {
      if(e->out_corrected[j].lepton()) muon_id = j;
      else{
        G4INCL::Particle *p = new G4INCL::Particle(ConvertPDGtoINCL(e->out_corrected[j].pdg),e->out_corrected[j].p(),e->out_corrected[j].r*inverseMeV_to_fm);

        cascadeSeed->cascadeParticles.push_back(p);
      }
    }

    if(e->flag.isCorrelated){
        G4INCL::Particle *p = new G4INCL::Particle(ConvertPDGtoINCL(e->out_corrected[1].pdg),e->out_corrected[1].p(),e->out_corrected[1].r*inverseMeV_to_fm); 
        cascadeSeed->struckNucleons.push_back(p);
    }


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

void inclcxx_modify_nuwro_stack_with_result(G4INCL::EventInfo& result) {

    for(int j=0; j < result.nParticles; j++){
        particle part;
        if(A[j] > 1){
          part.set_pdg_and_mass(PDGCode[j]*10 + 1e9);
        }
        else{
          part.set_pdg_and_mass(PDGCode[j]);
        }        
        part.set_momentum(px[j],py[j],pz[j]);
        if(emissionTime[j] < 0){
            part.endproc = deex;
        }
        else{
            part.endproc = escape;
        }

        e->post.push_back(part);

    }
    // Add particles to nuwro stack & fill endproc - jailed, escape, need also deex
    // Can't have particle tracking through FSI since pass them to INCL - can only fill endproc for escaped or deex particles


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
    inclcxx_initialize_default_nucleus_and_seed();

    // // Add the NuWro Incoming Particles
    inclcxx_add_struck_nucleons();

    // // Add the NuWro outgoing particles
    inclcxx_add_outgoing_particles();

    // // Run internal position swapper
    cascadeSeed->correctNucleusStruckNucleons(targetNucleus);

    // // Update INCLs energy calculations
    cascadeSeed->updateNucleusMissingEnergyAndCharge(targetNucleus);

    // /// START OF FSI CASCADE RUNNING
    G4INCL::EventInfo result;
    result = theINCLModel->processFSICascadeEvent(targetNucleus, cascadeSeed);

    // De-excite result
    if(theABLAModel != 0){
      theABLAModel->deExcite(&result);
    }

    // // Update stack with event info
    inclcxx_modify_nuwro_stack_with_result(result);


  return 0;
}