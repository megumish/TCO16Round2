#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <vector>
#include <iomanip>

using namespace std;

class StarTraveller {
public:
    int NStars;
    vector<int> used;
    vector<int> stars;
    double totalEnergy;
    int init(vector<int> stars)
    {
        NStars = stars.size()/2;
        used.resize(NStars, 0);
        this->stars = stars;
        totalEnergy = 0;
        return 0;
    }
    vector<int> makeMoves(vector<int> ufos, vector<int> ships)
    {
        vector<int> ret;
        for (int i=0;i<NStars;i++)
            if (!used[i])
            {
                used[i] = 1;
                ret.push_back(i);
                if (ret.size()==ships.size()) break;
            }
        // Make sure the return is filled with valid moves for the final move.
        while (ret.size()<ships.size())
        {
            ret.push_back((ships[ret.size()]+1)%NStars);
        }
        double energy = 0;
        for (int shipNum = 0; shipNum < ret.size(); shipNum++)
        {
            double distance = 0;
            distance += (stars[2*ret[shipNum]] - stars[2*ships[shipNum]])*(stars[2 * ret[shipNum]] - stars[2 * ships[shipNum]]);
            distance += (stars[2 * ret[shipNum] + 1] - stars[2 * ships[shipNum] + 1])*(stars[2 * ret[shipNum] + 1] - stars[2 * ships[shipNum] + 1]);
            distance = sqrt(distance);
            for (int ufoNum = 0; ufoNum < ufos.size() / 3; ufoNum++)
            {
                if (ufos[3 * ufoNum] == ships[shipNum] && ufos[3 * ufoNum + 1] == ret[shipNum]) distance *= 0.001;
            }
            energy += distance;
        }
        totalEnergy += energy;
        cerr << setprecision(13) << fixed << energy << endl;
        cerr << setprecision(13) << fixed << "total:" << totalEnergy << endl;
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

