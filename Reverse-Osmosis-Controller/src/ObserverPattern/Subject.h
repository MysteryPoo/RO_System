/*
  Subject
*/
#ifndef _SUBJECT_
#define _SUBJECT_

#include <list>

class IObserver;
enum MessageType : int;

class Subject
{
public:
  Subject() {};
  virtual ~Subject() {};
  virtual void Attach(IObserver* observer);
  virtual void Detach(IObserver* observer);
  virtual void Notify(const MessageType type, void* msg);
private:
  std::list<IObserver*> _observers;
};

#endif
