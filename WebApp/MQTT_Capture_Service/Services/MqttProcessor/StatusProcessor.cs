
public class StatusProcessor : AbstractMqttProcessor
{
  public StatusProcessor(SupabaseService supabase, MQTTService mqtt) : base(supabase, mqtt)
  {

  }

  public override async Task Process(string[] topic, string payload)
  {
    string deviceId = AbstractMqttProcessor.GetDeviceFromTopic(topic);
    switch(payload) {
      case "online": {
        await _supabase.UpdateOnlineStatusForDevice(deviceId, true);
        break;
      }
      case "offline": {
        await _supabase.UpdateOnlineStatusForDevice(deviceId, false);
        break;
      }
      case "connected": {
        Console.WriteLine("Sending configuration, because device just connected.");
        await _mqtt.SendDeviceConfiguration(deviceId);
        break;
      }
      default:
        await _mqtt.ProcessUnknownMessage(topic, payload);
        break;
    }
  }
}
