using Postgrest.Attributes;
using Postgrest.Models;

namespace Capture.DbRow;

[Table("restart")]
public class RestartDbRow : BaseModel
{
  [PrimaryKey("id")]
  public long Id { get; set; }
  [Column("device_id")]
  public long DeviceId { get; set; }
  [PrimaryKey("inserted_at", false)] // This is a hack to prevent the api from sending the field so it defaults on the DB level
  public DateTimeOffset InsertedAt { get; set; } = DateTimeOffset.Now;
  [Column("datetime")]
  public DateTimeOffset Datetime { get; set; }
  [Column("reason")]
  public string Reason { get; set; } = string.Empty;
}
