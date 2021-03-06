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

#include "Util.h"
#include "Graph.h"

GraphAdj erdos_renyi_gnm_random_graph(int n, int m)
{
    GraphAdj g;
    g.setZero(n,n);
    int countm = 0;
    while (countm<=m)
    {
        int n1 = random()%n;
        int n2 = random()%n;
        if (g.coeffRef(n1,n2)==0 && g.coeffRef(n2,n1)==0)
        {
            g.coeffRef(n1,n2)=g.coeffRef(n2,n1)=1;
            countm++;
        }
    }
    return g;
}

/**
 * @brief read_graph
 * @param filename
 * @return
 */
GraphAdj readAdjacencyMatrix(const std::string &filename)
{
    std::ifstream is(filename.c_str());
    if (!is.good())
        throw std::runtime_error(std::string("File \"") + filename +std::string("\" not found"));

    std::string line;
    const char separator=' ';
    int nrows = 0;
    int ncols = 0;
    // First let's count the rows of the file
    while (std::getline(is,line))
        nrows++;

    // Return the pointer of file to beginning
    is.clear();
    is.seekg(0, std::ios::beg);
    // Then count the number of columns
    std::getline(is,line);
    std::istringstream iss(line);
    std::string result;
    while( std::getline( iss, result, separator ) )
        ++ncols;

    // Return the pointer of file to beginning
    is.clear();
    is.seekg(0, std::ios::beg);

    GraphAdj graph;
    graph.setZero(nrows,ncols);
    //std::cout << nrows << ncols << std::endl;
    // Fill the matrix
    int nodeFrom = 0;
    while (getline(is, line))
    {
        std::stringstream strstr(line);
        int nodeTo=0;
        double val;
        while (strstr >> val)
        {
            // Makes the matrix binary
            graph(nodeFrom,nodeTo) = (val!=0);
            ++nodeTo;
        }
        ++nodeFrom;
    }
    return graph;
}


/**
 * @brief loadCoordinates
 * @param filename
 * @return
 */
MatrixXf loadCoordinates(const std::string &filename)
{
    std::ifstream is(filename.c_str());
    if (!is.good())
        throw std::runtime_error(std::string("Coordinate file \"" + filename + "\" not found"));

    std::string line;
    const char separator=' ';
    int nrows = 0;
    int ncols = 0;
    // First let's count the rows of the file
    while (std::getline(is,line))
        nrows++;

    // Return the pointer of file to beginning
    is.clear();
    is.seekg(0, std::ios::beg);
    // Then count the number of columns
    std::getline(is,line);
    std::istringstream iss(line);
    std::string result;
    while( std::getline( iss, result, separator ) )
        ++ncols;

    // Return the pointer of file to beginning
    is.clear();
    is.seekg(0, std::ios::beg);

    MatrixXf coords;
    coords.setZero(nrows,ncols);
    //std::cout << nrows << ncols << std::endl;
    // Fill the matrix
    int nodeFrom = 0;
    while (getline(is, line))
    {
        std::stringstream strstr(line);
        int nodeTo=0;
        float val;
        while (strstr >> val)
        {
            coords(nodeFrom,nodeTo) = val;
            ++nodeTo;
        }
        ++nodeFrom;
    }
    return coords;
}


/**
 * @brief loadDimacs
 * @param is
 */
GraphAdj readDimacs(const std::string &filename)
{
    std::map<int, std::vector<int> > graph;

    std::ifstream is(filename.c_str());
    if (!is.good())
        throw std::runtime_error(std::string("File not found"));

    std::string line;
    std::map<int,int> nodesmap;
    int nline=0;
    while (getline(is, line))
    {
        ++nline;
        if (line.size() == 0)
            continue;
        std::stringstream str(line);
        // Read the node neighbors
        int nodeId1, nodeId2;
        double weight;
        // Contains the node coordinates
        double coordinates[] = {0.0, 0.0, 0.0};

        int nodesCount;
        str >> nodeId1;

        skipSpaces(str);

        if ( skipSpacesReadChar(str, '(')==1)
        {
            std::cerr << "at line=" << nline << std::endl;
        }
        str >> coordinates[0];
        if ( skipSpacesReadChar(str, ',')==1)
        {
            std::cerr << "at line=" << nline << std::endl;
        }
        str >> coordinates[1];
        skipSpaces(str);
        if ( nextChar(str) == ',')
        {
            if ( skipSpacesReadChar(str, ',')==1 )
            {
                std::cerr << "at line=" << nline << std::endl;
            }
            str >> coordinates[2];
        }
        if ( skipSpacesReadChar(str, ')')==1)
        {
            std::cerr << "at line=" << nline << std::endl;
        }

        str >> nodesCount;
        if (nodesCount==0)
        {
            // ignore the next things until a new line begins
            str.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
        nodesmap[nodeId1] = 0;
        // Fill the neighbors of node1Id
        std::vector<int> node1neighbors;
        for (int i = 0; i < nodesCount; i++)
        {
            str >> nodeId2;
            if (str.get() == ':')
            {
                str >> weight;
            }
            else
            {
                str.unget();
                weight = 1;
            }
            nodesmap[nodeId2] = 0;
            node1neighbors.push_back(nodeId2);
        }
        graph[nodeId1] = node1neighbors;
    }
    int k=0;
    for (std::map<int,int>::iterator iter = nodesmap.begin(); iter!=nodesmap.end(); ++iter)
    {
        iter->second = k++;
    }

    int nNodes = nodesmap.size();
    GraphAdj graphAdj;
    graphAdj.setZero(nNodes,nNodes);
    for (std::map<int, std::vector<int> >::iterator i = graph.begin(); i!=graph.end(); ++i)
    {
        int n1 = i->first;
        n1 = nodesmap[n1];
        for (std::vector<int>::iterator neigh = i->second.begin(); neigh!=i->second.end(); ++neigh)
        {
            int n2 = (*neigh);
            n2 = nodesmap[n2];
            graphAdj.coeffRef(n1,n2)=graphAdj.coeffRef(n2,n1) = 1;
        }
    }
    return graphAdj;
}

/**
 * @brief membership2clustering
 * @param m
 * @return
 */
VertexClustering membership2clustering(const VertexMembership &m)
{
    VertexClustering clu;
    for (VertexMembershipConstIterator i = m.begin(); i!=m.end(); ++i)
    {
        clu[i->second].insert(i->first);
    }
    return clu;
}
