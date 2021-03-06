/* This file is part of FAGSO, a program to find network partitions
*
*  Copyright (C) 2014-2015 Carlo Nicolini <carlo.nicolini@iit.it>
*
*  FAGSO is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 3 of the License, or (at your option) any later version.
*
*  Alternatively, you can redistribute it and/or
*  modify it under the terms of the GNU General Public License as
*  published by the Free Software Foundation; either version 2 of
*  the License, or (at your option) any later version.
*
*  FAGSO is distributed in the hope that it will be useful, but WITHOUT ANY
*  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
*  FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License and a copy of the GNU General Public License along with
*  FAGSO. If not, see <http://www.gnu.org/licenses/>.
*/


#include <vector>
#include <set>
#include <Eigen/Core>
#include "SimilarityIndices.h"

using std::vector;
using std::set;
using Eigen::VectorXd;

/**
 * @brief getNeighbors
 * @param A
 * @param node
 * @return
 */
std::set<int> getNeighbors(const GraphAdj &A, int node)
{
    VectorXd neigh = A.col(node);
    set<int> neighbors;
    int r = A.cols();
    for (int i=0; i<r; ++i)
    {
        if (A.coeffRef(node,i))
            neighbors.insert(i);
    }
    return neighbors;
}

/**
 * @brief getDegree
 * @param A
 * @param node
 * @return
 */
size_t getDegree(const GraphAdj &A, int node)
{
    return A.row(node).sum() /2 ;
}

/**
 * @brief getIntraDegree
 * @param A
 * @param ds
 * @param node
 * @return
 */
// FUNZIONE DA CONTROLLARE NON SO SE FA LE COSE GIUSTE
size_t getIntraDegree(const GraphAdj &A, DisjointSet &ds, int node)
{
    int membNode = ds.find(node);
    size_t intraDeg = 0;
    for (set<int>::iterator iter = ds.clustering[membNode].begin(); iter!=ds.clustering[membNode].end(); ++iter)
    {
        int r = A.cols();
        int neigh = *iter;
        for (int i=0; i<r; ++i)
        {
            if (ds.find(neigh)==membNode && A.coeffRef(node,i))
            {
                intraDeg++;
            }
        }
    }
    return intraDeg/2;
}

// FUNZIONE DA CONTROLLARE NON SO SE FA LE COSE GIUSTE
/*
vector<int> gainLossMerge(const GraphAdj &A, DisjointSet &ds, int node1, int node2)
{
    // Get the number of nodes of the communities node1 and node2 belong
    size_t nNodesComm1 = ds.clustering[node1].size();
    size_t nNodesComm2 = ds.clustering[node2].size();

    // Compute the intracluster edges before the merge
    // Intracluster edges of node1
    size_t N = A.cols();
    int membNode1 = ds.find(node1);
    size_t intraEdges1 = 0;
    for (set<int>::iterator iter = ds.clustering[membNode1].begin(); iter!=ds.clustering[membNode1].end();++iter)
    {
        int neigh = *iter;
        for (int i=0; i<N; ++i)
        {
            if (ds.find(neigh)==membNode1 && A.coeffRef(node1,i))
            {
                intraEdges1++;
            }
        }
    }

    // Intracluster edges of node2
    int membNode2 = ds.find(node2);
    size_t intraEdges2 = 0;
    for (set<int>::iterator iter = ds.clustering[membNode2].begin(); iter!=ds.clustering[membNode2].end();++iter)
    {
        int neigh = *iter;
        for (int i=0; i<N; ++i)
        {
            if (ds.find(neigh)==membNode2 && A.coeffRef(node2,i))
            {
                intraEdges2++;
            }
        }
    }


    size_t intraEdgesAfterMerge  = 0;
    size_t intraEdgesBeforeMerge = intraEdges1+intraEdges2;

    size_t intraPairsAfterMerge = (nNodesComm1+nNodesComm2)*(nNodesComm1+nNodesComm2-1)/2;
    size_t intraPairsBeforeMerge = nNodesComm1*(nNodesComm1-1)/2 + nNodesComm2*(nNodesComm2-1)/2 ;

    size_t intraEdgesDelta=intraEdgesAfterMerge-intraEdgesBeforeMerge;
    size_t intraEdgesGain=0;
    size_t intraPairsLoss=0;
    size_t intraPairsGain=0;
}
*/

/**
 * @brief similarities
 * @param A
 * @param n1
 * @param n2
 * @param nIntersection
 * @param nUnion
 * @param deg1
 * @param deg2
 */
void similarities(const GraphAdj &A, int n1, int n2, double *nIntersection, double *nUnion, double *deg1, double *deg2)
{
    *nIntersection=0;
    *nUnion = 0;
    *deg1 = 0;
    *deg2 = 0;
    size_t N = A.cols();
    for (size_t k=0; k<N; ++k)
    {
        *nIntersection += int(A.coeffRef(n1,k)) & int(A.coeffRef(n2,k));
        *nUnion += int(A.coeffRef(n1,k)) | int(A.coeffRef(n2,k));
        *deg1 += std::ceil(A.coeffRef(n1,k)!=0);
        *deg2 += std::ceil(A.coeffRef(n1,k)!=0);
    }
    *deg1/=2.0;
    *deg2/=2.0;
}

/**
 * @brief jaccardIndex
 * @param A
 * @param n1
 * @param n2
 * @return
 */
double jaccardIndex(const GraphAdj &A, int n1, int n2)
{
    double nInt=0;
    double nUnion=0;
    double k1;
    double k2;
    similarities(A,n1,n2,&nInt,&nUnion,&k1,&k2);
    return nInt/(nUnion);
}

/**
 * @brief jaccardNormalized
 * @param A
 * @param n1
 * @param n2
 * @return
 */
double jaccardNormalized(const GraphAdj &A, int n1, int n2)
{
    // http://journals.plos.org/plosone/article?id=10.1371/journal.pone.0099755#pone-0099755-g008
    double nInt=0;
    double nUnion=0;
    double k1;
    double k2;
    similarities(A,n1,n2,&nInt,&nUnion,&k1,&k2);
    return 0.5*(nInt/k1 + nInt/k2);
}

/**
 * @brief cosineIndex
 * @param A
 * @param n1
 * @param n2
 * @return
 */
double cosineIndex(const GraphAdj &A, int n1, int n2)
{
    // https://class.coursera.org/nlp/lecture/184 for 0-1 bag of words
    double nInt=0,nUnion=0;
    double k1;
    double k2;
    similarities(A,n1,n2,&nInt,&nUnion,&k1,&k2);
    return nInt/sqrt(nUnion);
}

/**
 * @brief hubPromotedIndex
 * @param A
 * @param n1
 * @param n2
 * @return
 */
double hubPromotedIndex(const GraphAdj &A, int n1, int n2)
{
    double nInt;
    double nUnion;
    double k1;
    double k2;
    similarities(A,n1,n2,&nInt,&nUnion,&k1,&k2);
    return nInt/std::min(k1,k2);
}

/**
 * @brief hubDepressedIndex
 * @param A
 * @param n1
 * @param n2
 * @return
 */
double hubDepressedIndex(const GraphAdj &A, int n1, int n2)
{
    double nInt;
    double nUnion;
    double k1;
    double k2;
    similarities(A,n1,n2,&nInt,&nUnion,&k1,&k2);
    return nInt/std::max(k1,k2);
}

/**
 * @brief resourceAllocationIndex
 * @param A
 * @param n1
 * @param n2
 * @return
 */
double resourceAllocationIndex(const GraphAdj &A, int n1, int n2)
{
    set<int> s1 = getNeighbors(A,n1);
    set<int> s2 = getNeighbors(A,n2);
    std::set<int> neighIntersection;
    std::set_intersection(s1.begin(),s1.end(),s2.begin(),s2.end(),std::inserter(neighIntersection, neighIntersection.end()));
    double rai=0.0;
    for (std::set<int>::iterator iter = neighIntersection.begin(); iter!=neighIntersection.end(); ++iter)
    {
        rai += 1.0/(A.col(*iter).sum()/2);
    }
    return -rai;
}

/**
 * @brief leightHolmeNewmanIndex
 * @param A
 * @param n1
 * @param n2
 * @return
 */
double leightHolmeNewmanIndex(const GraphAdj &A, int n1, int n2)
{
    double nInt;
    double nUnion;
    double k1;
    double k2;
    similarities(A,n1,n2,&nInt,&nUnion,&k1,&k2);
    return nInt/(k1*k2);
}

/**
 * @brief preferentialAttachmentIndex
 * @param A
 * @param n1
 * @param n2
 * @return
 */
double preferentialAttachmentIndex(const GraphAdj &A, int n1, int n2)
{
    double nInt;
    double nUnion;
    double k1;
    double k2;
    similarities(A,n1,n2,&nInt,&nUnion,&k1,&k2);
    return (k1*k2);
}

/**
 * @brief adamicAdarIndex
 * @param A
 * @param n1
 * @param n2
 * @return
 */
double adamicAdarIndex(const GraphAdj &A, int n1, int n2)
{
    set<int> s1 = getNeighbors(A,n1);
    set<int> s2 = getNeighbors(A,n2);
    std::set<int> neighIntersection;
    std::set_intersection(s1.begin(),s1.end(),s2.begin(),s2.end(),std::inserter(neighIntersection, neighIntersection.end()));
    double aai=0.0;
    for (std::set<int>::iterator iter = neighIntersection.begin(); iter!=neighIntersection.end(); ++iter)
    {
        aai += 1.0/(std::log(A.col(*iter).sum()/2));
    }
    return aai;
}

/**
 * @brief sorensenIndex
 * @param A
 * @param n1
 * @param n2
 * @return
 */
double sorensenIndex(const GraphAdj &A, int n1, int n2)
{
    double nInt;
    double nUnion;
    double k1;
    double k2;
    similarities(A,n1,n2,&nInt,&nUnion,&k1,&k2);
    return 2*nInt/(k1+k2);
}

/**
 * @brief saltonIndex
 * @param A
 * @param n1
 * @param n2
 * @return
 */
double saltonIndex(const GraphAdj &A, int n1, int n2)
{
    double nInt;
    double nUnion;
    double k1;
    double k2;
    similarities(A,n1,n2,&nInt,&nUnion,&k1,&k2);
    return nInt/std::sqrt(k1*k2);
}

/**
 * @brief commonNeighbors
 * @param A
 * @param n1
 * @param n2
 * @return
 */
double commonNeighbors(const GraphAdj &A, int n1, int n2)
{
    double nInt;
    double nUnion;
    double k1;
    double k2;
    similarities(A,n1,n2,&nInt,&nUnion,&k1,&k2);
    return nInt;
}

/**
 * @brief averageDegreeIndex
 * @param A
 * @param n1
 * @param n2
 * @return
 */
double averageDegreeIndex(const GraphAdj &A, int n1, int n2)
{
    double nInt;
    double nUnion;
    double k1;
    double k2;
    similarities(A,n1,n2,&nInt,&nUnion,&k1,&k2);
    return k1+k2;
}
