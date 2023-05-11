using Postgrest.Attributes;
using Postgrest.Models;

namespace Capture.DbRow;

[Table("wifi_quality_reports")]
public class WifiQualityDbRow : BaseModel
{
  [PrimaryKey("Id")]
  public long Id { get; set; }
  [Column("device_id")]
  public long DeviceId { get; set; }
  [PrimaryKey("inserted_at", false)] // This is a hack to prevent the api from sending the field so it defaults on the DB level
  public DateTimeOffset InsertedAt { get; set; }
  [Column("datetime")]
  public DateTimeOffset Datetime { get; set; }
  [Column("quality")]
  public float Quality { get; set; }
}
