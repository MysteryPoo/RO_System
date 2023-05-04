
using Capture.DbRow;

public partial class SupabaseService
{
  public async Task CreateOrUpdateFloatSwitchPin(string deviceId, FloatSwitchPinJson pin)
  {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Insert FloatSwitch Error: Device does not exist");
    var existingPin = (await _client.From<FloatSwitchPinDbRow>().Where(c => c.DeviceId == device.Id).Get()).Models.FirstOrDefault();
    if (existingPin is null)
    {
      await CreateFloatSwitchPin(device.Id, pin.Pin);
    }
    else
    {
      await UpdateFloatSwitchPin(existingPin, pin.Pin);
    }
  }

  private async Task CreateFloatSwitchPin(long deviceId, int pin)
  {
    var pinToInsert = new FloatSwitchPinDbRow()
    {
      DeviceId = deviceId,
      Pin = pin
    };
    await _client.From<FloatSwitchPinDbRow>().Insert(pinToInsert);
  }

  private async Task UpdateFloatSwitchPin(FloatSwitchPinDbRow pin, int newPin)
  {
    pin.Pin = newPin;
    await _client.From<FloatSwitchPinDbRow>().Update(pin);
  }
  public async Task InsertFloatSwitchStatus(string deviceId, ReporterValue<bool> report)
  {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Insert FloatSwitch Error: Device does not exist");
    var statusToInsert = new FloatSwitchStatusDbRow()
    {
      DeviceId = device.Id,
      Datetime = report.Datetime,
      Status = report.Value
    };
    try
    {
      await _client.From<FloatSwitchStatusDbRow>().Insert(statusToInsert);
    }
    catch (Exception e)
    {
      Console.WriteLine(e.Message);
    }
  }
  public async Task InsertFloatSwitchReliable(string deviceId, ReporterValue<bool> report)
  {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Insert FloatSwitch Error: Device does not exist");
    var reliableToInsert = new FloatSwitchReliableDbRow()
    {
      DeviceId = device.Id,
      Datetime = report.Datetime,
      Reliable = report.Value
    };
    try
    {
      await _client.From<FloatSwitchReliableDbRow>().Insert(reliableToInsert);
    }
    catch (Exception e)
    {
      Console.WriteLine(e.Message);
    }
  }
}
