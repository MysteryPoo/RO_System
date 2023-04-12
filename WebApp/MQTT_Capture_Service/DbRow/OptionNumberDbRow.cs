using Postgrest.Attributes;
using Postgrest.Models;

namespace Capture.DbRow;

[Table("option_number_list")]
public class OptionNumberDbRow : BaseModel {
  [PrimaryKey("id")]
  public long Id { get; set; }
  [Column("option_id")]
  public long OptionId { get; set; }
  [Column("units")]
  public string Units { get; set; } = string.Empty;
  [Column("default")]
  public int Default { get; set; }
}
