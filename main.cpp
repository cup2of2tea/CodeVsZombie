#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>


using namespace std;

class Coord
{
public:

    int x;
    int y;

    Coord() {}
    Coord(int _x, int _y):x(_x),y(_y) {}

};

int dist(Coord &c, Coord &c2)
{
    return (c.x-c2.x)*(c.x-c2.x)+(c.y-c2.y)*(c.y-c2.y);
}

Coord moveTo(Coord &from, Coord &to, int speed)
{
    Coord res;
    res.x = from.x;
    res.y = from.y;
    double dir = atan2(-to.y + from.y, to.x - from.x);
    if (dist(from, to) <= speed*speed)
    {
        res.x = to.x;
        res.y = to.y;
    }
    else
    {
        res.x += speed * cos(dir);
        res.y -= speed * sin(dir);
    }
    return res;
}

class Game
{
public:


};


class Solution
{
public:

    //virtual void apply(Game &g) = 0;

    //virtual void adaptSolution(Game &g) = 0;

    virtual void insertMutation() = 0;
    virtual void deleteMutation() = 0;
    virtual void updateMutation() = 0;

    //virtual void crossSolution(Solution &s) = 0;

    //virtual void generateSolution(Game &g) = 0;

};

class AG
{
    //vector<Solution> population;

public:


    //virtual void getScore(Solution &s, Game &g) = 0;
    virtual void evolute() = 0;
    virtual void select() = 0;

};

class ZombieGame : public Game
{
public:

    int alive;
    vector<Coord> humans;

    vector<bool> humansDead;


    vector<Coord> zombies;

    vector<bool> zombiesDead;


    Coord player;

    int WIDTH = 16000;
    int HEIGHT = 9000;

    int zombieSpeed = 400;
    int playerSpeed = 1000;

    int zombieKillRadius = 400;
    int playerKillRadius = 2000;


    int score;

    ZombieGame()
    {
        score=0;
    }
    ZombieGame(ZombieGame &z)
    {
        alive = z.alive;
        humans.resize(z.humans.size());
        humansDead.resize(z.humansDead.size());
        zombies.resize(z.zombies.size());
        zombiesDead.resize(z.zombiesDead.size());
        score = z.score;
        player.x = z.player.x;
        player.y = z.player.y;
        copy(z.zombies.begin(),z.zombies.end(),zombies.begin());
        copy(z.humans.begin(),z.humans.end(),humans.begin());
        copy(z.zombiesDead.begin(),z.zombiesDead.end(),zombiesDead.begin());
        copy(z.humansDead.begin(),z.humansDead.end(),humansDead.begin());

    }


    bool isEnded() {
        if(alive == 0) return true;
        for(int c=0;c<zombiesDead.size();c++) if(!zombiesDead[c]) return false;
        return true;
    }

    void moveZombies()
    {
        for(int c=0; c<zombies.size(); c++)
        {
            if(zombiesDead[c]) continue;
            int bestIdx = 0;
            int bestDistance = 1e9;
            for(int c2=0; c2<humans.size(); c2++)
            {
                if(humansDead[c2]) continue;
                int d = dist(humans[c2], zombies[c]);
                if(d < bestDistance)
                {
                    bestIdx = c2;
                    bestDistance = d;
                }
            }
            if(dist(player,zombies[c]) < bestDistance){
                zombies[c] = moveTo(zombies[c],player,400);
            } else {

                zombies[c] = moveTo(zombies[c],humans[bestIdx], 400);
            }
        }
    }

    void movePlayer(int xPlayer, int yPlayer)
    {
        Coord c(xPlayer, yPlayer);
        player = moveTo(player, c, 1000);
    }

    void killZombies()
    {
        int gauche = 1, droite = 1;
        for(int c=0; c<zombies.size(); c++)
        {
            if(zombiesDead[c])
            {
                continue;
            }
            if(dist(zombies[c],player) <= playerKillRadius*playerKillRadius)
            {
                score += alive * alive * 10 * droite;
                int tmp = droite;
                droite += gauche;
                gauche = tmp;
                zombiesDead[c] = true;
            }
        }
    }

    void killHumans()
    {
        for(int c=0; c<zombies.size(); c++)
        {
            if(zombiesDead[c]) continue;
            for(int c2=0; c2<humans.size(); c2++)
            {
                if(humansDead[c2]) continue;
                if(dist(zombies[c],humans[c2])<= 0)
                {
                    humansDead[c2] = true;
                    alive--;
                }
            }
        }
    }

    void oneTurn(int xPlayer, int yPlayer)
    {
        moveZombies();
        movePlayer(xPlayer, yPlayer);
        killZombies();
        killHumans();
    }

    int getScore()
    {
        int minDist = 1e9;
        bool somebodyOutThere = false;
        for(int c=0; c<humans.size(); c++)
        {
            if(!humansDead[c]) somebodyOutThere = true;
            if(humansDead[c] && alive > 0) continue;
            minDist = min(dist(player,humans[c]),minDist);
        }
        if(somebodyOutThere && alive <= 0) {
            cerr<<"??"<<endl;
        }
        if(alive<=0) return -100000;
        if(score < 0) cerr<<"oulalal"<<endl;
        if(alive < 0) cerr<<"oulalabis"<<endl;
        return score*100;
    }

    void display()
    {
        cerr<<"HUMANS : "<<endl;
        for(int c=0; c<humans.size(); c++)
        {
            cerr<<humans[c].x<<" "<<humans[c].y<<" "<<(humansDead[c]?"RIP":"")<<endl;
        }
        cerr<<"ZOMBIES : "<<endl;
        for(int c=0; c<zombies.size(); c++)
        {
            cerr<<zombies[c].x<<" "<<zombies[c].y<<" "<<(zombiesDead[c]?"RIP":"")<<endl;
        }
        cerr<<"PLAYER : "<<endl;
        cerr<<player.x<<" "<<player.y<<endl;
        cerr<<endl<<endl;
    }
};

class ZombieSolution: public Solution
{
public:
    vector<pair<Coord,int> > targets;
    int score;

    void apply(ZombieGame &g)
    {
        Coord act(g.player);
        for(int c=0; c<targets.size() && !g.isEnded(); c++)
        {
            Coord targetCoord = targets[c].first;
            int wait = targets[c].second;
            while(act.x != targetCoord.x || act.y != targetCoord.y)
            {
                act = moveTo(act, targetCoord, 1000);
                g.oneTurn(act.x, act.y);
            }
            while(wait--)
            {
                g.oneTurn(act.x, act.y);
            }
            //g.display();
        }

        while(!g.isEnded()) {
            g.oneTurn(act.x, act.y);
        }
        score = g.getScore();
        //g.display();

    }


    void adaptSolution(ZombieGame &g)
    {
        for(int c=0; c<targets.size(); c++)
        {
            targets[c].first.x = max(0,min(g.WIDTH,targets[c].first.x));
            targets[c].first.y = max(0,min(g.HEIGHT,targets[c].first.y));
        }
    }

    virtual void insertMutation()
    {

    }

    virtual void deleteMutation()
    {

    }

    virtual void updateMutation()
    {
        for(int c=0; c<targets.size(); c++)
        {
            if(rand()%100 < 10)
            {
                targets[c].first.x = rand()%1000-500;
                targets[c].first.y += rand()%1000-500;
            }
            if(rand()%100 < 30)
            {
                targets[c].second = rand()%5;
            }
        }
    }

    void crossSolution(ZombieSolution &s)
    {
        for(int c=0; c<s.targets.size(); c++)
        {
            if(rand()%2 == 1)
            {
                targets[c].first.x = s.targets[c].first.x;
                targets[c].first.y = s.targets[c].first.y;
            }
            if(rand()%2 == 1) {
                targets[c].second = s.targets[c].second;
            }
        }
    }

    void mutation()
    {
        updateMutation();
    }

    void generateSolution(ZombieGame &g)
    {
        targets.resize(5);
        for(int c=0; c<5; c++)
        {
            targets[c].first.x = rand()%g.WIDTH;
            targets[c].first.y = rand()%g.HEIGHT;
            targets[c].second = rand()%5;
        }
        adaptSolution(g);
    }

    void generateFromCoord(Coord &coord) {
        targets.resize(5);
        for(int c=0;c<5;c++) {
            targets[c].first.x = coord.x;
            targets[c].first.y = coord.y;
            targets[c].second = 5;
        }
    }

    void display()
    {
        for(int c=0; c<targets.size(); c++)
        {
            cerr<<targets[c].first.x<<" "<<targets[c].first.y<<"->";
        }
        cerr<<endl;
    }

    void operator=(ZombieSolution &s2)
    {
        targets.resize(s2.targets.size());
        copy(s2.targets.begin(),s2.targets.end(),targets.begin());
    }

};


class ZombieAG
{
public:

    int sizeRes;

    int sizeCrossing;

    vector<ZombieSolution> population;
    vector<pair<int,int> > ordering;

    ZombieAG() {}

    ZombieAG(ZombieGame &g, int _sizeRes = 10, int _sizeCrossing = 4)
    {
        sizeRes =_sizeRes;
        sizeCrossing = _sizeCrossing;
        population.resize(sizeRes);
        ordering.resize(sizeRes);
        population[0].generateFromCoord(g.player);
        ordering[0] = make_pair(0,0);
        for(int c=0;c<g.humans.size();c++) {
            population[c+1].generateFromCoord(g.humans[c]);
            ZombieGame g2(g);
            population[c+1].apply(g2);
            ordering[c+1] = make_pair(0,c+1);
        }
        for(int c=g.humans.size()+1; c<sizeRes; c++)
        {
            population[c].generateSolution(g);
        }

    }

    void evaluate(ZombieGame &g)
    {
        for(int c=0; c<population.size(); c++)
        {
            ZombieGame gtmp(g);
            population[c].apply(gtmp);
            if(c==1) {
                cerr<<"first "<<endl;
                gtmp.display();
                cerr<<population[c].score<<endl;
            }
            ordering[c].second = c;
            ordering[c].first = population[c].score;
        }
    }

    void precomputeForCrossing()
    {
        sort(ordering.rbegin(), ordering.rend());
    }

    void select(int &idx1, int &idx2)
    {
        do
        {
            idx1 = rand() % sizeCrossing;
            idx2 = rand() % sizeCrossing;
        }
        while(idx1 == idx2);
        idx1 = ordering[idx1].second;
        idx2 = ordering[idx2].second;
    }

    void crossPopulation()
    {
        vector<ZombieSolution> pop(population);
        for(int c=0; c<sizeRes; c++)
        {
            int i1, i2;
            select(i1,i2);
            population[c] = pop[i1];
            population[c].crossSolution(pop[i2]);
        }
    }

    void mutate()
    {
        for(int c=0; c<population.size(); c++)
        {
            population[c].mutation();
        }
    }

    void adapt(ZombieGame &g)
    {
        for(int c=0; c<population.size(); c++)
        {
            population[c].adaptSolution(g);
        }
    }

    ZombieSolution evolute(int steps, ZombieGame &g)
    {
        ZombieSolution best;
        int bestScore = -1e9;
        for(int c=0; c<steps; c++)
        {
            evaluate(g);

            precomputeForCrossing();
            if(c == 0) {
                for(int c2=0;c2<ordering.size();c2++) {
                    cerr<<ordering[c2].first<<" "<<ordering[c2].second<<endl;
                }
            }
            if(ordering[0].first > bestScore) {
                bestScore = ordering[0].first;
                best = population[ordering[0].second];
                cerr<<bestScore<<endl;
                best.display();
                ZombieGame g2(g);
                best.apply(g2);
                g2.display();
                cerr<<best.score<<endl;
            }
            crossPopulation();
            mutate();
            adapt(g);
        }
        evaluate(g);
        precomputeForCrossing();
        return best;
    }

};





int main()
{

    ZombieGame g;
    while(1)
    {
        cin>>g.player.x>>g.player.y;
        cin>>g.alive;
        g.humans.resize(g.alive);
        g.humansDead.resize(g.alive);
        int trash;
        for(int c=0; c<g.humans.size(); c++)
        {
            cin>>trash>>g.humans[c].x>>g.humans[c].y;
            g.humansDead[c]=false;
        }
        int zombies;
        cin>>zombies;
        g.zombies.resize(zombies);
        g.zombiesDead.resize(zombies);
        for(int c=0; c<g.zombies.size(); c++)
        {
            cin>>trash>>g.zombies[c].x>>g.zombies[c].y>>trash>>trash;
            g.zombiesDead[c]=false;
        }
        g.display();

        ZombieAG ag(g,20,5);
        ZombieSolution zs = ag.evolute(20,g);

        zs.display();
        zs.apply(g);
        g.display();
        cerr<<zs.score<<endl;
        cout<<zs.targets[0].first.x<<" "<<zs.targets[0].first.y<<endl;

        continue;
    }

}



