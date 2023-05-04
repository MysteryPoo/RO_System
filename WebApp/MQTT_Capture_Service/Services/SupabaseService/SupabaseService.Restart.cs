
using Capture.DbRow;

public partial class SupabaseService
{
  public async Task InsertRestart(string deviceId, RestartJson restart)
  {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Insert Restart Error: Device does not exist");
    var restartToInsert = new RestartDbRow() {
      DeviceId = device.Id,
      Datetime = restart.Datetime,
      Reason = restart.Reason
    };
    try
    {
      await _client.From<RestartDbRow>().Insert(restartToInsert);
    }
    catch (Exception e)
    {
      Console.WriteLine(e.Message);
    }
  }
}
