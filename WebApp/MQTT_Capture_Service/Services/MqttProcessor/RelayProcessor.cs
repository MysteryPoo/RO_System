
public class RelayProcessor : AbstractMqttProcessor
{
  public RelayProcessor(SupabaseService supabase, MQTTService mqtt) : base(supabase, mqtt)
  {
  }

  public override Task Process(string[] topic, string payload)
  {
    throw new NotImplementedException();
  }
}
