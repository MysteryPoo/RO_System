using System.Text.Json.Serialization;

public class AbstractHasDatetimeJson
{
  [JsonPropertyName("datetime")]
  [JsonConverter(typeof(DateTimeParticleToDateTimeOffset))]
  public DateTimeOffset Datetime { get; set; }
}
