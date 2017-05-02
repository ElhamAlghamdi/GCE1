#include <iostream>
#include <map>
#include <algorithm>
#include "graph_representation.hpp"
#include "graph_loading.hpp"
#include "cliques.hpp"
#include "Seed.h"
#include "Community_Finder.h"
#include <string.h>
#include <iterator> //for std::ostream_iterator
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <vector>       // std::vector


using namespace std;



//global graph
SimpleIntGraph theGlobalGraph;
//This vector is a map of each node to the set of seeds that are in it.
vector< set<Seed*> > nodeToSeeds;

int main (int argc, char **argv) {
	cerr << "Greedy Clique Expansion Community Finder" << endl;
	if(argc!=6 && (argc!=3 || ((strcmp (argv[1],"--?")) == 0) || ((strcmp (argv[1],"--help")) == 0)|| ((strcmp (argv[1],"-h")) == 0)  )) {
		cout << "Community finder. Requires edge list of nodes. Processes graph in undirected, unweighted form. Edgelist must be two values separated with non digit character." << endl<<endl;
		cout << "Use with either full (if specify all 5) or default (specify just graph file) parameters:" << endl;
		cout << "Full parameters are:" << endl;
		cout << "The name of the file to load" << endl;
		cout << "The minimum size of cliques to use as seeds. Recommend 4 as default, unless particularly small communities are required (in which case use 3)." << endl;
		cout << "The minimum value for one seed to overlap with another seed before it is considered sufficiently overlapping to be discarded (eta). 1 is complete overlap. However smaller values may be used to prune seeds more aggressively. A value of 0.6 is recommended." << endl;
		cout << "The alpha value to use in the fitness function greedily expanding the seeds. 1.0 is recommended default. Values between .8 and 1.5 may be useful. As the density of edges increases, alpha may need to be increased to stop communities expanding to engulf the whole graph. If this occurs, a warning message advising that a higher value of alpha be used, will be printed." << endl;
		cout << "The proportion of nodes (phi) within a core clique that must have already been covered by other cliques, for the clique to be 'sufficiently covered' in the Clique Coveage Heuristic" << endl;
		cout << endl<< "Usage: " << argv[0] << " graphfilename minimumCliqueSizeK overlapToDiscardEta fitnessExponentAlpha CCHthresholdPhi" << endl;
		cout << endl<< "Usage (with defaults): " << argv[0] << " graphfilename" << endl;
		cout << "This will run with the default values of: minimumCliqueSizeK 4, overlapToDiscardEta 0.6, fitnessExponentAlpha 1.0, CCHthresholdPhi .75" << endl;
		cout << "Communities will be output, one community per line, with the same numbering as the original nodes were provided." << endl;
		exit(1);
	}

	const char * filename = argv[1];
    const char * Cons_filename = argv[2];

	
	// the defulte parameters
	int minimumCliqueSize = 4; // maximal cliques k
	float minimumOverlapToMerge = 0.6; // community distance measurment
	float alphaValueForFitness = 1.0; // fitnessfuction
	float numberOfTimesRequiredToBeSpokenFor = 1;// ?
	float spokenForThresholdOfUniqueness = .75;//? // for the CHH fuction
	if (argc==6)
	{
		minimumCliqueSize = atoi(argv[2]);
		minimumOverlapToMerge = atof(argv[3]);
		alphaValueForFitness = atof(argv[4]);
		spokenForThresholdOfUniqueness = atof(argv[5]);
	}

	cerr <<"Running with parameters: k: " << minimumCliqueSize << " eta: " << minimumOverlapToMerge << " alpha: " << alphaValueForFitness << " Phi: " << spokenForThresholdOfUniqueness << endl;


	cerr << "Loading file: " << filename << endl;
    
    // creating object of community finder class with the above defult values
	Community_Finder communityFinder(filename, minimumCliqueSize, minimumOverlapToMerge, alphaValueForFitness, numberOfTimesRequiredToBeSpokenFor, spokenForThresholdOfUniqueness );

	cerr << "Edges in loaded graph:\t" << theGlobalGraph.ecount() << endl;
	cerr << "Nodes in loaded graph:\t" << theGlobalGraph.vcount() << endl;
    
    

    //--------------------reading network clusters -----------------------
    
    cerr << "-------------------- Reading network clusters -----------------------\n";

    vector< vector<V> > clusters;
    ifstream inData;
    string str;
    //inData.open("/Users/elhamalghamdi/Desktop/dorm_partition.txt");
    /*
    inData.open("LFRG.txt");
    while (getline(inData , str))
    {
        
        char split_char = ' ';
        istringstream split(str);
        vector<V> tokens;
        for (string each; getline(split, each, split_char);tokens.push_back(atoi(each.c_str())));
        clusters.push_back(tokens);
        //cerr << "set" <<clusters.size() <<" : " ;
        
        for(vector<V>::iterator innerSeedItr= tokens.begin(); innerSeedItr != tokens.end(); ++innerSeedItr)
        {
            cerr << ((*innerSeedItr)-1)<<" ";
        }
        cerr << "\n";
        
    }
    */
    cerr << "-------------------- End reading clusters -----------------------\n";
    //ifstream infile("constraints-LFR.txt");
    ifstream infile(Cons_filename);

    string a, b, c;
    //string a1, b1, c1;
/*
    infile >> communityFinder.a1 >> communityFinder.b1 >> communityFinder.c1;
    cerr << " data ="<< communityFinder.a1<<"\n";
    cerr << " percent ="<< communityFinder.b1<<"\n";
    cerr << " trail ="<< communityFinder.c1<<"\n";
*/
    while (infile >> a >> b >> c)
    {
        if (c=="M")
        {
            set<V> m;
            m.insert(atoi(a.c_str()));
            m.insert(atoi(b.c_str()));
            communityFinder.ChosenMust.push_back(m);
            communityFinder.ChosenMust1.insert(m);

        }
        
        if (c=="C")
        {
            set<V> m1;
            m1.insert(atoi(a.c_str()));
            m1.insert(atoi(b.c_str()));
            communityFinder.ChosenCannot.push_back(m1);
            communityFinder.ChosenCannot1.insert(m1);

        }
    
    
    }
    
    cerr << "-------------------- start of sorting -----------------------\n";

       sort(communityFinder.ChosenMust.begin(), communityFinder.ChosenMust.end());
       sort(communityFinder.ChosenCannot.begin(), communityFinder.ChosenCannot.end());

    cerr << "-------------------- End of sorting -----------------------\n";

    
    for(vector< set<V> >::iterator eachVector1= communityFinder.ChosenMust.begin(); eachVector1 != communityFinder.ChosenMust.end(); ++eachVector1)
    {
        for( set<V> ::iterator eachVector2= (*eachVector1).begin(); eachVector2 != (*eachVector1).end(); ++eachVector2)
        {
            cerr << "m :"<< *eachVector2;

        }
        cerr << "\n ";
    }
   
    cerr << "-------------------- Nnnnnnnnnnnnnnnnnnnnnnjksannnnnnnnnnnn---------------\n";

    
   // cerr << "-------------------- Generate Mustlink Constraints ---------------\n";
   /*
    for(vector< vector<V> >::iterator eachVector= clusters.begin(); eachVector != clusters.end(); ++eachVector)
    {
    int r = 2;
    int n = (*eachVector).size();
    communityFinder.GenerateMustLink(clusters,(*eachVector), n, r);
        
    }
   */
   // cerr << "-------------------- End of Generate Mustlink Constraints ---------\n";

   // cerr << "-------------------- Generate Cannotlink Constraints ---------------\n";

   // int z = clusters.size();
   // communityFinder.GenerateCannotLink(clusters,z,2);
    

    //cerr << "-------------------- End of Generate Cannotlink Constraints ----------\n";
   

   
   // cerr << "-------------------- Random selecting Constraints ---------------------\n";
    
   /* int percentMust = 1000; //(communityFinder.listMust.size()* 0.01);
    int percentCannot = 1000; //(communityFinder.listCannot.size()* 0.01);
    cerr << "\n listMust = \n" << communityFinder.listMust.size();
    cerr << "\n listCannot = \n" << communityFinder.listCannot.size();
    cerr << "\n percentMust = \n" << percentMust;
    cerr << "\n percentCannot = \n" << percentCannot;
   */
   
   //  communityFinder.ChooseConstraints(percentMust,percentCannot);
   //  cerr << "\n befor chosentMust = \n" << communityFinder.ChosenMust.size();
   //cerr << "\n befor chosenCannot = \n" << communityFinder.ChosenCannot.size();

   // cerr << "\n------------------ End of Random selecting Constraints ---------------\n";
    
    //cerr << "-------------------- Processing Constraints ----------------------------\n";
   //communityFinder.ProcessConstraints();
   //cerr << "\n befor chosentMust = \n" << communityFinder.ChosenMust.size();
   //cerr << "\n befor chosenCannot = \n" << communityFinder.ChosenCannot.size();

    //cerr << "-------------------- End of processing Constraints ---------------------\n";
 
    cerr << "-------------------- Run the algorithm ---------------------------------\n";
    communityFinder.run();
    cerr << "-------------------- Finished ------------------------------------------\n";
    communityFinder.rawPrint();


// print node->seed assignments
    /*
	int count = 0;
	for( vector< set<Seed*> >::iterator nodeToSeedsItr = nodeToSeeds.begin(); nodeToSeedsItr != nodeToSeeds.end(); ++nodeToSeedsItr)
	{
		cout << "Node " << count++ << " : ";
		for( set<Seed*> ::iterator innerItr = (*nodeToSeedsItr).begin(); innerItr != (*nodeToSeedsItr).end(); ++innerItr)
		{
			 (*innerItr)->prettyPrint();

		}
		cout << endl;
}
     */

}
