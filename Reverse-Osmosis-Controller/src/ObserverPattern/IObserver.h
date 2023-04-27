/*
  Observer
*/
#ifndef _OBSERVER_
#define _OBSERVER_

class Subject;

class IObserver
{
public:
  IObserver() {};
  virtual ~IObserver() {};
  virtual void Update(Subject* subject) = 0;
};

#endif
