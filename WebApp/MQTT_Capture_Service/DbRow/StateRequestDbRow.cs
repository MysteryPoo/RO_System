using Postgrest.Attributes;
using Postgrest.Models;

[Table("state_request")]
public class StateRequestDbRow : BaseModel {
  [PrimaryKey("id")]
  public long Id { get; set; }
  [Column("device_id")]
  public long DeviceId { get; set; }
  [Column("datetime")]
  public DateTimeOffset DateTime { get; set; }
  [Column("state")]
  public string State { get; set; } = string.Empty;
  [Column("success")]
  public bool Success { get; set; }
  [Column("request_reason")]
  public string RequestReason { get; set; } = string.Empty;
  [Column("failure_reason")]
  public string? FailureReason { get; set; }
}
