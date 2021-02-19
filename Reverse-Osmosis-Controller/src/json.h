#ifndef _JSON_
#define _JSON_
class JHelp
{
    static String value(String value, bool raw = false)
    {
        if(raw)
        {
            return value;
        }
        else
        {
            return "\"" + value + "\"";
        }
    }

    static String value(const char* val, bool raw = false)
    {
        return value(String(val), raw);
    }

    static String value(int value, bool raw = false)
    {
        return String(value);
    }

    static String value(long value, bool raw = false)
    {
        return String(value);
    }

    static String value(float value, bool raw = false)
    {
        return String(value);
    }

    static String value(bool value, bool raw = false)
    {
        return String(value ? "true" : "false");
    }
    
public:
    template <class T>
    static String field(String key, T val, bool raw = false)
    {
        return value(key) + ":" + value(val, raw);
    }

    static String begin()
    {
        return "{";
    }

    static String next()
    {
        return ",";
    }

    static String end()
    {
        return "}";
    }
};
#endif
