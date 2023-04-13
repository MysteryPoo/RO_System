using Capture.DbRow;

public partial class SupabaseService {
  public async Task InsertHeartbeat(string deviceId, HeartbeatJson heartbeat) {
    var device = (await Client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Insert Heartbeat Error: Device does not exist");
    var heartbeatToInsert = new HeartbeatDbRow() {
      DeviceId = device.Id,
      Datetime = heartbeat.Datetime,
      MessageQueueSize = heartbeat.Data!.MessageQueueSize,
      Version = heartbeat.Data.Version,
      HeartbeatRate = heartbeat.Data.HeartbeatRate,
    };
    try {
      var heartbeatDb = await Client.From<HeartbeatDbRow>().Insert(heartbeatToInsert);
      if (heartbeat.Data.WiFi is not null) { await CreateWifiHeartbeat(heartbeatDb.Models.First().Id, heartbeat.Data.WiFi); }
      if (heartbeat.Data.RoSystem is not null) { await CreateRoSystemHeartbeat(heartbeatDb.Models.First().Id, heartbeat.Data.RoSystem); }
      if (heartbeat.Data.FloatSwitch is not null) { await CreateFloatSwitchHeartbeat(heartbeatDb.Models.First().Id, heartbeat.Data.FloatSwitch); }
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
