
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
      default:
        await _mqtt.ProcessUnknownMessage(topic, payload);
        break;
    }
  }

  // TODO : Consolidate configuration
  private async Task ProcessConfiguration(string[] topic, string payload)
  {
    string deviceId = AbstractMqttProcessor.GetDeviceFromTopic(topic);
    try {
      var options = new JsonSerializerOptions();
      options.Converters.Add(new OptionConverterWithTypeDiscriminator());
      var configuration = JsonSerializer.Deserialize<ConfigurationJson>(payload, options);
      if (configuration is null) throw new Exception("Unable to deserialize configuration.");
      await _supabase.CreateOrUpdateComponentForDevice(deviceId, topic[3], configuration);
    } catch (Exception e) {
      Console.WriteLine(e.Message);
    }
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
