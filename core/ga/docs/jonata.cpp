evorobotComponent

   /*instanciate with Factory using configuration parameters*/
   Alg = new Algorithm();
   Alg->setExperiment(new Experiment());
   for(int s=0;s<numSeed;s++){
      Alg->setSeed(s);
      Alg->run();
   }


//----

void AlgoSteadyState::evolve(){ /*maybe a better name would be run*/
   initPopulation();
   for(int gen=0;gen<maxgen||bestFitness>fitnessThreshold;gen++){
      emit initGeneration;
      evaluate(Parents); /* QVector<Genotype> Parents;*/
      reproduce();
      evaluate(Children); /* QVector<Genotype> Children;*/
      select();
      saveToFile();
      emit endGeneration;
   }
}

void AlgoSteadyState::setExperiment(Experiment exp){
    this->numGenes = exp->getMapper()->getNumGenesRequired(); ***
    for(int i=0;i<numThreads;i++){
       Exp[i] = exp; /*clone object*/
       Exp[i]->setAlgorithm(this); ***
    }
}

void AlgoSteadyState::evaluate(QVector<genotype> gen){
    for(int i=0;i<gen.size();i++){ /* monothread version */
       Exp[0]->getMapper()->setGenotype(gen[i]); ***
       Exp[0]->evaluate();
       gen[i]->setFitness(Exp[0]->getFitness());
    }
    /*multithread version clone numThread times the Experiment and evaluate*/
}

void AlgoSteadyState::reproduce(){
    /*mutate and crossover according to the algorithm*/
}

void AlgoSteadyState::initPopulation(){
   if(isResume())
      loadPopulationFromFile();
   else
      createRandomPopulation();
}

//-------------------------

void Experiment::evaluate(){
   Experiment->initIndividual(); ***
   Experiment->doAllTrials();
   Experiment->endIndividual(); ***
}
