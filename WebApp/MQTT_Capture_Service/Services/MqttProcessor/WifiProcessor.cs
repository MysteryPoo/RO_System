
using System.Text.Json;

public class WifiProcessor : AbstractMqttProcessor
{
  public WifiProcessor(SupabaseService supabase, MQTTService mqtt) : base(supabase, mqtt)
  {
  }

  public override async Task Process(string[] topic, string payload)
  {
    string deviceId = AbstractMqttProcessor.GetDeviceFromTopic(topic);
    switch(topic[3])
    {
      case "signal": {
        var json = JsonSerializer.Deserialize<ReporterValue<float>>(payload);
        if (json is null) throw new Exception("Unable to deserialize Wifi Signal.");
        await _supabase.InsertWifiSignal(deviceId, json);
        break;
      }
      case "quality": {
        var json = JsonSerializer.Deserialize<ReporterValue<float>>(payload);
        if (json is null) throw new Exception("Unable to deserialize Wifi Signal.");
        await _supabase.InsertWifiQuality(deviceId, json);
        break;
      }
      default:
        await _mqtt.ProcessUnknownMessage(topic, payload);
        break;
    }
  }
}
