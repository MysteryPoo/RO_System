using Capture.DbRow;

public partial class SupabaseService
{
  public async Task InsertVersion(string deviceId, string version)
  {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Insert Version Error: Device does not exist.");
    var versionToInsert = new VersionDbRow()
    {
      DeviceId = device.Id,
      Version = version
    };
    try {
      await _client.From<VersionDbRow>().Insert(versionToInsert);
    } catch (Exception e) {
      Console.WriteLine(e.Message);
    }
  }
}
