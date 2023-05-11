using Capture.DbRow;

public partial class SupabaseService
{
  public async Task InsertRoSystemEnabled(string deviceId, ReporterValue<bool> enabled)
  {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Insert RoSystem Enabled Error: Device does not exist.");
    var enabledToInsert = new RoSystemEnabledDbRow()
    {
      DeviceId = device.Id,
      Datetime = enabled.Datetime,
      Enabled = enabled.Value
    };
    try {
      await _client.From<RoSystemEnabledDbRow>().Insert(enabledToInsert);
    } catch (Exception e) {
      Console.WriteLine(e.Message);
    }
  }

  public async Task InsertRoSystemFlushed(string deviceId, ReporterValue<bool> flushed)
  {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Insert RoSystem Flushed Error: Device does not exist.");
    var flushedToInsert = new RoSystemFlushedDbRow()
    {
      DeviceId = device.Id,
      Datetime = flushed.Datetime,
      Flushed = flushed.Value
    };
    try {
      await _client.From<RoSystemFlushedDbRow>().Insert(flushedToInsert);
    } catch (Exception e) {
      Console.WriteLine(e.Message);
    }
  }
}
