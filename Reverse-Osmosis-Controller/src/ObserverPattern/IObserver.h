/*
  Observer
*/
#ifndef _OBSERVER_
#define _OBSERVER_

class Subject;
enum MessageType : int;

class IObserver
{
public:
  IObserver() {};
  virtual ~IObserver() {};
  virtual void Update(Subject* subject) = 0;
  virtual void Update(Subject* subject, MessageType type, void* msg) {};
};

#endif
