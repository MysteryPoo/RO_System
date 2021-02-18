
class JHelp
{
    static String value(String value)
    {
        return "\"" + value + "\"";
    }

    static String value(int value)
    {
        return String(value);
    }

    static String value(float value)
    {
        return String(value);
    }

    static String value(bool value)
    {
        return String(value);
    }
    
public:
    template <class T>
    static String field(String key, T val)
    {
        return value(key) + ":" + value(val);
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
