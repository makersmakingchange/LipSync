/*
 *  Based on QueueArray.h
 *
 */

// header defining the interface of the source.
#ifndef _LSQUEUE_H
#define _LSQUEUE_H


// the definition of the queue class.
template<typename T>
class LSQueue {
  public:
    LSQueue();                  // init the queue (constructor).
    ~LSQueue();                 // clear the queue (destructor).
    void clear();
    void enqueue(const T i);       // add an item to the queue.
    T dequeue();                   // remove an item from the queue.
    void push(const T i);          // push an item to the queue.
    void update(const T i);          // update last item in the queue.    
    T pop();                       // pop an item from the queue.
    T front() const;               // get the front of the queue.
    T end() const;                // get the end of the queue.
    T peek (const int i) const;    // get an item from the queue.
    T peekFromEnd (const int i) const; // get an item from the queue.
    bool isEmpty () const;         // check if the queue is empty.
    int count () const;            // get the number of items in the queue.
    bool isFull () const;          // check if the queue is full.
    void setPrinter (Print & p);   // set the printer of the queue.
  private:
    void resize (const int s);              // resize the size of the queue.
    void exit (const char * m) const;       // exit report method in case of error.
    void blink () const;                    // led blinking method in case of error.
    static const int initialSize = 2;       // the initial size of the queue.
    Print * printer;                        // the printer of the queue.
    T * contents;                           // the array of the queue.
    int size;                               // the size of the queue.
    int items;                              // the number of items of the queue.
    int head;                               // the head of the queue.
    int tail;                               // the tail of the queue.
};

// init the queue (constructor).
template<typename T>
LSQueue<T>::LSQueue () {
  size = 0;       // set the size of queue to zero.
  items = 0;      // set the number of items of queue to zero.
  head = 0;       // set the head of the queue to zero.
  tail = 0;       // set the tail of the queue to zero.
  printer = NULL; // set the printer of queue to point nowhere.

  // allocate enough memory for the array.
  contents = (T *) malloc (sizeof (T) * initialSize);

  // if there is a memory allocation error.
  if (contents == NULL)
    exit ("QUEUE: insufficient memory to initialize queue.");

  // set the initial size of the queue.
  size = initialSize;
}

// clear the queue (destructor).
template<typename T>
LSQueue<T>::~LSQueue () {
  clear();
}

// add an item to the queue.
template<typename T>
void LSQueue<T>::clear () {
  free (contents); // deallocate the array of the queue.

  contents = NULL; // set queue's array pointer to nowhere.
  printer = NULL;  // set the printer of queue to point nowhere.

  size = 0;        // set the size of queue to zero.
  items = 0;       // set the number of items of queue to zero.

  head = 0;        // set the head of the queue to zero.
  tail = 0;        // set the tail of the queue to zero. 
}
// clear the queue.

template<typename T>
void LSQueue<T>::resize (const int s) {
  // defensive issue.
  if (s <= 0)
    exit ("QUEUE: error due to undesirable size for queue size.");

  // allocate enough memory for the temporary array.
  T * temp = (T *) malloc (sizeof (T) * s);

  // if there is a memory allocation error.
  if (temp == NULL)
    exit ("QUEUE: insufficient memory to initialize temporary queue.");
  
  // copy the items from the old queue to the new one.
  for (int i = 0; i < items; i++)
    temp[i] = contents[(head + i) % size];

  // deallocate the old array of the queue.
  free (contents);

  // copy the pointer of the new queue.
  contents = temp;

  // set the head and tail of the new queue.
  head = 0; tail = items;

  // set the new size of the queue.
  size = s;
}

// add an item to the queue.
template<typename T>
void LSQueue<T>::enqueue (const T i) {
  // check if the queue is full.
  if (isFull ())
    // double size of array.
    resize (size * 2);

  // store the item to the array.
  contents[tail++] = i;
  
  // wrap-around index.
  if (tail == size) tail = 0;

  // increase the items.
  items++;
}

// push an item to the queue.
template<typename T>
void LSQueue<T>::push (const T i) {
  enqueue(i);
}

// update item in the queue.
template<typename T>
void LSQueue<T>::update (const T i) {

  // store the item to the array.
  contents[head+items-1] = i;

}

// remove an item from the queue.
template<typename T>
T LSQueue<T>::dequeue () {
  // check if the queue is empty.
  if (isEmpty ())
    exit ("QUEUE: can't pop item from queue: queue is empty.");

  // fetch the item from the array.
  T item = contents[head++];

  // decrease the items.
  items--;

  // wrap-around index.
  if (head == size) head = 0;

  // shrink size of array if necessary.
  if (!isEmpty () && (items <= size / 4))
    resize (size / 2);

  // return the item from the array.
  return item;
}

// pop an item from the queue.
template<typename T>
T LSQueue<T>::pop () {
  return dequeue();
}

// get the front of the queue.
template<typename T>
T LSQueue<T>::front () const {
  // check if the queue is empty.
  if (isEmpty ())
    exit ("QUEUE: can't get the front item of queue: queue is empty.");
    
  // get the item from the array.
  return contents[head];
}

// get the end of the queue.
template<typename T>
T LSQueue<T>::end () const {
  // check if the queue is empty.
  if (isEmpty ())
    exit ("QUEUE: can't get the front item of queue: queue is empty.");
    
  // get the item from the array.
  return contents[head+items-1];
}

// get an item from the queue.
template<typename T>
T LSQueue<T>::peek (const int i) const {
  // check if the queue is empty.
  if (isEmpty ())
    exit ("QUEUE: can't get the front item of queue: queue is empty.");
  if (i>size)
    exit ("QUEUE: out of range");
  return contents[head+i];
}

// get an item from the queue.
template<typename T>
T LSQueue<T>::peekFromEnd (const int i) const {
  // check if the queue is empty.
  if (isEmpty ())
    exit ("QUEUE: can't get the front item of queue: queue is empty.");
  int index = head+items-1-i;
  if (index<0)
    exit ("QUEUE: out of range");
    
  return contents[index];
}
// check if the queue is empty.
template<typename T>
bool LSQueue<T>::isEmpty () const {
  return items == 0;
}

// check if the queue is full.
template<typename T>
bool LSQueue<T>::isFull () const {
  return items == size;
}

// get the number of items in the queue.
template<typename T>
int LSQueue<T>::count () const {
  return items;
}

// set the printer of the queue.
template<typename T>
void LSQueue<T>::setPrinter (Print & p) {
  printer = &p;
}

// exit report method in case of error.
template<typename T>
void LSQueue<T>::exit (const char * m) const {
  // print the message if there is a printer.
  if (printer)
    printer->println (m);

}



#endif // _LSQUEUE_H
