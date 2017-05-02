/*
 * Community_Finder.h
 *
 *  Created on: 04-Dec-2009
 *      Author: freid
 */

#ifndef COMMUNITY_FINDER_H_
#define COMMUNITY_FINDER_H_
#include <iostream>
#include <map>
#include <algorithm>
#include "graph_representation.hpp"
#include "graph_loading.hpp"
#include "cliques.hpp"
#include "Seed.h"
#include <time.h>
#include <iterator>     // std::advance

using namespace std;


//global graph
extern SimpleIntGraph theGlobalGraph;

extern vector< set<Seed*> > nodeToSeeds;

class Community_Finder {

private:
	void initialiseSeeds(const char * filename, int minimumCliqueSize);

public:
	static float minimumOverlapToMerge;
	static float numberOfTimesRequiredToBeSpokenFor;
	static float spokenForThresholdOfUniqueness;
	
	vector< vector<V> > cliques;
    vector< set<V> > listMust;
    vector< set<V> > listCannot;
    vector< set<V> > ChosenMust;
    vector< set<V> > ChosenCannot;
    
    set< set<V> > ChosenMust1;
    set< set<V> > ChosenCannot1;

    set<V> seednodes;
    set<V> markednodesMustlink;
    set<V> markednodesCannotlink;
    string a1, b1, c1;
	int removeOverlappingFast();
	int removeOverlapping();
	vector< Seed* > seeds;
    bool contains(set<V> Nodes,V theNode);
	void rawPrint();
	void printSeeds();
	void refreshAllSeedInternalCaches();
	void run();
    void ProcessConstraints();
    bool IsPaireExist(set<V> Pair, vector< set<V> > Constraints);
    void ChooseConstraints(int percentMust, int percentCannot );
    void Combination_CannotLink2(vector<V> vec, vector<V> vec2);
    void GenerateCannotLink(vector< vector<V> > clusters, int n, int r);
    void Combination_CannotLink1(vector< vector<V> > clusters, vector< vector<V> > temporaryVerctor, int n, int r, int index, int i);
    void Combination_MustLink(vector< vector<V> > clusters,vector<V> eachVerctor,vector<V> temporaryVerctor, int n, int r, int index, int i);
    void GenerateMustLink(vector< vector<V> > clusters,vector<V> vec, int n, int r);
	void operator () (const vector<V> & clique);
	Community_Finder(const char * filename, int minimumCliqueSize, float minimumOverlapToMerge, float alphaValueForFitness, float numberOfTimesRequiredToBeSpokenFor, float spokenForThresholdOfUniqueness);
	virtual ~Community_Finder();
	void sweepTheDead();
	void doSpokenForPruning();
	

};

#endif /* COMMUNITY_FINDER_H_ */
