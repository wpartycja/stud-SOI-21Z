#include "smart_buffer.h"
#include <random>

SmartBuffer::SmartBuffer(const string &name) : name(name), empty(Condition()),full(Condition()) {};

void SmartBuffer::produce(int item){
    enter();

    // wait if it's empty
    if (buffer.size() == BUFFER_SIZE){
        wait(full);
    }

    buffer.push(item);
    cout<<"Produced to buffer " << name << ": "<< item << "; Buffer size: " << buffer.size() << endl;
    signal(empty);

    leave();
}

void SmartBuffer::consume(){
    enter();

    // wait if it's full
    if (buffer.size() == 0){
        wait(empty);
    }

    int item = buffer.front();
    buffer.pop();
    cout<<"Got by consument " << name << ": "<< item << "; Buffer size: " << buffer.size() << endl;
    signal(full);

    leave();
}
