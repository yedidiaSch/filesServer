#include "subject.h"
#include <algorithm>

void Subject::attach(IObserver* observer) 
{
    observers.push_back(observer);
}

void Subject::detach(IObserver* observer) 
{
    observers.erase(std::remove(observers.begin(), observers.end(), observer), observers.end());
}

void Subject::notify(void* params) 
{
    for (IObserver* observer : observers) 
    {
        observer->update(params);
    }
}
