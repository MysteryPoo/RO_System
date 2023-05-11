using System.Text.Json.Serialization;

public class LogJson : AbstractHasDatetimeJson
{
  [JsonPropertyName("message")]
  public string Message { get; set; } = string.Empty;
}
