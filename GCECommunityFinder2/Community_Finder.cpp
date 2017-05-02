/*
 * Community_Finder.cpp
 *
 *  Created on: 04-Dec-2009
 *      Author: freid
 */

//Release canidate.
#include "Community_Finder.h"

//TIMEKEEPING
time_t t0 = clock();

//REPORTING
#define REPORTING_OUTPUT_FREQUENCY 1000

//PARAMETER STORAGE
float Community_Finder::minimumOverlapToMerge;
float Community_Finder::numberOfTimesRequiredToBeSpokenFor;
float Community_Finder::spokenForThresholdOfUniqueness;
//SORT PREDICATES



//Note - flipped order in sort function - returns them sorted in reverse order - biggest first
bool sizeSortFunctionLargestFirst (Seed * two,  Seed * one)
{
	return one->getNumberOfNodes() < two->getNumberOfNodes();
} 

bool sizeSortFunctionSmallestFirst (Seed * one,  Seed * two)
{
	return one->getNumberOfNodes() < two->getNumberOfNodes();
}

bool vectorSizeSortFunctionLargestFirst (vector <V> two,  vector <V> one)
{
        return one.size() < two.size();
}

bool vectorSizeSortFunctionSmallestFirst (vector <V> one,  vector <V> two)
{
        return one.size() < two.size();
}
//END SORT PREDICATES

//CONSTRUCTURORS

Community_Finder::Community_Finder(const char * filename, int minimumCliqueSize, float minimumOverlapToMerge, float alphaValueForFitness, float numberOfTimesRequiredToBeSpokenFor, float spokenForThresholdOfUniqueness) {
    
	Community_Finder::minimumOverlapToMerge = minimumOverlapToMerge;
	Seed::minimumOverlapToMerge = minimumOverlapToMerge;
	Seed::alphaValueForFitness = alphaValueForFitness;
	Community_Finder::numberOfTimesRequiredToBeSpokenFor = numberOfTimesRequiredToBeSpokenFor;
	Community_Finder::spokenForThresholdOfUniqueness = spokenForThresholdOfUniqueness;
	
	this->initialiseSeeds(filename, minimumCliqueSize);
}

Community_Finder::~Community_Finder(){
	for (vector< Seed * >::iterator seedItr = this->seeds.begin(); seedItr != this->seeds.end(); ++seedItr)
	{
		delete (*seedItr);
	}
}

inline bool Community_Finder::contains(set<V> Nodes,V theNode)
{
    //return true if the node is contained in this seed
    if (Nodes.find(theNode) != Nodes.end())
    {
        return true;
    }
    else
    {
        return false;
    }
    
}

//INTERFACE WITH CLIQUE FINDING CODE
int numberOfCliquesProcessed = 0;
void Community_Finder::operator () (const vector<V> & clique) { // V is simply typedef'd to an int. I suppose it might become a long if we deal with graphs with over 2 billion nodes/edges.
	// WARNING: Do NOT take a pointer to the clique. It will be invalidated by the clique finding code upon returning. You must copy the data into your own structures.
	vector <V > temp;

	for ( vector<V>::const_iterator cliqueVertexIterator = (clique).begin(); cliqueVertexIterator != (clique).end(); ++cliqueVertexIterator)
	{

		temp.push_back(*cliqueVertexIterator);		
	}

	this->cliques.push_back(temp);
	numberOfCliquesProcessed++;
	if (numberOfCliquesProcessed % REPORTING_OUTPUT_FREQUENCY == 0)
	{
		fprintf(stderr, "%.2fs: ",(double)(clock()-t0)/CLOCKS_PER_SEC);
		cerr << "Processed: " << numberOfCliquesProcessed << " cliques..." << endl;

	}
}


// Implementaion of part2 (generate - choose - process constraints) 
void Community_Finder::ProcessConstraints(){
        
    int i=0;
    int j=1;
    
    vector<V> temp(1000000);
    vector<V> setR(4);
    set<V> set1;
    set<V> set2;
set<V> temp1;
    bool NotEmpty=false;
    

    
    for(vector< set<V> >::iterator eachSet = this->ChosenMust.begin(); eachSet != this->ChosenMust.end(); ++eachSet)
    {
        for(vector< set<V> >::iterator eachSet2 = this->ChosenMust.begin()+j; eachSet2 != this->ChosenMust.end(); ++eachSet2)
        {
            vector<V>::iterator it;
            it=set_intersection((*eachSet).begin(), (*eachSet).end(), (*eachSet2).begin(), (*eachSet2).end(), temp.begin());
            if(!temp.empty())
            {
                temp.resize(it-temp.begin());
                //cerr << "inside !temp.empty() \n";

                set1.insert((*eachSet).begin(), (*eachSet).end());
                set2.insert((*eachSet2).begin(), (*eachSet2).end());
                NotEmpty=true;
                break;
            }
        }
        j++;
    
    
       if(NotEmpty)
       {

           //cerr << "inside NotEmpty \n";

           vector<V>::iterator it1;
           it1=set_symmetric_difference(set1.begin(), set1.end(), set2.begin(), set2.end(),setR.begin());
           setR.resize(it1-setR.begin());
           temp1.insert(setR.begin(), setR.end());
           //cerr << "after temp1.insert \n";

           if(IsPaireExist(temp1,this->listCannot)){
               //cerr << "inside IsPaireExist(temp1,this->listCannot) \n";
                if(!IsPaireExist(temp1,this->ChosenCannot)){
                    //cerr << "inside !IsPaireExist(temp1,this->ChosenCannot) \n";

                    this->ChosenCannot.push_back(temp1);
                
                }
            }else if(IsPaireExist(temp1,this->listMust)){
                //cerr << "inside IsPaireExist(temp1,this->listMust) \n";

                if(!IsPaireExist(temp1,this->ChosenMust)){
                    //cerr << "inside !IsPaireExist(temp1,this->ChosenMust) \n";

                this->ChosenMust.push_back(temp1);
                }
            }
       }
      
    }
}

inline bool Community_Finder::IsPaireExist(set<V> Pair, vector< set<V> > Constraints){
    
    set<V>::iterator it = Pair.begin();
    advance(it, 0);
    V FirstNode =*it;
    
    advance(it, 1);
    V SecNode =*it;

    for(vector< set<V> >::iterator eachSet = Constraints.begin(); eachSet != Constraints.end(); ++eachSet)
    {
        
        if(contains(*eachSet,FirstNode) && contains(*eachSet,SecNode))
        {
           // cerr << " return true \n" ;
            return true;
            
        }
    }
    
   return false;
}

void Community_Finder::ChooseConstraints(int percentMust, int percentCannot)
{
    int RandM, RandC, counterM=0, counterC=0;
    int m=listMust.size();
    int c=listCannot.size();
    srand (time(NULL));

    while(counterM<percentMust){
        RandM = rand() % m;
        //cerr << " RandM : " << RandM ;
        //cerr << "\n" ;
        set<V> Must= listMust.at(RandM);
        //cerr << " IsPaireExist(Must,ChosenMust) "<< IsPaireExist(Must,ChosenMust);
    if(!IsPaireExist(Must,ChosenMust)){
        //cerr << " inside if Must : \n" ;
        ChosenMust.push_back(Must);
        counterM++;
    }else{
   // cerr << " inside else M \n";

    }
    }
    
    while(counterC<percentCannot){
        RandC = rand() % c;
       // cerr << " RandC : " << RandC ;
       // cerr << "\n" ;
        set<V> Cannot= listCannot.at(RandC);
    if(!IsPaireExist(Cannot,ChosenCannot)){
        ChosenCannot.push_back(Cannot);
        counterC++;
    }else{
        
    }
    }
}

void Community_Finder::Combination_CannotLink2(vector<V> vec, vector<V> vec1)
{
    vector<V> mm(vec.size()*2);
    vector<V>::iterator it;
    
    sort (vec.begin(),vec.end());
    sort (vec1.begin(),vec1.end());

    it=set_intersection (vec.begin(), vec.end(), vec1.begin(), vec1.end(), mm.begin());
    mm.resize(it-mm.begin());
    
    for (it=mm.begin(); it!=mm.end(); ++it)
        std::cout << ' ' << *it;
    std::cout << '\n';
    
    
    
    
    for(vector<V>::iterator it= vec.begin(); it != vec.end(); ++it)
    {
        if (find(mm.begin(), mm.end(),(*it))!=mm.end()){
            continue;
            
        }else{
            
            for(vector<V>::iterator it1= vec1.begin(); it1 != vec1.end(); ++it1)
            {
                if(find(mm.begin(), mm.end(),(*it1))!=mm.end()){
                    continue;
                }else{
                    set<V> m;
                    m.insert(*it);
                    m.insert(*it1);
                    listCannot.push_back(m);
                }
                
            }
        }
        
    }
    
}

void Community_Finder::GenerateCannotLink(vector< vector<V> > clusters, int n, int r)
{
    // A temporary array to store all combination one by one
    vector< vector<V> > temporaryVerctor;
    // Print all combination using temprary vector 'temporaryVerctor'
    Combination_CannotLink1(clusters,temporaryVerctor, n, r, 0, 0);
}

void Community_Finder:: Combination_CannotLink1(vector< vector<V> > clusters ,vector< vector<V> > temporaryVerctor, int n, int r, int index, int i)
{
    
    vector< vector<V> >::iterator iterator = temporaryVerctor.begin();
    set<V> temporarySet;
    
    // Current combination is ready, insert it to mustlink
    if (index == r)
    {
        //temporarySet.insert(temporaryVerctor[j]);
            Combination_CannotLink2(temporaryVerctor.at(0),temporaryVerctor.at(1));
       // listMust.push_back(temporarySet);
        return;
    }
    
    // When no more elements are there to put in temporaryVerctor
    if (i >= n)
        return;
    
    // current is included, put next at next location
    temporaryVerctor.insert(temporaryVerctor.begin()+index, clusters.at(i));
    Combination_CannotLink1(clusters,temporaryVerctor , n, r, index+1, i+1);
    // current is excluded, replace it with next (Note that
    // i+1 is passed, but index is not changed)
    Combination_CannotLink1(clusters,temporaryVerctor, n, r, index, i+1);
}

void Community_Finder::GenerateMustLink(vector< vector<V> > clusters,vector<V> eachVerctor, int n, int r)
{
    // A temporary array to store all combination one by one
    vector<int> temporaryVerctor;
    // Print all combination using temprary vector 'temporaryVerctor'
     Combination_MustLink(clusters,eachVerctor,temporaryVerctor, n, r, 0, 0);
}

void Community_Finder:: Combination_MustLink(vector< vector<V> > clusters, vector<V> eachVerctor,vector<V> temporaryVerctor, int n, int r, int index, int i)
{
    vector<int>::iterator iterator = temporaryVerctor.begin();
    set<V> temporarySet;
    
    // Current combination is ready, insert it to mustlink
    if (index == r)
    {
        for(int j=0; j<r; j++)
            temporarySet.insert(temporaryVerctor[j]);
        listMust.push_back(temporarySet);
        return;
    }
    
    // When no more elements are there to put in temporaryVerctor
    if (i >= n)
        return;
    
    // current is included, put next at next location
    temporaryVerctor.insert(temporaryVerctor.begin()+index, eachVerctor[i]);
    Combination_MustLink(clusters,eachVerctor,temporaryVerctor , n, r, index+1, i+1);
    // current is excluded, replace it with next (Note that
    // i+1 is passed, but index is not changed)
    Combination_MustLink(clusters,eachVerctor,temporaryVerctor, n, r, index, i+1);
}



//Responsible for taking the initialised structures, and operating the algorithm on them.
void Community_Finder::run()
{

    // print graph ----------------------------------------------------------------------------------------------------------
	/*for (int i = 0; i < theGlobalGraph.vertex_count;i++)
	{
		pair <V*,V*> startAndEnd = theGlobalGraph.neighbours(i);
		cerr << "Neighbours of vertex " << i << " : ";
		for (V * otherVertex = startAndEnd.first; otherVertex != startAndEnd.second; otherVertex++)
		{
			cerr << " " << *otherVertex;
		}
		cerr << endl;
	}*/
    //-----------------------------------------------------------------------------------------------------------------------
    
		fprintf(stderr, "%.2fs: ",(double)(clock()-t0)/CLOCKS_PER_SEC);
		fprintf(stderr, "%.2fs: ",(double)(clock()-t0)/CLOCKS_PER_SEC);
		cerr << "--------------------------------" << endl;
		cerr << "Number of seeds: " << this->seeds.size() << endl;
		sort(this->seeds.begin(),this->seeds.end(),sizeSortFunctionLargestFirst);
    
		//printSeeds();
    

  //start processing Must links  --------------------------------------------------------------------------------------------------------
 
  cerr << "-------------------------------- start processing Must links: " << endl;
    
  for(vector< Seed * >::iterator eachSeed = this->seeds.begin(); eachSeed != this->seeds.end(); ++eachSeed)
    {
        set<V> eachSeedNodes_Mustlink= (*eachSeed)->getNodes();
        //cerr << "-------------------------------- print seed : " << endl;
        //(*eachSeed)->prettyPrint();
        
    for(vector < set<V> >::iterator eachMustPairSet = this->ChosenMust.begin(); eachMustPairSet != this->ChosenMust.end(); ++eachMustPairSet)
        {
            set<V>::iterator it = (*eachMustPairSet).begin();
            advance(it, 0);
            V FirstNode =*it;
            //cerr << "-------------------------------- FirstNode : " << FirstNode << endl;

            advance(it, 1);
            V SecNode =*it;
            //cerr << "-------------------------------- SecNode : " << SecNode << endl;

        if( contains(eachSeedNodes_Mustlink,FirstNode) && contains(eachSeedNodes_Mustlink,SecNode))
        {
            //cerr << "both exist : "  << endl;
            continue;
        }else{
            //cerr << "both nodes NOT exist : "  << endl;
        }
            
        if( contains(eachSeedNodes_Mustlink,FirstNode) || contains(eachSeedNodes_Mustlink,SecNode))
        {
                   //cerr << "one of them exist : "  << endl;
            
            
                   (*eachSeed)->addNode(FirstNode);
           // cerr << "added first : "  << endl;

                   (*eachSeed)->addNode(SecNode);
            //cerr << "added second : "  << endl;

            //cerr << "--------------------------------print seed after adding mustlink nodes : " << endl;
            //(*eachSeed)->prettyPrint();
                   
        }else{
                   
                   //cerr << "NON of them exist  : "  << endl;
        }
        }
        
        (*eachSeed)->updateCachedEdgeValuesFromScratch();
        
    }
   

cerr << "-------------------------------- end processing Must links: " << endl;
// end of processing mustlink code ------------------------------------------------------------------------------------------------------------
  
        //printSeeds();
    
		vector< Seed* > resultsVec;
		int numberSeedsDiscardedBeforeExpansion = 0;
		int numberSeedsDiscardedAfterExpansion = 0;
		int numberSeedsKept = 0;
		int numberSeedsProcessed = 0;
		//for each seed

		bool issueAlphaWarning = false;
		
		for (vector< Seed * >::iterator seedItr = this->seeds.begin(); seedItr != this->seeds.end(); ++seedItr)
            {
			  (*seedItr)->putIntoNodeToSeedsCache();
                    
                 /*   // print node->seed assignments
                    int count = 0;
                    for( vector< set<Seed*> >::iterator nodeToSeedsItr = nodeToSeeds.begin(); nodeToSeedsItr != nodeToSeeds.end(); ++nodeToSeedsItr)
                    {
                        cout << "Node " << count++ << " : ";
                        for( set<Seed*> ::iterator innerItr = (*nodeToSeedsItr).begin(); innerItr != (*nodeToSeedsItr).end(); ++innerItr)
                        {
                            (*innerItr)->prettyPrint();
                            
                        }
                        cout << endl;
                    
                }*/
			
			if (numberSeedsProcessed % REPORTING_OUTPUT_FREQUENCY == 0)
			{
				fprintf(stderr, "%.2fs: ",(double)(clock()-t0)/CLOCKS_PER_SEC);
				cerr << "Processed: " << numberSeedsProcessed << " seeds\n";
		
				if(issueAlphaWarning)
				{
					cerr << "Warning: size of growing communities exceeds probable size: try increasing Alpha value." << endl; 
				}
				issueAlphaWarning = false;
			}
                
			numberSeedsProcessed++;
			
			bool alreadyCounted = false;
			alreadyCounted = (*seedItr)->overlapsAlreadyAcceptedSeed();
			
			if(!alreadyCounted)
			{

				(*seedItr)->updateCachedEdgeValuesFromScratch();
                (*seedItr)->updateFrontierFromScratch();
                cerr << "-------------------------------- adding best node from frontier: " << endl;
				//expand to first peak fitness, within threshold
				while ( (*seedItr)->addBestNodeFromFrontierToSeed(ChosenMust1,ChosenCannot1) > 0) // add here the check for pw
				{
                    
				}
                cerr << "-------------------------------- end of adding best node from frontier: " << endl;


				if( (*seedItr)->getNumberOfNodes() > (theGlobalGraph.vertex_count / 4 ))
				{
					if(!issueAlphaWarning)
					{
						cerr << "Warning: size of growing communities exceeds probable size: try increasing Alpha value." << endl;
						issueAlphaWarning = true;
					}
                }
      

//  start processing Cannot links  --------------------------------------------------------------------------------------------------------
cerr << "-------------------------------- start processing Cannot links: " << endl;
                
    set<V> eachSeedNodes_Cannotlink = (*seedItr)->getNodes();
    
        for(vector < set<V> >::iterator eachCannotPairSet = this->ChosenCannot.begin(); eachCannotPairSet != this->ChosenCannot.end(); ++eachCannotPairSet)
                    {
                     
                if (includes(eachSeedNodes_Cannotlink.begin(),eachSeedNodes_Cannotlink.end(),(*eachCannotPairSet).begin(),(*eachCannotPairSet).end())){
                    
                    
                    set<V>::iterator it = (*eachCannotPairSet).begin();
                    advance(it, 0);
                    V FirstNode =*it;
                    // cerr << "-------------------------------- FirstNode : " << FirstNode << endl;
                    
                    advance(it, 1);
                    V SecNode =*it;
                    //cerr << "-------------------------------- SecNode : " << SecNode << endl;
                    
                    (*seedItr)->RemoveLeastFitnessNodeFromSeed(FirstNode,SecNode);

                
                
                  }
                        
                    /*
                        set<V>::iterator it = (*eachCannotPairSet).begin();
                        advance(it, 0);
                        V FirstNode =*it;
                       // cerr << "-------------------------------- FirstNode : " << FirstNode << endl;
                        
                        advance(it, 1);
                        V SecNode =*it;
                        //cerr << "-------------------------------- SecNode : " << SecNode << endl;
                        
                        
                        if( contains(eachSeedNodes_Cannotlink,FirstNode) && contains(eachSeedNodes_Cannotlink,SecNode))
                        {
                        
                            (*seedItr)->RemoveLeastFitnessNodeFromSeed(FirstNode,SecNode);
                        
                        }else{
                            //cerr << "-------------------------------- not in the seed togather  : " << endl;
                        }
                        */
                    }
                    
               // (*seedItr)->updateCachedEdgeValuesFromScratch();
                
cerr << "-------------------------------- end processing Cannot links: " << endl;
// end of processing Cannotlink code ------------------------------------------------------------------------------------------------------------
 
                // check for duplicate one
				alreadyCounted = (*seedItr)->overlapsAlreadyAcceptedSeed();
				if(!alreadyCounted)
				{
				// cerr << "Was not a duplicate\n";
					//add it to results
					numberSeedsKept++;
					resultsVec.push_back((*seedItr));
				}
				else
				{
				//	cerr << "Was a duplicate\n";
					numberSeedsDiscardedAfterExpansion++;
				}

				(*seedItr)->clearCaches();				

			}
			else
			{
				numberSeedsDiscardedBeforeExpansion++;
			}			

			if(alreadyCounted)
			{
				(*seedItr)->dead = true;
				(*seedItr)->removeFromNodeToSeedsList();
			}
			
		}
		
		fprintf(stderr, "%.2fs: ",(double)(clock()-t0)/CLOCKS_PER_SEC);
		cerr << "Number seeds discarded before expansion: " << numberSeedsDiscardedBeforeExpansion << " seeds\n";
		cerr << "Number seeds discarded after expansion: " << numberSeedsDiscardedAfterExpansion << " seeds\n";
		cerr << "Number seeds kept: " << numberSeedsKept << " seeds\n";

		this->sweepTheDead();
		//TODO make this more efficient in future.
		this->seeds.assign(resultsVec.begin(), resultsVec.end());
    //printSeeds();

}

//This is the CCH implementation, which gets the number of times each node is 'spoken for' by another clique.
//In our release, we always use '1' as the value for this number.
//It also checks what percentage of each candidate clique has then being 'spoken for' or 'covered'. If this percentage
//is above a threshold, then the clique is rejected as being insufficiently distinct.
//This function uses the following static parameters: Community_Finder::numberOfTimesRequiredToBeSpokenFor (normally set to 1)
// and Community_Finder::spokenForThresholdOfUniqueness, normally set to around .75
void Community_Finder::doSpokenForPruning()
{
	sort(this->seeds.begin(),this->seeds.end(),sizeSortFunctionLargestFirst);
	cerr << "Cliques sorted. About to run spokenFor pruning..." << "number of seeds before: " << this->seeds.size()<< endl;
	
	vector <int> spokenForTimes( theGlobalGraph.vertex_count );
	int numberOfCliquesProcessed = 0;
	
	for (vector< Seed * >::iterator seedItr = this->seeds.begin(); seedItr != this->seeds.end(); ++seedItr)
	{

		numberOfCliquesProcessed++;
		if (numberOfCliquesProcessed % REPORTING_OUTPUT_FREQUENCY == 0)
		{
			fprintf(stderr, "%.2fs: ",(double)(clock()-t0)/CLOCKS_PER_SEC);
			cerr << "Processed: " << numberOfCliquesProcessed << " cliques..." << endl;
		}


		int numberOfNodesAlreadySpokenFor = 0;
		for (set<V>::iterator nodeItr = (*seedItr)->getNodes().begin(); nodeItr != (*seedItr)->getNodes().end(); ++nodeItr)
		{
			int numberOfTimesRequiredToBeSpokenFor = Community_Finder::numberOfTimesRequiredToBeSpokenFor;
                        if (spokenForTimes[(*nodeItr)] > numberOfTimesRequiredToBeSpokenFor)
			{	
				numberOfNodesAlreadySpokenFor++;
			}
		}		
		
		//As the amount of nodes spoken for gets large, this value increases towards 1
		float proportionOfNodesAlreadySpokenFor = (float)numberOfNodesAlreadySpokenFor / (float)(*seedItr)->getNodes().size();	
	
		if (proportionOfNodesAlreadySpokenFor >= Community_Finder::spokenForThresholdOfUniqueness)
		{
			//if the proportion of nodes that have been taken is greater than the threshold, then discard this clique.
		
			//	cerr << "pruning, number of nodes spoken for: " << (float)numberOfNodesAlreadySpokenFor << " size of seed: " << (float)(*seedItr)->getNodes().size() <<  "value: " << (float)numberOfNodesAlreadySpokenFor / (float)(*seedItr)->getNodes().size()   << endl;		
			(*seedItr)->dead = true;
		}
		else
		{
			//this clique is good
			//so tag its nodes as taken.
			for (set<V>::iterator nodeItr = (*seedItr)->getNodes().begin(); nodeItr != (*seedItr)->getNodes().end(); ++nodeItr)
	                {
				spokenForTimes[(*nodeItr)]++;
			}
		}
	}
	
	this->sweepTheDead();
	cerr << "SpokenFor complete. Number of seeds after: "<< this->seeds.size() << endl;
}



//Responsible for using the clique finding code (which is an implementation of bron-kerbosch) to
//generate the maximal cliques in the graph.
//These cliques are then stored (in the callback) into the 'cliques' vector.
//This 'cliques' vector is then loaded into our 'Seed' objects, which are again stored in a vector.
//The Seed objects at this point are really just responsible for storing the initial, core, cliques.
//We then do the 'spoken for pruning' - or the CCH as it is also called, to prune out some of the highly
//degenerate cliques.
//After this, we consider the set of seeds to be initialised, and this function returns.
//Next, the 'run' function will be called to
//grow them into the candidate communities.
//TODO this implementation stores _every_ clique into memory before any are pruned using the CCH.
//this should later be optimised to run a clique at a time, only storing those not found to be discarded by the CCH, to save memory.
void Community_Finder::initialiseSeeds(const char * filename, int minimumCliqueSize)
{



	graph_loading::loadSimpleIntGraphFromFile(theGlobalGraph, filename); // read the network from the file

	cliques::findCliquesJustIDs(theGlobalGraph, *this, minimumCliqueSize); //?? i think find the max cliques
	
	cerr << "Loaded : " << this->cliques.size() << " cliques" << endl;
	//cliques now has all the cliques.  sort it by size.
        nodeToSeeds.clear();
        nodeToSeeds.resize(theGlobalGraph.vertex_count);


    //This 'cliques' vector is then loaded into our 'Seed' objects
	numberOfCliquesProcessed = 0;
for ( vector < vector <V> >::iterator cliqueItr = this->cliques.begin(); cliqueItr != this->cliques.end(); ++cliqueItr)
	{
		Seed * mySeed = new Seed();
		
		for (vector<V>::iterator innerCliqueItr = (*cliqueItr).begin(); innerCliqueItr != (*cliqueItr).end(); ++innerCliqueItr)
		{
			//mySeed->addNode((*innerCliqueItr));
			mySeed->addNodeNoCaching((*innerCliqueItr));
		}
		
		this->seeds.push_back(mySeed);

		numberOfCliquesProcessed++;
		if (numberOfCliquesProcessed % REPORTING_OUTPUT_FREQUENCY == 0)
		{
			fprintf(stderr, "%.2fs: ",(double)(clock()-t0)/CLOCKS_PER_SEC);
			cerr << "Processed: " << numberOfCliquesProcessed << " cliques..." << endl;
			cerr << "Current size of clique being processed: " << (*cliqueItr).size() << endl;
		}
	}

	//remove initial clique storage
	this->cliques.clear();
	
	//Run spoken for optimisation.
	this->doSpokenForPruning();

	fprintf(stderr, "%.2fs: ",(double)(clock()-t0)/CLOCKS_PER_SEC);
	cerr << "Total number of seeds remaining: " << this->seeds.size() << endl;
}


//Utility function which discards seeds that have been marked as dead.
//Remove all the seeds that have previously been marked as 'dead' from the vector storing the seeds
//This is necessary because we store the seeds in a vector.
void Community_Finder::sweepTheDead()
{
	//makes use of temporary vector
	//This temporary vector means we can write all the seeds to the new vector without having to delete and reallocate as we go.
	vector <Seed*> newVector;

	cerr << "Sweeping the seeds marked for deletion\n";
        for (vector< Seed * >::iterator seedItr = this->seeds.begin(); seedItr != this->seeds.end();++seedItr)
        {
                if ((*seedItr)->dead)
                {
                        delete *seedItr;
                }
                else
                {
			newVector.push_back(*seedItr);
                }
        }

	this->seeds.assign(newVector.begin(), newVector.end());

	cerr << "Done\n";
	
}


//Seeds have some cached values.
//They cache a map of nodes that make up their frontier.
//The values in this map are the number of internal and external edges that each of these nodes has - ie, the internal and external degree of the frontier node.
//We also cache the total internal and external degree of all the nodes inside the seed not on the frontier.
void Community_Finder::refreshAllSeedInternalCaches()
{
	for (vector< Seed * >::iterator seedItr = this->seeds.begin(); seedItr != this->seeds.end(); ++seedItr)
	{
		(*seedItr)->updateCachedEdgeValuesFromScratch();
		(*seedItr)->updateFrontierFromScratch();
	}
}

//TODO look up node names in map, here
void Community_Finder::rawPrint()
{
    cerr << " data ="<< a1<<"\n";
    cerr << " percent ="<< b1<<"\n";
    cerr << " trail ="<< c1<<"\n";

	for (vector< Seed * >::iterator seedItr = this->seeds.begin(); seedItr != this->seeds.end(); ++seedItr)
	{
		//(*seedItr)->rawPrint();
		(*seedItr)->rawPrintInOrderOfAddition();
		//(*seedItr)->prettyPrintFrontier();
		//(*seedItr)->prettyPrintFrontier();
	}


}

void Community_Finder::printSeeds()
{
	for (vector< Seed * >::iterator seedItr = this->seeds.begin(); seedItr != this->seeds.end(); ++seedItr)
	{
		(*seedItr)->prettyPrint();
	}
}
