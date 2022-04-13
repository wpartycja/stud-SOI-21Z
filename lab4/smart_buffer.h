#ifndef SMART_BUFFER_H
#define SMART_BUFFER_H

#include <iostream>
#include "monitor.h"
#include <queue>
#include <string>

using namespace std;

#define BUFFER_SIZE 2


class SmartBuffer : public Monitor{
    private:
        queue<int> buffer;
        string name;
        Condition empty;
        Condition full;

    public:
        SmartBuffer(const string &name);

        void produce(int item);
        void consume();
};

#endif