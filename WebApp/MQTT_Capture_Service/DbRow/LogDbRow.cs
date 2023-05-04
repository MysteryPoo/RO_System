using Postgrest.Attributes;
using Postgrest.Models;

namespace Capture.DbRow;

[Table("log")]
public class LogDbRow : BaseModel
{
  [PrimaryKey("Id")]
  public long Id { get; set; }
  [Column("device_id")]
  public long DeviceId { get; set; }
  [PrimaryKey("inserted_at", false)] // This is a hack to prevent the api from sending the field so it defaults on the DB level
  public DateTimeOffset InsertedAt { get; set; }
  [Column("datetime")]
  public DateTimeOffset Datetime { get; set; }
  [Column("severity")]
  public string Severity { get; set; } = string.Empty;
  [Column("message")]
  public string Message { get; set; } = string.Empty;
}
