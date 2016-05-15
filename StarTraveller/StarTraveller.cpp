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
    vector<int> ownedGalaxies;
    vector<bool> inTerritory;
    int init(vector<int> stars)
    {
        NStar = stars.size() / 2;
        allStars = stars;
        maxTurn = 4 * NStar;
        currentTurn = 0;
        initSettings = true;
        return 0;
    }
    vector<int> makeMoves(vector<int> ufos, vector<int> ships)
    {
        currentTurn++;
        random_device seed_gen;
        mt19937 engine(seed_gen);
        uniform_int_distribution<int> distStar(0, NStar - 1);
        if (initSettings)
        {
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
            }
            inTerritory = vector<bool>(ships.size(), false);
            ownedGalaxies = vector<int>(ships.size());
            vector<bool> owned(galaxies.size(), false);
            for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
            {
                int nearGalaxy = -1;
                double minDist = 1e10;
                int numOfStar = ships[numOfShip];
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
                ownedGalaxies[numOfShip] = nearGalaxy;
                owned[nearGalaxy] = true;
                for (auto& numOfStar : galaxies[ownedGalaxies[numOfShip]].stars)
                {
                    if (numOfStar == ships[numOfShip])
                    {
                        inTerritory[numOfShip] = true;
                        break;
                    }
                }
            }
        }
        vector<int> ret(ships.size());
        for (int numOfShip = 0; numOfShip < ships.size(); numOfShip++)
        {
            ret[numOfShip] = ships[numOfShip];
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

