#ifndef Scores_H
#define Scores_H

#include <vector>
#include <stdio.h>

class GameData;
class Entry;

using namespace std;

class Scores {
public:
    Scores(GameData* GD);
    ~Scores();
    
    void LoadScores();
    void SaveScores();
    void Clear();
    void Sort();
    void AddEntry(char* nick,int pts);
    void AddPoints(char* nick,int pts);
    int FindEntry(char* nick);
    
    vector <Entry*> List;
    
    GameData* Gamedata;
};

class Entry {
public:
    Entry(char* Name, int startpoints);
    ~Entry();
    char* name;
    int score;
};
#endif


