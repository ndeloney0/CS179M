#ifndef HELPER_H
#define HELPER_H
#include <queue>
#include <fstream>

#include <QObject>

#include "ship.h"

class helper : public QObject {
    Q_OBJECT
    public:
        helper() = default;

        bool isGoalState(Ship*);

        vector<string> outputGoalSteps(Ship*);

        Ship* loadManifest(string);

        void emptyQueue(queue<Ship*> &);

        queue<Ship*> sortQueue(queue<Ship*>);

        Ship* balanceAlgorithm(queue<Ship*> &);

        bool unloadAndLoadAlgorithm(vector<pair<int,int>>, Ship*, vector<Container*> c);

        vector<vector<Container*>> initializeVec();

        vector<vector<Container*>> intializeBuf();

        void moveToBuffer(vector<vector<Container*>>&, Container*);
        void outputBuffer(vector<vector<Container*>>);

        pair<int,int> bufferEmpty(vector<vector<Container*>>);
        void removeFromBuffer(vector<vector<Container*>>&, pair<int,int> idx);
        int availableColumn(vector<pair<int,int>>);
        bool compare(Ship* a, Ship* b);

        void buf_print(vector<vector<Container*>>);
        pair<int, int> first_buffer_loc(vector<vector<Container*>>);
        vector<Container*> sort_buf(vector<Container*>);

        vector<string> SIFT(Ship *);
        vector<int> estimated_time_SIFT(Ship*);
        vector<int> estimatedTime(Ship* , vector<Container*> );
        vector<int> getTimes(){return times;}

        void updateManifest(Ship* tmp, string fileName);
//        Ship* searchAlgorithm(queue<Ship*> &);
        Ship *getGoal() {return goal;}
        void balance(Ship *problem);
        vector<string> getMoves() { return moves; }

    signals:
        void balanceFinished(bool success);
        void containersFound(int c);
        void loadAndUnloadFinished(bool success);

    private:
        map<double, vector<vector<Container*>>> duplicate;
        unsigned long qSize;
        vector<string> moves;
        vector<int> times;
        Ship *goal;
};

#endif // HELPER_H
