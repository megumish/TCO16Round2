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
    int currentTurn;
    int maxTurn;
    double maxScore;
    double totalConsumedEnergy = 0;
    bool initPath;
    vector<vector<int>> paths;
    vector<vector<double>> energies;
    int init(vector<int> stars)
    {
        NStars = stars.size() / 2;
        this->stars = stars;
        currentTurn = 0;
        maxTurn = NStars * 4;
        initPath = true;
        totalConsumedEnergy = 0;
        maxScore = -1e10;
        return 0;
    }

    vector<int> makeMoves(vector<int> ufos, vector<int> ships)
    {
        currentTurn++;
        random_device seed_gen;
        mt19937 engine(seed_gen());
        uniform_int_distribution<> distStar(0, NStars - 1);
        if (initPath)
        {
            paths.resize(ships.size());
            for (int turn = 0; turn <= maxTurn; turn++) for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
            {
                paths[numOfShip].push_back(distStar(engine));
            }
            initPath = false;
        }
        double currentConsumedEnergy = 0;
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        {
            auto path = paths[numOfShip];
            double energy = 0;
            energy += (stars[2 * path[currentTurn]] - stars[2 * path[currentTurn - 1]])
                *(stars[2 * path[currentTurn]] - stars[2 * path[currentTurn - 1]]);
            energy += (stars[2 * path[currentTurn] + 1] - stars[2 * path[currentTurn - 1] + 1])
                *(stars[2 * path[currentTurn] + 1] - stars[2 * path[currentTurn - 1] + 1]);
            energy = sqrt(energy);
            for (int numOfUfo = 0; numOfUfo < (ufos.size() / 3); numOfUfo++)
            {
                if (ufos[3 * numOfUfo] == path[currentTurn - 1] && ufos[3 * numOfUfo + 1] == path[currentTurn])
                {
                    energy *= 0.001 * 0.001;
                }
            }
            currentConsumedEnergy += energy;
        }
        vector<double> initEnergy(ships.size());
        vector<int> initNumOfStar(ships.size());
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        {
            double initMaxScore = -1e10;
            for (int numOfStar = 0; numOfStar < NStars; ++numOfStar)
            {
                bool usingNewStar = false;
                if (visitedStars.find(numOfStar) == visitedStars.end()) usingNewStar = true;
                double energy = 0;
                energy += (stars[2 * numOfStar] - stars[2 * ships[numOfShip]])*(stars[2 * numOfStar] - stars[2 * ships[numOfShip]]);
                energy += (stars[2 * numOfStar + 1] - stars[2 * ships[numOfShip] + 1])*(stars[2 * numOfStar + 1] - stars[2 * ships[numOfShip] + 1]);
                energy = sqrt(energy);
                for (int numOfUfo = 0; numOfUfo < (ufos.size() / 3); numOfUfo++)
                {
                    if (ufos[3 * numOfUfo] == ships[numOfShip] && ufos[3 * numOfUfo + 1] == numOfStar)
                    {
                        energy *= 0.001 * 0.001;
                    }
                }
                double initScore = -energy + (double)(currentTurn * currentTurn) * 1e6 * (usingNewStar ? 1. : 0.) / (maxTurn * maxTurn);
                if (initScore > initMaxScore)
                {
                    initMaxScore = initScore;
                    initEnergy[numOfShip] = energy;
                    initNumOfStar[numOfShip] = numOfStar;
                }
            }
        }
        uniform_int_distribution<> distTurn(currentTurn, maxTurn);
        uniform_int_distribution<> distShip(0, ships.size() - 1);
        for (int trial = 0; trial < 1; trial++)
        {
            auto prevPaths = paths;
            auto willVisitStar = visitedStars;
            double totalEnergy = 0;
            double consumedEnergy = 0;
            if (trial == 0)
            {
                for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
                {
                    paths[numOfShip][currentTurn] = initNumOfStar[numOfShip];
                }
            }
            else
            {
                paths[distShip(engine)][distTurn(engine)] = distStar(engine);
            }
            int turn;
            for (turn = currentTurn; turn <= maxTurn; turn++) for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
            {
                if (willVisitStar.size() == NStars) break;
                double energy = 0;
                auto path = paths[numOfShip];
                energy += (stars[2 * path[turn]] - stars[2 * path[turn - 1]])
                    *(stars[2 * path[turn]] - stars[2 * path[turn - 1]]);
                energy += (stars[2 * path[turn] + 1] - stars[2 * path[turn - 1] + 1])
                    *(stars[2 * path[turn] + 1] - stars[2 * path[turn - 1] + 1]);
                energy = sqrt(energy);
                if (turn <= currentTurn + 1)
                {
                    for (int numOfUfo = 0; numOfUfo < (ufos.size() / 3); numOfUfo++)
                    {
                        if (ufos[3 * numOfUfo + (turn - currentTurn)] == path[turn - 1] && ufos[3 * numOfUfo + (turn - currentTurn) + 1] == path[turn])
                        {
                            energy *= 0.001 * 0.001;
                        }
                    }
                }
                if (turn == currentTurn)
                {
                    consumedEnergy += energy;
                }
                energy *= (turn+1) - currentTurn;
                totalEnergy += energy;
                willVisitStar.insert(path[turn]);
            }
            double shipsDist = 0;
            for (int numOfShip0 = 0; numOfShip0 < ships.size(); numOfShip0++) for (int numOfShip1 = 0; numOfShip1 < ships.size(); numOfShip1++)
            {
                if (numOfShip0 != numOfShip1)
                {
                    double distX = abs(stars[2 * paths[numOfShip0][currentTurn]] - stars[2 * paths[numOfShip1][currentTurn]]);
                    double distY = abs(stars[2 * paths[numOfShip0][currentTurn] + 1] - stars[2 * paths[numOfShip1][currentTurn] + 1]);
                    if (distX == 0 && distY == 0) shipsDist += -100;
                    else shipsDist += distX + distY;
                }
            }
            double score = -(totalEnergy + totalConsumedEnergy) + shipsDist + (turn * turn) * (willVisitStar.size() - visitedStars.size()) / ships.size() * 1e3 / (maxTurn * maxTurn);
            if (score > maxScore)
            {
                cerr << setprecision(13) << fixed << totalEnergy + totalConsumedEnergy << endl;
                maxScore = score;
                currentConsumedEnergy = consumedEnergy;
            }
            else paths = prevPaths;
        }
        vector<int> ret(ships.size());
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        {
            ret[numOfShip] = paths[numOfShip][currentTurn];
            visitedStars.insert(paths[numOfShip][currentTurn]);
        }
        totalConsumedEnergy += currentConsumedEnergy;
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

