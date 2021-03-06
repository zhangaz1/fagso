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

#include <iostream>
#include <algorithm>
#include <fstream>
#include <stdexcept>
#include "DisjointSet.h"

using namespace std;

/**
 * @brief DisjointSet::DisjointSet
 */
DisjointSet::DisjointSet()
{
}

/**
 * @brief DisjointSet::DisjointSet
 * @param N
 */
DisjointSet::DisjointSet(int N, bool randomize)
{
    vmembership.resize(N);
    // Initialize component id array.
    for (int i = 0; i < N; i++)
    {
        int c = i;
        if (randomize)
            c=rand()%N;
        membership[i] = c;
        vmembership[i]=c;
        clustering[c].insert(i);
        oldMembership[i]=c;
        oldClustering[c].insert(i);
    }
}

DisjointSet::DisjointSet(const VertexMembership &initialMembership)
{
    this->membership = initialMembership;

    this->oldMembership = initialMembership;
    this->clustering.clear();
    this->oldClustering.clear();
    for (VertexMembershipIterator iter = membership.begin(); iter!=membership.end(); ++iter)
    {
        this->clustering[iter->second].insert(iter->first);
        this->vmembership.push_back(iter->second);
    }
    this->oldClustering = this->clustering;
}

/**
 * @brief DisjointSet::DisjointSet
 * @param rhs
 */
DisjointSet::DisjointSet(const DisjointSet &rhs)
{
    this->clustering = rhs.clustering;
    this->membership = rhs.membership;
    this->oldClustering = rhs.oldClustering;
    this->oldMembership = rhs.oldMembership;
    this->vmembership = rhs.vmembership;
}

/**
 * @brief DisjointSet::add
 * @param p
 * @param q
 * @return
 */
void DisjointSet::add(int p, int q)
{
    int a = p;
    int b = q;
    this->oldClustering = this->clustering;
    this->oldMembership = this->membership;

    VertexMembershipIterator imembershipA = this->membership.find(a);
    VertexMembershipIterator imembershipB = this->membership.find(b);
    if (imembershipA != membership.end())
    {
        int membershipA = imembershipA->second;
        if (imembershipB != membership.end())
        {
            int membershipB = imembershipB->second;
            if (membershipA == membershipB)
                return;

            VertexClusteringIterator iGroupA = this->clustering.find(membershipA);
            VertexClusteringIterator iGroupB = this->clustering.find(membershipB);
            VertexCluster groupA = iGroupA->second;
            VertexCluster groupB = iGroupB->second;
            if (groupA.size() < groupB.size())
            {
                std::swap(a,b);
                std::swap(membershipA, membershipB);
                std::swap(groupA, groupB); //altri swap che non capisco
                std::swap(b, a);
                std::swap(membershipB, membershipA);
                std::swap(groupB, groupA);
            }

            // Update groupA with elements from which are not groupB // forse vedere se usare sorted o no
            groupA.insert(groupB.begin(),groupB.end());
            clustering.erase(membershipB);
            clustering[membershipA] = groupA; // XXX carlo Aggiunto rispetto al codice python
            for (VertexClusterIterator k=groupB.begin(); k!=groupB.end(); ++k)
            {
                this->membership[*k] = membershipA;
                this->vmembership[*k] = membershipA;
            }
        }
        else
        {
            this->clustering[membershipA].insert(b);
            this->membership[b] = membershipA;
            this->vmembership[b]= membershipA;
        }
    }
    else
    {
        if (imembershipB != membership.end())
        {
            int membershipB = imembershipB->second;
            this->clustering[membershipB].insert(a);
            this->membership[a] = membershipB;
            this->vmembership[a] = membershipB;
        }
        else
        {
            this->membership[a] = a;
            this->membership[b] = a;
            this->vmembership[a] = a;
            this->vmembership[b] = a;
            VertexCluster ab;
            ab.insert(a);
            ab.insert(b);
            this->clustering[a] = ab;
        }
    }
    return;
}

/**
 * @brief DisjointSet::connected
 * @param p
 * @param q
 * @return
 */
bool DisjointSet::connected(int p, int q)
{
    int a = p;
    int b = q;

    VertexMembershipIterator imembershipA = this->membership.find(a);
    VertexMembershipIterator imembershipB = this->membership.find(b);
    if (imembershipA != membership.end())
    {
        int membershipA = imembershipA->second;
        if (imembershipB != membership.end())
            return membershipA == imembershipB->second;
        else
            return false;
    }
    else
        return false;
}

int DisjointSet::find(int p)
{
    VertexMembershipIterator imembershipA = this->membership.find(p);
    if (imembershipA != membership.end())
    {
        int membershipA = imembershipA->second;
        return membershipA;
    }
    else
    {
        throw std::runtime_error("Can't find node");
    }
}

/**
 * @brief DisjointSet::undo
 */
void DisjointSet::undo()
{
    this->clustering = this->oldClustering;
    this->membership = this->oldMembership;
}

/**
 * @brief DisjointSet::print
 */
void DisjointSet::print(ostream &os)
{
    os << "==== Memberships ====" << endl;
    os << "{";
    for (VertexMembershipIterator iter = membership.begin(); iter!=membership.end(); ++iter)
    {
        os << iter->first << ": "  << iter->second << ", ";
    }
    os << "}" << endl;

    os << "=== Groups ===" << endl;
    os << "{";
    for (std::map<int,set<int> >::iterator iter = clustering.begin(); iter!=clustering.end(); ++iter)
    {
        os << iter->first << ": [" ;
        for (set<int>::iterator iter2 = iter->second.begin(); iter2!=iter->second.end(); ++iter2)
        {
            os << *iter2 << ", " ;
        }
        os << "]," ;
    }
    os << "}" << endl;
}

/**
 * @brief DisjointSet::save
 * @param filename
 */
void DisjointSet::save(const string &filename)
{
    std::string membershipfile = filename + std::string(".memb");
    std::string groupfile = filename + std::string(".group");
    std::ofstream outputmembership(membershipfile.c_str());
    std::ofstream outputGroup(groupfile.c_str());

    if (!outputmembership.good())
        throw std::runtime_error("Error, file" + membershipfile+ " exists");

    if (!outputGroup.good())
        throw std::runtime_error("Error, file" + groupfile +" exists");

    for (VertexMembershipIterator iter = membership.begin(); iter!=membership.end(); ++iter)
    {
        outputmembership << iter->first << ": "  << iter->second << endl;
    }

    for (VertexClusteringIterator iter = clustering.begin(); iter!=clustering.end(); ++iter)
    {
        outputGroup << iter->first << ": [";
        for (VertexClusterIterator iter2 = iter->second.begin(); iter2!=iter->second.end(); ++iter2)
        {
            outputGroup << *iter2 << " " ;
        }
        outputGroup << "]" << endl;
    }
}

/**
 * @brief setSizeCmp
 * @param s1
 * @param s2
 * @return
 */
bool setSizeCmp(const set<int> &s1, const set<int> &s2)
{
    return s1.size() > s2.size();
}

/**
 * @brief DisjointSet::sortBySize
 */
void DisjointSet::sortBySize()
{
    std::vector< VertexCluster > groupSorted;
    for (VertexClustering::const_iterator iter = this->clustering.begin(); iter!=this->clustering.end(); ++iter)
    {
        groupSorted.push_back(iter->second);
    }
    std::sort(groupSorted.begin(),groupSorted.end(),setSizeCmp);
    this->clustering.clear();
    for (unsigned int i=0; i<groupSorted.size(); i++)
    {
        this->clustering[i] = groupSorted.at(i);
    }
    this->membership.clear();
    for (VertexClustering::const_iterator iter = this->clustering.begin(); iter!=this->clustering.end(); ++iter)
    {
        for (VertexClusterIterator iter2 = iter->second.begin(); iter2!=iter->second.end(); ++iter2)
        {
            this->membership[*iter2] = iter->first;
        }
    }
}
