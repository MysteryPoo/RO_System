
public class VersionProcessor : AbstractMqttProcessor
{
  public VersionProcessor(SupabaseService supabase, MQTTService mqtt) : base(supabase, mqtt)
  {
  }

  public override async Task Process(string[] topic, string payload)
  {
    string deviceId = AbstractMqttProcessor.GetDeviceFromTopic(topic);
    await _supabase.InsertVersion(deviceId, payload);
  }
}
