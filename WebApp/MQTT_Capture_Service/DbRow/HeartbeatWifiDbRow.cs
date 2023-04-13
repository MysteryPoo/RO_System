
using Postgrest.Attributes;
using Postgrest.Models;

namespace Capture.DbRow;

[Table("heartbeat_wifi")]
public class HeartbeatWifiDbRow : BaseModel {
  [PrimaryKey("id")]
  public long Id { get; set; }
  [Column("heartbeat_id")]
  public long HeartbeatId { get; set; }
  [Column("signal")]
  public short Signal { get; set; }
  [Column("quality")]
  public short Quality { get; set; }
}
