using System.Text.Json.Serialization;

public class ReporterValue<T> : AbstractHasDatetimeJson
{
  [JsonPropertyName("value")]
  public T? Value { get; set; }
}
