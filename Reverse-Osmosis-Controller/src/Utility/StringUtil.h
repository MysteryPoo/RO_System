#ifndef _STRINGUTIL_
#define _STRINGUTIL_

#include <vector>

namespace StringUtil
{
  static std::vector<std::string> GetTokens(std::string s, std::string delimiter)
  {
    std::vector<std::string> tokens;
    size_t pos = 0;
    std::string token;
    while ((pos = s.find(delimiter)) != std::string::npos) {
        token = s.substr(0, pos);
        tokens.push_back(token);
        s.erase(0, pos + delimiter.length());
    }
    return tokens;
  }
}

#endif
