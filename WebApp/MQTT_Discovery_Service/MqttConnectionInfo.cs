
using System.Text.Json.Serialization;

public class MqttConnectionInfo {
  [JsonPropertyName("username")]
  public string Username { get; set; } = String.Empty;
  [JsonPropertyName("password")]
  public string Password { get; set; } = String.Empty;
  [JsonPropertyName("override")]
  public string Override { get; set; } = "<Blank>";
}
