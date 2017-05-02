#!/usr/bin/env python
"""
Tool to generate pairwise constraints from a set of ground truth communities.

Sample usage:
python generate.py karate-ground.comm --seed 500 -m 15 -c 15 -o constraints-karate.txt 

python generate.py sample-overlap.comm --seed 1000 -m 10 -c 10 -o constraints-sample.txt
"""
import random, os, itertools
import time
from optparse import OptionParser
from datetime import datetime


# --------------------------------------------------------------

def read_communities(in_path):
    """
    Read communities from the specified file.
    We assume each node identifier is an integer, and there is one community per line.
    """
    print("Loading communities from %s ..." % in_path)
    communities = []
    fin = open(in_path, "r")
    for line in fin.readlines():
        community = set()
        for node in line.strip().split(" "):
            community.add(int(node))
        if len(community) > 0:
            communities.append(community)
    fin.close()
    print("Number of communities: %d" % len(communities))
    return communities


def assigned_nodes(communities):
    """
    Get all nodes assigned to at least one community.
    """
    assigned = set()
    for community in communities:
        assigned = assigned.union(community)
    return assigned

def exists_pair(seq):
    if len(seq) < 2:
        return False
    a, b = seq[0]  # could be: seq.pop() if modifications are allowed
    a_group = set()
    b_group = set()
    for c, d in seq:
        if {a,b} == {c,d}:
            continue
        elif {a,b} & {c,d}:
            return True

    # b_group - a_group: elements of b_group that do not appear in a_group
    return bool(b_group - a_group)
import operator as op
import functools
def ncr(n, r):
    r = min(r, n-r)
    if r == 0: return 1
    numer = functools.reduce(op.mul, range(n, n-r, -1))
    denom = functools.reduce(op.mul, range(1, r+1))
    return numer//denom
# --------------------------------------------------------------

def main():
    parser = OptionParser(usage="usage: %prog [options] community_file")
    parser.add_option("--seed", action="store", type="int", dest="seed", help="initial random seed", default=1000)
    parser.add_option("-o", action="store", type="string", dest="out_path", help="output file path",
                      default="constraints.txt")
    parser.add_option("-m", "--must", action="store", type="int", dest="num_must",
                      help="number of must-link constraints", default=10)
    parser.add_option("-c", "--cannot", action="store", type="int", dest="num_cannot",
                      help="number of cannot-link constraints", default=10)
    (options, args) = parser.parse_args()
    if (len(args) != 1):
        parser.error("Must specify one input community file")
    num_must = options.num_must
    num_cannot = options.num_cannot
    verify = False
    x=0
    # Set random state
    random.seed(time.time())  # options.seed
    print("Random seed: %s" % options.seed)
    print("Random seed: %s" % time.time())
    mul=options.seed
    # Load the ground-truth communities
    communities = read_communities(args[0])
    nodes = list(assigned_nodes(communities))
    n = len(nodes)
    print("Nodes assigned to communities: %d" % len(nodes))

    # Build an assignment map for all nodes
    print("Building node assignment map ...")
    node_map = {}
    for node in nodes:
        node_map[node] = set()
    for community in communities:
        x=x+(ncr(len(community), 2))
        #print(ncr(len(community), 2))
        for pair in itertools.combinations(community, 2):
            node_map[pair[0]].add(pair[1])
            node_map[pair[1]].add(pair[0])

    print("Selecting constraints: %d must-link, %d cannot-link" % (num_must, num_cannot))
    c=0;
    m=0;
    must_constraints = []
    cannot_constraints = []
    must_constraints2 = []
    cannot_constraints2 = []
    must_constraints1 = []
    cannot_constraints1 = []

    cannot = False
    must = False
    #print(x)
    while ((len(must_constraints) < num_must) and (len(must_constraints) <= x)) or (len(cannot_constraints) < num_cannot):
        while len(must_constraints1) < (mul*100) or len(cannot_constraints1) < (mul*100):
            node_index1, node_index2 = random.randint(0, n - 1), random.randint(0, n - 1)
            #print("--------indexs")
            #print(node_index1)
            #print(node_index2)
            # ignore self-constraints
            if node_index1 == node_index2:
                continue
            node1, node2 = nodes[node_index1], nodes[node_index2]
            # print("---------nodes")
            # print(node1) # order the pair uniquely
            # print(node2)
            if node1 < node2:
                pair = (node1, node2)
            else:
                pair = (node2, node1)
            # is this a must-link?
            if node2 in node_map[node1]:
                # do we have enough already? also avoid duplicates
                if len(must_constraints1)< (mul*100) and (not pair in must_constraints) and (not pair in must_constraints1):
                    must_constraints1.append(pair)
            # must be cannot-link
            else:
                # do we have enough already? also avoid duplicates
                #print("cannot")
                #print(pair)
                if len(cannot_constraints1)< (mul*100) and (not pair in cannot_constraints) and (not pair in cannot_constraints1):
                    cannot_constraints1.append(pair)
        #y=len(must_constraints)+len(must_constraints2)

        for pairx in must_constraints1:
            if len(must_constraints) < num_must and (not pairx in must_constraints):
                must_constraints.append(pairx)
            else:
                break
        for pairx in cannot_constraints1:
            if len(cannot_constraints) < num_cannot and (not pairx in cannot_constraints):
                cannot_constraints.append(pairx)
            else:
                break
        del must_constraints1[:]
        del cannot_constraints1[:]
        print("before processing Selected constraints: %d must-link, %d cannot-link" % (len(must_constraints), len(cannot_constraints)))

        difference = []
        j=1
        if not cannot or not must:
            for set1 in must_constraints:
                #print("set1 =")
                #print(set1)
                #print("--")
                for set2 in must_constraints[j:]:
                    #print("set2 =")
                    #print(set2)
                    #print("--")
                    if set(set1).intersection(set(set2)):
                        if set(set1).symmetric_difference(set(set2)):
                            diff = set(set1).symmetric_difference(set(set2))
                            diff=tuple(diff)
                            if diff[0] < diff[1]:
                                pair1 = (diff[0], diff[1])
                            else:
                                pair1 = (diff[1], diff[0])
                            if diff[1] in node_map[diff[0]]:
                                if ((len(must_constraints2)+len(must_constraints)< num_must)) and (not pair1 in must_constraints) and (not pair1 in must_constraints2):
                                    # print("---------Must =")
                                    # print(pair1)
                                    m=m+1;
                                    must_constraints2.append(pair1)
                                elif (len(must_constraints2)+len(must_constraints)) >= num_must:
                                    must=True
                            else:
                                if (len(cannot_constraints)+len(cannot_constraints2))< num_cannot and (not pair1 in cannot_constraints) and (not pair1 in cannot_constraints2):
                                    #  print("cannot =")
                                    # print(pair1)
                                    c=c+1;
                                    cannot_constraints.append(pair1)
                                elif (len(cannot_constraints)+len(cannot_constraints2)) >= num_cannot:
                                    cannot=True
                            #print("diff =")
                            #print(diff)
                            #print("--")
                            #difference.extend(diff)
                j=j+1
            #print("---Must before extend")
            #print(len(must_constraints))
            must_constraints.extend(must_constraints2)
            #print("---Must after extend")
            #print(len(must_constraints))
            del must_constraints2[:]

            #print("---Cannot before extend")
            #print(len(cannot_constraints))
            cannot_constraints.extend(cannot_constraints2)
            #print("---Cannot after extend")
            #print(len(cannot_constraints))
            del cannot_constraints2[:]
            #combinedCont=must_constraints2+must_constraints+cannot_constraints

#print("Selected constraints after: %d must-link, %d cannot-link" % (len(must_constraints), len(cannot_constraints)))

    print("Selected constraints after: %d must-link, %d cannot-link" % (len(must_constraints), len(cannot_constraints)))
    print("c")
    print(c)
    print("---m")
    print(m)
    # Write the results
    print("Writing constraints to %s" % options.out_path)
    fout = open(options.out_path, "w")
    for pair in must_constraints:
        fout.write("%s %s M\n" % (pair[0],pair[1]))
    for pair in cannot_constraints:
        fout.write("%s %s C\n" % ((pair[0],pair[1])))
    fout.close()

    ## Verify constraints. This is just for debugging purposes, this is slow!
    if verify:
        print("Verifying constraints ...")
        import numpy as np
        m = max(nodes)
        S = np.zeros((m + 1, m + 1))
        for community in communities:
            community = list(community)
            for i in range(len(community)):
                for j in range(len(community)):
                    if i != j:
                        S[community[i], community[j]] = 1
        for pair in must_constraints:
            if S[pair[0], pair[1]] != 1:
                print("Bad must-link constraint: %s,%s" % (pair[0], pair[1]))
        for pair in cannot_constraints:
            if S[pair[0], pair[1]] == 1:
                print("Bad cannot-link constraint: %s,%s" % (pair[0], pair[1]))


# --------------------------------------------------------------

if __name__ == "__main__":
    main()

