
using System.Text.Json;

public class FloatSwitchProcessor : AbstractMqttProcessor
{
  public FloatSwitchProcessor(SupabaseService supabase, MQTTService mqtt) : base(supabase, mqtt)
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
      case "status": {
        var json = JsonSerializer.Deserialize<ReporterValue<bool>>(payload);
        if (json is null) throw new Exception("Unable to deserialize Float Switch Status.");
        await _supabase.InsertFloatSwitchStatus(deviceId, json);
        break;
      }
      case "reliable": {
        var json = JsonSerializer.Deserialize<ReporterValue<bool>>(payload);
        if (json is null) throw new Exception("Unable to deserialize Float Switch Reliable.");
        await _supabase.InsertFloatSwitchReliable(deviceId, json);
        break;
      }
      case "pin": {
        try
        {
          var json = JsonSerializer.Deserialize<FloatSwitchPinJson>(payload);
          if (json is null) throw new Exception("Unable to deserialize Float Switch Pin.");
          await _supabase.CreateOrUpdateFloatSwitchPin(deviceId, json);
        }
        catch (Exception e)
        {
          Console.WriteLine(e.Message);
        }
        break;
      }
      default: {
        await _mqtt.ProcessUnknownMessage(topic, payload);
        break;
      }
    }
  }

  private async Task ProcessConfiguration(string[] topic, string payload)
  {
    string deviceId = AbstractMqttProcessor.GetDeviceFromTopic(topic);
    await _supabase.ProcessConfiguration(deviceId, topic[2], payload);
  }
}
