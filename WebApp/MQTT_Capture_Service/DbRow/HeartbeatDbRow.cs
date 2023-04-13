
using Postgrest.Attributes;
using Postgrest.Models;

namespace Capture.DbRow;

[Table("heartbeat")]
public class HeartbeatDbRow : BaseModel {
  [PrimaryKey("id", false)]
  public long Id { get; set; }
  [Column("device_id")]
  public long DeviceId { get; set; }
  [PrimaryKey("inserted_at", false)] // This is a hack to prevent the api from sending the field so it defaults on the DB level
  public DateTimeOffset InsertedAt { get; set; } = DateTimeOffset.Now;
  [Column("received_at")]
  public DateTimeOffset Datetime { get; set; }
  [Column("message_queue_size")]
  public int MessageQueueSize { get; set; }
  [Column("version")]
  public string Version { get; set; } = string.Empty;
  [Column("heartbeat_rate")]
  public int HeartbeatRate { get; set; }
}
