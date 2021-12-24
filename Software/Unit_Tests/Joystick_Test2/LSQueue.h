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
    LSQueue(int size);                 // init the queue (constructor).
    ~LSQueue();                        // clear the queue (destructor).
    void clear();
    void enqueue(const T i);           // add an item to the queue.
    T dequeue();                       // remove an item from the queue.
    void push(const T i);              // push an item to the queue.
    void update(const T i);            // update last item in the queue.    
    T pop();                           // pop an item from the queue.
    T front() const;                   // get the front of the queue.
    T end() const;                     // get the end of the queue.
    T peek (const int i) const;        // get an item from front of the queue.
    T peekFromEnd (const int i) const; // get an item from the end of the queue.
    bool isEmpty () const;             // check if the queue is empty.
    int count () const;                // get the number of items in the queue.
    bool isFull () const;              // check if the queue is full.
    void setPrinter (Print & p);       // set the printer of the queue.
  private:
    void exit (const char * m) const;  // exit report method in case of error.
    Print * printer;                   // the printer of the queue.
    T * contents;                      // the array of the queue.
    int _size;                         // the size of the queue.
    int items;                         // the number of items of the queue.
    int head;                          // the head of the queue.
    int tail;                          // the tail of the queue.
};

// init the queue (constructor).
template<typename T>
LSQueue<T>::LSQueue (int size) {
  _size = size;           // set the initial size of the queue.
  items = 0;              // set the number of items of queue to zero.
  head = 0;               // set the head of the queue to zero.
  tail = 0;               // set the tail of the queue to zero.
  printer = NULL;         // set the printer of queue to point nowhere.

  // allocate enough memory for the array.
  contents = (T *) malloc (sizeof (T) * size);

  // if there is a memory allocation error.
  if (contents == NULL)
    exit ("QUEUE: insufficient memory to initialize queue.");

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

  _size = 0;       // set the size of queue to zero.
  items = 0;       // set the number of items of queue to zero.

  head = 0;        // set the head of the queue to zero.
  tail = 0;        // set the tail of the queue to zero. 
}
// clear the queue.

// add an item to the queue.
template<typename T>
void LSQueue<T>::enqueue (const T i) {
  // check if the queue is full.
  if (isFull ()){
    memmove( contents, &contents[1], (_size-1) * sizeof(contents[0]));
    contents[_size-1] = i;
  }else {
   // store the item to the array.
  contents[tail++] = i;
  
  // wrap-around index.
  //if (tail == _size) tail = 0;

  // increase the items.
  items++;   
  }


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
  contents[tail] = i;

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
  if (head == _size) head = 0;

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
  return contents[tail-1];
}

// get an item from the front of the queue.
template<typename T>
T LSQueue<T>::peek (const int i) const {
  // check if the queue is empty.
  if (isEmpty ())
    exit ("QUEUE: can't get the front item of queue: queue is empty.");
  if (i>_size)
    exit ("QUEUE: out of range");
  return contents[head+i];
}

// get an item from the end of the queue.
template<typename T>
T LSQueue<T>::peekFromEnd (const int i) const {
  // check if the queue is empty.
  if (isEmpty ())
    exit ("QUEUE: can't get the front item of queue: queue is empty.");
  int index = tail-1-i;
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
  return items == _size;
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
