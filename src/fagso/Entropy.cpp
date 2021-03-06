/* This file is part of FAGSO, a program to find network partitions
*
*  Copyright (C) 2014-2015 Carlo Nicolini <carlo.nicolini@iit.it>
*
*  Surprise++ is free software; you can redistribute it and/or
*  modify it under the terms of the GNU Lesser General Public
*  License as published by the Free Software Foundation; either
*  version 3 of the License, or (at your option) any later version.
*
*  Alternatively, you can redistribute it and/or
*  modify it under the terms of the GNU General Public License as
*  published by the Free Software Foundation; either version 2 of
*  the License, or (at your option) any later version.
*
*  Surprise++ is distributed in the hope that it will be useful, but WITHOUT ANY
*  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
*  FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License or the
*  GNU General Public License for more details.
*
*  You should have received a copy of the GNU Lesser General Public
*  License and a copy of the GNU General Public License along with
*  Surprise++. If not, see <http://www.gnu.org/licenses/>.
*/

#include "Entropy.h"
#include "SimilarityIndices.h"
#include "FagsoParams.h"
/**
 * @brief entropy
 * @param adj
 * @param ds
 * @return
 */
double entropyJaccard(const GraphAdj &A, const DisjointSet &ds)
{
    // Presa la definizione di entropia da
    // http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=6085937
    // Dove dicono:
    // Thus, the entropy of a cluster is minimized when its nodes are semantically similar.
    // Io però ho messo sij = jaccardIndex che è una delle tante scelte che si possono fare
    std::map<int,double> entropies;
    for (std::map<int,std::set<int> >::const_iterator itCluster = ds.clustering.begin(); itCluster!=ds.clustering.end(); ++itCluster)
    {
        double H = 0.0;
        entropies[itCluster->first] = 0.0;
        for (std::set<int>::iterator iter2 = itCluster->second.begin(); iter2!=itCluster->second.end(); ++iter2)
        {
            int n1 = *iter2;
            for (std::set<int>::iterator iter3 = iter2; iter3!=itCluster->second.end(); ++iter3)
            {
                int n2 = *iter3;
                if (n2 > n1)
                {
                    // Vedi su Mezard Montanari - pagina 5 questa è entropia di un Bernoulli process
                    // Praticamente è la frequenza dei lati intracluster rispetto ai nodi totali
                    double sij = jaccardIndex(A,n1,n2);
                    if (sij!=0.0) // Avoid the bad limit computation when sij=0 because lim x->0 x*log(x)=0
                        H+= sij*log2(sij) + (1.0-sij)*log2(1.0-sij);
                }
            }
        }
        entropies[itCluster->first] = H;
    }

    double totEntropy=0.0;
    for (std::map<int,double>::iterator it=entropies.begin(); it!=entropies.end(); ++it)
    {
        //cerr << "TOT ENTROPY" << it->second << endl;
        totEntropy+=it->second;
    }
    return totEntropy;
}

/**
 * @brief entropy
 * @param A
 * @param ds
 * @return
 */
double entropy(const GraphAdj &A, const DisjointSet &ds)
{
    // Definizione di entropia secondo Ronhovde http://arxiv.org/pdf/1208.5052.pdf
    // La variabile è la probabilità che il nodo n stia nella comunità che ha probabilità n_k/N equazione (3)
    std::map<int,double> entropies;
    double N = A.rows(); // Number of nodes in the network
    double H = 0.0;
    for (std::map<int,std::set<int> >::const_iterator itCluster = ds.clustering.begin(); itCluster!=ds.clustering.end(); ++itCluster)
    {
        double nk= itCluster->second.size();
        H -= nk/N*log2(nk/N);
    }
    return H;
}
