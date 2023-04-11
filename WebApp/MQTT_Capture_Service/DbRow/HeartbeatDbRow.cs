
using Postgrest.Attributes;
using Postgrest.Models;

[Table("heartbeat")]
public class HeartbeatDbRow : BaseModel {
  [PrimaryKey("id")]
  public long Id { get; set; }
  [Column("inserted_at")]
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
