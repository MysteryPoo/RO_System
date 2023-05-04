using Capture.DbRow;

public partial class SupabaseService
{
  public async Task InsertWifiSignal(string deviceId, ReporterValue<float> wifi)
  {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Insert Wifi-Signal Error: Device does not exist");
    var wifiSignal = new WifiSignalDbRow() {
      DeviceId = device.Id,
      Datetime = wifi.Datetime,
      Signal = wifi.Value
    };
    try
    {
      await _client.From<WifiSignalDbRow>().Insert(wifiSignal);
    }
    catch (Exception e)
    {
      Console.WriteLine(e.Message);
    }
  }

  public async Task InsertWifiQuality(string deviceId, ReporterValue<float> wifi)
  {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Insert Wifi-Quality Error: Device does not exist");
    var wifiQuality = new WifiQualityDbRow() {
      DeviceId = device.Id,
      Datetime = wifi.Datetime,
      Quality = wifi.Value
    };
    try
    {
      await _client.From<WifiQualityDbRow>().Insert(wifiQuality);
    }
    catch (Exception e)
    {
      Console.WriteLine(e.Message);
    }
  }
}
