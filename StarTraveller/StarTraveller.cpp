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

class Galaxy {
public:
    Point center;
    vector<int> stars;
    Galaxy(): stars(0), sumX(0), sumY(0){}
    void addStars(int numOfStar);
    static Galaxy merge(Galaxy& g, Galaxy& h);
private:
    int sumX;
    int sumY;
};

void Galaxy::addStars(int numOfStar)
{
    stars.push_back(numOfStar);
    sumX += allStars[2 * numOfStar];
    sumY += allStars[2 * numOfStar + 1];
    center.x = sumX / stars.size();
    center.y = sumY / stars.size();
}

Galaxy Galaxy::merge(Galaxy& g, Galaxy& h)
{
    for (auto& numOfStar : h.stars)
    {
        g.addStars(numOfStar);
    }
    return g;
}
class StarTraveller {
public:
    int NStar;
    vector<Galaxy> galaxies;
    int currentTurn;
    int maxTurn;
    bool initSettings;
    set<int> visitedStars;
    vector<int> ownedGalaxies;
    vector<bool> inTerritory;
    vector<bool> usingUFO;
    int expedition;
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
        expedition = -1;
        return 0;
    }
    vector<int> makeMoves(vector<int> ufos, vector<int> ships)
    {
        currentTurn++;
        random_device seed_gen;
        mt19937 engine(seed_gen());
        uniform_int_distribution<int> distStar(0, NStar - 1);
        vector<int> ret(ships.size());
        if (initSettings)
        {
            cerr << "init" << endl;
            initSettings = false;
            vector<bool> includedStars(NStar,false);
            galaxies = vector<Galaxy>(ships.size());
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
            bool ok = false;
            while (!ok)
            {
                cerr << galaxies.size() << endl;
                ok = true;
                vector<Galaxy> newGalaxies;
                vector<bool> merged(galaxies.size(), false);
                for (int numOfGalaxy0 = 0; numOfGalaxy0 < galaxies.size(); numOfGalaxy0++)
                {
                    if (merged[numOfGalaxy0]) continue;
                    for (int numOfGalaxy1 = numOfGalaxy0 + 1; numOfGalaxy1 < galaxies.size(); numOfGalaxy1++)
                    {
                        if (numOfGalaxy0 == numOfGalaxy1) continue;
                        if (merged[numOfGalaxy1]) continue;
                        int dist = 0;
                        dist += abs(galaxies[numOfGalaxy0].center.x - galaxies[numOfGalaxy1].center.x);
                        dist += abs(galaxies[numOfGalaxy0].center.y - galaxies[numOfGalaxy1].center.y);
                        if (dist < 500)
                        {
                            merged[numOfGalaxy0] = merged[numOfGalaxy1] = true;
                            newGalaxies.push_back(Galaxy::merge(galaxies[numOfGalaxy0], galaxies[numOfGalaxy1]));
                            ok = false;
                            break;
                        }
                    }
                }
                for (int numOfGalaxy = 0; numOfGalaxy < galaxies.size(); numOfGalaxy++) if (!merged[numOfGalaxy])
                {
                    newGalaxies.push_back(galaxies[numOfGalaxy]);
                }
                galaxies = newGalaxies;
            }
            for (int numOfGalaxy = 0; numOfGalaxy < galaxies.size(); numOfGalaxy++)
            {
                cerr << galaxies[numOfGalaxy].center.x << "," << galaxies[numOfGalaxy].center.y << endl;
            }
            inTerritory = vector<bool>(ships.size(),false);
            ownedGalaxies = vector<int>(ships.size());
            vector<bool> owned(galaxies.size(), false);
            double maxDist = -1e10;
            int expeditionShip = -1;
            for (int numOfShipOwing = 0; numOfShipOwing < ships.size(); numOfShipOwing++)
            {
                int nearGalaxy = -1;
                double minDist = 1e10;
                int numOfStar = ships[numOfShipOwing];
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
                if (minDist > maxDist)
                {
                    maxDist = minDist;
                    expeditionShip = numOfShipOwing;
                }
                ownedGalaxies[numOfShipOwing] = nearGalaxy;
                owned[nearGalaxy] = true;
                for (auto& numOfStar : galaxies[ownedGalaxies[numOfShipOwing]].stars)
                {
                    if (ships[numOfShipOwing] == numOfStar)
                    {
                        inTerritory[numOfShipOwing] = true;
                        break;
                    }
                }
            }
            expedition = expeditionShip;
            usingUFO = vector<bool>(ufos.size() / 3, false);
        }
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        {
            int numOfGalaxy = ownedGalaxies[numOfShip];
            if (!inTerritory[numOfShip] || numOfShip == expedition )
            {
                double maxScore = -1e10;
                int nextNumOfStar = -1;
                int useUFO = -1;
                for (int numOfStar = 0; numOfStar < NStar; ++numOfStar)
                {
                    bool usingNewStar = false;
                    int seeUFO = -1;
                    if (visitedStars.find(numOfStar) == visitedStars.end()) usingNewStar = true;
                    double energy = 0;
                    energy += (allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]])*(allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]]);
                    energy += (allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1]);
                    double distUFO = 1e10;
                    for (int numOfUFO = 0; numOfUFO < (ufos.size() / 3); numOfUFO++)
                    {
                        if (usingUFO[numOfUFO])
                        {
                            double dist = 0;
                            dist += (allStars[2 * numOfStar] - allStars[2 * ufos[3 * numOfUFO + 1]])*(allStars[2 * numOfStar] - allStars[2 * ufos[3 * numOfUFO + 1]]);
                            dist += (allStars[2 * numOfStar + 1] - allStars[2 * ufos[3 * numOfUFO + 1] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ufos[3 * numOfUFO + 1] + 1]);
                            if (ufos[3 * numOfUFO] == ships[numOfShip] && ufos[3 * numOfUFO + 1] == numOfStar)
                            {
                                seeUFO = numOfUFO;
                                energy = 1e-10;
                            }
                            distUFO = min(dist, distUFO);
                        }
                    }
                    double score = -energy + (double)(currentTurn * currentTurn) * 1e6 * (usingNewStar ? 1. : 0) / (maxTurn * maxTurn) - distUFO*0.01;
                    if (score > maxScore)
                    {
                        maxScore = score;
                        nextNumOfStar = numOfStar;
                        useUFO = seeUFO;
                    }
                }
                if (useUFO != -1) usingUFO[useUFO] = true;
                ret[numOfShip] = nextNumOfStar;
                visitedStars.insert(ret[numOfShip]);
                for (auto& numOfStar : galaxies[ownedGalaxies[numOfShip]].stars)
                {
                    if (ships[numOfShip] == numOfStar)
                    {
                        inTerritory[numOfShip] = true;
                        break;
                    }
                }
            }
            else
            {
                double maxScore = -1e10;
                int nextNumOfStar = -1;
                int useUFO = -1;
                bool stop = true;
                for (auto& numOfStar : galaxies[numOfGalaxy].stars)
                {
                    bool usingNewStar = false;
                    int seeUFO = -1;
                    if (visitedStars.find(numOfStar) == visitedStars.end())
                    {
                        usingNewStar = true;
                        stop = false;
                    }
                    double energy = 0;
                    energy += (allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]])*(allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]]);
                    energy += (allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1]);
                    double distUFO = 1e10;
                    for (int numOfUFO = 0; numOfUFO < (ufos.size() / 3); numOfUFO++)
                    {
                        if (!usingUFO[numOfUFO])
                        {
                            double dist = 0;
                            dist += (allStars[2 * numOfStar] - allStars[2 * ufos[3 * numOfUFO + 1]])*(allStars[2 * numOfStar] - allStars[2 * ufos[3 * numOfUFO + 1]]);
                            dist += (allStars[2 * numOfStar + 1] - allStars[2 * ufos[3 * numOfUFO + 1] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ufos[3 * numOfUFO + 1] + 1]);
                            if (ufos[3 * numOfUFO] == ships[numOfShip] && ufos[3 * numOfUFO + 1] == numOfStar)
                            {
                                seeUFO = numOfUFO;
                                energy = 1e-10;
                            }
                            distUFO = min(dist, distUFO);
                        }
                    }
                    double score = -energy + (double)(currentTurn * currentTurn) * 1e6 * (usingNewStar ? 1. : 0) / (maxTurn * maxTurn) - distUFO*0.01;
                    if (score > maxScore)
                    {
                        maxScore = score;
                        nextNumOfStar = numOfStar;
                        useUFO = seeUFO;
                    }
                }
                if (useUFO != -1) usingUFO[useUFO] = true;
                ret[numOfShip] = nextNumOfStar;
                if (stop) ret[numOfShip] = ships[numOfShip];
                visitedStars.insert(ret[numOfShip]);
            }
        }
        for (int numOfUFO = 0; numOfUFO < ufos.size() / 3; numOfUFO++)
        {
            usingUFO[numOfUFO] = false;
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

