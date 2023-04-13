using Capture.DbRow;
using System.Text.Json.Serialization;

public class DeviceParticleModel {
  [JsonPropertyName("id")]
  public string DeviceId { get; set; } = string.Empty;
  [JsonPropertyName("name")]
  public string Name { get; set; } = string.Empty;
  [JsonPropertyName("last_ip_address")]
  public string? LastIpAddress { get; set; }
  [JsonPropertyName("last_heard")]
  public DateTimeOffset LastHeard { get; set; }
  [JsonPropertyName("last_handshake_at")]
  public DateTimeOffset LastHandshakeAt { get; set; }
  [JsonPropertyName("product_id")]
  public int ProductId { get; set; }
  [JsonPropertyName("online")]
  public bool Online { get; set; }
  [JsonPropertyName("connected")]
  public bool Connected { get; set; }
  [JsonPropertyName("platform_id")]
  public int PlatformId { get; set; }
  [JsonPropertyName("cellular")]
  public bool Cellular { get; set; }
  [JsonPropertyName("functions")]
  public string[]? Functions { get; set; }
  [JsonPropertyName("status")]
  public string? Status { get; set; }
  [JsonPropertyName("serial_number")]
  public string? SerialNumber { get; set; }
  [JsonPropertyName("system_firmware_version")]
  public string? SystemFirmwareVersion { get; set; }

  public DeviceDbRow AsSupabaseModel() {
    return new DeviceDbRow {
      DeviceId = this.DeviceId,
      Name = this.Name,
      LastIpAddress = this.LastIpAddress,
      Online = this.Online,
      Connected = this.Connected,
      LastHeard = this.LastHeard,
    };
  }
}
