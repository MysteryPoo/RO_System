using System.Text.Json;

public class LogProcessor : AbstractMqttProcessor
{
  public LogProcessor(SupabaseService supabase, MQTTService mqtt) : base(supabase, mqtt)
  {
  }

  public override async Task Process(string[] topic, string payload)
  {
    string deviceId = AbstractMqttProcessor.GetDeviceFromTopic(topic);
    string severity = topic[3];
    try
    {
      var json = JsonSerializer.Deserialize<LogJson>(payload);
      await _supabase.InsertLog(deviceId, severity, json!);
    }
    catch (Exception e)
    {
      Console.WriteLine(e.Message);
    }
  }
}
