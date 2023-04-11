
using System.Text.Json.Serialization;

public class ConfigurationJson {
  [JsonPropertyName("display")]
  public string Display { get; set; } = string.Empty;
  [JsonPropertyName("description")]
  public string Description { get; set; } = string.Empty;
  [JsonPropertyName("options")]
  public List<OptionJson> Options { get; set; } = new();
}
