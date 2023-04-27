/*
  Subject
*/
#ifndef _SUBJECT_
#define _SUBJECT_

#include <list>

class IObserver;

class Subject
{
public:
  Subject() {};
  virtual ~Subject() {};
  virtual void Attach(IObserver* observer);
  virtual void Detach(IObserver* observer);
  virtual void Notify();
private:
  std::list<IObserver*> _observers;
};

#endif
