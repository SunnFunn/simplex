#include <vector>
using namespace std;

// declare here all data types we need for our app
namespace datatypes
{
    // 2D Matrix Node type
    typedef struct
    {
        int idx;
        int cost;
    } Costs;

    // bool comp(Costs, Costs);

    // declare data type of main Transportation 2D Matrix
    class Simplex
    {
    public:
        // static data member
        static float INF;
        static float M;

        //Constructor;
        // Simplex (pair<int,int> sizea, int sizeb, int sizec, float totalCost);

        //Getters as references to member vectors
        auto geta() const& -> const vector< vector<float> >&;
        auto getb() const& -> const vector<float>&;
        auto getc() const& -> const vector<float>&;
        auto getTotalCosts() const& -> const float&;

        //Setters as references to member vectors
        auto seta() & -> vector< vector<float> >&;
        auto setb() & -> vector<float>&;
        auto setc() & -> vector<float>&;
        auto setTotalCosts() & -> float&;

        // find pivot column
        auto findPivotColumn() -> vector<Costs>;
        // find pivot row
        auto findPivotRow(int&, bool&) -> int;
        // do pivoting
        auto doPivotting(int&, int&, vector<float>&, vector<int>&,
            vector<int>&, vector<float>&) -> void;
        // chek if plan is optimal
        auto checkOptimality() -> bool;
        // // print results of optimization
        // auto printResult() -> void;

        /* custom destructor. we need to declare it due to dynamical array memory allocation 
        to avoid memory leaks and dangling pointers risks*/
        // ~NodesMatrix();
    private:
        vector< vector<float> > m_a;
        vector<float> m_b;
        vector<float> m_c;
        float m_totalCost;
    };
}