#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <stdlib.h>
#include "datatypes.h"

using namespace std;
using namespace datatypes;

// helper function to compare Costs of target function
bool comp(Costs a, Costs b) {
    return a.cost > b.cost;
}

// Constructor of optimization problem vectors
// Simplex::Simplex(pair<int,int> sizea = {0,0}, int sizeb = 0, int sizec = 0, float totalCost = 0.0)
// {
//     m_a.reserve(sizea.first);
//     m_b.reserve(sizeb);
//     m_c.reserve(sizec);
//     m_totalCost = totalCost;
// }

auto Simplex::geta() const& -> const vector< vector<float> >& { return m_a; };
auto Simplex::getb() const& -> const vector<float>& { return m_b; };
auto Simplex::getc() const& -> const vector<float>& { return m_c; };
auto Simplex::getTotalCosts() const& -> const float& { return m_totalCost; };

auto Simplex::seta() & -> vector< vector<float> >& { return m_a; };
auto Simplex::setb() & -> vector<float>& { return m_b; };
auto Simplex::setc() & -> vector<float>& { return m_c; };
auto Simplex::setTotalCosts() & -> float& { return m_totalCost; };

float Simplex::INF = 1000000.0;
float Simplex::M = 99999.0;

auto Simplex::findPivotColumn() -> vector<Costs>
{
    vector<Costs> positiveCosts;
    int cSize = m_c.size();
    for (int i = 0; i < cSize; i++)
    {
        if (m_c[i] > 0)
        {
            // positiveCosts.push_back({i,c[i]});
            Costs costIdx;
            costIdx.cost = m_c[i];
            costIdx.idx = i;
            positiveCosts.push_back(costIdx);
        }
    }
    // Sort struct in ascending idx order
    sort(positiveCosts.begin(), positiveCosts.end(), comp);

    return positiveCosts;
};


auto Simplex::findPivotRow(int &pivotColumn, bool &isUnbounded) -> int
{
    int INFValueCount = 0;
    int rows = m_a.size();
    vector<float> result(rows, 0);

    for (int i = 0; i < rows; i++)
    {
        if (m_a[i][pivotColumn] == 0)
        {
            result[i] = INF;
            INFValueCount += 1;
        }
        else if (m_a[i][pivotColumn] < 0 && m_b[i] == 0)
        {
            result[i] = INF;
            INFValueCount += 1;
        }
        else if (m_a[i][pivotColumn] > 0 && m_b[i] == 0)
        {
            result[i] = 0;
        }
        else if (m_a[i][pivotColumn]*m_b[i] < 0)
        {
            result[i] = INF;
            INFValueCount += 1;
        }
        else
        {
            result[i] = m_b[i]/m_a[i][pivotColumn];
        }
    }
    // Checking the unbound condition if all the values are negative ones
    if (INFValueCount == rows)
    {
        isUnbounded = true;
    }
    
    // find the minimum's location of the smallest item of the b array
    float minimum = 99999999;
    int location = 0;
    int rSize = result.size();
    for (int i = 0; i < rSize; i++)
    {
        if (result[i] >= 0)
        {
            if (result[i] < minimum)
            {
                minimum = result[i];

                location = i;
            }
        }
    }
    // float maximum = 0;
    // int location = 0;
    // int rSize = result.size();
    // for (int i = 0; i < rSize; i++)
    // {
    //     if (result[i] > 0)
    //     {
    //         if (result[i] > maximum)
    //         {
    //             maximum = result[i];

    //             location = i;
    //         }
    //     }
    // }

    return location;
};


auto Simplex::doPivotting(int &pivotRow, int &pivotColumn, vector<float> &vars,
    vector<int> &basicVarsIdxs, vector<int> &DummiesIdxs, vector<float> &C) -> void
{
    float pivotValue = m_a[pivotRow][pivotColumn]; // Gets the pivot value
    vector< vector<float> > oldA = m_a;
    vector<float> oldb = m_b;
    vector<float> oldc = m_c;
    int rows = m_a.size();
    int cols = m_a[0].size();

    // Process coefficients in the a 2D vector except key row
    for (int m = 0; m < rows; m++)
    {
        if (m == pivotRow) { continue; }
        for (int p = 0; p < cols; p++)
        {
            // m_a[m][p] = oldA[m][p] - oldA[m][pivotColumn]*oldA[pivotRow][p] / pivotValue;
            m_a[m][p] -= oldA[m][pivotColumn]*oldA[pivotRow][p] / pivotValue;
        }
    }

    // Process key row in the new calculated a 2D vector
    for (int i = 0; i < cols; i++)
    {
        m_a[pivotRow][i] = oldA[pivotRow][i] / pivotValue;
    }

    // Process the values of the b vector
    int bSize = m_b.size();
    for (int j = 0; j < bSize; j++)
    {
        if (j == pivotRow) { continue; }
        m_b[j] = oldb[j] - oldA[j][pivotColumn]* oldb[pivotRow] / pivotValue;
    }
    m_b[pivotRow] = oldb[pivotRow] / pivotValue;

    // exchange one of the basis variables index
    basicVarsIdxs[pivotRow] = pivotColumn;

    // Process the c vector
    for (int i = 0; i < cols; i++)
    { 
        float delta = -C[i];
        for (int j = 0; j < rows; j++)
        {
            delta += C[basicVarsIdxs[j]]*m_a[j][i];
        }
        m_c[i] = delta;
    }
    m_totalCost = 0.0;
    for (int j = 0; j < rows; j++)
    {
        m_totalCost += C[basicVarsIdxs[j]]*m_b[j];      
    }   
}


auto Simplex::checkOptimality() -> bool
{
    bool isOptimal = false;
    int negativeValueCount = 0;

    // Count positive values of objective function coefficients
    int cSize = m_c.size();
    for (int i = 0; i < cSize; i++)
    {
        if (m_c[i] <= 0)
        {
            negativeValueCount++;
        }
    }
    // If all the coeffitients are positive now,the plan is optimal
    if (negativeValueCount == cSize)
    {
        isOptimal = true;
    }
    return isOptimal;
}



