using Postgrest.Attributes;
using Postgrest.Models;

namespace Capture.DbRow;

[Table("floatswitch_pin")]
public class FloatSwitchPinDbRow : BaseModel
{
  [PrimaryKey("id")]
  public long Id { get; set; }
  [Column("device_id")]
  public long DeviceId { get; set; }
  [Column("pin")]
  public int Pin { get; set; }
}
