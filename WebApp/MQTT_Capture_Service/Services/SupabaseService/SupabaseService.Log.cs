using Capture.DbRow;

public partial class SupabaseService
{
  public async Task InsertLog(string deviceId, string severity, LogJson log)
  {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Insert Version Error: Device does not exist.");
    var logToInsert = new LogDbRow()
    {
      DeviceId = device.Id,
      Datetime = log.Datetime,
      Severity = severity,
      Message = log.Message
    };
    try {
      await _client.From<LogDbRow>().Insert(logToInsert);
    } catch (Exception e) {
      Console.WriteLine(e.Message);
    }
  }
}
