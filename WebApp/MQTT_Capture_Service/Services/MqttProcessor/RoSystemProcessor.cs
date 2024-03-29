
using System.Text.Json;

public class RoSystemProcessor : AbstractMqttProcessor
{
  public RoSystemProcessor(SupabaseService supabase, MQTTService mqtt) : base(supabase, mqtt)
  {

  }

  public override async Task Process(string[] topic, string payload)
  {
    string deviceId = AbstractMqttProcessor.GetDeviceFromTopic(topic);
    switch(topic[3]) {
      case "configuration": {
        await ProcessConfiguration(topic, payload);
        break;
      }
      case "state-request": {
        await ProcessStateRequest(topic, payload);
        break;
      }
      case "enabled": {
        var json = JsonSerializer.Deserialize<ReporterValue<bool>>(payload);
        if (json is not null)
        {
          await _supabase.InsertRoSystemEnabled(deviceId, json);
        }
        break;
      }
      case "flushed": {
        var json = JsonSerializer.Deserialize<ReporterValue<bool>>(payload);
        if (json is not null)
        {
          await _supabase.InsertRoSystemFlushed(deviceId, json);
        }
        break;
      }
      case "pump-cooldown": {
        break;
      }
      case "flush-duration": {
        break;
      }
      default:
        await _mqtt.ProcessUnknownMessage(topic, payload);
        break;
    }
  }

  private async Task ProcessConfiguration(string[] topic, string payload)
  {
    string deviceId = AbstractMqttProcessor.GetDeviceFromTopic(topic);
    await _supabase.ProcessConfiguration(deviceId, topic[2], payload);
  }

  private async Task ProcessStateRequest(string[] topic, string payload)
  {
    string deviceId = AbstractMqttProcessor.GetDeviceFromTopic(topic);
    try
    {
      StateRequestJson? json = JsonSerializer.Deserialize<StateRequestJson>(payload);
      if (json is not null) {
        await _supabase.InsertStateRequest(deviceId, json);
      }
    }
    catch (Exception e)
    {
      Console.WriteLine(e.Message);
    }
  }
}
