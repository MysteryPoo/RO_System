using System.Text.Json;

public class BarometerProcesser : AbstractMqttProcessor
{
  public BarometerProcesser(SupabaseService supabase, MQTTService mqtt) : base(supabase, mqtt)
  {
  }

  public override async Task Process(string[] topic, string payload)
  {
    string deviceId = AbstractMqttProcessor.GetDeviceFromTopic(topic);
    switch(topic[3])
    {
      case "configuration": {
        await ProcessConfiguration(topic, payload);
        break;
      }
      case "pressure": {
        var json = JsonSerializer.Deserialize<ReporterValue<int>>(payload);
        await _supabase.InsertBarometerPressure(deviceId, json!);
        break;
      }
    }
    throw new NotImplementedException();
  }

  private async Task ProcessConfiguration(string[] topic, string payload)
  {
    string deviceId = AbstractMqttProcessor.GetDeviceFromTopic(topic);
    await _supabase.ProcessConfiguration(deviceId, topic[2], payload);
  }
}
