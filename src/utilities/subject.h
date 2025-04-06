#ifndef SUBJECT_H
#define SUBJECT_H

#include <vector>
#include "IObserver.h"

/**
 * @brief Subject class that maintains a list of observers and notifies them of changes.
 */
class Subject 
{

public:

    /**
     * @brief Attach an observer to the subject.
     * @param observer Pointer to the observer to be attached.
     */
    void attach(IObserver* observer);

    /**
     * @brief Detach an observer from the subject.
     * @param observer Pointer to the observer to be detached.
     */
    void detach(IObserver* observer);

    /**
     * @brief Notify all attached observers of a change.
     * @param params Pointer to the parameters to be passed to the observers.
     */
    void notify(void* params = nullptr);

private:

    std::vector<IObserver*> observers; ///< List of observers
};

#endif // SUBJECT_H
