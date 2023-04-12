
using Postgrest.Attributes;
using Postgrest.Models;

namespace Capture.DbRow;

[Table("heartbeat_floatswitch")]
public class HeartbeatFloatSwitchDbRow : BaseModel {
  [PrimaryKey("id")]
  public long Id { get; set; }
  [Column("heartbeat_id")]
  public long HeartbeatId { get; set; }
  [Column("float")]
  public bool Float { get; set; }
  [Column("reliable")]
  public bool Reliabe { get; set; }
}
