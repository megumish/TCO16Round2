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

class UFOInfo {
public:
    int numOfUFO;
    bool onShip;
    double averageDistance;
    Galaxy territory;
    UFOInfo() : numOfUFO(-1), totalDistance(0), count(0) {};
    void computeAverage(double newDistance);
private:
    double totalDistance;
    int count;
};

void UFOInfo::computeAverage(double newDistance)
{
    totalDistance += newDistance;
    count++;
    averageDistance = totalDistance / count;
}

class StarTraveller {
public:
    int NStar;
    int currentTurn;
    int maxTurn;
    int visited;
    bool initSettings;
    bool finishedEndingSettings;
    vector<int> visitedStars;
    vector<UFOInfo> infosOfOwnedUFO;
    vector<bool> inTerritory;
    vector<Galaxy> finalArea;
    vector<bool> inFinalArea;
    vector<int> startPointsInFinalArea;
    vector<double> toFinalAreaDistance;
    vector<UFOInfo> UFOInfos;
    vector<vector<int>> paths;
    int randEndTurn;
    int predictionSucceeded;
    int predictionCount;
    bool ready;
    int init(vector<int> stars)
    {
        NStar = stars.size() / 2;
        allStars = stars;
        maxTurn = 4 * NStar;
        currentTurn = 0;
        initSettings = true;
        finishedEndingSettings = false;
        ready = false;
        visitedStars = vector<int>(NStar, 0);
        visited = 0;
        predictionSucceeded = 0;
        predictionCount = 0;
        return 0;
    }
    vector<int> makeMoves(vector<int> ufos, vector<int> ships)
    {
        currentTurn++;
        random_device seed_gen;
        mt19937 engine(seed_gen());
        uniform_int_distribution<int> distStar(0, NStar - 1);
        uniform_int_distribution<int> dist(10, 100);
        vector<int> ret(ships.size());
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++) ret[numOfShip] = ships[numOfShip];

        if (currentTurn == 1)
        {
            UFOInfos.resize(ships.size());
        }
        if (!finishedEndingSettings && (ufos.size() == 0 || NStar - visited > (maxTurn - 100 - currentTurn) * ships.size()))
        {
            finishedEndingSettings = true;
            cerr << "endingSettings" << endl;
            vector<Galaxy> galaxies;
            for (int numOfStar = 0; numOfStar < NStar; numOfStar++)
            {
                if (visitedStars[numOfStar] == 0)
                {
                    Galaxy galaxy;
                    galaxy.addStars(numOfStar);
                    galaxies.push_back(galaxy);
                }
            }
            int k = 1;
            while (galaxies.size() > ships.size())
            {
                k++;
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
                        if (distance < k)
                        {
                            merged[numOfGalaxy0] = merged[numOfGalaxy1] = true;
                            newGalaxies.push_back(Galaxy::merge(galaxies[numOfGalaxy0], galaxies[numOfGalaxy1]));
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
                cerr << galaxies[numOfGalaxy].stars.size() << endl;
            }
            finalArea = vector<Galaxy>(ships.size());
            inFinalArea = vector<bool>(ships.size(), false);
            startPointsInFinalArea = vector<int>(ships.size(), -1);
            toFinalAreaDistance = vector<double>(ships.size());
            vector<bool> appointed(ships.size(), false);
            for (int numOfGalaxy = 0; numOfGalaxy < galaxies.size(); numOfGalaxy++)
            {
                int startPoint = -1;
                double maxDistance = -1e10;
                auto& galaxy = galaxies[numOfGalaxy];
                for (auto& numOfStar : galaxy.stars)
                {
                    double distance = 0;
                    distance += (galaxy.center.x - allStars[2 * numOfStar])*(galaxy.center.x - allStars[2 * numOfStar]);
                    distance += (galaxy.center.y - allStars[2 * numOfStar + 1])*(galaxy.center.y - allStars[2 * numOfStar + 1]);
                    if (distance > maxDistance)
                    {
                        maxDistance = distance;
                        startPoint = numOfStar;
                    }
                }
                int nearShip = -1;
                double minDistance = 1e10;
                for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
                {
                    if (appointed[numOfShip]) continue;
                    auto& prevArea = UFOInfos[numOfShip].territory;
                    if (prevArea.stars.size() == 0)
                    {
                        prevArea.addStars(ships[numOfShip]);
                    }
                    for (auto& numOfStar : prevArea.stars)
                    {
                        double distance = 0;
                        distance += (allStars[2 * startPoint] + allStars[2 * numOfStar])*(allStars[2 * startPoint] + allStars[2 * numOfStar]);
                        distance += (allStars[2 * startPoint + 1] + allStars[2 * numOfStar + 1])*(allStars[2 * startPoint + 1] + allStars[2 * numOfStar + 1]);
                        distance = sqrt(distance);
                        if (distance < minDistance)
                        {
                            minDistance = distance;
                            nearShip = numOfShip;
                        }
                    }
                }
                cerr << "Hello" << endl;
                if (nearShip != -1)
                {
                    appointed[nearShip] = true;
                    finalArea[nearShip] = galaxy;
                    startPointsInFinalArea[nearShip] = startPoint;
                    toFinalAreaDistance[nearShip] = minDistance;
                }
            }
        }

        if (ufos.size() != 0 && currentTurn * 100 <= maxTurn)
        {
            if (currentTurn == 1)
            {
                cerr << "Observing UFO" << endl;
                UFOInfos.resize(ufos.size() / 3);
                for (int numOfUFO = 0; numOfUFO < UFOInfos.size(); numOfUFO++) UFOInfos[numOfUFO].numOfUFO = numOfUFO;
            }
            for (int numOfUFO = 0; numOfUFO < ufos.size() / 3; numOfUFO++)
            {
                int numOfStar0 = ufos[3 * numOfUFO];
                int numOfStar1 = ufos[3 * numOfUFO + 1];
                double distance = 0;
                distance += (allStars[2 * numOfStar1] - allStars[2 * numOfStar0])*(allStars[2 * numOfStar1] - allStars[2 * numOfStar0]);
                distance += (allStars[2 * numOfStar1 + 1] - allStars[2 * numOfStar0 + 1])*(allStars[2 * numOfStar1 + 1] - allStars[2 * numOfStar0 + 1]);
                distance = sqrt(distance);
                UFOInfos[numOfUFO].computeAverage(distance);
            }
        }
        else if (ufos.size() != 0 && initSettings)
        {
            cerr << "InitSettings" << endl;
            initSettings = false;
            sort(UFOInfos.begin(), UFOInfos.end(), [](UFOInfo info0, UFOInfo info1)
            {
                return info0.averageDistance > info1.averageDistance;
            });
            cerr << "define ufo's territory" << endl;
            for (int numOfUFOInfo = 0; numOfUFOInfo < UFOInfos.size(); numOfUFOInfo++)
            {
                auto& info = UFOInfos[numOfUFOInfo];
                int numOfStarOnUFO = ufos[3 * info.numOfUFO];
                auto& territory = info.territory;
                territory.addStars(numOfStarOnUFO);
                vector<bool> added(NStar, false);
                added[numOfStarOnUFO] = true;
                bool ok = false;
                while (!ok)
                {
                    ok = true;
                    Galaxy newTerritory = territory;
                    for (auto& numOfStar0 : territory.stars)
                    {
                        for (int numOfStar1 = 0; numOfStar1 < NStar; numOfStar1++)
                        {
                            if (added[numOfStar1]) continue;
                            double distance = 0;
                            distance += (allStars[2 * numOfStar1] - allStars[2 * numOfStar0])*(allStars[2 * numOfStar1] - allStars[2 * numOfStar0]);
                            distance += (allStars[2 * numOfStar1 + 1] - allStars[2 * numOfStar0 + 1])*(allStars[2 * numOfStar1 + 1] - allStars[2 * numOfStar0 + 1]);
                            distance = sqrt(distance);
                            if (distance < info.averageDistance)
                            {
                                newTerritory.addStars(numOfStar1);
                                added[numOfStar1] = true;
                                ok = false;
                            }
                        }
                    }
                    territory = newTerritory;
                }
            }
            for (int numOfUFOInfo = 0; numOfUFOInfo < UFOInfos.size(); numOfUFOInfo++)
            {
                cerr << UFOInfos[numOfUFOInfo].territory.center.x << "," << UFOInfos[numOfUFOInfo].territory.center.y << endl;
                cerr << UFOInfos[numOfUFOInfo].territory.stars.size() << endl;
            }
            cerr << "give ownership each ships" << endl;
            inTerritory = vector<bool>(ships.size(), false);
            infosOfOwnedUFO = vector<UFOInfo>(ships.size());
            vector<bool> owing(ships.size(), false);
            double maxDist = -1e10;
            int expeditionShip = -1;
            for (int numOfUFOInfo = 0; numOfUFOInfo < UFOInfos.size(); numOfUFOInfo++)
            {
                int nearShip = -1;
                double minDistance = 1e10;
                auto territory = UFOInfos[numOfUFOInfo].territory;
                for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
                {
                    int numOfStarOnShip = ships[numOfShip];
                    if (owing[numOfShip]) continue;
                    double distance = 0;
                    distance += (territory.center.x - allStars[2 * numOfStarOnShip]) * (territory.center.x - allStars[2 * numOfStarOnShip]);
                    distance += (territory.center.y - allStars[2 * numOfStarOnShip + 1]) * (territory.center.y - allStars[2 * numOfStarOnShip + 1]);
                    distance = sqrt(distance);
                    if (distance < minDistance)
                    {
                        minDistance = distance;
                        nearShip = numOfShip;
                    }
                }
                if (nearShip != -1)
                {
                    infosOfOwnedUFO[nearShip] = UFOInfos[numOfUFOInfo];
                    owing[nearShip] = true;
                    for (auto& numOfStar : infosOfOwnedUFO[nearShip].territory.stars)
                    {
                        if (ships[nearShip] == numOfStar)
                        {
                            inTerritory[nearShip] = true;
                            break;
                        }
                    }
                }
            }
            for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
            {
                cerr << "Ship:" << numOfShip << "owns UFO:" << infosOfOwnedUFO[numOfShip].numOfUFO << endl;
            }
            randEndTurn = dist(engine);
            cerr << randEndTurn << endl;
        }
        else if (ufos.size() != 0 && !finishedEndingSettings)
        {
            if (currentTurn == 1)
            {
                cerr << "Run" << endl;
            }
            for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
            {
                auto& info = infosOfOwnedUFO[numOfShip];
                if (info.numOfUFO != -1)
                {
                    auto territory = info.territory;
                    if (!info.onShip)
                    {
                        if (!inTerritory[numOfShip])
                        {
                            double minDistance = 1e10;
                            for (auto& numOfStar : territory.stars)
                            {
                                double distance = 0;
                                distance += (allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]])*(allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]]);
                                distance += (allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1]);
                                distance = sqrt(distance);
                                if (distance < minDistance)
                                {
                                    minDistance = distance;
                                    ret[numOfShip] = numOfStar;
                                }
                            }
                            inTerritory[numOfShip] = true;
                        }
                        else
                        {
                            int UFONextStar = ufos[3 * info.numOfUFO + 2];
                            int shipStar = ships[numOfShip];
                            double distance = 0;
                            distance += (allStars[2 * UFONextStar] - allStars[2 * shipStar])*(allStars[2 * UFONextStar] - allStars[2 * shipStar]);
                            distance += (allStars[2 * UFONextStar + 1] - allStars[2 * shipStar + 1])*(allStars[2 * UFONextStar + 1] - allStars[2 * shipStar + 1]);
                            distance = sqrt(distance);
                            if (distance < 100)
                            {
                                ret[numOfShip] = UFONextStar;
                                info.onShip = true;
                            }
                        }
                    }
                    else
                    {
                        ret[numOfShip] = ufos[3 * info.numOfUFO + 1];
                    }
                }
            }
        }
        else
        {
            cerr << "In FinalMission" << endl;
            for (int numOfShip = 0; numOfShip < ships.size(); ++numOfShip)
            {
                auto& ufoInfo = infosOfOwnedUFO[numOfShip];
                if (finalArea[numOfShip].stars.size() != 0)
                {
                    if (ufos.size() != 0 && ufoInfo.onShip)
                    {
                        int startPoint = startPointsInFinalArea[numOfShip];
                        double distance = 0;
                        distance += (allStars[2 * startPoint] - allStars[2 * ships[numOfShip]])*(allStars[2 * startPoint] - allStars[2 * ships[numOfShip]]);
                        distance += (allStars[2 * startPoint + 1] - allStars[2 * ships[numOfShip] + 1])*(allStars[2 * startPoint + 1] - allStars[2 * ships[numOfShip] + 1]);
                        distance = sqrt(distance);
                        if (distance < toFinalAreaDistance[numOfShip])
                        {
                            cerr << toFinalAreaDistance[numOfShip] << endl;
                            ret[numOfShip] = startPoint;
                            ufoInfo.onShip = false;
                            inFinalArea[numOfShip] = true;
                        }
                        else
                        {
                            ret[numOfShip] = ufos[3 * ufoInfo.numOfUFO + 1];
                        }
                    }
                    else if (!inFinalArea[numOfShip])
                    {
                        ret[numOfShip] = startPointsInFinalArea[numOfShip];
                        inFinalArea[numOfShip] = true;
                    }
                    else
                    {
                        double maxScore = -1e10;
                        int numOfNextStar = -1;
                        for (auto& numOfStar : finalArea[numOfShip].stars)
                        {
                            bool usingNewStar = false;
                            if (visitedStars[numOfStar] == 0) usingNewStar = true;
                            double distance = 0;
                            distance += (allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]])*(allStars[2 * numOfStar] - allStars[2 * ships[numOfShip]]);
                            distance += (allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1])*(allStars[2 * numOfStar + 1] - allStars[2 * ships[numOfShip] + 1]);
                            sqrt(distance);
                            for (int numOfUFO = 0; numOfUFO < ufos.size() / 3; numOfUFO++)
                            {
                                if (ufos[3 * numOfUFO] == ships[numOfShip] && ufos[3 * numOfUFO + 1] == numOfStar) distance *= 0.001;
                            }
                            double score = -distance + (double)(currentTurn * currentTurn) * (usingNewStar ? 1. : 0.) * 1e6 / (maxTurn * maxTurn);
                            if (score > maxScore)
                            {
                                maxScore = score;
                                numOfNextStar = numOfStar;
                            }
                        }
                        ret[numOfShip] = numOfNextStar;
                    }
                }
            }
        }

        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        {
            if (ret[numOfShip] != ships[numOfShip] || visitedStars[ret[numOfShip]] == 0)
            {
                if (visitedStars[ret[numOfShip]] == 0) visited++;
                ++visitedStars[ret[numOfShip]];
            }
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

