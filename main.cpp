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

    ZombieGame() {score=0;}
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
                int d = dist(humans[c2], zombies[c2]);
                if(d < bestDistance)
                {
                    bestIdx = c2;
                    bestDistance = d;
                }
            }
            zombies[c] = moveTo(zombies[c],humans[bestIdx], 400);
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
                if(dist(zombies[c],humans[c2])<= 400)
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
        for(int c=0;c<humans.size();c++) {
           // if(humansDead[c]) continue;
            minDist = min(dist(player,humans[c]),minDist);
        }
        if(alive<=0) return -100000-minDist/100;

        return score*10000+alive*100-minDist/100;
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
    vector<Coord> path;
    int score;

    void apply(ZombieGame &g)
    {
        for(int c=1; c<path.size(); c++)
        {
            //g.display();
            g.oneTurn(path[c].x, path[c].y);
        }
        score = g.getScore();
        //g.display();
    }

    void adaptSolution(ZombieGame &g)
    {
        for(int c=1; c<path.size(); c++)
        {
            path[c].x = max(0,min(g.WIDTH,path[c].x));
            path[c].y = max(0,min(g.HEIGHT,path[c].y));
            path[c] = moveTo(path[c-1],path[c],1000);
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
        for(int c=1; c<path.size(); c++)
        {
            if(rand()%100 < 5)
            {
                path[c].x += rand()%100-50;
                path[c].y += rand()%100-50;
            }
        }
    }

    void crossSolution(ZombieSolution &s)
    {
        for(int c=1; c<s.path.size(); c++)
        {
            if(rand()%2 == 1)
            {
                path[c].x = s.path[c].x;
                path[c].y = s.path[c].y;

            }
        }
    }

    void mutation()
    {
        updateMutation();
    }

    void generateSolution(ZombieGame &g)
    {
        path.resize(2);
        path[0] = g.player;

        for(int c=1; c<2; c++)
        {
            path[c].x = path[c-1].x + rand()%2000-1000;
            path[c].y = path[c-1].y + rand()%2000-1000;
            path[c].x = max(0,min(g.WIDTH,path[c].x));
            path[c].y = max(0,min(g.HEIGHT,path[c].y));
        }
        adaptSolution(g);
    }
    void display() {
        for(int c=0;c<path.size();c++) {
            cerr<<path[c].x<<" "<<path[c].y<<"->";
        }
        cerr<<endl;
    }

    void operator=(ZombieSolution &s2) {
    path.resize(s2.path.size());
    copy(s2.path.begin(),s2.path.end(),path.begin());
}

};


class ZombieAG {
public:

    int sizeRes;

    int sizeCrossing;

    vector<ZombieSolution> population;
    vector<pair<int,int> > ordering;

    ZombieAG(){}

    ZombieAG(ZombieGame &g, int _sizeRes = 10, int _sizeCrossing = 4){
        sizeRes =_sizeRes;
        sizeCrossing = _sizeCrossing;
        population.resize(sizeRes);
        ordering.resize(sizeRes);
        for(int c=0;c<sizeRes;c++) {
            population[c].generateSolution(g);
            ordering[c] = make_pair(0,c);
        }
    }

    void evaluate(ZombieGame &g) {
        for(int c=0;c<population.size();c++) {
            ZombieGame gtmp(g);
            population[c].apply(gtmp);
            ordering[c].first = population[c].score;
        }
    }

    void precomputeForCrossing() {
        sort(ordering.rbegin(), ordering.rend());
    }

    void select(int &idx1, int &idx2) {
        do{
            idx1 = rand() % sizeCrossing;
            idx2 = rand() % sizeCrossing;
        }while(idx1 == idx2);
        idx1 = ordering[idx1].second;
        idx2 = ordering[idx2].second;
    }

    void crossPopulation() {
        vector<ZombieSolution> pop(population);
        for(int c=0;c<sizeRes;c++) {
            int i1, i2;
            select(i1,i2);
            population[c] = pop[i1];
            population[c].crossSolution(pop[i2]);
        }
    }

    void mutate() {
        for(int c=0;c<population.size();c++) {
            population[c].mutation();
        }
    }

    void adapt(ZombieGame &g) {
        for(int c=0;c<population.size();c++) {
            population[c].adaptSolution(g);
        }
    }

    void evolute(int steps, ZombieGame &g) {
        for(int c=0;c<steps;c++) {
            evaluate(g);
            precomputeForCrossing();
            crossPopulation();
            mutate();
            adapt(g);
        }
        evaluate(g);
        precomputeForCrossing();
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

        ZombieAG ag(g,50,5);
        ag.evolute(40,g);

        ZombieSolution zs= ag.population[ag.ordering[0].second];
        zs.display();
        cerr<<zs.score<<endl;
        cout<<zs.path[1].x<<" "<<zs.path[1].y<<endl;

        continue;
        int best = -1e9;
        Coord bestAction;
        ZombieSolution bestSolution;
        //ZombieSolution zs;
        for(int c=0; c<1000; c++)
        {
            ZombieGame g2(g);

            zs.generateSolution(g2);
            zs.apply(g2);
            //cerr<<g2.getScore()<<endl;
            if(g2.getScore() > best)
            {
                best = g2.getScore();
                bestAction = zs.path[1];
                bestSolution = zs;
            }
            //g2.display();

        }
        bestSolution.display();
        cerr<<best<<endl;
        cout<<bestAction.x<<" "<<bestAction.y<<endl;
    }


}



