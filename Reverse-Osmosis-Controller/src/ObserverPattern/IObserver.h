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
  virtual void Update(const Subject* subject, const MessageType type, void* msg) const = 0;
};

#endif
