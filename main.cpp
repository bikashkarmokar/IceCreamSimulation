#include <iostream>
#include <vector>
#include <queue>
//#include <ctime>
#include <cstdlib>

//#include <stdlib.h>     /* srand, rand */
#include <time.h>       /* time */

class Simulator;

int highestWaitingTime=0;
int waitingTime=0;


using namespace std;


int randomNumber(int min, int max){
    int n = max - min + 1;
    int remainder = RAND_MAX % n;
    int x;
    do{
        x = rand();
    }while (x >= RAND_MAX - remainder);
    return min + x % n;
}




class Event
{
public:
    double time;
    bool LessThan(Event* e)
    {
        return this-> time < e->time;
    }
    virtual void Execute(Simulator* sim)=0;

};


///EventList represents the event list for the DES
class EventList
{
public:
    ///vector of events
    vector<Event*> event_list;
    ///insert an event to the list
    void Insert(Event* e)
    {
        int i=0;
        while(i<event_list.size() && event_list.at(i)->LessThan(e))
        {
            i++;
        }
        event_list.insert(event_list.begin()+i,e);

    }

    ///Get the first event to be executed from the list
    Event* RemoveFirst()
    {
        Event* e = event_list.front();
        event_list.erase(event_list.begin());
        return e;
    }

    int Size()
    {
        return event_list.size();
    }
};


class Entity
{
public:
    double time_generated;
    double time_serviced;
    double time_done;
    int identification;

    ///-----------------------------------------------bks
    int numberOfIceCream;

    ///-----------------------------------------------bks

    Entity(double time, int id, int iceNum)
    {
        identification = id;
        time_generated = time;

        ///-----------------------------
            ///add ice cream number
            numberOfIceCream = iceNum;

        ///-----------------------------
    }

};

class Simulator
{
public:
    double time;
    double end_time;
    vector<Entity*> entity_list;
    vector<queue<Entity*>*> queue_list;

    double Now()
    {
        return time;
    }

    EventList* events;

    void Insert(Event* e)
    {
        events->Insert(e);
    }

    Event* Cancel(Event* e)
    {

    }

    void DoAllEvents()
    {
        Event* e;
        while(events->Size()>0 && time<end_time)
        {
            e = events->RemoveFirst();
            time = e->time;


            e->Execute(this);
        }
    }
};







class Generating : public Event
{
public:
    int number_of_generated_entities;

    Generating(double time)
    {
        number_of_generated_entities = 0;
        this->time = time;

    }
    void Execute(Simulator* sim);
};



class SingleServerQueueSimulator : public Simulator
{
public:
    SingleServerQueueSimulator()
    {
        time = 0.0;
        end_time = 540;
        queue<Entity*>*q = new queue<Entity*>();
        queue_list.push_back(q);
        ///initialize random seed:
        //srand(0);
    }
    void Start()
    {
        events = new EventList();
        Insert(new Generating(0.0));
        DoAllEvents();
    }
};

class Serving : public Event
{
public:
    Serving(double time)
    {
        this->time=time;
        //srand(0);
    }
    void Execute(Simulator *sim);
};

class Done : public Event
{
public:
    Done(double time)
    {
        this->time = time;
    }
    void Execute(Simulator* sim);
};




void Generating :: Execute(Simulator* sim)
{
    number_of_generated_entities++;
    ///----------------------------------------------------------------------
    int iceNum;

    if(time < 120)
    {
         iceNum =  randomNumber(1,3); ///between 1 and 3 scoops//bks
    }else if(time>=120 and time < 240)
    {
         iceNum =  randomNumber(3,5); ///between 3 and 5 scoops//bks
    }else if(time>=240 and time <= 540)
    {
         iceNum =  randomNumber(1,4); ///between 1 and 2 scoops//bks
    }
    ///----------------------------------------------------------------------

    Entity* ent = new Entity(time,number_of_generated_entities,iceNum);///added 3rd parameter for ice cream number

    sim->entity_list.push_back(ent);

    cout<<"Entity "<<number_of_generated_entities<<" is generated at time " <<time<<endl;
    //cout<<"Entity "<<number_of_generated_entities<<" is generated at time " <<time<<", Ice Cream Number:"<<iceNum<<endl;

    ///add the entity to the list
    sim->queue_list.at(0)->push(ent);
    ///if there is no other element in the queue, start a serve event
    if(sim->queue_list.at(0)->size()==1)
    {
        Serving* s = new Serving(time);
        sim->Insert(s);
    }

    //------------------------------------------------------
    ///next generation of new entity
    //double next_event_time =  rand()%15+10;///between 5 and 55 seconds //rand() % 3 + 1;///between 1 and 3 minutes//bks
    //double next_event_time =  randomNumber(2,10); ///between 2 and 5 minutes//bks
    double next_event_time;

    if(time <120)
    {
         next_event_time =  randomNumber(1,3); ///between 1 and 3 minutes//bks
    }else if(time>=120 and time < 360)
    {
         next_event_time =  randomNumber(2,10); ///between 2 and 10 minutes//bks
    }else if(time>=360 and time < 480)
    {
         next_event_time =  randomNumber(1,2); ///between 1 and 2 minutes//bks
    }else if(time>=480 and time <540)
    {
         next_event_time =  randomNumber(2,5); ///between 2 and 5 minutes//bks
    }

    //------------------------------------------------------

    time+=next_event_time;
    sim->Insert(this);
}



void Serving::Execute(Simulator *sim)
{
    Entity* ent = sim->queue_list.at(0)->front();
    ent->time_serviced = time;
    //cout<<"Entity "<<ent->identification<<" is being served at time "<<time<<endl;
    cout<<"Entity "<<ent->identification<<" is being served at time "<<time<<", He want "<<ent->numberOfIceCream <<" Ice Cream"<<endl;

    //-------------------------------------------------
    ///new event is scheduled in between 5 and 55 seconds
    //double next_event_time = rand()%15+10;
    double next_event_time =  0.5*ent->numberOfIceCream; ///time depends on ice cream number

    ///multiplied .5(30 seconds) with number of ice cream for serviced time


    ///waiting time - have to find here or in done section
    ///----------------------------------------------------
    double wait = ent->time_serviced - ent->time_generated;

    if(wait>highestWaitingTime)
    {
        //cout<<"waiting: "<<wait<<endl;
        //cout<<"waiting: "<<ent->identification<<endl;
        //int c = (11+((int)(ent->time_generated)/60))>12?(11+((int)(ent->time_generated)/60))-12:(11+((int)(ent->time_generated)/60));
        //cout<<"waiting during: "<<c <<" : "<<(int)(ent->time_generated)%60<<endl;
        highestWaitingTime = wait;
        waitingTime = ent->time_generated;

    }else if(wait==highestWaitingTime)
    {
        ///have to store in global array
    }
    ///----------------------------------------------------

    time+=next_event_time;
    Done* d = new Done(time);
    sim->Insert(d);
}

void Done::Execute(Simulator* sim)
{
    Entity* ent = sim->queue_list.at(0)->front();
    sim->queue_list.at(0)->pop();
    ent->time_done = time;
    cout<<"Entity "<<ent->identification<<" is being done at time "<<time<<endl;
    ///new event is scheduled in between 5 and 55 seconds
    if(sim->queue_list.at(0)->size()>0)
    {
        Serving *s = new Serving(time);
        sim->Insert(s);
    }
}



int main()
{
    srand(time(NULL));

    SingleServerQueueSimulator ssq;
    ssq.Start();

    int c = (11+((int)(waitingTime)/60))>12?(11+((int)(waitingTime)/60))-12:(11+((int)(waitingTime)/60));
    cout<<endl<<"Part time employee Needed during "<<c <<":"<<(int)(waitingTime)%60<<" because in this time long waiting of "<<highestWaitingTime<<" minutes occurs"<<endl;


    return 0;
}
