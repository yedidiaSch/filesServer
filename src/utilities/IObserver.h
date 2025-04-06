#ifndef IOBSERVER_H
#define IOBSERVER_H

/**
 * @brief IObserver interface that requires an update method to be implemented.
 */
class IObserver {
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~IObserver() = default;

    /**
     * @brief Pure virtual method to be implemented by concrete observers.
     * @param params Pointer to the parameters to be passed to the observer.
     */
    virtual void update(void* params) = 0;
};

#endif // IOBSERVER_H
