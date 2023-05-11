using System.Text.Json.Serialization;

public class FloatSwitchPinJson
{
  [JsonPropertyName("pin")]
  public int Pin { get; set; }
}
