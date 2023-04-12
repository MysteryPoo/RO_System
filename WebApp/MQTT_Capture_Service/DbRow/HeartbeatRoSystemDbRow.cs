
using Postgrest.Attributes;
using Postgrest.Models;

namespace Capture.DbRow;

[Table("heartbeat_rosystem")]
public class HeartbeatRoSystemDbRow : BaseModel {
  [PrimaryKey("id")]
  public long Id { get; set; }
  [Column("heartbeat_id")]
  public long HeartbeatId { get; set; }
  [Column("enabled")]
  public bool Enabled { get; set; }
  [Column("total_pump_time")]
  public int TotalPumpTime { get; set; }
}
