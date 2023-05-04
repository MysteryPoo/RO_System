
public abstract class AbstractMqttProcessor {
  protected SupabaseService _supabase;
  protected MQTTService _mqtt;
  public AbstractMqttProcessor(SupabaseService supabase, MQTTService mqtt)
  {
    _supabase = supabase;
    _mqtt = mqtt;
  }
  public abstract Task Process(string[] topic, string payload);

  static protected string GetDeviceFromTopic(string[] topic)
  {
    return topic[1];
  }
}