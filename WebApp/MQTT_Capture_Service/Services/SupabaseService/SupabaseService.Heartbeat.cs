using Capture.DbRow;

public partial class SupabaseService {
  public async Task InsertHeartbeat(string deviceId, HeartbeatJson heartbeat) {
    var heartbeatToInsert = new HeartbeatDbRow() {
      Datetime = heartbeat.Datetime,
      MessageQueueSize = heartbeat.Data!.MessageQueueSize,
      Version = heartbeat.Data.Version,
      HeartbeatRate = heartbeat.Data.HeartbeatRate,
    };
    try {
      var heartbeatDb = await Client.From<HeartbeatDbRow>().Insert(heartbeatToInsert);
      await CreateWifiHeartbeat(heartbeatDb.Models.First().Id, heartbeat.Data.WiFi!);
      await CreateRoSystemHeartbeat(heartbeatDb.Models.First().Id, heartbeat.Data.RoSystem!);
      await CreateFloatSwitchHeartbeat(heartbeatDb.Models.First().Id, heartbeat.Data.FloatSwitch!);
    } catch (Exception e) {
      Console.WriteLine(e.Message);
    }
  }

  private async Task CreateWifiHeartbeat(long heartbeatId, HeartbeatWiFiJson wifi) {
    var wifiDb = new HeartbeatWifiDbRow() {
      HeartbeatId = heartbeatId,
      Signal = wifi.Signal,
      Quality = wifi.Quality,
    };
    await Client.From<HeartbeatWifiDbRow>().Insert(wifiDb);
  }

  private async Task CreateRoSystemHeartbeat(long heartbeatId, HeartbeatROSystemJson roSystem) {
    var roSystemDb = new HeartbeatRoSystemDbRow() {
      HeartbeatId = heartbeatId,
      Enabled = roSystem.Enabled,
      TotalPumpTime = roSystem.TotalPumpTime,
    };
    await Client.From<HeartbeatRoSystemDbRow>().Insert(roSystemDb);
  }

  private async Task CreateFloatSwitchHeartbeat(long heartbeatId, HeartbeatFloatSwitchJson floatSwitch) {
    var floatSwitchDb = new HeartbeatFloatSwitchDbRow() {
      HeartbeatId = heartbeatId,
      Float = floatSwitch.Float,
      Reliabe = floatSwitch.Reliable,
    };
    await Client.From<HeartbeatFloatSwitchDbRow>().Insert(floatSwitchDb);
  }
}
