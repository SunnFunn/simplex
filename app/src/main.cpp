#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "datatypes.h"
#include "redis.h"

using namespace std;
using namespace datatypes;

// Print the problem data
void print(int &rows, int &cols, vector< vector<float> > &A, vector<float> &B, vector<int> &basicVarsIdxs,
    vector<float> &C, vector<float> &c, float &F)
{
    cout << "a matrix:  " << endl;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            cout << A[i][j] << " ";
        }
        cout << "" << endl;
    }
    cout << "" << endl;

    cout << "B vector:  " << endl;
    for (int i = 0; i < rows; i++)
    {

        cout << B[i] << " ";
    }
    cout << "" << endl;

    cout << "basis idxs:  " << endl;
    int bVSize = basicVarsIdxs.size();
    for (int i = 0; i < bVSize; i++)
    {
        cout << basicVarsIdxs[i] << endl; 
    }

    cout << "C array:  " << endl;
    int Csize = C.size();
    for (int i = 0; i < Csize; i++)
    {
        cout << C[i] << " ";
    }
    cout << "  " << endl;

    cout << "c array:  " << endl;
    int cSize = c.size();
    for (int i = 0; i < cSize; i++)
    {
        cout << c[i] << " ";
    }
    cout << "  " << endl;

    cout << "Target F value: " << F << endl;
}


int main(int argc, const char *argv[])
{
    Redis *r = new Redis();
    string host = argv[1];
    int port = stoi(argv[2]);
    string pass = argv[3];

    if(!r->connect(host, port))
    {
        return 0;
    }

    string supply_str = r->get("supply_str", pass).c_str();
    string demand_str = r->get("demand_str", pass).c_str();
    string costs_str = r->get("costs_str", pass).c_str();

    int sNodesNumber = 0;
    int dNodesNumber = 0;

    char del = ',';

    // suply and demand nodes
    vector<float> B;
    stringstream supply(supply_str);
    string s_line;
    while (getline(supply, s_line, del))
    {
        float Sqty = stof(s_line);
        B.push_back(Sqty);
        sNodesNumber++;
    }
    stringstream demand(demand_str);
    string d_line;
    while (getline(demand, d_line, del))
    {
        float Dqty = stof(d_line);
        B.push_back(Dqty);
        dNodesNumber++;
    }

    // costs nodes
    vector<float> C;
    stringstream costs(costs_str);
    string c_line;
    while (getline(costs, c_line, del))
    {
        float cost = stof(c_line);
        C.push_back(cost);
    }
    
    int varsNumber = sNodesNumber*dNodesNumber;
    int constraintsNumber = sNodesNumber + dNodesNumber;

    int varsDummiesNumber = dNodesNumber;
    int colNumber = varsNumber + constraintsNumber + varsDummiesNumber;
    int rowNumber = B.size();
    
    vector<int> basicVarsIdxs;
    vector<int> DummiesIdxs;
    vector<float> vars(varsNumber, 0);
    vector<float> varsIdxs;

    for (int i = 0; i < varsNumber; i++)
    {
        varsIdxs.push_back(i);
    }

    for (int i = varsNumber + constraintsNumber; i < colNumber; i++)
    {
        DummiesIdxs.push_back(i);
    }

    vector< vector<float> > a(rowNumber, vector<float>(colNumber, 0.0));
    vector<float> c;
    float F = 0.0;
    bool isUnbounded = false;


    // initiate first iteration of a matrix
    // fill matrix with constraints coefficients
    //------------------------------------------------------------------------------------
    for (int j = 0; j < rowNumber; j++)
    {
        if (j < sNodesNumber )
        {
            for (int k = 0; k < dNodesNumber; k++)
            {
                a[j][k + j*dNodesNumber] = 1;
            }
        }
        else
        {
            for (int l = 0; l < dNodesNumber; l++)
            {
                a[j][l*dNodesNumber + j - sNodesNumber] = 1;
            }
        }
    }

    // add basis coefficients to matrix
    //------------------------------------------------------------------------------------
    for (int j = 0; j < rowNumber; j++)
    {
        if (j >= sNodesNumber)
        {
            for (int i = 0; i < constraintsNumber; i++)
            {
                if ( i == j ) { a[j][i + varsNumber] = -1; }
                else { a[j][i + varsNumber] = 0; }
            }
        }
        else if (j < sNodesNumber)
        {
            for (int i = 0; i < constraintsNumber; i++)
            {
                if ( i == j ) { a[j][i + varsNumber] = 1; basicVarsIdxs.push_back(i + varsNumber); }
                else { a[j][i + varsNumber] = 0; }
            }
        }
    }
    
    // add dummies coefficients to matrix
    //------------------------------------------------------------------------------------
    if ( varsDummiesNumber != 0 )
    {
        int dummIdx = DummiesIdxs[0];
        for (int i = 0; i < constraintsNumber; i++)
        {
            if ( i < sNodesNumber ) { continue; }
            for (int j = 0; j < rowNumber; j++)
            {
                if ( a[j][varsNumber + i] == 0 ) { a[j][dummIdx] = 0; }
                else if ( a[j][varsNumber + i] == -1 )
                {
                    a[j][dummIdx] = 1;
                    basicVarsIdxs.push_back(dummIdx);
                }
            }
            dummIdx += 1;
        }
    }

    
    // construct C vector
    //------------------------------------------------------------------------------------
    for (int i = 0; i < colNumber; i++)
    {
        if (i >= varsNumber && i < varsNumber + constraintsNumber) { C.push_back(0); }
        else if (i >= varsNumber + constraintsNumber) { C.push_back(Simplex::M); }
    }

    // initiate target and c vector
    //------------------------------------------------------------------------------------
    for (int j = 0; j < rowNumber; j++)
    {
        F += C[basicVarsIdxs[j]]*B[j];      
    }
    // c elements
    for (int i = 0; i < colNumber; i++)
    { 
        float delta = -C[i];
        for (int j = 0; j < rowNumber; j++)
        {
            delta += C[basicVarsIdxs[j]]*a[j][i];
        }
        c.push_back(delta);
    }
    

    bool optimCheck = false;
    print(rowNumber, colNumber, a, B, basicVarsIdxs, C, c, F);

    int loop = 0;
    int keyColIdx = 0;
    int keyRowIdx = 0;
    vector<Costs> positiveCosts;

    Simplex simplex;
    for (int j = 0; j < rowNumber; j++) { simplex.seta().push_back(a[j]); }
    for (int i = 0; i < rowNumber; i++) { simplex.setb().push_back(B[i]); }
    for (int i = 0; i < colNumber; i++) { simplex.setc().push_back(c[i]); }
    simplex.setTotalCosts() = F;

    vector< vector<float> > ref_a;
    vector<float> ref_b;
    vector<float> ref_c;
    float ref_totalCosts;
    
    while (true)
    // while( loop != 30)
    {   
        positiveCosts = simplex.findPivotColumn();
        keyColIdx = positiveCosts[0].idx;
        keyRowIdx = simplex.findPivotRow(keyColIdx, isUnbounded);
        cout << "F: " << F << " " << keyColIdx << " " << keyRowIdx << endl;

        if (isUnbounded == true)
        {
            cout << "Error unbounded" << endl;
            break;
        }
        simplex.doPivotting(keyRowIdx, keyColIdx, vars, basicVarsIdxs, DummiesIdxs, C);
        
        loop += 1;
        ref_a = simplex.geta();
        ref_b = simplex.getb();
        ref_c = simplex.getc();
        ref_totalCosts = simplex.getTotalCosts();
        print(rowNumber, colNumber, ref_a, ref_b, basicVarsIdxs, C, ref_c, ref_totalCosts);
        
        optimCheck = simplex.checkOptimality();
        if ( optimCheck ) { cout << "total loops: " << loop << endl; break; }
    }

    ref_a = simplex.geta();
    ref_b = simplex.getb();
    ref_c = simplex.getc();
    ref_totalCosts = simplex.getTotalCosts();
    print(rowNumber, colNumber, ref_a, ref_b, basicVarsIdxs, C, ref_c, ref_totalCosts);
    
    int basicVarsIdxsSize = basicVarsIdxs.size();
    int vSize = vars.size();
    for (int i = 0; i < basicVarsIdxsSize; i++)
    {
        if (basicVarsIdxs[i] >= vSize) { continue; }
        else { vars[basicVarsIdxs[i]] = ref_b[i]; }
    }
    
    cout << "plan is optimal. Vars are: " << endl;
    for (int i = 0; i < vSize; i++)
    {
        cout << vars[i] << " ";
    }
    cout << endl;

    delete r;
    return 0;
}