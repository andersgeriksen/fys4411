#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>


#include "sampler.h"
#include "system.h"
#include "particle.h"
#include "Hamiltonians/hamiltonian.h"
#include "WaveFunctions/wavefunction.h"

using std::cout;
using std::endl;


Sampler::Sampler(System* system) {
    m_system = system;
    m_stepNumber = 0;
}

void Sampler::setNumberOfMetropolisSteps(int steps) {
    m_numberOfMetropolisSteps = steps;
}

void Sampler::sample(bool acceptedStep) {
    // Make sure the sampling variable(s) are initialized at the first step.
    if (m_stepNumber == 0) {
        m_cumulativeEnergy = 0;
        m_energy2 = 0;
        m_acceptedSteps = 0;
    }

    /* Here you should sample all the interesting things you want to measure.
     * Note that there are (way) more than the single one here currently.
     */

    double localEnergy = m_system->getHamiltonian()->
                         computeLocalEnergy(m_system->getParticles());

    m_cumulativeEnergy  += localEnergy;

    m_cumulativeEnergy2  += localEnergy*localEnergy;
    m_acceptedSteps += acceptedStep;

    m_stepNumber++;

}

void Sampler::printOutputToTerminal() {
    int     np = m_system->getNumberOfParticles();
    int     nd = m_system->getNumberOfDimensions();
    int     ms = m_system->getNumberOfMetropolisSteps();
    int     p  = m_system->getWaveFunction()->getNumberOfParameters();
    double  ef = m_system->getEquilibrationFraction();
    std::vector<double> pa = m_system->getWaveFunction()->getParameters();
    //std::chrono::duration<double> time = m_system->getTimer()->timeDifference(0, 1);

    cout << endl;
    cout << "  -- System info -- " << endl;
    cout << " Number of particles  : " << np << endl;
    cout << " Number of dimensions : " << nd << endl;
    cout << " Number of Metropolis steps run : 10^" << std::log10(ms) << endl;
    cout << " Number of equilibration steps  : 10^" << std::log10(std::round(ms*ef)) << endl;
    cout << endl;
    cout << "  -- Wave function parameters -- " << endl;
    cout << " Number of parameters : " << p << endl;
    for (int i=0; i < p; i++) {
        cout << " Parameter " << i+1 << " : " << pa.at(i) << endl;
    }
    cout    <<  endl;
    cout    <<  "       ---- Reults -----           "   << endl;
    //cout    <<  " time spent                    :   "   <<  (double)time.count()<<"s"<<  endl;
    cout    <<  " Energy                        :   "   <<  m_energy        <<  endl;
    cout    <<  " Energy/particle               :   "   <<  m_energy/np     <<  endl;
    cout    <<  " Energy/(particle*dimension)   :   "   <<  m_energy/np/nd  <<  endl;
    cout    <<  " variance                      :   "   <<  m_variance      <<  endl;
    cout    <<  " acceptance ratio              :   "   <<  m_acceptRatio   <<  endl;
    cout    <<  endl;
}

void Sampler::computeAverages() {
    /* Compute the averages of the sampled quantities. You need to think
     * thoroughly through what is written here currently; is this correct?
     */
    m_N = m_stepNumber;
    m_energy = m_cumulativeEnergy   / m_N;
    m_energy2 = m_cumulativeEnergy2 / m_N;
    m_acceptRatio = m_acceptedSteps / m_N;
    
    m_variance = ( m_energy2 - m_energy*m_energy);
}

void Sampler::writeResults()
{
    /*
     * # comment
     * % comment 
     * header1 header2 math_expression 
     * {val}    {val}   {val}
     */
    std::vector<double> param = m_system->getWaveFunction()->getParameters();
    int MCC = std::log10((int)m_system->getNumberOfMetropolisSteps());

    std::ofstream ofile;
    std::string result, newdir; 
    std::string dir = "data";
    newdir = "./" + dir; 
    std::string resultfile = 
                "1e"+std::to_string(MCC)+"MC_"
            +   std::to_string(m_system->getNumberOfParticles())+"part_"
            +   std::to_string(m_system->getNumberOfDimensions())+"dim"+".txt";
    std::string header = "\\alpha\tE\t\\sigma^2\n";
    std::string directoryerror = "could not create "+newdir+".";
    result = std::to_string(param[0])
        +"\t"+std::to_string(m_energy)
        +"\t"+std::to_string(m_variance);
    

    if(!fileexists(dir) && mkdir(newdir.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH)<0) std::cout << directoryerror << std::endl;
    if(!fileexists(dir+"/"+resultfile)){ofile.open(dir+"/"+resultfile, std::ios::ate); ofile<<header; ofile.close();}

    if(!ofile.is_open()) ofile.open(dir+"/"+resultfile, std::ios::app);
    ofile << result+"\n";
    ofile.close();

}

void Sampler::writeTime(double time)
{
    int MCC = std::log10(m_system->getNumberOfMetropolisSteps());
    std::ofstream ofile;
    std::string s_time = std::to_string(time);
    std::string dir = "data";
    std::string resultfile = 
                "1e"+std::to_string(MCC)+"MC_"
            +   std::to_string(m_system->getNumberOfParticles())+"part_"
            +   std::to_string(m_system->getNumberOfDimensions())+"dim"+".txt";
    if(fileexists(dir+"/"+resultfile))
    {
        ofile.open(dir+"/"+resultfile, std::ios::app);
        ofile << "time(s)\t"+s_time+"\n";
        ofile.close();
    }
}

inline bool Sampler::fileexists( const std::string& name)
{
    struct stat buffer;
    return (stat (name.c_str(), &buffer) == 0);
}
