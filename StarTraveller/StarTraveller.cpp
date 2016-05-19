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
    UFOInfo(): numOfUFO(-1),totalDistance(0),count(0){};
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
    vector<int> visitedStars;
    vector<UFOInfo> ownedUFO;
    vector<bool> inTerritory;
    vector<UFOInfo> UFOInfos;
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
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++) ret[numOfShip] = ships[numOfShip];
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
            sort(UFOInfos.begin(),UFOInfos.end(),[](UFOInfo info0,UFOInfo info1)
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
            ownedUFO = vector<UFOInfo>(ships.size());
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
                    ownedUFO[nearShip] = UFOInfos[numOfUFOInfo];
                    owing[nearShip] = true;
                    for (auto& numOfStar : ownedUFO[nearShip].territory.stars)
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
                cerr << "Ship:" << numOfShip << "owns UFO:" << ownedUFO[numOfShip].numOfUFO << endl;
            }
        }
        else if (currentTurn * 4 <= maxTurn * 3)
        {
            if(currentTurn == 1) cerr << "Run" << endl;
            for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
            {
                auto& info = ownedUFO[numOfShip];
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
                            if (distance < 10)
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

