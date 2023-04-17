
using System.Text.Json.Serialization;

public class StateRequestJson {
  [JsonPropertyName("datetime")]
  [JsonConverter(typeof(DateTimeParticleToDateTimeOffset))]
  public DateTimeOffset DateTime { get; set; }
  [JsonPropertyName("state")]
  public string State { get; set; } = string.Empty;
  [JsonPropertyName("success")]
  public bool Success { get; set; }
  [JsonPropertyName("requestReason")]
  public string RequestReason { get; set; } = string.Empty;
  [JsonPropertyName("failureReason")]
  public string FailureReason { get; set; } = string.Empty;
}
