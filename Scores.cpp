#include "Scores.h"
#include "GameData.h"

Scores::Scores(GameData* GD)
{
    Gamedata=GD;
    return;
}

Scores::~Scores()
{
    SaveScores();
    for (int i=0; i < List.size();++i)
        delete List[i];
    List.clear();
    return;
}


void Scores::LoadScores()
{
    for (int i=0; i < List.size();++i)
        delete List[i];
    List.clear();
    
    FILE *input = fopen( "HighScores", "rb" );
    if (input == NULL)
        return;
    char *name; int score,len;
    int count=0;
    fread(&count,sizeof(int),1,input);
    for (int i=0;i < count;++i) {
        fread(&len,sizeof(int),1,input);
        fread(&score,sizeof(int),1,input);
        name=new char[len];
        fread(name,sizeof(char),len,input);
        AddEntry(name,score);
        delete [] name;
    }
    fclose(input);
    return;
}

void Scores::SaveScores()
{
    int size=List.size();
    if (size == 0)
        return;
        
    FILE *output=fopen("HighScores", "wb");
    int len;
    if (output == NULL) {
        Error("Can't write HighScores!");
        return;
    }
    fwrite(&size,sizeof(int),1,output);
    for (int i=0; i < size;++i) {
        len=strlen(List[i]->name)+1;
        fwrite(&len,sizeof(int),1,output);
        fwrite(&List[i]->score,sizeof(int),1,output);
        fwrite(List[i]->name,sizeof(char),len,output);
    }
    fclose(output);
    return;
}

void Scores::Clear()
{
    for (int i=0; i < List.size();++i)
        delete List[i];
    List.clear();
    return;
}

void Scores::Sort()
{
    int n=List.size();
    Entry* temp;
    for (int i=0; i<n-1; i++) 
        for (int j=0; j<n-1-i; j++) 
            if (List[j+1]->score > List[j]->score) {  
                temp = List[j];         
                List[j] = List[j+1];
                List[j+1] = temp;
            }
    return;
}

void Scores::AddEntry(char* nick,int pts)
{
    Entry* temp=new Entry(nick,pts);
    List.insert(List.end(),temp);
    return;
}

void Scores::AddPoints(char* nick,int pts)
{
    int loc=FindEntry(nick);
    if (loc == -1)
        AddEntry(nick,pts);
    else
        List[loc]->score=List[loc]->score+pts;
    Sort();
    return;
}

int Scores::FindEntry(char* nick)
{
    int size=List.size();
    int loc=-1;
    for (int i=0; i < size;++i)
        if (strcmp(nick,List[i]->name) == 0)
            loc=i;
    return loc;
}

Entry::Entry(char* Name, int startpoints)
{
    score=startpoints;
    name=new char[strlen(Name)+1];
    strcpy(name,Name);
    return;
}

Entry::~Entry()
{
    if (name != NULL)
        delete [] name;
    return;
}

