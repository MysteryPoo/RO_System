
using System.Text.Json.Serialization;

public class HeartbeatWiFiJson {
  [JsonPropertyName("signal")]
  public short Signal { get; set; }
  [JsonPropertyName("quality")]
  public short Quality { get; set; }
}

public class HeartbeatROSystemJson {
  [JsonPropertyName("enabled")]
  public bool Enabled { get; set; }
  [JsonPropertyName("Total Pump Time")]
  public int TotalPumpTime { get; set; }
}

public class HeartbeatFloatSwitchJson {
  [JsonPropertyName("float")]
  public bool Float { get; set; }
  [JsonPropertyName("reliable")]
  public bool Reliable { get; set; }
}

public class HeartbeatDataJson {
  [JsonPropertyName("event")]
  public string Event { get; set; } = string.Empty;
  [JsonPropertyName("messageQueueSize")]
  public int MessageQueueSize { get; set; }
  [JsonPropertyName("version")]
  public string Version { get; set; } = string.Empty;
  [JsonPropertyName("heartbeat-rate")]
  public int HeartbeatRate { get; set; }
  [JsonPropertyName("WiFi")]
  public HeartbeatWiFiJson? WiFi { get; set; }
  [JsonPropertyName("ro-system")]
  public HeartbeatROSystemJson? RoSystem { get; set; }
  [JsonPropertyName("float-switch")]
  public HeartbeatFloatSwitchJson? FloatSwitch { get; set; }
}

public class HeartbeatJson {
  [JsonPropertyName("datettime")]
  public DateTimeOffset Datetime { get; set; } = DateTime.Now;
  [JsonPropertyName("data")]
  public HeartbeatDataJson? Data { get; set; }
}
