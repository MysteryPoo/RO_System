
using System.Text.Json;

public class FeatureListProcessor : AbstractMqttProcessor {
  public FeatureListProcessor(SupabaseService supabase, MQTTService mqtt) : base(supabase, mqtt)
  {
    
  }

  public override async Task Process(string[] topic, string payload) {
    await _supabase.PruneFeaturesForDevice(AbstractMqttProcessor.GetDeviceFromTopic(topic), JsonSerializer.Deserialize<string[]>(payload)!);
  }
}