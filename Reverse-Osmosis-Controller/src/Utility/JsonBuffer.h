
#ifndef _JSONBUFFER_
#define _JSONBUFFER_

#include "spark_wiring_json.h"

using namespace spark;

namespace JsonBuffer
{
  inline JSONBufferWriter createBuffer(int size)
  {
    char* buffer = new char[size];
    memset(buffer, 0, size * sizeof(char));
    return JSONBufferWriter(buffer, size * sizeof(char));
  }
};

#endif
