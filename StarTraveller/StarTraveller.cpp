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
    int init(vector<int> stars)
    {
        NStars = stars.size()/2;
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
                if (turn == 0) paths[numOfShip].push_back(ships[numOfShip]);
                paths[numOfShip].push_back(distStar(engine));
            }
            initPath = false;
        }
        uniform_int_distribution<> distTurn(currentTurn, maxTurn);
        uniform_int_distribution<> distShip(0, ships.size() - 1);
        double currentConsumedEnergy = 0;
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        {
            double energy = 0;
            auto path = paths[numOfShip];
            energy += (stars[2 * path[currentTurn]] - stars[2 * path[currentTurn - 1]])
                *(stars[2 * path[currentTurn]] - stars[2 * path[currentTurn - 1]]);
            energy += (stars[2 * path[currentTurn] + 1] - stars[2 * path[currentTurn - 1] + 1])
                *(stars[2 * path[currentTurn] + 1] - stars[2 * path[currentTurn - 1] + 1]);
            energy = sqrt(energy);
            currentConsumedEnergy += energy;
        }
        for (int trial = 0; trial < 1000; trial++)
        {
            auto prevPaths = paths;
            auto willVisitStar = visitedStars;
            double totalEnergy = 0;
            double consumedEnergy = 0;
            paths[distShip(engine)][distTurn(engine)] = distStar(engine);
            for (int turn = currentTurn; turn <= maxTurn; turn++) for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
            {
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
                totalEnergy += energy;
                willVisitStar.insert(path[turn]);
            }
            double score = -(totalEnergy + totalConsumedEnergy) + willVisitStar.size() * 1e6;
            if (score > maxScore)
            {
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

