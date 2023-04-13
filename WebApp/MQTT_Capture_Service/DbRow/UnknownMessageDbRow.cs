using Postgrest.Attributes;
using Postgrest.Models;

[Table("unknown_message")]
public class UnknownMessageDbRow : BaseModel {
  [PrimaryKey("id", false)]
  public long Id { get; set; }
  [Column("device_id")]
  public long DeviceId { get; set; }
  [PrimaryKey("created_at", false)] // This is a hack to prevent the api from sending the field so it defaults on the DB level
  public DateTimeOffset CreatedAt { get; set; }
  [Column("topic")]
  public string Topic { get; set; } = string.Empty;
  [Column("payload")]
  public string Payload { get; set; } = string.Empty;
}
