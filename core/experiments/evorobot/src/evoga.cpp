 /********************************************************************************
 *  SALSA Experiments Library                                                   *
 *  Copyright (C) 2007-2012                                                     *
 *  Stefano Nolfi <stefano.nolfi@istc.cnr.it>                                   *
 *  Onofrio Gigliotta <onofrio.gigliotta@istc.cnr.it>                           *
 *  Gianluca Massera <emmegian@yahoo.it>                                        *
 *  Tomassino Ferrauto <tomassino.ferrauto@istc.cnr.it>                         *
 *                                                                              *
 *  This program is free software; you can redistribute it and/or modify        *
 *  it under the terms of the GNU General Public License as published by        *
 *  the Free Software Foundation; either version 2 of the License, or           *
 *  (at your option) any later version.                                         *
 *                                                                              *
 *  This program is distributed in the hope that it will be useful,             *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of              *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the               *
 *  GNU General Public License for more details.                                *
 *                                                                              *
 *  You should have received a copy of the GNU General Public License           *
 *  along with this program; if not, write to the Free Software                 *
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA  *
 ********************************************************************************/

#include "evoga.h"
#include "evodataviewer.h"
#include "logger.h"
#include "randomgenerator.h"
#include <configurationhelper.h>
#include <QVector>
#include <QThreadPool>
#include <QtConcurrentMap>
#include <QtAlgorithms>
#include <QTime>
#include <QFile>

#include <cmath>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <unsupported/Eigen/MatrixFunctions>

#include <exception>

// All the suff below is to avoid warnings on Windows about the use of unsafe
// functions. This should be only a temporary workaround, the solution is stop
// using C string and file functions...
#if defined(_MSC_VER)
	#pragma warning(push)
	#pragma warning(disable:4996)
#endif

namespace salsa {

/*! \brief this is an helper class for implementing multithread in Evoga */
class EvaluatorThreadForEvoga
{
public:
	/**
	 * \brief Constructor
	 *
	 * \param ga a pointer to the genetic algorithm
	 * \param exp the experiment to run. We take ownership of the
	 *            experiment and free its memory at destruction
	 */
	EvaluatorThreadForEvoga(Evoga *ga, EvoRobotExperiment *exp) :
		m_ga(ga),
		m_exp(exp)
	{
	}

	/**
	 * \brief Destructor
	 */
	~EvaluatorThreadForEvoga()
	{
		delete m_exp;
	}

	/**
	 * \brief Configures the experiment with the given genotype id
	 *
	 * \param id the genotype id to use in this experiment
	 */
	void setGenotype(int id)
	{
		m_id = id;
		m_exp->setNetParameters(m_ga->getGenes(id));
	}

	/**
	 * \brief Returns the id of the genotype used in this experiment
	 *
	 * \return the id of the genotype used in this experiment
	 */
	int getGenotypeId() const
	{
		return m_id;
	}

	/**
	 * \brief Runs the experiment
	 */
	void run()
	{
		m_exp->doAllTrialsForIndividual(m_id);
		m_fitness = m_exp->getFitness();
	}

	/**
	 * \brief Returns the fitness of the last run
	 *
	 * \return the fitness of the last run
	 */
	double getFitness() const
	{
		return m_fitness;
	}

	/**
	 * \brief Returns the experiment we run
	 *
	 * \return the experiment we run
	 */
	EvoRobotExperiment* getExperiment()
	{
		return m_exp;
	}

private:
	/**
	 * \brief The generic algorithm object
	 */
	Evoga *const m_ga;

	/**
	 * \brief The experiment to run
	 */
	EvoRobotExperiment *const m_exp;

	/**
	 * \brief The id of the genotype used in this experiment
	 */
	int m_id;

	/**
	 * \brief The resulting fitness
	 */
	double m_fitness;
};

/**
 * \brief Executes the run() function of the given
 *        EvaluatorThreadForEvoga object
 */
void runEvaluatorThreadForEvoga(EvaluatorThreadForEvoga* e)
{
	e->run();
}

/**
 * \brief A simple structure keeping a fitness value and the id of a
 *        genotype
 */
struct FitnessAndId
{
	/**
	 * \brief Fitness
	 */
	float fitness;

	/**
	 * \brief Id
	 */
	int id;
};

/**
 * \brief Lesser-than operator overloading for FitnessAndId
 */
bool operator<(FitnessAndId first, FitnessAndId second)
{
	return (first.fitness < second.fitness);
}

int Evoga::mrand(int i)
{
	int r;

	r = rand();
	r = r % (int) i;
	return r;

}

Evoga::Evoga(ConfigurationManager& params)
	: Component(params)
	, exp(nullptr)
	, popSize(1)
	, glen(0)
	, elitism(false)
	, nreproducing(1)
	, noffspring(1)
	, genome()
	, bestgenome()
	, loadedIndividuals(1)
	, tfitness(nullptr)
	, ntfitness(nullptr)
	, statfit(nullptr)
	, terror(nullptr)
	, evolutionType()
	, nogenerations(0)
	, nreplications(1)
	, seed(1)
	, currentSeed(1)
	, mutation(0.05)
	, mutationdecay(0.01)
	, initial_mutation(0.05)
	, mutations(nullptr)
	, cgen(0)
	, savebest(1)
	, fmin(0.0)
	, fmax(0.0)
	, faverage(0.0)
	, fbest(0.0)
	, fbestgen(0)
	, ccycle(0)
	, stopEvolution(false)
	, isStepByStep(false)
	, mutexStepByStep()
	, waitForNextStep()
	, numThreads(1)
	, savePopulationEachNGenerations(0)
	, averageIndividualFitnessOverGenerations(true)
{
}

Evoga::~Evoga()
{
	delete exp;
	delete[] tfitness;
	delete[] terror;
	if (statfit != nullptr) {
		for(int i = 0; i < nogenerations; i++) {
			delete[] statfit[i];
		}
	}
	delete[] statfit;
	delete[] ntfitness;
	delete[] mutations;
}

void Evoga::setSeed(int s)
{
	srand(s);
	globalRNG->setSeed( s );
	currentSeed = s;
}

//return a random value between 0-1
double Evoga::drand()
{
	return (double)rand()/(double)RAND_MAX;
}

double Evoga::getNoise(double minn, double maxn)
{
	double nrange;
	if(maxn>minn)
		nrange=maxn-minn;
	else
		nrange=minn-maxn;

	return drand()*nrange+minn;
}

int Evoga::mutate(int w, double mut)
{
	int b[8];
	int val;
	int ii;

	val = w;
	for(ii=0;ii < 8;ii++) {
		b[ii] = val % 2;
		val  = val / 2;
	}
	for(ii=0;ii < 8;ii++) {
		//if (mrand(100) < percmut)
		if(drand()<mut) {
			b[ii] =(b[ii]+1)%2; // con questa modifica il bit switcha //mrand(2);
		}
	}
	w = 0;
	w += b[0] * 1;
	w += b[1] * 2;
	w += b[2] * 4;
	w += b[3] * 8;
	w += b[4] * 16;
	w += b[5] * 32;
	w += b[6] * 64;
	w += b[7] * 128;

	return(w);
}

void Evoga::putGenome(int fromgenome, int tobestgenome)
{
	if (tobestgenome < this->nreproducing) {
		for (int i = 0; i < this->glen; i++) {
			bestgenome[tobestgenome][i] = genome[fromgenome][i];
		}
	} else {
		Logger::error("putGenomeError!");
	}
}

void Evoga::getGenome(int frombestgenome, int togenome, int mut)
{
	for(int i = 0; i < this->glen; i++) {
		if (mut == 0) {
			genome[togenome][i] = bestgenome[frombestgenome][i];
		} else {
			if (mutations[i] == Evonet::DEFAULT_VALUE) {	//standard mutation
				genome[togenome][i] = mutate(bestgenome[frombestgenome][i], mutation);
			} else {				//specific mutation
				genome[togenome][i] = mutate(bestgenome[frombestgenome][i], mutations[i]);
			}
		}
	}
}

void Evoga::copyGenes(int from, int to, int mut)
{
	for(int i = 0; i < this->glen; i++) {
		if (mut == 0) {
			genome[to][i] = genome[from][i];
		} else {
			if (mutations[i] == Evonet::DEFAULT_VALUE) {	//standard mutation
				genome[to][i] = mutate(genome[from][i], mutation);
			} else {				//specific mutation
				genome[to][i] = mutate(genome[from][i], mutations[i]);
			}
		}
	}
}

//Reproduce individuals with higher ranking
void Evoga::reproduce()
{
	//to do
	//selecting best fathers
	int i;
	int bi,bx;
	double bn;

	char sbuffer[64];
	FILE *fp;

	//first of all we compute fitness stat
	this->computeFStat();


	for(bi=0;bi<this->nreproducing;bi++) {
		bn=-9999.0;
		bx=-1; //individual to be copied
		for(i=0;i< this->popSize;i++) {
			if(tfitness[i]>bn) {
				bn=tfitness[i];
				bx=i;
			}
		}

		this->putGenome(bx,bi);

		//here we save best genome
		if ((bi+1)<=this->savebest && cgen< this->nogenerations) {
			sprintf(sbuffer,"B%dS%d.gen",bi+1,this->currentSeed);
			if (cgen==0) {
				if ((fp=fopen(sbuffer, "w")) == nullptr) {
					Logger::error(QString("I cannot open file B%1S%2.gen").arg(bi+1).arg(this->currentSeed));
				}
			} else {
				if ((fp=fopen(sbuffer, "a")) == nullptr) {
					Logger::error(QString("I cannot open file B%1S%2.gen").arg(bi+1).arg(this->currentSeed));
				}
			}

			fprintf(fp,"**NET : s%d_%d.wts\n",cgen,bx);
			saveagenotype(fp,bx);
			fflush(fp);
			fclose(fp);
		}
		tfitness[bx]=-9999.0;
	}

	//reproducing best
	bx=0;
	for(bi=0;bi<this->nreproducing;bi++)
		for(i =0;i< this->noffspring;i++) {
			if(this->elitism && bi==0)
				this->getGenome(bi,bx,0);
			else
				this->getGenome(bi,bx,1);// 1 mette mutazione

			bx++;
		}

	//resetting fitness
	for (i=0;i<this->popSize;i++) tfitness[i]=0.0;
	this->saveFStat();
	cgen++;
}

void Evoga::saveBestInd()
{
	//to do
	//selecting best fathers
	int i;
	int bi;
	double bn, ffit;
	bn=-999999.0;// it was 0 possible source of bug in case of negsative fitness
	bi=-1;

	char sbuffer[64];
	FILE *fp;

	sprintf(sbuffer,"B%dS%d.gen",bi+1,this->currentSeed);
	if (cgen==0) {
		if ((fp=fopen(sbuffer, "w")) == nullptr) {
			Logger::error(QString("I cannot open file B%1S%2.gen").arg(bi+1).arg(this->currentSeed));
		}
	} else {
		if ((fp=fopen(sbuffer, "a")) == nullptr) {
			Logger::error(QString("I cannot open file B%1S%2.gen").arg(bi+1).arg(this->currentSeed));
		}
	}

	//finding the best simply the best, one individual
	for(i=0;i<this->popSize;i++) {
		ffit=this->tfitness[i]/this->ntfitness[i];
		if(ffit>bn) {
			bn=ffit;
			bi=i;
		}
	}

	//now saving
	fprintf(fp,"**NET : s%d_%d.wts\n",cgen,bi);
	saveagenotype(fp,bi);
	fflush(fp);
	fclose(fp);
}

void Evoga::saveBestTeam(QVector< QVector<int> > teams, QVector<double> fitness)
{
    //to do
    //selecting best fathers
    int i;
    int bi;
    double bn, ffit;
    bn=-999999.0;// it was 0 possible source of bug in case of negsative fitness
    bi=-1;
    double max;
    int indMax;

    char sbuffer[64];
    FILE *fp;

    sprintf(sbuffer,"B%dS%d.G%d.gen",bi+1,this->currentSeed,cgen);
    if ((fp=fopen(sbuffer, "w")) == nullptr) {
        Logger::error(QString("I cannot open file B%1S%2.G%3.gen").arg(bi+1).arg(this->currentSeed).arg(cgen));
    }

    max = fitness[0];
    indMax = 0;
    for(int i=1;i<fitness.size();i++){
        if(fitness[i]>max){
            max = fitness[i];
            indMax = i;
        }
    }

    for(int i=0;i<numModules;i++){
        //now saving
        fprintf(fp,"**NET : s%d_%d.wts\n",cgen,teams[indMax][i]);
        saveagenotype(fp,teams[indMax][i]);
    }
    fclose(fp);
    fflush(fp);
}


//Reproduce individuals with higher ranking
void Evoga::mreproduce()
{
	//to do
	//selecting best fathers
	int i;
	int bi,bx;
	double bn;

	char sbuffer[64];
	FILE *fp;

	//first of all we compute fitness stat
	this->computeFStat();


	for(bi=0;bi<this->nreproducing;bi++) {
		bn=9999.0;
		bx=-1; //individual to be copied
		for(i=0;i< this->popSize;i++) {
			if(tfitness[i]<bn) {
				bn=tfitness[i];
				bx=i;
			}
		}

		this->putGenome(bx,bi);

		//here we save best genome
		if ((bi+1)<=this->savebest && cgen< this->nogenerations) {
			sprintf(sbuffer,"B%dS%d.gen",bi+1,this->currentSeed);
			if (cgen==0) {
				if ((fp=fopen(sbuffer, "w")) == nullptr) {
					Logger::error(QString("I cannot open file B%1S%2.gen").arg(bi+1).arg(this->currentSeed));
				}
			} else {
				if ((fp=fopen(sbuffer, "a")) == nullptr) {
					Logger::error(QString("I cannot open file B%1S%2.gen").arg(bi+1).arg(this->currentSeed));
				}
			}

			fprintf(fp,"**NET : s%d_%d.wts\n",cgen,bx);
			saveagenotype(fp,bx);
			fflush(fp);
			fclose(fp);
		}
		tfitness[bx]=9999.0;
	}

	//reproducing best
	bx=0;
	for(bi=0;bi<this->nreproducing;bi++)
		for(i =0;i< this->noffspring;i++) {
			if(this->elitism && bi==0)
				this->getGenome(bi,bx,0);
			else
				this->getGenome(bi,bx,1);

			bx++;
		}

	//resetting fitness
	for (i=0;i<this->popSize;i++) tfitness[i]=9999.0;
	this->saveFStat();
	cgen++;
}


void Evoga::printPop()
{
	for(int i = 0; i < this->popSize; i++) {
		QString output = QString("Fit %1 | ").arg(tfitness[i]);
		for(int l = 0; l < this->glen; l++) {
			output += QString("%1 ").arg(this->genome[i][l]);
		}
		Logger::info(output);
	}
}

void Evoga::printBest()
{
	for(int i = 0; i < bestgenome.size(); i++) {
		QString output = QString("Best %d | ").arg(i);
		for (int s = 0; s < this->glen; s++) {
			output += QString("%1 ").arg(this->bestgenome[i][s]);
		}
		Logger::info(output);
	}
}

void Evoga::computeFStat()
{
	int i;
	double min, max, av;

	min=max=tfitness[0];
	av=0.0;

	for(i=0;i<this->popSize;i++) {
		if(tfitness[i]<min) min=tfitness[i];
		if(tfitness[i]>max) max=tfitness[i];
		av+=tfitness[i];
	}
	this->faverage=av/(double)this->popSize;
	this->fmax=max;
	this->fmin=min;
	this->statfit[this->cgen][0]=this->fmax;
	this->statfit[this->cgen][1]=this->faverage;
	this->statfit[this->cgen][2]=this->fmin;

	if (this->fmax > this->fbest) {
		this->fbest = this->fmax;
		this->fbestgen = this->cgen;
	}
}

void Evoga::computeFStat2()
{
	int i;
	double min, max, av;

	//min=max=tfitness[0]/ntfitness[0];
	//try to fix a problem
	min=9999.00;
	max=-9999.00;
	av=0.0;

	for(i=0;i<this->popSize;i++) {
		if((tfitness[i]/ntfitness[i])<min) {
			min=tfitness[i]/ntfitness[i];
		}
		if((tfitness[i]/ntfitness[i])>max) max=tfitness[i]/ntfitness[i];
		av+=(tfitness[i]/ntfitness[i]);
	}
	this->faverage=av/(double)this->popSize;
	this->fmax=max;
	this->fmin=min;
	this->statfit[this->cgen][0]=this->fmax;
	this->statfit[this->cgen][1]=this->faverage;
	this->statfit[this->cgen][2]=this->fmin;

	if (this->fmax > this->fbest) {
		this->fbest = this->fmax;
		this->fbestgen = this->cgen;
	}
}

void Evoga::saveagenotype(FILE *fp, int ind)
{
	int j;
	fprintf(fp, "DYNAMICAL NN\n");
	for (j=0; j < this->glen; j++)
		fprintf(fp, "%d\n", this->genome[ind][j]);
	fprintf(fp, "END\n");
}

//save all current generation
void Evoga::saveallg()
{
	FILE *fp;
	char filename[64];
	int i;

	sprintf(filename,"G%dS%d.gen",cgen,currentSeed);
	if ((fp=fopen(filename, "w+")) == nullptr) {
		Logger::error(QString("Cannot open file %1").arg(filename));
	} else {
		//we save
		for(i=0;i<this->popSize;i++) {
			fprintf(fp,"**NET : %d_%d_%d.wts\n",cgen,0,i);
			this->saveagenotype(fp,i);
		}
		fclose( fp );
	}
}

void Evoga::saveallgComposed(QVector< QVector<int> > composedGen)
{
    FILE *fp;
    char filename[64];
    int i;

    sprintf(filename,"G%dS%d.composed.gen",cgen,currentSeed);
    if ((fp=fopen(filename, "w+")) == nullptr) {
        Logger::error(QString("Cannot open file %1").arg(filename));
    } else {
        //we save
        for(i=0;i<composedGen.size();i++) {
            fprintf(fp,"**TEAM : %d_%d_%d.wts\n",cgen,0,i);
            for(int j=0;j<numModules;j++)
                fprintf(fp,"%d ",composedGen[i][j]);

            fprintf(fp, "\nEND\n");
        }
        fclose( fp );
    }
}


void Evoga::saveFStat()
{
	FILE *fp;
	char sbuffer[128];
	sprintf(sbuffer,"statS%d.fit",currentSeed);
        if (cgen == 0)
		fp=fopen(sbuffer , "w");
	else
		fp=fopen(sbuffer , "a");

	if (fp != nullptr) {
		fprintf(fp,"%.3f %.3f %.3f\n",fmax,faverage,fmin);
		fclose(fp);
	} else
		Logger::error("unable to save statistics on a file");
}

void Evoga::saveRStat(QVector<int> subsVec)
{
    FILE *fp;
    char sbuffer[128];
    sprintf(sbuffer,"statS%d.ret",currentSeed);

    if (cgen == 0)
        fp=fopen(sbuffer , "w");
    else
        fp=fopen(sbuffer , "a");

    if(fp!=nullptr){
        for(int i=0;i<subsVec.size();i++){
            fprintf(fp,"%i ",subsVec[i]);
        }
        fprintf(fp,"\n");
        fclose(fp);
    } else
        Logger::error("unable to save statistics of retentions on a file");
}

void Evoga::getLastFStat( double &min, double &max, double &average ) {
	min = fmin;
	max = fmax;
	average = faverage;
}

void Evoga::loadgenotype(FILE *fp, int ind)
{
	int j;
	int v;

	fscanf(fp, "DYNAMICAL NN\n");
	for (j=0; j <this->glen; j++) {
		fscanf(fp,"%d\n",&v);//this->genome[ind][j]);
		this->genome[ind][j]=v;
	}
	fscanf(fp, "END\n");
}

int Evoga::loadallg(int gen, const char *filew)
{
	FILE *fp;
	char filename[512];//[64];
	char message[512];//[128];
	char flag[512];//[64];

	if (gen >= 0) {
		sprintf(filename, "G%dS%d.gen", gen, seed);
	} else {
		sprintf(filename, "%s", filew);//in case of B%P$S.gen
	}

	if ((fp = fopen(filename, "r")) != nullptr) {
		genome.clear();
		while (true) {
			flag[0] = '\0'; //sprintf(flag,""); //flag = nullptr;
			fscanf(fp, "%s : %s\n", flag, message);
			if (strcmp(flag, "**NET") == 0) {
				loadgenotype(fp, genome.addOne());
			} else {
				break;
			}
		}
		Logger::info(QString("Loaded ind: %1").arg(genome.size()));
		fclose(fp);
	} else {
		Logger::error(QString("File %1 could not be opened").arg(filename));
	}

	loadedIndividuals = genome.size();

	return genome.size();
}

void Evoga::loadallTeams(int gen, const char *filew, QVector< QVector<int> > &teams)
{
    FILE *fp;
    char filename[512];//[64];
    char message[512];//[128];
    char flag[512];//[64];

    if (gen >= 0) {
        sprintf(filename, "G%dS%d.composed.gen", gen, seed);
    } else {
        sprintf(filename, "%s", filew);//in case of B%P$S.gen
    }

    if ((fp = fopen(filename, "r")) != nullptr) {
        int teamInd=0;
        while (true) {
            if(teams.size()<teamInd+1){
                teams.resize(teamInd+1);
                teams[teamInd].resize(numModules);
            }
            flag[0] = '\0'; //sprintf(flag,""); //flag = nullptr;
            fscanf(fp, "%s : %s\n", flag, message);
            if (strcmp(flag, "**TEAM") == 0) {
                for(int i=0;i<numModules;i++){

                    fscanf(fp,"%i",&teams[teamInd][i]);

                    int w=0;
                }
                fscanf(fp,"\n");
                fscanf(fp, "END\n");
                teamInd++;
            } else {
                break;
            }
        }
        Logger::info(QString("Loaded teams: %1").arg(teamInd));
        fclose(fp);
    } else {
        Logger::error(QString("File %1 could not be opened").arg(filename));
    }

}

/*
 * load a .fit file (return the number loaded individuals, 0 if the file does not exists)
 */
int Evoga::loadStatistics(char *filename)
{
	FILE *fp;
	int loaded=0;
	int i=0;
	float max,av,min;
	max=min=av=-1;
	if ((fp=fopen(filename, "r")) != nullptr) {
		while(fscanf(fp,"%f %f %f\n",&max,&av,&min)!=EOF) {
			this->statfit[i][0]=max;
			this->statfit[i][1]=av;
			this->statfit[i][2]=min;
			i++;
		}
		loaded=i;
		fflush(fp);
		fclose(fp);
		return(loaded);
	} else {
		return(0);
	}
}

void Evoga::randomizePop()
{
	for (int i = 0; i < genome.size(); i++) {
		for(int g = 0; g < glen; g++) {
			genome[i][g] = mrand(256);
		}
	}
}

void Evoga::setInitialPopulation(int* ge)
{
	int i,g;

	//fill genome with .phe parameters
	for(i=0; i<genome.size(); i++)
		for(g=0; g<glen; g++)
		{
			if(ge[g] == Evonet::DEFAULT_VALUE)
				genome[i][g] = mrand(256);
			else
				genome[i][g] = ge[g];
		}
}

void Evoga::setMutations(float* mut)
{
	//fill mutation vector
	for(int i=0; i<glen; i++)
		mutations[i] = mut[i];
}

int* Evoga::getGenes(int ind)
{
#if defined(__GNUC__) && defined(DEVELOPER_WARNINGS)
	#warning CONTROLLARE INDICE? ALLE VOLTE CRASHA SE NELLA GUI SI SELEZIONA UN INDICE OLTRE IL MASSIMO
#endif
	return this->genome[ind];
}

int* Evoga::getBestGenes(int ind)
{
	return this->bestgenome[ind];
}

void Evoga::resetGenerationCounter()
{
	this->cgen=0;
}

void Evoga::getPheParametersAndMutationsFromEvonet()
{
	Evonet* evonet = getResource<Evonet>( "evonet" );
	if (evonet->pheFileLoaded()) {
		//temporary mutations vector
		float* muts = new float[evonet->freeParameters()];
		int* pheParams = new int[evonet->freeParameters()];

		//setting the mutation vector
		evonet->getMutations(muts);	//taking it from the net
		setMutations(muts);		//pushing it inside GA

		//setting initial genome
		evonet->copyPheParameters(pheParams);	//copy *phe parameters
		setInitialPopulation(pheParams);	//put *phe parameters inside genome

		delete[] muts;
		delete[] pheParams;
	}
}

void Evoga::updateGenomeFromEvonet(int ind) {

	Evonet* evonet = getResource<Evonet>( "evonet" );

	float wrange = evonet->getWrange();

	for (int i = 0; i<glen; i++) {
		genome[ind][i] = -(evonet->getFreeParameter(i)-wrange)/(2*wrange)*255;
	}
	/*
	printf("%s : %d : ",__PRETTY_FUNCTION__,ind);
	for (int i = 0; i<glen; i++) {
		printf("%d ",genome[ind][i]);
	}
	printf("\n");
	*/
}

QString Evoga::getEvolutionType()
{
	return evolutionType;
}

void Evoga::saveBestFitness()
{
	FILE *fp;
	char sbuffer[128];
	sprintf(sbuffer, "bestgenS%d.fit", currentSeed);
	if (cgen == 0)
	{
		fp = fopen(sbuffer, "w");
	}
	else
	{
		fp = fopen(sbuffer, "a");
	}

	if (fp != nullptr)
	{
		fprintf(fp, "%d %.3f\n", fbestgen, fbest);
		fclose(fp);
	}
	else
	{
		Logger::error("unable to save best generation statistics on a file");
	}
}

/*
 * Main function of the Genetic Algorithm (Steady State Version)
 */
void Evoga::    evolveSteadyState()
{
	int rp; //replication
	int gn; //generation
	int id; //individuals
	double fit;
	double minfit=9999;
	int    minid=-1;
	double mfit;
	float  final_mrate;
	int startGeneration=0;
	char statfile[128];
	char genFile[128];
	char filename[64];
    QVector< int > subsVec;
    double currentRetentionRate;

    subsVec.resize(popSize);
	// Resizing genome
	genome.resize(popSize * 2);

	final_mrate = mutation;
	Logger::info("EVOLUTION: steady state");
	Logger::info("Number of replications: " + QString::number(nreplications));

	// Creating evaluator objects in case of a multithread simulation. Also setting the actual number of threads used
	QVector<EvaluatorThreadForEvoga*> evaluators(popSize, nullptr);
	if (numThreads > 1) {
		const QString experimentGroup = confPath() + "Experiment";
		for (int i = 0; i < evaluators.size(); i++) {
			// Duplicating group
			const QString copiedExperimentGroup = experimentGroup + ":" + QString::number(i);
			configurationManager().copyGroup(experimentGroup, copiedExperimentGroup);

			EvoRobotExperiment* newExp = configurationManager().getComponentFromGroup<EvoRobotExperiment>(copiedExperimentGroup);
			newExp->setEvoga(this);
			evaluators[i] = new EvaluatorThreadForEvoga(this, newExp);
		}
		QThreadPool::globalInstance()->setMaxThreadCount(numThreads);
	}

	for(rp=0;rp<nreplications;rp++) {	// replications
		startGeneration = 0;
        limitationFactor = 1.0;
		mutation=initial_mutation; // initially mutation (default 50%)
		//setSeed(getSeed());
		setSeed(getStartingSeed()+rp);
		Logger::info(QString("Replication %1, seed: %2").arg(rp+1).arg(getStartingSeed()+rp));
		resetGenerationCounter();
		randomizePop();
		// --- section runnable only if there is an Evonet object
		if ( resourceExists( "evonet" ) ) {
			getPheParametersAndMutationsFromEvonet();
		}

		// Set fbest to a very low value
		this->fbest = -99999.0;

		emit startingReplication( rp );

		// Resetting seed in experiments
		exp->newGASeed(getCurrentSeed());
		if (numThreads > 1) {
			for (int i = 0; i < evaluators.size(); i++) {
				evaluators[i]->getExperiment()->newGASeed(getCurrentSeed());
			}
		}

		QTime evotimer;
		evotimer.start();
		for (int i=0;i<popSize+1;i++) {
			tfitness[i]=0.0;
			ntfitness[i]=0.0;
		}
		//code to recovery a previous evolution: Experimental
		sprintf(statfile,"statS%d.fit", getStartingSeed()+rp);
		//now check if the file exists
		DataChunk statTest(QString("stattest"),Qt::blue,2000,false);
		if (statTest.loadRawData(QString(statfile),0)) {
			startGeneration=statTest.getIndex();
			sprintf(genFile,"G%dS%d.gen",startGeneration,getStartingSeed()+rp);
			Logger::info("Recovering from startGeneration: " + QString::number(startGeneration));
			Logger::info(QString("Loading file: ") + genFile);
            loadallg(-1,genFile);
			cgen=startGeneration;
			mutation=mutation-startGeneration*mutationdecay;
			if (mutation<final_mrate) mutation=final_mrate;
			// Resizing genome the loading process changed the genome size = popSize
			genome.resize(popSize * 2);
			emit recoveredInterruptedEvolution( QString(statfile) );
		} //end evolution recovery code

		for(gn=startGeneration;gn<nogenerations;gn++) {	// generations
			evotimer.restart();
            currentRetentionRate = 0.0;
			Logger::info(" Generation " + QString::number(gn+1));
			// Here we do this to avoid too many complications: if we have to run no threads, we use the old
			// code, otherwise we go for the multithread code below
			if (numThreads <= 1) {
				exp->initGeneration(gn);
				if ( commitStep() ) { return; }
				// Not running with multiple threads, using the old code
				for(id=0;id<popSize;id++) {	//individuals
					fit=0.0;
					exp->setNetParameters(getGenes(id)); // get the free parameters from the genotype
					exp->doAllTrialsForIndividual(id);
					fit = exp->getFitness();
					if (averageIndividualFitnessOverGenerations) {
						tfitness[id] += fit;
						ntfitness[id]++;
					} else {
						tfitness[id] = fit;
						ntfitness[id] = 1;
					}
					if (isStopped()) { // stop evolution
						return;
					}
                    copyGenes(id, popSize+id,1); //generate a variation by duplicating and mutating
                    tfitness[popSize+id]=0;
                    ntfitness[popSize+id]=0;

					exp->setNetParameters(getGenes(popSize)); // get the free parameters from the genotype
					exp->doAllTrialsForIndividual(popSize + id);
					fit = exp->getFitness();
					if (averageIndividualFitnessOverGenerations) {
                        tfitness[popSize+id] += fit;
                        ntfitness[popSize+id]++;
					} else {
                        tfitness[popSize+id] = fit;
                        ntfitness[popSize+id] = 1;
					}
					if (isStopped()) { // stop evolution
						return;
					}

				}
				exp->endGeneration(gn);
				if ( commitStep() ) { return; }
			} else {
				// Multithread code

				// Calling initGeneration on all evaluator
				for (int i = 0; i < popSize; i++) {
					evaluators[i]->getExperiment()->initGeneration(gn);
				}
				if (commitStep()) return; // stop the evolution process

				// We first evaluate all parents, so setting genotypes of parents (we have as many evaluators as individuals)
				for (int i = 0; i < popSize; i++) {
					evaluators[i]->setGenotype(i);
				}
				if (commitStep()) return; // stop the evolution process

				// Now starting parallel evaluation of parents and wating for it to finish
				QFuture<void> evaluationFuture = QtConcurrent::map(evaluators, runEvaluatorThreadForEvoga);
				evaluationFuture.waitForFinished();
				if (commitStep()) return; // stop the evolution process

				// We have finished evaluating parents, updating the fitness vectors
				for (int i = 0; i < popSize; i++) {
					if (averageIndividualFitnessOverGenerations) {
						tfitness[evaluators[i]->getGenotypeId()] += evaluators[i]->getFitness();
						ntfitness[evaluators[i]->getGenotypeId()]++;
					} else {
						tfitness[evaluators[i]->getGenotypeId()] = evaluators[i]->getFitness();
						ntfitness[evaluators[i]->getGenotypeId()] = 1;
					}
				}
				if (commitStep()) return; // stop the evolution process

				// Now we can generate all children for all individuals and set them in the evaluators
				for (int i = 0; i < popSize; i++) {
					copyGenes(i, popSize + i, 1); //generate a variation by duplicating and mutating
					tfitness[popSize + i] = 0;
					ntfitness[popSize + i] = 0;
					evaluators[i]->setGenotype(popSize + i);
				}
				if (commitStep()) return; // stop the evolution process

				// Now starting parallel evaluation of children and wating for it to finish
				evaluationFuture = QtConcurrent::map(evaluators, runEvaluatorThreadForEvoga);
				evaluationFuture.waitForFinished();
				if (commitStep()) return; // stop the evolution process

				// We have finished evaluating parents, updating the fitness vectors
				for (int i = 0; i < popSize; i++) {
					if (averageIndividualFitnessOverGenerations) {
						tfitness[evaluators[i]->getGenotypeId()] += evaluators[i]->getFitness();
						ntfitness[evaluators[i]->getGenotypeId()]++;
					} else {
						tfitness[evaluators[i]->getGenotypeId()] = evaluators[i]->getFitness();
						ntfitness[evaluators[i]->getGenotypeId()] = 1;
					}
				}
				if (commitStep()) return; // stop the evolution process

				// Calling endGeneration on all evaluator
				for (int i = 0; i < popSize; i++) {
					evaluators[i]->getExperiment()->endGeneration(gn);
				}
				if (commitStep()) return; // stop the evolution process

			}

            // ========= Selection part ========== //
            // Finally, we look for the worst individuals (parents) and substitute them with best children.
            // What we do is: we order both the parents and the children in descending order, then we take the
            // popSize best individuals. This is not the same as what is done in the sequential version of
            // the algorithm, but should be similar. We overwrite the worst parents with the best children
            QVector<FitnessAndId> parents(popSize);
            for (int i = 0; i < popSize; i++) {
                parents[i].fitness = tfitness[i] / ntfitness[i];
                parents[i].id = i;
            }
            QVector<FitnessAndId> children(popSize);
            for (int i = 0; i < popSize; i++) {
                children[i].fitness = tfitness[popSize + i] / ntfitness[popSize + i];
                children[i].id = popSize + i;
            }
            // Sorting both parents and children. They are sorted in ascending order but we need the best
            // individuals (those with the highest fitness) first
            qSort(parents);
            qSort(children);
            int p = popSize - 1;
            int c = popSize - 1;
            for (int i = 0; i < popSize; i++) {
                if(limitRetention)
                    children[c].fitness *= limitationFactor;
                if (parents[p].fitness > children[c].fitness) {
                    // No need to swap, parents are already in the population vector
                    p--;
                } else {
                    // Swapping with one of the worst parents (we know for sure that p + c = popSize)
                    copyGenes(children[c].id, parents[popSize - 1 - c].id, 0);
                    tfitness[parents[popSize - 1 - c].id] = tfitness[children[c].id];
                    ntfitness[parents[popSize - 1 - c].id] = ntfitness[children[c].id];
                    subsVec[children[c].id-popSize] = parents[popSize - 1 - c].id;
                    c--;
                    currentRetentionRate += 1.0/popSize;
                }
            }
            for (int i=0; i<=c;i++)
                subsVec[children[i].id-popSize] = -1;

			saveBestInd();
			computeFStat2();
			saveFStat();

            if(saveRetStat)
               saveRStat(subsVec);

			emit endGeneration( cgen, fmax, faverage, fmin );
			if (commitStep()) {
				return; // stop the evolution process
			}

			cgen++;
			if (mutation > final_mrate) {
				mutation -= mutationdecay;
			} else {
				mutation = final_mrate;
			}

            limitationFactor += (targetRetentionRate-currentRetentionRate)/10.0;
            if (limitationFactor > 1.0)
                limitationFactor = 1.0;

			//always save in order to be able to resume the evolution process, but keep only the last gen file unless it has to be saved by the param savePopulationEachNGenerations
			saveallg();

			//remove the previous genfile unless it has to be kept because of the savePopulationEachNGenerations param
			if ((savePopulationEachNGenerations == 0) || (gn>1 && ((gn-1) % (savePopulationEachNGenerations) != 0))) {
				//EX: gn 998 = G999S1.gen --- gn 999 = G1000S1.gen --- gn = 1000 = G1001S1.gen --- gn 1001 = G1002S1.gen
				sprintf(filename,"G%dS%d.gen",gn,currentSeed);
				if(!QFile::remove(filename)) {
					Logger::warning(QString("Error deleting temporary gen file: ") + QString::fromStdString(filename));
				}
			}

            Logger::info(QString("Generation %1 took %2 minutes - Best fitness = %3").arg(gn+1).arg((double)evotimer.elapsed()/60000.0, 0, 'f', 2).arg(fmax));
            if(limitRetention)
                Logger::info(QString(" --- Target retention rate: %1; current rate: %2; fitness limitation factor: %3").arg(targetRetentionRate).arg(currentRetentionRate).arg(limitationFactor));
			fflush(stdout);
		}
		saveallg();

		// Save the best generation fitness statistics
		saveBestFitness();
	}

	// Deleting all evaluators
	for (int i = 0; i < evaluators.size(); i++) {
		delete evaluators[i];
	}
}

/*
 * Main function of the Genetic Algorithm (generational version with truncation selection)
 */
void Evoga::evolveGenerational()
{
	int rp;	//replication
	int gn;	//generation
	int id;	//individuals
	double fit;
	int startGeneration=0;
    char statfile[128];
	char genFile[128];

	// Resizing genome
	genome.resize(popSize);

	for(rp=0;rp<nreplications;rp++) {// replications
		startGeneration = 0;
		setSeed(getStartingSeed()+rp);
		Logger::info(QString("Replication %1 seed: %2").arg(rp+1).arg(getStartingSeed()));
		resetGenerationCounter();
		randomizePop();
		// --- section runnable only if there is an Evonet object
		if ( resourceExists( "evonet" ) ) {
			getPheParametersAndMutationsFromEvonet();
		}

		// Set fbest to a very low value
		this->fbest = -99999.0;

		emit startingReplication( rp );

		// Resetting seed in experiments
		exp->newGASeed(getCurrentSeed());

		QTime evotimer;
		evotimer.start();

		//code to recovery a previous evolution: Experimental
		sprintf(statfile,"statS%d.fit", getStartingSeed()+rp);
		//now check if the file exists
		DataChunk statTest(QString("stattest"),Qt::blue,2000,false);
		if (statTest.loadRawData(QString(statfile),0)) {
			startGeneration=statTest.getIndex();
			sprintf(genFile,"G%dS%d.gen",startGeneration,getStartingSeed()+rp);
			Logger::info("Recovering from startGeneration: " + QString::number(startGeneration));
			Logger::info(QString("Loading file: ") + genFile);
			loadallg(startGeneration,genFile);
			emit recoveredInterruptedEvolution( QString(statfile) );
		} //end evolution recovery code

		for(gn=startGeneration;gn<nogenerations;gn++) { // generations
			evotimer.restart();
			Logger::info(" Generation " + QString::number(gn+1));
			exp->initGeneration( gn );
			for(id=0;id<popSize;id++) { //individuals
				exp->setNetParameters(getGenes(id)); // get the free parameters from the genotype
				exp->doAllTrialsForIndividual(id);
				fit = exp->getFitness();
				tfitness[id]=fit;
				if (commitStep()) { // stop evolution
					return;
				}
			}
			reproduce();

			emit endGeneration( gn, fmax, faverage, fmin );
			exp->endGeneration( gn );

			if(savePopulationEachNGenerations!=0 && gn%savePopulationEachNGenerations == 0)
				saveallg();

			Logger::info(QString("Generation %1 took %2 minutes").arg(gn+1).arg((double)evotimer.elapsed()/60000.0, 0, 'f', 2));
			fflush(stdout);
		}

		saveallg();

		// Save the best generation fitness statistics
		saveBestFitness();
	}
}


// this function generate a random seed for initialize the random number generator
unsigned int generateRandomSeed() {
	// this number is always different amongs processes because this function
	// is on the stack of the process
	unsigned long int stackMem = (unsigned long int)( generateRandomSeed );
	// time on which the process has been started
#ifdef SALSA_WIN
	unsigned long int startTime = GetTickCount();
#else
	unsigned long int startTime = time(nullptr);
#endif
	// the seed is generated mixing the values above
	unsigned long int randSeed = 0;

	stackMem=stackMem-startTime;  stackMem=stackMem-randSeed;  stackMem=stackMem^(randSeed >> 13);
	startTime=startTime-randSeed;  startTime=startTime-stackMem;  startTime=startTime^(stackMem << 8);
	randSeed=randSeed-stackMem;  randSeed=randSeed-startTime;  randSeed=randSeed^(startTime >> 13);
	stackMem=stackMem-startTime;  stackMem=stackMem-randSeed;  stackMem=stackMem^(randSeed >> 12);
	startTime=startTime-randSeed;  startTime=startTime-stackMem;  startTime=startTime^(stackMem << 16);
	randSeed=randSeed-stackMem;  randSeed=randSeed-startTime;  randSeed=randSeed^(startTime >> 5);
	stackMem=stackMem-startTime;  stackMem=stackMem-randSeed;  stackMem=stackMem^(randSeed >> 3);
	startTime=startTime-randSeed;  startTime=startTime-stackMem;  startTime=startTime^(stackMem << 10);
	randSeed=randSeed-stackMem;  randSeed=randSeed-startTime;  randSeed=randSeed^(startTime >> 15);

	return randSeed%10000;
}

void Evoga::configure()
{
	genome.clear();
	bestgenome.clear();

	evolutionType = ConfigurationHelper::getEnum(configurationManager(), confPath() + "evolutionType");
	nogenerations = ConfigurationHelper::getInt(configurationManager(), confPath() + "ngenerations");
	nreplications = ConfigurationHelper::getInt(configurationManager(), confPath() + "nreplications");
	nreproducing = ConfigurationHelper::getInt(configurationManager(), confPath() + "nreproducing");
	noffspring = ConfigurationHelper::getInt(configurationManager(), confPath() + "noffspring");
	// starting seed - if not specified (or setted as 0) it auto generate a seed
	seed = ConfigurationHelper::getInt(configurationManager(), confPath() + "seed");
	if ( seed == 0 ) {
		seed = generateRandomSeed();
	}
	Logger::info(QString("Evoga - Random seed set to ") + QString::number(seed));

	savebest = ConfigurationHelper::getInt(configurationManager(), confPath() + "savenbest");
	elitism = ConfigurationHelper::getBool(configurationManager(), confPath() + "elitism");
	numThreads = ConfigurationHelper::getInt(configurationManager(), confPath() + "numThreads");
	savePopulationEachNGenerations = ConfigurationHelper::getInt(configurationManager(), confPath() + "savePopulationEachNGenerations");
	averageIndividualFitnessOverGenerations = ConfigurationHelper::getBool(configurationManager(), confPath() + "averageIndividualFitnessOverGenerations");
    saveRetStat = ConfigurationHelper::getBool(configurationManager(), confPath() + "saveRetetionStatistics");

    limitRetention =  ConfigurationHelper::getBool(configurationManager(), confPath() + "limitRetention");
    targetRetentionRate =  ConfigurationHelper::getReal(configurationManager(), confPath() + "targetRetentionRate");
    numModules = ConfigurationHelper::getInt(configurationManager(), confPath() + "numModules");

    crossRate = ConfigurationHelper::getReal(configurationManager(), confPath() + "crossoverRate");
    modulesMutationRate = ConfigurationHelper::getReal(configurationManager(), confPath() + "modulesMutationRate");
    overwriteRate = ConfigurationHelper::getReal(configurationManager(), confPath() + "overwriteRate");
    mutateOnlyRelatives = ConfigurationHelper::getBool(configurationManager(), confPath() + "mutateOnlyRelatives");
    rankBasedProb = ConfigurationHelper::getReal(configurationManager(), confPath() + "rankBasedProbability");
    selectionType = ConfigurationHelper::getEnum(configurationManager(), confPath() + "selectionType");

	//mutation rate can be written both as int or as double
	mutation = ConfigurationHelper::getReal(configurationManager(), confPath() + "mutation_rate");
	if(mutation >= 1) {
		mutation /= 100;
	}
	//speed of decay (only valid for steady-state GA)
	mutationdecay = ConfigurationHelper::getReal(configurationManager(), confPath() + "mutation_decay");
	initial_mutation = ConfigurationHelper::getReal(configurationManager(), confPath() + "initial_mutation");

	useGaussian = ConfigurationHelper::getBool(configurationManager(), confPath() + "useGaussian");
	gaussianMean = ConfigurationHelper::getReal(configurationManager(), confPath() + "gaussianMean");
	gaussianStdDev = ConfigurationHelper::getReal(configurationManager(), confPath() + "gaussianStdDev");
	offspringStdDev = ConfigurationHelper::getReal(configurationManager(), confPath() + "offspringStdDev");
	variableStdDev = ConfigurationHelper::getBool(configurationManager(), confPath() + "variableStdDev");
	mutationLearningRate = ConfigurationHelper::getReal(configurationManager(), confPath() + "mutationLearningRate");
	backPropOffspringFitnessIncrease = ConfigurationHelper::getBool(configurationManager(), confPath() + "backPropOffspringFitnessIncrease");
	backPropOffspringFitnessIncreasePercentage = ConfigurationHelper::getReal(configurationManager(), confPath() + "backPropOffspringFitnessIncreasePercentage");
	pheGen = ConfigurationHelper::getInt(configurationManager(), confPath() + "pheGen");
	minimization = ConfigurationHelper::getBool(configurationManager(), confPath() + "minimization");
	specialUtilityRanking = ConfigurationHelper::getBool(configurationManager(), confPath() + "specialUtilityRanking");

	exp = configurationManager().getComponentFromGroup<EvoRobotExperiment>(confPath() + "Experiment", false);
	exp->setEvoga(this);
	Logger::info( "Created EvoRobotExperiment " + configurationManager().getValue(confPath()+"Experiment/type") + " from group " + confPath() + "Experiment" );
}

void Evoga::describe(RegisteredComponentDescriptor& d)
{
	Component::describe(d);

	d.help("Implements the genetic algorithm developed by Stefano Nolfi" );

	d.describeEnum( "evolutionType" ).def("steadyState").values( QStringList() << "steadyState" << "generational").props( ParamIsMandatory ).help("Specify the type of evolution process to execute");
	d.describeInt( "ngenerations" ).def(100).limits(0,MaxInteger).help("Number of generations");
	d.describeInt( "nreplications" ).def(10).limits(1,MaxInteger).help("The number of which the evolution process will be replicated with a different random initial population");
	d.describeInt( "nreproducing" ).def(20).limits(1,MaxInteger).help("The number of individual allowed to produce offsprings; The size of populazion will be nreproducing x noffspring");
	d.describeInt( "noffspring" ).def(5).limits(1,MaxInteger).help("The number of offsprings generated by an individual; The size of populazion will be nreproducing x noffspring");
	d.describeInt( "seed" ).def(1234).limits(0,MaxInteger).help("The number used to initialize the random number generator", "When a new replication will start, this value will be incremented by one to guarantee a truly different initial population for the next replica. If set to 0 uses a random seed");
	d.describeInt("savenbest").def(1).limits(1,MaxInteger).help("The number of best genotypes to save each generation");
	d.describeBool("elitism").def(false).help("If use elitism or not");
	d.describeInt("numThreads").def(1).limits(1,MaxInteger).help("The number of thread used to parallelize the evaluation of individuals");
	d.describeInt("savePopulationEachNGenerations").def(0).limits(0,MaxInteger).help("If is zero only the population of the last generation are saved into a file; otherwise it saves the population each N generations done");
	d.describeReal("mutation_rate").def(0.05).limits(0,100).help("The mutation rate", "The rate at which a mutation will occur during a genotype copy; a real value below 1 (i.e. 0.12) is considered as a rate (i.e. 0.12 correspond to 12% of mutation); a value egual or above 1 is considered as a percentage of mutation (i.e. 25 correspond to 25% of mutation, or 0.25 rate of mutation)");
	d.describeReal("mutation_decay").def(0.01).limits(0,1).help("At first generation the mutation rate will be always 0.5, and at each generation done the mutation rate will be decreased by this value until it reachs the mutation_rate value");
	d.describeReal("initial_mutation").def(0.5).limits(0,1).help("The initial value of the mutation rate in case of the steadyState evolution type");
	d.describeSubgroup( "Experiment" ).props(ParamIsMandatory).componentType("EvoRobotExperiment").help("The object delegated to simulate and to evaluate the fitness of an individual");
	d.describeBool("averageIndividualFitnessOverGenerations").def(true).help("Whether to average the current fitness with the previous one or not");
    d.describeBool("saveRetetionStatistics").def(false).help("Whether to save the retetions statistics or not");
	d.describeReal("mutationRange").def(0.0).help("The mutation range used to generate the individual in case of the xnes evolution type");
	d.describeReal("offspringMutRange").def(0.0).help("The mutation range used to generate the offspring in case of the xnes evolution type");
	d.describeBool("useGaussian").def(false).help("Whether to use a gaussian distribution or not in order to generate offspring in case of the xnes evolution type");
	d.describeReal("gaussianMean").def(0.0).help("The mean of the gaussian distribution used to generate both parent and offspring in case of the xnes evolution type");
	d.describeReal("gaussianStdDev").def(1.0).help("The standard deviation of the gaussian distribution used to generate parent in case of the xnes evolution type");
	d.describeReal("offspringStdDev").def(1.0).help("The standard deviation of the gaussian distribution used to generate offspring in case of the xnes evolution type");
	d.describeBool("variableStdDev").def(false).help("Whether the standard deviation of the gaussian distribution used to generate offspring is variable or constant");
	d.describeInt("numStartInd").def(1).limits(1,MaxInteger).help("The number of individuals to be tested so to find the starting individual in case of the xnes evolution type");
	d.describeReal("mutationLearningRate").def(0.0).help("The learning rate used to generate \"mutated\" offspring in case of the xnes evolution type");
	d.describeBool("xnesCombination").def(false).help("Whether to combine xnes with backpropagation algorithm or not");
	d.describeInt("xnesCombinationType").def(0).limits(0,MaxInteger).help("Type of combination between xnes and backpropagation algorithms");
	d.describeInt("backPropOffspringUtilityRank").def(255).limits(0,MaxInteger).help("The utility rank of the backpropagation algorithm in case of the second type of combination with xnes");
	d.describeBool("backPropOffspringFitnessIncrease").def(false).help("Whether to increase the fitness of the backpropagation offspring in case of the second type of combination with xnes");
	d.describeReal("backPropOffspringFitnessIncreasePercentage").def(0.1).help("Percentage of increase of the backpropagation offspring fitness value in case of the second type of combination with xnes");
	d.describeInt("pheGen").def(1).limits(1,MaxInteger).help("How often the best phenotype must be saved");
	d.describeBool("standardNes").def(false).help("Whether to use standard version of xnes or not");
	d.describeBool("minimization").def(false).help("Whether the fitness function corresponds to a minimization problem");
	d.describeBool("debugPrintInfoToFile").def(false).help("Whether to print debug information to file");
	d.describeBool("dissociateInd").def(false).help("Whether to dissociate back-propagation effect on xNES individual update");
	d.describeBool("dissociateCovMatrix").def(false).help("Whether to dissociate back-propagation effect on xNES covariance matrix update");
	d.describeBool("specialUtilityRanking").def(false).help("Whether to use a special version to compute the utility ranking");
    d.describeInt("numModules").def(1).limits(0,MaxInteger).help("The number of modules that will compose the genotype");

    d.describeReal("crossoverRate").def(0.05).help("The probability that a crossover will occur in an offspring team");
    d.describeReal("modulesMutationRate").def(0.25).help("The probability that a module will be mutated on a team");
    d.describeBool("mutateOnlyRelatives").def(true).help("Wheater a module should be replace by its own offsprings or by any offspring");
    d.describeReal("overwriteRate").def(0.05).help("The learning rate used to generate \"mutated\" offsprings in case of the xnes evolution type");
    d.describeBool("limitRetention").def(false).help("Wheater the retention should be limited or not");
    d.describeReal("targetRetentionRate").def(0.2).help("The rate in which retention should be limited");
    d.describeBool("saveRetetionStatistics").def(false).help("Whether to save the retetions statistics or not");
    d.describeEnum( "selectionType" ).def("rankBased").values( QStringList() << "rankBased" << "species").props( ParamIsMandatory ).help("Specify the type of selection that will be used to define which individuals will survive");
    d.describeReal("rankBasedProbability").def(0.75).help("The probability that the individuals with high fitness will be selected");
}

void Evoga::postConfigureInitialization()
{
	// Allocating memory
	tfitness = new double[MAXINDIVIDUALS];
	statfit = new double*[nogenerations];
	ntfitness = new double[MAXINDIVIDUALS]; //used with super ag implementation
	for(int i = 0; i < nogenerations; i++) {
		statfit[i] = new double[3];
	}
	terror = new double[MAXINDIVIDUALS];

	//allocating memory for the mutation vector
	glen = exp->getGenomeLength();
	mutations = new float[glen];
	for(int mi = 0; mi < glen; mi++) {
		mutations[mi] = Evonet::DEFAULT_VALUE;
	}

	//now we allocate memory for genome and best genome
	cgen = 0;
	popSize = noffspring * nreproducing;

	//dynamic allocation of genome
	genome.setGenomeLength(glen);
	genome.resize(popSize); // An initial set of individuals just to avoid problems...
	for (int i = 0; i < genome.size(); i++) {
		for (int r = 0; r < glen; r++) {
			genome[i][r] = i;//mrand(256);test
		}
	}

	//dynamic allocation of bestgenome
	bestgenome.setGenomeLength(glen);
	bestgenome.resize(nreproducing);

	//resetting fitness
	for (int i = 0; i < MAXINDIVIDUALS; i++) {
		tfitness[i] = 0.0;//
	}

	for (int i = 0; i < nogenerations; i++) {
		statfit[i][0] = 0.0; //Average fitness of population
		statfit[i][1] = 0.0; //max fitness
		statfit[i][2] = 0.0; //min fitness
	}

	Logger::info("Evoga Configured - Number of genes: " + QString::number(glen));
}

void Evoga::evolveAllReplicas()
{
	stopEvolution = false;
	if ( evolutionType == "steadyState" ) {
		evolveSteadyState();
	} else if ( evolutionType == "generational" ) {
		evolveGenerational();
	} else {
		Logger::error( QString("Evoga - request to execute a unrecognized evolution type: %1").arg(evolutionType) );
	}
}

void Evoga::stop() {
	stopEvolution = true;
	waitForNextStep.wakeAll();
}

bool Evoga::commitStep() {
	if ( isStepByStep && !stopEvolution ) {
		// will block waiting the command for going ahead
		mutexStepByStep.lock();
		waitForNextStep.wait( &mutexStepByStep );
		mutexStepByStep.unlock();
	}
	return stopEvolution;
}

bool Evoga::isStopped() {
	return stopEvolution;
}

void Evoga::resetStop() {
	stopEvolution = false;
}

void Evoga::enableStepByStep( bool enable ) {
	isStepByStep = enable;
	// if disable the step-by-step it wake any eventually blocked commitStep
	if ( !enable ) {
		waitForNextStep.wakeAll();
	}
}

bool Evoga::isEnabledStepByStep() {
	return isStepByStep;
}

void Evoga::doNextStep() {
	waitForNextStep.wakeAll();
}

EvoRobotExperiment* Evoga::getEvoRobotExperiment()
{
	return exp;
}

QVector<EvoRobotExperiment*> Evoga::getEvoRobotExperimentPool()
{
	QVector<EvoRobotExperiment*> v;
	v.append(exp);
	return v;
}

unsigned int Evoga::getCurrentGeneration()
{
	return cgen;
}

unsigned int Evoga::getStartingSeed()
{
	return seed;
}

unsigned int Evoga::getCurrentSeed()
{
	return currentSeed;
}

unsigned int Evoga::getNumReplications()
{
	return nreplications;
}

unsigned int Evoga::getNumOfGenerations() {
	return nogenerations;
}

double Evoga::getCurrentMutationRate() {
	return mutation;
}

void Evoga::setCurrentMutationRate( double mutation_rate ) {
	mutation = mutation_rate;
}

unsigned int Evoga::loadGenotypes(QString filename)
{
	return loadallg(-1, filename.toLatin1().data());
}

unsigned int Evoga::numLoadedGenotypes() const
{
	return loadedIndividuals;
}

int* Evoga::getGenesForIndividual(unsigned int id)
{
	return getGenes(id);
}

QString Evoga::statisticsFilename(unsigned int seed)
{
	return "statS" + QString::number(seed) + QString(".fit");
}

QString Evoga::bestsFilename(unsigned int seed)
{
	return "B0S" + QString::number(seed) + QString(".gen");
}

QString Evoga::bestsFilename()
{
	return "B0S*.gen";
}

QString Evoga::generationFilename(unsigned int generation, unsigned int seed)
{
	return "G" + QString::number(generation) + "S" + QString::number(seed) + QString(".gen");
}

QString Evoga::generationFilename()
{
	return "G*S*.gen";
}

void Evoga::doNotUseMultipleThreads()
{
	numThreads = 1;
}

QString Evoga::retentionsFilename(unsigned int seed)
{
    return "retentionS" + QString::number(seed) + QString(".stat");
}


int Evoga::rouletteWheel(QVector<double> candidates){
    long double raffle, sum = 0.0;
    int i;
    for(i=0;i<candidates.size();sum += candidates[i]+1,i++);
    raffle = (rand()%((int) sum*1000))/1000.0; //generating a random double with 3 decimal places
    for(i=0;raffle>0;raffle-=(candidates[i]+1),i++);
    return i-1;
}

} // end namespace salsa

// All the suff below is to restore the warning state on Windows
#if defined(_MSC_VER)
	#pragma warning(pop)
#endif
