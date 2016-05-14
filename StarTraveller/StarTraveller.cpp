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
    int endTurn;
    bool initPath;
    vector<vector<int>> paths;
    vector<vector<double>> energies;
    vector<int> stops;
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
        uniform_real_distribution<double> dist(0, 1);
        if (initPath)
        {
            stops = vector<int>(ships.size());
            paths = vector<vector<int>>(ships.size(), vector<int>(2*maxTurn));
            energies = vector<vector<double>>(ships.size(), vector<double>(2*maxTurn,0.));
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
            cerr << initVisitedStars.size() << endl;
            auto willVisitStar = visitedStars;
            double totalEnergy = 0;
            double consumedEnergy = 0;
            int turn;
            [&]()
            {
                for (turn = currentTurn; turn <= maxTurn; turn++) for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
                    {
                        if (willVisitStar.size() == NStars) return;
                        double energy = 0;
                        auto& path = paths[numOfShip];
                        energy = energies[numOfShip][turn];
                        if (turn == currentTurn)
                        {
                            consumedEnergy += energy;
                        }
                        totalEnergy += energy;
                        willVisitStar.insert(path[turn]);
                    }
            }();
            double score = -(totalEnergy + totalConsumedEnergy);
            cerr << fixed << "score:" << score << endl;
            if (score > maxScore && willVisitStar.size() == NStars)
            {
                cerr << setprecision(13) << fixed << totalEnergy + totalConsumedEnergy << endl;
                maxScore = score;
                endTurn = turn;
                initPath = false;
            }
        }
        //if (endTurn < maxTurn)
        //{
        //    for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        //    {
        //        paths[numOfShip].insert(paths[numOfShip].begin() + currentTurn, maxTurn - endTurn, ships[numOfShip]);
        //        energies[numOfShip].insert(energies[numOfShip].begin() + currentTurn, maxTurn - endTurn, 0.);
        //    }
        //    endTurn = maxTurn;
        //}
        for (int numOfUFO = 0; numOfUFO < (ufos.size() / 3); numOfUFO++)
        {
            for (int numOfShipOnUFO = 0; numOfShipOnUFO < ships.size(); numOfShipOnUFO++)
            {
                if (ufos[3 * numOfUFO] == ships[numOfShipOnUFO] )
                {
                    cerr << "UFO���I�I�I" << endl;
                    set<int> initVisitedStars = visitedStars;
                    auto prevPaths = paths;
                    auto prevEnergies = energies;
                    for (int turn = currentTurn; turn <= maxTurn; turn++) for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
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
                            if (numOfShip == numOfShipOnUFO && turn == currentTurn && numOfStar == ufos[3 * numOfUFO + 1])
                            {
                                energy = sqrt(energy);
                                energy *= 0.001;
                                initNumOfStar = numOfStar;
                                initEnergy = energy;
                                break;
                            }
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
                    auto willVisitStar = visitedStars;
                    double totalEnergy = 0;
                    double consumedEnergy = 0;
                    int turn;
                    [&]()
                    {
                        for (turn = currentTurn; turn <= maxTurn; turn++) for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
                        {
                            if (willVisitStar.size() == NStars) return;
                            double energy = 0;
                            auto& path = paths[numOfShip];
                            energy = energies[numOfShip][turn];
                            if (turn == currentTurn)
                            {
                                consumedEnergy += energy;
                            }
                            totalEnergy += energy;
                            willVisitStar.insert(path[turn]);
                        }
                    }();
                    double score = -(totalEnergy + totalConsumedEnergy);
                    if (score > maxScore && willVisitStar.size() == NStars)
                    {
                        cerr << setprecision(13) << fixed << totalEnergy + totalConsumedEnergy << endl;
                        maxScore = score;
                        endTurn = turn;
                    }
                    else
                    {
                        paths = prevPaths;
                        energies = prevEnergies;
                    }
                }
            }
        }
        uniform_int_distribution<int> distTurn(currentTurn, maxTurn);
        uniform_int_distribution<int> distShip(0, ships.size() - 1);
        uniform_real_distribution<double> distBool(0, 1);
        vector<int> ret(ships.size());
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        {
            ret[numOfShip] = paths[numOfShip][currentTurn];
            if (ret[numOfShip] == ships[numOfShip]) stops[numOfShip]++;
            else stops[numOfShip] == 0;
            visitedStars.insert(paths[numOfShip][currentTurn]);
        }
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        {
            totalConsumedEnergy += energies[numOfShip][currentTurn];
        }
        //for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        //{
        //    for (int turn = 0; turn < maxTurn; turn++)
        //    {
        //        if (turn != 0) cerr << ",";
        //        cerr << paths[numOfShip][turn];
        //    }
        //    cerr << endl;
        //}
        return ret;
    }
};
// -------8<------- end of solution submitted to the website -------8<-------

template<class T> void getVector(vector<T>& v)
{
    for (int i = 0; i < v.size(); ++i)
    {
        cin >> v[i];
        //cerr << v[i] << endl;
    }
}

int main()
{
    int NStars;
    cin >> NStars;
    //cerr << NStars << endl;
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
        //cerr << NUfo << endl;
        if (NUfo<0) break;
        vector<int> ufos(NUfo);
        getVector(ufos);
        int NShips;
        cin >> NShips;
        //cerr << NShips << endl;
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

