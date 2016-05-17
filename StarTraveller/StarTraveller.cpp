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
    Galaxy() : stars(0), sumX(0), sumY(0) {}
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
    int currentTurn;
    int maxTurn;
    int visited;
    bool initSettings;
    vector<int> visitedStars;
    vector<Galaxy> ownedGalaxies;
    vector<bool> inTerritory;
    vector<bool> usingUFO;
    vector<bool> expedition;
    int expeditionCount;
    bool ready;
    int init(vector<int> stars)
    {
        NStar = stars.size() / 2;
        allStars = stars;
        maxTurn = 4 * NStar;
        currentTurn = 0;
        initSettings = true;
        ready = false;
        visitedStars = vector<int>(NStar,0);
        visited = 0;
        return 0;
    }
    vector<int> makeMoves(vector<int> ufos, vector<int> ships)
    {
        currentTurn++;
        random_device seed_gen;
        mt19937 engine(seed_gen());
        uniform_int_distribution<int> distStar(0, NStar - 1);
        vector<int> ret(ships.size());
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++) ret[numOfShip] == ships[numOfShip];
        if (initSettings)
        {
            vector<Galaxy> galaxies;
            cerr << "init" << endl;
            initSettings = false;
            galaxies = vector<Galaxy>(NStar);
            for (int numOfGalaxy = 0; numOfGalaxy < galaxies.size(); numOfGalaxy++)
            {
                galaxies[numOfGalaxy].addStars(numOfGalaxy);
            }
            bool ok = false;
            int k = 100;
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
                        double distance = 0;
                        distance += (galaxies[numOfGalaxy0].center.x - galaxies[numOfGalaxy1].center.x) * (galaxies[numOfGalaxy0].center.x - galaxies[numOfGalaxy1].center.x);
                        distance += (galaxies[numOfGalaxy0].center.y - galaxies[numOfGalaxy1].center.y) * (galaxies[numOfGalaxy0].center.y - galaxies[numOfGalaxy1].center.y);
                        if (distance < k*k)
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
            //for (int numOfGalaxy = 0; numOfGalaxy < galaxies.size(); numOfGalaxy++)
            //{
            //    cerr << galaxies[numOfGalaxy].center.x << "," << galaxies[numOfGalaxy].center.y << endl;
            //}
            inTerritory = vector<bool>(ships.size(), false);
            ownedGalaxies = vector<Galaxy>(ships.size());
            vector<bool> owned(galaxies.size(), false);
            double maxDist = -1e10;
            int expeditionShip = -1;
            for (int numOfShipOwing = 0; numOfShipOwing < ships.size(); numOfShipOwing++)
            {
                int nearGalaxy = -1;
                double minDistance = 1e10;
                int numOfStar = ships[numOfShipOwing];
                for (int numOfGalaxy = 0; numOfGalaxy < galaxies.size(); numOfGalaxy++)
                {
                    auto galaxy = galaxies[numOfGalaxy];
                    double distance = 0;
                    distance += (galaxy.center.x - allStars[2 * numOfStar]) * (galaxy.center.x - allStars[2 * numOfStar]);
                    distance += (galaxy.center.y - allStars[2 * numOfStar + 1]) * (galaxy.center.y - allStars[2 * numOfStar + 1]);
                    distance = sqrt(distance);
                    if (distance < minDistance)
                    {
                        minDistance = distance;
                        nearGalaxy = numOfGalaxy;
                    }
                }
                if (minDistance > maxDist)
                {
                    maxDist = minDistance;
                    expeditionShip = numOfShipOwing;
                }
                ownedGalaxies[numOfShipOwing] = galaxies[nearGalaxy];
                owned[nearGalaxy] = true;
                for (auto& numOfStar : ownedGalaxies[numOfShipOwing].stars)
                {
                    if (ships[numOfShipOwing] == numOfStar)
                    {
                        inTerritory[numOfShipOwing] = true;
                        break;
                    }
                }
            }
            Galaxy newGalaxy;
            for (int numOfGalaxy = 0; numOfGalaxy < galaxies.size(); numOfGalaxy++)
            {
                if (!owned[numOfGalaxy]) newGalaxy = Galaxy::merge(newGalaxy, galaxies[numOfGalaxy]);
            }
            expedition = vector<bool>(ships.size(), false);
            expedition[expeditionShip] = true;
            expeditionCount = 1;
            ownedGalaxies[expeditionShip] = newGalaxy;
            usingUFO = vector<bool>(ufos.size() / 3, false);
        }
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        {
            auto galaxy = ownedGalaxies[numOfShip];
            if (expedition[numOfShip])
            {
                double maxScore = -1e10;
                int nextNumOfStar = -1;
                int useUFO = -1;
                bool stop = true;
                for (int numOfStar = 0; numOfStar < NStar; numOfStar++)
                {
                    bool usingNewStar = false;
                    int seeUFO = -1;
                    if (visitedStars[numOfStar] == 0)
                    {
                        usingNewStar = true;
                        stop = false;
                    }
                    double energy = 0;
                    energy += (allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]])*(allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]]);
                    energy += (allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1]);
                    double UFODistance = 1e6;
                    for (int numOfUFO = 0; numOfUFO < (ufos.size() / 3); numOfUFO++)
                    {
                        if (!usingUFO[numOfUFO])
                        {
                            double distance = 0;
                            distance += (allStars[2 * numOfStar] - allStars[2 * ufos[3 * numOfUFO + 2]])*(allStars[2 * numOfStar] - allStars[2 * ufos[3 * numOfUFO + 2]]);
                            distance += (allStars[2 * numOfStar + 1] - allStars[2 * ufos[3 * numOfUFO + 2] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ufos[3 * numOfUFO + 2] + 1]);
                            auto temp = distance;
                            distance = 0;
                            distance += (allStars[2 * numOfStar] - allStars[2 * ufos[3 * numOfUFO + 1]])*(allStars[2 * numOfStar] - allStars[2 * ufos[3 * numOfUFO + 1]]);
                            distance += (allStars[2 * numOfStar + 1] - allStars[2 * ufos[3 * numOfUFO + 1] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ufos[3 * numOfUFO + 1] + 1]);
                            distance = min(distance, temp);
                            if (ufos[3 * numOfUFO] == ships[numOfShip] && ufos[3 * numOfUFO + 1] == numOfStar)
                            {
                                energy = 1e-10;
                            }
                            if (distance < UFODistance)
                            {
                                UFODistance = distance;
                                seeUFO = numOfUFO;
                            }
                        }
                    }
                    double score = -energy + (double)(currentTurn * currentTurn) * 1e6 * (usingNewStar ? 1. : 0) / (maxTurn * maxTurn)
                                   - UFODistance*abs(0.5 - (double)visited / NStar)*0.01;
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
                if (ret[numOfShip] != ships[numOfShip] || visitedStars[ret[numOfShip]] == 0)
                {
                    if (visitedStars[ret[numOfShip]] == 0) visited++;
                    ++visitedStars[ret[numOfShip]];
                }
                for (auto& numOfStar : ownedGalaxies[numOfShip].stars)
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
                set<int> enableUFO;
                for (int numOfUFO = 0; numOfUFO < (ufos.size() / 3); numOfUFO++)
                {
                    for (auto& numOfStar : galaxy.stars)
                    {
                        if (numOfStar == ufos[3 * numOfUFO + 1])
                        {
                            enableUFO.insert(numOfUFO);
                            break;
                        }
                    }
                }
                set<int> otherShipsInTerritory;
                for (int numOfOtherShip = 0; numOfOtherShip < ships.size(); numOfOtherShip++)
                {
                    if (numOfOtherShip == numOfShip) continue;
                    for (auto& numOfStar : galaxy.stars)
                    {
                        if (numOfStar == ships[numOfOtherShip])
                        {
                            otherShipsInTerritory.insert(numOfOtherShip);
                            break;
                        }
                    }
                }
                if (false)
                {
                    for (auto& numOfStar : galaxy.stars)
                    {
                        bool usingNewStar = false;
                        bool UFODriver = true;
                        int seeUFO = -1;
                        if (visitedStars[numOfStar] == 0)
                        {
                            usingNewStar = true;
                            stop = false;
                        }
                        double energy = 0;
                        energy += (allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]])*(allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]]);
                        energy += (allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1]);
                        double UFODistance = 1e6;

                        for (auto& numOfUFO : enableUFO)
                        {
                            if (!usingUFO[numOfUFO])
                            {
                                double distance = 0;
                                distance += (allStars[2 * numOfStar] - allStars[2 * ufos[3 * numOfUFO + 2]])*(allStars[2 * numOfStar] - allStars[2 * ufos[3 * numOfUFO + 2]]);
                                distance += (allStars[2 * numOfStar + 1] - allStars[2 * ufos[3 * numOfUFO + 2] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ufos[3 * numOfUFO + 2] + 1]);
                                auto temp = distance;
                                distance = 0;
                                distance += (allStars[2 * numOfStar] - allStars[2 * ufos[3 * numOfUFO + 1]])*(allStars[2 * numOfStar] - allStars[2 * ufos[3 * numOfUFO + 1]]);
                                distance += (allStars[2 * numOfStar + 1] - allStars[2 * ufos[3 * numOfUFO + 1] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ufos[3 * numOfUFO + 1] + 1]);
                                distance = min(distance, temp);
                                if (ufos[3 * numOfUFO] == ships[numOfShip] && ufos[3 * numOfUFO + 1] == numOfStar)
                                {
                                    energy = 0;
                                }
                                if (distance < UFODistance)
                                {
                                    UFODistance = distance;
                                    seeUFO = numOfUFO;
                                }
                            }
                        }
                        double otherShipDistance = 1e6;
                        for (auto& numOfOtherShip : otherShipsInTerritory)
                        {
                            double distance = 0;
                            distance += (allStars[2 * numOfStar] - allStars[2 * ret[numOfOtherShip]])*(allStars[2 * numOfStar] - allStars[2 * ret[numOfOtherShip]]);
                            distance += (allStars[2 * numOfStar + 1] - allStars[2 * ret[numOfOtherShip] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ret[numOfOtherShip] + 1]);
                            otherShipDistance = min(distance, otherShipDistance);
                        }
                        double score = -energy + (double)(currentTurn * currentTurn) * 1e6 * (usingNewStar ? 1. : 0) / (maxTurn * maxTurn)
                            - UFODistance*abs(0.5 - (double)visited/NStar) + otherShipDistance*0.01;
                        if (score > maxScore)
                        {
                            maxScore = score;
                            nextNumOfStar = numOfStar;
                            useUFO = seeUFO;
                        }
                    }
                }
                if (useUFO != -1) usingUFO[useUFO] = true;
                ret[numOfShip] = nextNumOfStar;
                if (stop)
                {
                    if (expeditionCount < (int)ships.size() - (int)ufos.size() / 3)
                    {
                        expeditionCount++;
                        expedition[numOfShip] = true;
                    }
                    ret[numOfShip] = ships[numOfShip];
                    for (auto& numOfUFO : enableUFO)
                    {
                        if (!usingUFO[numOfUFO])
                        {
                            if (ufos[3 * numOfUFO] == ships[numOfShip] && visitedStars[ufos[3 * numOfUFO + 1]] == 0)
                            {
                                ret[numOfShip] = ufos[3 * numOfUFO + 1];
                                usingUFO[numOfUFO] = true;
                            }
                        }
                    }
                }
                if (ret[numOfShip] != ships[numOfShip] || visitedStars[ret[numOfShip]] == 0)
                {
                    if (visitedStars[ret[numOfShip]] == 0) visited++;
                    ++visitedStars[ret[numOfShip]];
                }
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

