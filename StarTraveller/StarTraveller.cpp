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

vector<int> allStars;
//Should x,y are real numbers?
struct Point {
    int x, y;
    Point() {}
    Point(int x, int y) { this->x = x; this->y = y; }
    bool operator==(const Point p) const
    {
        return x == p.x && y == p.y;
    }
};

class Garaxy {
public:
    Point center;
    vector<int> stars;
    Garaxy(): stars(0), sumX(0), sumY(0){}
    void addStars(int numOfStar);
private:
    int sumX;
    int sumY;
};

void Garaxy::addStars(int numOfStar)
{
    stars.push_back(numOfStar);
    sumX += allStars[2 * numOfStar];
    sumY += allStars[2 * numOfStar + 1];
    center.x = sumX / stars.size();
    center.y = sumY / stars.size();
}

class StarTraveller {
public:
    int NStar;
    vector<Garaxy> galaxies;
    int currentTurn;
    int maxTurn;
    bool initSettings;
    set<int> visitedStars;
    vector<int> ownedGalaxies;
    vector<vector<int>> territories;
    vector<bool> decidePaths;
    vector<bool> onReady;
    vector<vector<int>> paths;
    bool ready;
    int init(vector<int> stars)
    {
        NStar = stars.size() / 2;
        allStars = stars;
        maxTurn = 4 * NStar;
        currentTurn = 0;
        initSettings = true;
        ready = false;
        visitedStars = set<int>();
        return 0;
    }
    vector<int> makeMoves(vector<int> ufos, vector<int> ships)
    {
        currentTurn++;
        random_device seed_gen;
        mt19937 engine(seed_gen());
        uniform_int_distribution<int> distStar(0, NStar - 1);
        if (initSettings)
        {
            cerr << "init" << endl;
            initSettings = false;
            vector<bool> includedStars(NStar,false);
            galaxies = vector<Garaxy>(ships.size());
            for (int numOfGalaxy = 0; numOfGalaxy < galaxies.size(); numOfGalaxy++)
            {
                int numOfStar = distStar(engine);
                galaxies[numOfGalaxy].addStars(numOfStar);
                includedStars[numOfStar] = true;
            }
            for (int numOfStar = 0; numOfStar < NStar; numOfStar++)
            {
                if (includedStars[numOfStar]) continue;
                int nearGalaxy = -1;
                double minDist = 1e10;
                for (int numOfGalaxy = 0; numOfGalaxy < galaxies.size(); numOfGalaxy++)
                {
                    auto galaxy = galaxies[numOfGalaxy];
                    double dist = 0;
                    dist += (galaxy.center.x - allStars[2 * numOfStar]) * (galaxy.center.x - allStars[2 * numOfStar]);
                    dist += (galaxy.center.y - allStars[2 * numOfStar + 1]) * (galaxy.center.y - allStars[2 * numOfStar + 1]);
                    dist = sqrt(dist);
                    if (dist < minDist)
                    {
                        minDist = dist;
                        nearGalaxy = numOfGalaxy;
                    }
                }
                galaxies[nearGalaxy].addStars(numOfStar);
                includedStars[numOfStar] = true;
            }
            for (int numOfGalaxy = 0; numOfGalaxy < galaxies.size(); numOfGalaxy++)
            {
                cerr << galaxies[numOfGalaxy].center.x << "," << galaxies[numOfGalaxy].center.y << endl;
            }
            territories = vector<vector<int>>(ships.size());
            ownedGalaxies = vector<int>(ships.size());
            vector<bool> owned(galaxies.size(), false);
            for (int numOfShipOwing = 0; numOfShipOwing < ships.size(); numOfShipOwing++)
            {
                int nearGalaxy = -1;
                double minDist = 1e10;
                int numOfStar = ships[numOfShipOwing];
                for (int numOfGalaxy = 0; numOfGalaxy < galaxies.size(); numOfGalaxy++)
                {
                    if (owned[numOfGalaxy]) continue;
                    auto galaxy = galaxies[numOfGalaxy];
                    double dist = 0;
                    dist += (galaxy.center.x - allStars[2 * numOfStar]) * (galaxy.center.x - allStars[2 * numOfStar]);
                    dist += (galaxy.center.y - allStars[2 * numOfStar + 1]) * (galaxy.center.y - allStars[2 * numOfStar + 1]);
                    dist = sqrt(dist);
                    if (dist < minDist)
                    {
                        minDist = dist;
                        nearGalaxy = numOfGalaxy;
                    }
                }
                ownedGalaxies[numOfShipOwing] = nearGalaxy;
                owned[nearGalaxy] = true;
                for (auto& numOfStar : galaxies[ownedGalaxies[numOfShipOwing]].stars)
                {
                    for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
                    {
                        if (ships[numOfShip] == numOfStar) territories[numOfShipOwing].push_back(numOfShip);
                    }
                }
            }
            paths = vector<vector<int>>(ships.size(), vector<int>(maxTurn + 1, -1));
            for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++) paths[numOfShip][0] = ships[numOfShip];
            decidePaths = vector<bool>(ships.size(), false);
        }
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        {
            auto shipsTerritory = territories[numOfShip];
            int numOfGalaxy = ownedGalaxies[numOfShip];
            if (find(shipsTerritory.begin(), shipsTerritory.end(), numOfShip) == shipsTerritory.end())
            {
                cerr << "out of territory:" << numOfShip << endl;
                double maxScore = -1e10;
                int nextNumOfStar = -1;
                for (int numOfStar = 0; numOfStar < NStar; ++numOfStar)
                {
                    bool usingNewStar = false;
                    bool usingUFO = false;
                    if (visitedStars.find(numOfStar) == visitedStars.end()) usingNewStar = true;
                    double energy = 0;
                    energy += (allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]])*(allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]]);
                    energy += (allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1]);
                    for (int numOfUfo = 0; numOfUfo < (ufos.size() / 3); numOfUfo++)
                    {
                        if (ufos[3 * numOfUfo] == ships[numOfShip] && ufos[3 * numOfUfo + 1] == numOfStar)
                        {
                            energy *= 0.001 * 0.001;
                        }
                    }
                    auto galaxy = galaxies[numOfGalaxy];
                    double dist = 0;
                    dist += (galaxy.center.x - allStars[2 * numOfStar]) * (galaxy.center.x - allStars[2 * numOfStar]);
                    dist += (galaxy.center.y - allStars[2 * numOfStar + 1]) * (galaxy.center.y - allStars[2 * numOfStar + 1]);
                    double othersDist = 0;
                    for (int numOfOthersGalaxy = 0; numOfOthersGalaxy < galaxies.size(); numOfOthersGalaxy++)
                    {
                        if (numOfOthersGalaxy != ownedGalaxies[numOfShip])
                        {
                            auto galaxy = galaxies[numOfOthersGalaxy];
                            othersDist += (galaxy.center.x - allStars[2 * numOfStar]) * (galaxy.center.x - allStars[2 * numOfStar]);
                            othersDist += (galaxy.center.y - allStars[2 * numOfStar + 1]) * (galaxy.center.y - allStars[2 * numOfStar + 1]);
                        }
                    }
                    double score = -energy + (double)(currentTurn * currentTurn) * 1e6 * (usingNewStar || usingUFO ? 1. : 0) / (maxTurn * maxTurn) - dist*0.01 + othersDist * 0.001;
                    if (score > maxScore)
                    {
                        maxScore = score;
                        nextNumOfStar = numOfStar;
                    }
                }
                paths[numOfShip][currentTurn] = nextNumOfStar;
            }
            else
            {
                cerr << "in territory:" << numOfShip << endl;
                double maxScore = -1e10;
                int nextNumOfStar = -1;
                for (auto& numOfStar : galaxies[numOfGalaxy].stars)
                {
                    bool usingNewStar = false;
                    bool usingUFO = false;
                    if (visitedStars.find(numOfStar) == visitedStars.end()) usingNewStar = true;
                    double energy = 0;
                    energy += (allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]])*(allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]]);
                    energy += (allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1]);
                    for (int numOfUfo = 0; numOfUfo < (ufos.size() / 3); numOfUfo++)
                    {
                        if (ufos[3 * numOfUfo] == ships[numOfShip] && ufos[3 * numOfUfo + 1] == numOfStar)
                        {
                            energy *= 0.001 * 0.001;
                        }
                    }
                    auto galaxy = galaxies[numOfGalaxy];
                    double othersDist = 0;
                    for (int numOfOthersGalaxy = 0; numOfOthersGalaxy < galaxies.size(); numOfOthersGalaxy++)
                    {
                        if (numOfOthersGalaxy != ownedGalaxies[numOfShip])
                        {
                            auto galaxy = galaxies[numOfOthersGalaxy];
                            othersDist += (galaxy.center.x - allStars[2 * numOfStar]) * (galaxy.center.x - allStars[2 * numOfStar]);
                            othersDist += (galaxy.center.y - allStars[2 * numOfStar + 1]) * (galaxy.center.y - allStars[2 * numOfStar + 1]);
                        }
                    }
                    double score = -energy + (double)(currentTurn * currentTurn) * 1e6 * (usingNewStar || usingUFO ? 1. : 0) / (maxTurn * maxTurn) + othersDist * 0.001;
                    if (score > maxScore)
                    {
                        maxScore = score;
                        nextNumOfStar = numOfStar;
                    }
                }
                paths[numOfShip][currentTurn] = nextNumOfStar;
            }
        }
        vector<int> ret(ships.size());
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        {
            ret[numOfShip] = paths[numOfShip][currentTurn];
            visitedStars.insert(paths[numOfShip][currentTurn]);
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

