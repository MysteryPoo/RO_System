
using Postgrest.Attributes;
using Postgrest.Models;

namespace Capture.DbRow;

[Table("option_list")]
public class OptionDbRow : BaseModel {
  [PrimaryKey("id")]
  public long Id { get; set; }
  [Column("name")]
  public string Name { get; set; } = string.Empty;
  [Column("component_id")]
  public long ComponentId { get; set; }
  [Column("type")]
  public string Type { get; set; } = string.Empty;
}
