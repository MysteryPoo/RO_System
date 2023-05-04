using System.Text.Json;

public class RestartProcessor : AbstractMqttProcessor
{
  public RestartProcessor(SupabaseService supabase, MQTTService mqtt) : base(supabase, mqtt)
  {
  }

  public override async Task Process(string[] topic, string payload)
  {
    string deviceId = AbstractMqttProcessor.GetDeviceFromTopic(topic);
    try
    {
      RestartJson? restart = JsonSerializer.Deserialize<RestartJson>(payload);
      if (restart is null) throw new Exception("Unable to deserialize restart message.");
      await _supabase.InsertRestart(deviceId, restart);
    }
    catch (Exception e)
    {
      Console.WriteLine(e.Message);
      await _mqtt.ProcessUnknownMessage(topic, payload);
    }
  }
}
