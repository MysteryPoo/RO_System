
using Postgrest.Attributes;
using Postgrest.Models;

namespace Capture.DbRow;

[Table("component_list")]
public class ComponentDbRow : BaseModel {
  [PrimaryKey("id", false)]
  public long Id { get; set; }
  [Column("component_name")]
  public string Name { get; set; } = string.Empty;
  [Column("device_id")]
  public long DeviceId { get; set; }
  [Column("display")]
  public string Display { get; set; } = string.Empty;
  [Column("description")]
  public string Description { get; set; } = string.Empty;
}
