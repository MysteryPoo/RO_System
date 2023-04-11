
using Postgrest.Attributes;
using Postgrest.Models;

namespace DbRow;

[Table("device_list")]
public class DeviceDbRow : BaseModel {
  [PrimaryKey("id", false)]
  public long Id { get; set; }
  [Column("device_id")]
  public string? DeviceId { get; set; }
  [Column("device_name")]
  public string? Name { get; set; }
  [Column("last_ip_address")]
  public string? LastIpAddress { get; set; }
  [Column("online")]
  public bool Online { get; set; }
  [Column("connected")]
  public bool Connected { get; set; }
  [Column("last_heard")]
  public DateTimeOffset LastHeard { get; set; }

  public void UpdateFromParticle(DeviceParticleModel model) {
    this.DeviceId = model.DeviceId;
    this.Name = model.Name;
    this.LastIpAddress = model.LastIpAddress;
    this.Online = model.Online;
    this.Connected = model.Connected;
    this.LastHeard = model.LastHeard;
  }
}
