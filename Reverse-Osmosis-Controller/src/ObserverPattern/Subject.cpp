#include "IObserver.h"
#include "MessageType.h"
#include "Subject.h"

void Subject::Attach(IObserver* observer)
{
  _observers.push_back(observer);
}

void Subject::Detach(IObserver* observer)
{
  _observers.remove(observer);
}

void Subject::Notify(const MessageType type, void* msg)
{
  for (IObserver* it : _observers)
  {
    it->Update(this, type, msg);
  }
}
