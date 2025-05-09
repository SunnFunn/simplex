#include <iostream>
#include <fstream>
#include <cmath>
#include <vector>
#include <algorithm>
// #include "/Users/atretyakov/Apps/simplex/include/data.pb.h"
using namespace std;

#define INF 1000000.0
#define M 99999.0


struct Costs
{
    int idx;
    int cost;
};

// Comparator function
bool comp(Costs a, Costs b) {
    return a.idx > b.idx;
}

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


// Check if optimal plan reached
bool checkOptimality(int &rows, int &cols, vector<float> &c)
    {
        bool isOptimal = false;
        int negativeValueCount = 0;

        // Count positive values of objective function coefficients
        
        int cSize = c.size();
        for (int i = 0; i < cSize; i++)
        {
            if (c[i] <= 0)
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


// Find the the key column index in pivot table
vector<Costs> findPivotColumn(vector<float> &c)
{
    vector<Costs> positiveCosts;
    int cSize = c.size();
    for (int i = 0; i < cSize; i++)
    {
        if (c[i] > 0)
        {
            // positiveCosts.push_back({i,c[i]});
            Costs costIdx;
            costIdx.cost = c[i];
            costIdx.idx = i;
            positiveCosts.push_back(costIdx);
        }
    }
    // Sort struct in ascending idx order
    sort(positiveCosts.begin(), positiveCosts.end(), comp);

    return positiveCosts;
}


// Find the row with the pivot value.The least value item's row in the B array
int findPivotRow(int &rows, int &pivotColumn, bool &isUnbounded, vector<float> &b, vector< vector<float> > &A)
{
    int INFValueCount = 0;
    vector<float> result(rows, 0);

    for (int i = 0; i < rows; i++)
    {
        if (A[i][pivotColumn] == 0)
        {
            result[i] = INF;
            INFValueCount += 1;
        }
        else if (A[i][pivotColumn] < 0 && b[i] == 0)
        {
            result[i] = INF;
            INFValueCount += 1;
        }
        else if (A[i][pivotColumn] > 0 && b[i] == 0)
        {
            continue;
        }
        else if (A[i][pivotColumn]*b[i] < 0)
        {
            result[i] = INF;
            INFValueCount += 1;
        }
        else
        {
            result[i] = b[i]/A[i][pivotColumn];
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
        if (result[i] > 0)
        {
            if (result[i] < minimum)
            {
                minimum = result[i];

                location = i;
            }
        }
    }

    return location;
}


void doPivotting(int &rows, int &cols, int &pivotRow, int &pivotColumn, float &F,
    vector<float> &b, vector<float> &c, vector<float> &C, vector< vector<float> > &A,
    vector<float> &vars, vector<int> &basicVarsIdxs, vector<int> &DummiesIdxs)
    {
        float pivotValue = A[pivotRow][pivotColumn]; // Gets the pivot value
        vector< vector<float> > oldA = A;
        vector<float> oldb = b;
        vector<float> oldc = c;

        // Process coefficients in the A array except key row
        for (int m = 0; m < rows; m++)
        {
            auto it = find(DummiesIdxs.begin(), DummiesIdxs.end(), basicVarsIdxs[m]);
  	        if (it != DummiesIdxs.end() or m == pivotRow) { continue; }
            
            for (int p = 0; p < cols; p++)
            {
                A[m][p] = oldA[m][p] - oldA[m][pivotColumn]*oldA[pivotRow][p] / pivotValue;
            }
        }

        // Process key row in the new calculated A array
        for (int i = 0; i < cols; i++)
        {
            A[pivotRow][i] = oldA[pivotRow][i] / pivotValue;
        }

        // Process the values of the B array
        int bSize = b.size();
        for (int j = 0; j < bSize; j++)
        {
            auto it = find(DummiesIdxs.begin(), DummiesIdxs.end(), basicVarsIdxs[j]);
  	        if (it != DummiesIdxs.end() or j == pivotRow) { continue; }

            b[j] = oldb[j] - oldA[j][pivotColumn]* oldb[pivotRow] / pivotValue;
        }
        b[pivotRow] = oldb[pivotRow] / pivotValue;

        // exchange one of the basis variables index
        basicVarsIdxs[pivotRow] = pivotColumn;

        // Process the c array
        for (int i = 0; i < cols; i++)
        { 
            float delta = -C[i];
            for (int j = 0; j < rows; j++)
            {
                delta += C[basicVarsIdxs[j]]*oldA[j][i];
            }
            c[i] = delta;
        }
        F = 0.0;
        for (int j = 0; j < rows; j++)
        {
            F += C[basicVarsIdxs[j]]*oldb[j];      
        }   
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
    vector<string> inequalitiesType = {"neg", "neg", "pos", "pos", "pos"};

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
        else if (i >= varsNumber + inequalitiesNumber) { C.push_back(M); }
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
    // while (true)
    while( loop != 30)
    {
        // beforeF = F;
        positiveCosts = findPivotColumn(c);
        int positiveCostsSize = positiveCosts.size();
        for ( int i = 0; i < positiveCostsSize; i++)
        {
            keyColIdx = positiveCosts[i].idx;
            cout << "keyColIdx: " << keyColIdx << endl;
            
            // Search
            auto it_bv = find(basicVarsIdxs.begin(), basicVarsIdxs.end(), keyColIdx); 
            if ( it_bv != basicVarsIdxs.end() ) { continue; }

            keyRowIdx = findPivotRow(rowNumber, keyColIdx, isUnbounded, B, a);

            cout << "keyRowIdx: " << keyRowIdx << endl;

            // // Search
            // auto it_b = find(varsIdxs.begin(), varsIdxs.end(), keyRowIdx); 
            // if ( it_b != varsIdxs.end() ) { continue; }

            cout << "F: " << F << " " << keyColIdx << " " << keyRowIdx << endl;
            break;
        }
        
        
        // keyColIdx = positiveCosts[0].idx;
        // keyRowIdx = findPivotRow(rowNumber, keyColIdx, isUnbounded, B, a);
        // cout << "F: " << F << " " << keyColIdx << " " << keyRowIdx << endl;

        if (isUnbounded == true)
        {
            cout << "Error unbounded" << endl;
            break;
        }
        doPivotting(rowNumber, colNumber, keyRowIdx, keyColIdx, F, B, c, C, a, vars, basicVarsIdxs, DummiesIdxs);
        // afterF = F;
        
        loop += 1;
        // print(rowNumber, colNumber, a, B, basicVarsIdxs, C, c, F);
        
        optimCheck = checkOptimality(colNumber, rowNumber, c);
        if ( optimCheck ) { cout << "total loops: " << loop << endl; break; }
    }

    print(rowNumber, colNumber, a, B, basicVarsIdxs, C, c, F);
    int basicVarsIdxsSize = basicVarsIdxs.size();
    int vSize = vars.size();
    for (int i = 0; i < basicVarsIdxsSize; i++)
    {
        if (basicVarsIdxs[i] >= vSize) { continue; }
        else { vars[basicVarsIdxs[i]] = B[i]; }
    }
    cout << "plan is optimal. Vars are: " << endl;
    for (int i = 0; i < vSize; i++)
    {
        cout << vars[i] << " ";
    }
    cout << endl;

    return 0;
}