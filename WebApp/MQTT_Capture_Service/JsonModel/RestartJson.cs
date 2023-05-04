using System.Text.Json.Serialization;

public class RestartJson : AbstractHasDatetimeJson
{
  [JsonPropertyName("reason")]
  public string Reason { get; set; } = string.Empty;
}
