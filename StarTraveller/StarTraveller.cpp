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
        uniform_int_distribution<int> distStar(0, NStars - 1);
        if (initPath)
        {
            paths = vector<vector<int>>(ships.size(),vector<int>(maxTurn + 1));
            energies = vector<vector<double>>(ships.size(), vector<double>(maxTurn + 1));
            for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++) paths[numOfShip][0] = ships[numOfShip];
            set<int> initVisitedStars;
            for (int turn = 1; turn <= maxTurn; turn++) for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
            {
                int initNumOfStar;
                double initEnergy;
                double initMaxScore = -1e10;
                int prevNumOfStar = paths[numOfShip][turn - 1];
                for (int numOfStar = 0; numOfStar < NStars; ++numOfStar)
                {
                    bool usingNewStar = false;
                    if (initVisitedStars.find(numOfStar) == initVisitedStars.end()) usingNewStar = true;
                    double energy = 0;
                    energy += (stars[2 * numOfStar] - stars[2 * prevNumOfStar])*(stars[2 * numOfStar] - stars[2 * prevNumOfStar]);
                    energy += (stars[2 * numOfStar + 1] - stars[2 * prevNumOfStar + 1])*(stars[2 * numOfStar + 1] - stars[2 * prevNumOfStar + 1]);
                    double initScore = -energy + (double)(turn * turn) * 1e6 * (usingNewStar ? 1. : 0.) / (maxTurn * maxTurn);
                    if (initScore > initMaxScore)
                    {
                        initMaxScore = initScore;
                        initNumOfStar = numOfStar;
                        initEnergy = sqrt(energy);
                    }
                }
                energies[numOfShip][turn] = initEnergy;
                paths[numOfShip][turn] = initNumOfStar;
                initVisitedStars.insert(initNumOfStar);
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
        uniform_int_distribution<int> distTurn(currentTurn, maxTurn);
        uniform_int_distribution<int> distShip(0, ships.size() - 1);
        for (int trial = 0; trial < 1; trial++)
        {
            auto prevPaths = paths;
            auto willVisitStar = visitedStars;
            double totalEnergy = 0;
            double consumedEnergy = 0;
            int randShip = distShip(engine);
            int randTurn = distTurn(engine);
            int randStar = distStar(engine);
            paths[randShip][randTurn] = randStar;
            int turn;
            for (turn = currentTurn; turn <= maxTurn; turn++) for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
            {
                if (willVisitStar.size() == NStars) break;
                double energy = 0;
                auto& path = paths[numOfShip];
                if (randShip == numOfShip && randTurn == turn)
                {
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
                }
                else energy = energies[numOfShip][turn];
                if (turn == currentTurn)
                {
                    consumedEnergy += energy;
                }
                totalEnergy += energy;
                willVisitStar.insert(path[turn]);
            }
            double score = -(totalEnergy + totalConsumedEnergy)  + (turn * turn) * (willVisitStar.size() - visitedStars.size()) * 1e3 / (maxTurn * maxTurn);
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

