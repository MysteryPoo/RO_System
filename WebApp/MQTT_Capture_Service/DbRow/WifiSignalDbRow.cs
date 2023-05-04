using Postgrest.Attributes;
using Postgrest.Models;

namespace Capture.DbRow;

[Table("wifi_signal_reports")]
public class WifiSignalDbRow : BaseModel
{
  [PrimaryKey("Id")]
  public long Id { get; set; }
  [Column("device_id")]
  public long DeviceId { get; set; }
  [PrimaryKey("inserted_at", false)] // This is a hack to prevent the api from sending the field so it defaults on the DB level
  public DateTimeOffset InsertedAt { get; set; }
  [Column("datetime")]
  public DateTimeOffset Datetime { get; set; }
  [Column("signal")]
  public float Signal { get; set; }
}
