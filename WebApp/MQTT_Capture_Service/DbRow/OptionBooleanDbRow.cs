using Postgrest.Attributes;
using Postgrest.Models;

[Table("option_boolean_list")]
public class OptionBooleanDbRow : BaseModel {
  [PrimaryKey("id")]
  public long Id { get; set; }
  [Column("option_id")]
  public long OptionId { get; set; }
  [Column("default")]
  public bool Default { get; set; }
}
