#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include "datatypes.h"

using namespace std;
using namespace datatypes;


void readData(const char* filename, vector<float> &A, vector<float> &B, vector<float> &C,
    int &varsNumber, int &constraintsNumber)
{
    ifstream inputFile(filename, std::ios::binary);
    if (inputFile.is_open())
    {
        // get length of file:
        inputFile.seekg (0, inputFile.end);
        int length = inputFile.tellg();
        inputFile.seekg (0, inputFile.beg);
        cout << length/sizeof(float) << endl;

        // get LP problem metadata:
        int meta;
        vector<int> metas;
        int loops = 2;
        while (loops != 0)
        {
            inputFile.read(reinterpret_cast<char*>(&meta), sizeof(int));
            metas.push_back(meta);
            loops--;
        }

        varsNumber = metas[0];
        constraintsNumber = metas[1];

        inputFile.seekg (sizeof(int)*2, inputFile.beg);
        float demand;
        loops = constraintsNumber;
        while (loops != 0)
        {
            inputFile.read(reinterpret_cast<char*>(&demand), sizeof(float));
            B.push_back(demand);
            loops--;
        }

        inputFile.seekg (sizeof(int)*2 + sizeof(float)*constraintsNumber, inputFile.beg);
        float cost;
        loops = varsNumber;
        while (loops != 0)
        {
            inputFile.read(reinterpret_cast<char*>(&cost), sizeof(float));
            C.push_back(cost);
            loops--;
        }

        inputFile.seekg (sizeof(int)*2 + sizeof(float)*constraintsNumber + + sizeof(float)*varsNumber, inputFile.beg);
        float coef;
        loops = varsNumber*constraintsNumber;
        while (loops != 0)
        {
            inputFile.read(reinterpret_cast<char*>(&coef), sizeof(float));
            A.push_back(coef);
            loops--;
        }
        
        inputFile.close();
        
        int bSize = B.size(); 
        for(int i = 0; i < bSize; i++)
        {
            cout << " " << B[i];
        }
        cout << endl;
    } else {
        std::cerr << "Unable to open file for reading." << std::endl;
    }
}

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
    vector<float> A;
    vector<float> B;
    vector<float> C;
    int varsNumber = 0;
    int constraintsNumber = 0;
    
    const char* filename = argv[1];
    readData(filename, A, B, C, varsNumber, constraintsNumber);

    int inequalitiesNumber = constraintsNumber;
    vector<string> inequalitiesType = {"neg", "neg", "neg", "pos", "pos", "pos", "pos"};

    int varsDummiesNumber = count(inequalitiesType.begin(), inequalitiesType.end(), "pos");
    vector<int> basicVarsIdxs;
    vector<int> DummiesIdxs;

    int colNumber = varsNumber + inequalitiesNumber + varsDummiesNumber;
    int rowNumber = B.size();
    
    vector<float> vars(varsNumber, 0);
    vector<float> varsIdxs;
    for (int i = 0; i < varsNumber; i++)
    {
        varsIdxs.push_back(i);
    }
    vector< vector<float> > a(rowNumber, vector<float>(colNumber, 0.0));
    vector<float> c;
    float F = 0.0;
    bool isUnbounded = false;

    for (int i = varsNumber + inequalitiesNumber; i < colNumber; i++)
    {
        DummiesIdxs.push_back(i);
    }

    // initiate first iteration of a matrix
    //--------------------------------------------------------------------
    // fill matrix with constaints coefficients
    for (int j = 0; j < rowNumber; j++)
    {
        for (int i = 0; i < varsNumber; i++)
        {
            a[j][i] = A[i + varsNumber*j];   
        }
    }

    // add basis coefficients to matrix
    //------------------------------------------------------------------------------------
    for (int j = 0; j < rowNumber; j++)
    {
        if (inequalitiesType[j] == "pos")
        {
            for (int i = 0; i < inequalitiesNumber; i++)
            {
                if ( i == j ) { a[j][i + varsNumber] = -1; }
                else { a[j][i + varsNumber] = 0; }
            }
        }
        else if (inequalitiesType[j] == "neg")
        {
            for (int i = 0; i < inequalitiesNumber; i++)
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
        for (int i = 0; i < inequalitiesNumber; i++)
        {
            if ( inequalitiesType[i] == "neg" ) { continue; }
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
        if (i >= varsNumber && i < varsNumber + inequalitiesNumber) { C.push_back(0); }
        else if (i >= varsNumber + inequalitiesNumber) { C.push_back(Simplex::M); }
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

    return 0;
}