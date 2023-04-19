
using System.Text.Json.Serialization;
using Supabase.Realtime.Models;

public class TriggerJson : BaseBroadcast {
  [JsonPropertyName("device_id")]
  public string DeviceId { get; set; } = string.Empty;
  [JsonPropertyName("component_name")]
  public string ComponentName { get; set; } = string.Empty;
  [JsonPropertyName("trigger_name")]
  public string TriggerName { get; set; } = string.Empty;
}
