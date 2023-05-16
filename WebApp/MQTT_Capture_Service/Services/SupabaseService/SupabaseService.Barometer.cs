using Capture.DbRow;

public partial class SupabaseService
{
  public async Task InsertBarometerPressure(string deviceId, ReporterValue<int> report)
  {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Insert Barometer Error: Device does not exist");
    var pressureToInsert = new BarometerPressureDbRow()
    {
      DeviceId = device.Id,
      Datetime = report.Datetime,
      Pressure = report.Value
    };
    try
    {
      await _client.From<BarometerPressureDbRow>().Insert(pressureToInsert);
    }
    catch (Exception e)
    {
      Console.WriteLine(e.Message);
    }
  }
}
