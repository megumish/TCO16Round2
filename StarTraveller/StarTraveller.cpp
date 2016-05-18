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
    int NStar;
    vector<int> stars;
    int currentTurn;
    int maxTurn;
    double bestScore;
    double nextScore;
    double totalConsumedEnergy;
    double bestTotalConsumedEnergy;
    bool initPath;
    vector<vector<int>> paths;
    vector<vector<int>> bestPaths;
    vector<vector<double>> energies;
    vector<vector<double>> bestEnergies;
    int init(vector<int> stars)
    {
        NStar = stars.size() / 2;
        this->stars.clear();
        this->stars = stars;
        currentTurn = 0;
        maxTurn = NStar * 4;
        initPath = true;
        totalConsumedEnergy = 0;
        bestTotalConsumedEnergy = 0;
        bestScore = -1e13;
        nextScore = -1e13;
        return 0;
    }

    vector<int> makeMoves(vector<int> ufos, vector<int> ships)
    {
        currentTurn++;
        random_device seed_gen;
        mt19937 engine(seed_gen());
        uniform_int_distribution<int> distStar(0, NStar - 1);
        if (initPath)
        {
            paths = vector<vector<int>>(ships.size(), vector<int>(NStar + 2,-1));
            energies = vector<vector<double>>(ships.size(), vector<double>(NStar + 2,0));
            vector<bool> visitedStars(NStar, false);
            for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++) paths[numOfShip][0] = ships[numOfShip];
            for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++) energies[numOfShip][0] = 0;
            int turn = 0;
            int visited = 0;
            [&]()
            {
                while (true)
                {
                    turn++;
                    for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
                    {
                        int initNumOfStar = -1;
                        double initEnergy = 0;
                        double minScore = 1e10;
                        for (int numOfStar = 0; numOfStar < NStar; numOfStar++)
                        {
                            if (visitedStars[numOfStar]) continue;
                            int prevNumOfStar = paths[numOfShip][turn - 1];
                            double energy = 0;
                            energy += (stars[2 * numOfStar] - stars[2 * prevNumOfStar])*(stars[2 * numOfStar] - stars[2 * prevNumOfStar]);
                            energy += (stars[2 * numOfStar + 1] - stars[2 * prevNumOfStar + 1])*(stars[2 * numOfStar + 1] - stars[2 * prevNumOfStar + 1]);
                            energy = sqrt(energy);
                            double score = energy;
                            if (score < minScore)
                            {
                                minScore = score;
                                initNumOfStar = numOfStar;
                                initEnergy = energy;
                            }
                        }
                        totalConsumedEnergy += initEnergy;
                        energies[numOfShip][turn] = initEnergy;
                        paths[numOfShip][turn] = initNumOfStar;
                        visitedStars[initNumOfStar] = true;
                        visited++;
                        if (visited == NStar) return;
                    }
                }
            }();
            cerr << turn << endl;
            maxTurn = turn;
            for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
            {
                if (paths[numOfShip][maxTurn] == -1)
                    paths[numOfShip][maxTurn] = paths[numOfShip][maxTurn - 1];
                if (paths[numOfShip][maxTurn + 1] == -1)
                    paths[numOfShip][maxTurn + 1] = paths[numOfShip][maxTurn];
            }
            bestScore = -totalConsumedEnergy;
            bestPaths = paths;
            bestEnergies = energies;
            bestTotalConsumedEnergy = totalConsumedEnergy;
            nextScore = -totalConsumedEnergy;
        }
        else
        {
            paths = bestPaths;
            energies = bestEnergies;
            totalConsumedEnergy = bestTotalConsumedEnergy;
        }
        uniform_int_distribution<int> distTurn(currentTurn, maxTurn);
        uniform_int_distribution<int> distShip(0, ships.size() - 1);
        uniform_real_distribution<double> distBool(0, 1);
        int maxTrial = 100;
        for (int trial = 0; trial < maxTrial; ++trial)
        {
            auto prevPaths = paths;
            auto prevEnergies = energies;
            double prevTotalConsumedEnergy = totalConsumedEnergy;
            int randShip = distShip(engine);
            int randTurn0 = distTurn(engine);
            int randTurn1 = distTurn(engine);
            swap(paths[randShip][randTurn0], paths[randShip][randTurn1]);
            int numOfStar0 = paths[randShip][randTurn0];
            int prevNumOfStar0 = paths[randShip][randTurn0 - 1];
            int advNumOfStar0 = paths[randShip][randTurn0 + 1];
            int numOfStar1 = paths[randShip][randTurn1];
            int prevNumOfStar1 = paths[randShip][randTurn1 - 1];
            int advNumOfStar1 = paths[randShip][randTurn1 + 1];
            double prevEnergy0 = 0;
            prevEnergy0 += (stars[2 * numOfStar0] - stars[2 * prevNumOfStar0])*(stars[2 * numOfStar0] - stars[2 * prevNumOfStar0]);
            prevEnergy0 += (stars[2 * numOfStar0 + 1] - stars[2 * prevNumOfStar0 + 1])*(stars[2 * numOfStar0 + 1] - stars[2 * prevNumOfStar0 + 1]);
            prevEnergy0 = sqrt(prevEnergy0);
            double advEnergy0 = 0;
            advEnergy0 += (stars[2 * advNumOfStar0] - stars[2 * numOfStar0])*(stars[2 * advNumOfStar0] - stars[2 * numOfStar0]);
            advEnergy0 += (stars[2 * advNumOfStar0 + 1] - stars[2 * numOfStar0 + 1])*(stars[2 * advNumOfStar0 + 1] - stars[2 * numOfStar0 + 1]);
            advEnergy0 = sqrt(advEnergy0);
            double prevEnergy1 = 0;
            prevEnergy1 += (stars[2 * numOfStar1] - stars[2 * prevNumOfStar1])*(stars[2 * numOfStar1] - stars[2 * prevNumOfStar1]);
            prevEnergy1 += (stars[2 * numOfStar1 + 1] - stars[2 * prevNumOfStar1 + 1])*(stars[2 * numOfStar1 + 1] - stars[2 * prevNumOfStar1 + 1]);
            prevEnergy1 = sqrt(prevEnergy1);
            double advEnergy1 = 0;
            advEnergy1 += (stars[2 * advNumOfStar1] - stars[2 * numOfStar1])*(stars[2 * advNumOfStar1] - stars[2 * numOfStar1]);
            advEnergy1 += (stars[2 * advNumOfStar1 + 1] - stars[2 * numOfStar1 + 1])*(stars[2 * advNumOfStar1 + 1] - stars[2 * numOfStar1 + 1]);
            advEnergy1 = sqrt(advEnergy1);
            for (int numOfUFO = 0; numOfUFO < ufos.size() / 3; numOfUFO++)
            {
                if (currentTurn == randTurn0)
                {
                    if (ufos[3 * numOfUFO] == prevNumOfStar0 && ufos[3 * numOfUFO + 1] == numOfStar0) prevEnergy0 *= 0.001;
                    if (ufos[3 * numOfUFO + 1] == numOfStar0 && ufos[3 * numOfUFO + 2] == advNumOfStar0) advEnergy0 *= 0.001;
                }
                if (currentTurn == randTurn1)
                {
                    if (ufos[3 * numOfUFO] == prevNumOfStar1 && ufos[3 * numOfUFO + 1] == numOfStar1) prevEnergy1 *= 0.001;
                    if (ufos[3 * numOfUFO + 1] == numOfStar1 && ufos[3 * numOfUFO + 2] == advNumOfStar1) advEnergy1 *= 0.001;
                }
            }
            totalConsumedEnergy += (prevEnergy0 - energies[randShip][randTurn0] + prevEnergy1 - energies[randShip][randTurn1]);
            totalConsumedEnergy += (advEnergy0 - energies[randShip][randTurn0 + 1] + advEnergy1 - energies[randShip][randTurn1 + 1]);
            //if(prevEnergy0 - energies[randShip][randTurn0] + prevEnergy1 - energies[randShip][randTurn1] < 0)
            //    cerr << fixed << prevEnergy0 << "," << energies[randShip][randTurn0] << "," << prevEnergy1 << "," << energies[randShip][randTurn1] << endl;
            //if (advEnergy0 - energies[randShip][randTurn0 + 1] + advEnergy1 - energies[randShip][randTurn1 + 1] < 0)
            //    cerr << fixed << advEnergy0 << "," << energies[randShip][randTurn0 + 1] << "," << advEnergy1 << "," << energies[randShip][randTurn1 + 1] << endl;
            energies[randShip][randTurn0] = prevEnergy0;
            energies[randShip][randTurn0 + 1] = advEnergy0;
            energies[randShip][randTurn1] = prevEnergy1;
            energies[randShip][randTurn1 + 1] = advEnergy1;
            double score = -totalConsumedEnergy;
            if (score > bestScore)
            {
                bestScore = score;
                bestPaths = paths;
                bestEnergies = energies;
                bestTotalConsumedEnergy = totalConsumedEnergy;
            }
            if (score > nextScore || distBool(engine) > 1.5 * (double)trial / maxTrial)
            {
                nextScore = score;
            }
            else
            {
                paths = prevPaths;
                totalConsumedEnergy = prevTotalConsumedEnergy;
                energies = prevEnergies;
            }
        }
        vector<int> ret(ships.size());
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        {
            ret[numOfShip] = bestPaths[numOfShip][currentTurn];
        }
        //double sum = 0;
        //for (int turn = 0; turn <= NStar; turn++) for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        //{
        //    cerr << turn << ":" << numOfShip << ":" << bestPaths[numOfShip][turn] << endl;
        //}
        //cerr << fixed << sum << endl;
        initPath = false;
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

