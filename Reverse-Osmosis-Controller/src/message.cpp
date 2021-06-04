
#include "message.h"
#include "system-log.h"
#include "json.h"

Message::Message()
    : component("undefined")
    , datetime(0)
    , data("\"undefined\"")
    {}

String Message::toJSON()
{
    String json = JHelp::begin();
    json += JHelp::field("component", this->component);
    json += JHelp::next();
    if(this->datetime != 0l)
    {
        json += JHelp::field("datetime", this->datetime);
    }
    else
    {
        json += JHelp::field("datetime", "undefined");
    }
    json += JHelp::next();
    json += JHelp::field("data", this->data, true);
    json += JHelp::end();

    return json;
}
