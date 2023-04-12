using Capture.DbRow;
using Supabase;

public partial class SupabaseService {
  private readonly SupabaseOptions _options = new SupabaseOptions
  {
    AutoConnectRealtime = true
  };

  private Client Client { get; }

  public SupabaseService(string? supabaseUrl, string? supabaseKey) {
    if (supabaseUrl is null || supabaseKey is null) {
      throw new Exception("SUPABASE_URL and SUPABASE_KEY are required.");
    }
    Client = new Client(supabaseUrl, supabaseKey, _options);
  }

  public async Task InitializeAsync() {
    Console.WriteLine("Initializing Supabase connection.");
    await Client.InitializeAsync();
  }

  public async Task UpdateDeviceList(DeviceParticleModel[] particleDevices) {
    var dbDevices = await GetDeviceListFromDatabase();
    foreach(var device in particleDevices) {
      var knownDevice = dbDevices.Where(d => d.DeviceId == device.DeviceId).FirstOrDefault();
      if (knownDevice is not null) {
        knownDevice.UpdateFromParticle(device);
        await Client.From<DeviceDbRow>().Update(knownDevice);
      } else {
        await Client.From<DeviceDbRow>().Insert(device.AsSupabaseModel());
      }
    }
  }

  public async Task UpdateOnlineStatusForDevice(string deviceId, bool isOnline) {
    var device = (await Client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is not null) {
      device.Online = isOnline;
      await Client.From<DeviceDbRow>().Update(device);
    }
  }

  public async Task WipeComponentsForDevice(string deviceId) {
    var device = (await Client.From<DeviceDbRow>().Select(d => new object[] { d.Id }).Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Cannot wipe components for unknown device.");
    await Client.From<ComponentDbRow>().Where(c => c.DeviceId == device.Id).Delete();
  }

  public async Task PruneFeaturesForDevice(string deviceId, string[] featureList) {
    var device = (await Client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Cannot prune features for non-existant device.");
    var components = (await Client.From<ComponentDbRow>().Where(c => c.DeviceId == device.Id).Get()).Models.Select(c => c.Name);
    foreach(var component in components) {
      if (featureList.Contains(component) is false) {
        // Remove it
        await Client.From<ComponentDbRow>().Where(c => c.DeviceId == device.Id && c.Name == component).Delete();
      }
    }
  }

  private async Task<List<DeviceDbRow>> GetDeviceListFromDatabase() {
    return (await Client.From<DeviceDbRow>().Get()).Models;
  }
}