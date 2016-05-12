#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <iomanip>
#include <random>
#include <set>

using namespace std;

class StarTraveller {
public:
    int NStars;
    set<int> visitedStars;
    vector<int> stars;
    double totalEnergy;
    int currentTurn;
    int maxTurn;
    int init(vector<int> stars)
    {
        NStars = stars.size()/2;
        this->stars = stars;
        totalEnergy = 0;
        currentTurn = 0;
        maxTurn = NStars * 4;
        return 0;
    }
    vector<int> makeMoves(vector<int> ufos, vector<int> ships)
    {
        currentTurn++;
        vector<int> ret;
        random_device seed_gen;
        mt19937 engine(seed_gen());
        uniform_int_distribution<> dist(0, NStars - 1);
        int turn = 0;
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        {
            double maxScore = -1e10;
            int nextNumOfStar;
            for (int i = 0; i < 100; i++)
            {
                int numOfStar;
                if (i == 0) numOfStar = ships[numOfShip];
                else numOfStar = dist(engine);
                bool usingNewStar = false;
                if (visitedStars.find(numOfStar) == visitedStars.end()) usingNewStar = true;
                double energy = 0;
                energy += (stars[2 * numOfStar] - stars[2 * ships[numOfShip]])*(stars[2 * numOfStar] - stars[2 * ships[numOfShip]]);
                energy += (stars[2 * numOfStar + 1] - stars[2 * ships[numOfShip] + 1])*(stars[2 * numOfStar + 1] - stars[2 * ships[numOfShip] + 1]);
                double score = -energy + (double)(currentTurn * currentTurn) * 1e6 * (usingNewStar ? 1. : 0.) / (maxTurn * maxTurn);
                if (score > maxScore)
                {
                    maxScore = score;
                    nextNumOfStar = numOfStar;
                }
            }
            for (int numOfUfo = 0; numOfUfo < (ufos.size() / 3) ; numOfUfo++)
            {
                if (ufos[3 * numOfUfo + turn] == ships[numOfShip])
                {
                    int numOfStar = ufos[3 * numOfUfo + turn + 1];
                    bool usingNewStar = false;
                    if (visitedStars.find(numOfStar) == visitedStars.end()) usingNewStar = true;
                    double energy = 0;
                    energy += (stars[2 * numOfStar] - stars[2 * ships[numOfShip]])*(stars[2 * numOfStar] - stars[2 * ships[numOfShip]]);
                    energy += (stars[2 * numOfStar + 1] - stars[2 * ships[numOfShip] + 1])*(stars[2 * numOfStar + 1] - stars[2 * ships[numOfShip] + 1]);
                    energy = energy * 0.001 * 0.001;
                    double score = -energy + (double)(currentTurn * currentTurn) * 1e10 * (usingNewStar ? 1. : 0.) / (maxTurn * maxTurn);
                    if (score > maxScore)
                    {
                        maxScore = score;
                        nextNumOfStar = numOfStar;
                    }
                }
            }
            visitedStars.insert(nextNumOfStar);
            ret.push_back(nextNumOfStar);
        }
        return ret;
    }
};
// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v)
{
    for (int i = 0; i < v.size(); ++i)
        cin >> v[i];
}

int main()
{
    int NStars;
    cin >> NStars;
    vector<int> stars(NStars);
    getVector(stars);

    StarTraveller algo;
    int ignore = algo.init(stars);
    cout << ignore << endl;
    cout.flush();

    while (true)
    {
        int NUfo;
        cin >> NUfo;
        if (NUfo<0) break;
        vector<int> ufos(NUfo);
        getVector(ufos);
        int NShips;
        cin >> NShips;
        vector<int> ships(NShips);
        getVector(ships);
        vector<int> ret = algo.makeMoves(ufos, ships);
        cout << ret.size() << endl;
        for (int i = 0; i < ret.size(); ++i) {
            cout << ret[i] << endl;
        }
        cout.flush();
    }
}

