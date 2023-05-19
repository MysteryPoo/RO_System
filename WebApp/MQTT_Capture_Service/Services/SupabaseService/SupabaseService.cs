using System.Text.Json;
using Capture.DbRow;
using Supabase;

public partial class SupabaseService {
  private readonly SupabaseOptions _options = new SupabaseOptions
  {
    AutoConnectRealtime = true
  };

  private Client _client { get; }
  public Client Client { get => _client; }

  public SupabaseService(string? supabaseUrl, string? supabaseKey) {
    if (supabaseUrl is null || supabaseKey is null) {
      throw new Exception("SUPABASE_URL and SUPABASE_KEY are required.");
    }
    _client = new Client(supabaseUrl, supabaseKey, _options);
  }

  public async Task InitializeAsync() {
    Console.WriteLine("Initializing Supabase connection.");
    await _client.InitializeAsync();
  }

  public async Task UpdateDeviceList(DeviceParticleModel[] particleDevices) {
    var dbDevices = await GetDeviceListFromDatabase();
    foreach(var device in particleDevices) {
      var knownDevice = dbDevices.Where(d => d.DeviceId == device.DeviceId).FirstOrDefault();
      if (knownDevice is not null) {
        knownDevice.UpdateFromParticle(device);
        await _client.From<DeviceDbRow>().Update(knownDevice);
      } else {
        await _client.From<DeviceDbRow>().Insert(device.AsSupabaseModel());
      }
    }
  }

  public async Task ProcessConfiguration(string deviceId, string componentName, string configJsonString)
  {
    try {
      var options = new JsonSerializerOptions();
      options.Converters.Add(new OptionConverterWithTypeDiscriminator());
      var configuration = JsonSerializer.Deserialize<ConfigurationJson>(configJsonString, options);
      if (configuration is null) throw new Exception("Unable to deserialize configuration.");
      await CreateOrUpdateComponentForDevice(deviceId, componentName, configuration);
    } catch (Exception e) {
      Console.WriteLine(e.Message);
    }
  }

  public async Task UpdateOnlineStatusForDevice(string deviceId, bool isOnline) {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is not null) {
      device.Online = isOnline;
      await _client.From<DeviceDbRow>().Update(device);
    }
  }

  public async Task PruneFeaturesForDevice(string deviceId, string[] featureList) {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Cannot prune features for non-existant device.");
    var components = (await _client.From<ComponentDbRow>().Where(c => c.DeviceId == device.Id).Get()).Models.Select(c => c.Name);
    foreach(var component in components) {
      if (featureList.Contains(component) is false) {
        // Remove it
        await _client.From<ComponentDbRow>().Where(c => c.DeviceId == device.Id && c.Name == component).Delete();
      }
    }
  }

  public async Task InsertUnknownMessage(string deviceId, string topic, string payload) {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Insert Unknown Message Error: Device does not exist.");
    var unknownMessage = new UnknownMessageDbRow() {
      DeviceId = device.Id,
      Topic = topic,
      Payload = payload,
    };
    await _client.From<UnknownMessageDbRow>().Insert(unknownMessage);
  }

  public async Task<DeviceDbRow?> GetDevice(string deviceId) {
    return (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
  }

  private async Task<List<DeviceDbRow>> GetDeviceListFromDatabase() {
    return (await _client.From<DeviceDbRow>().Get()).Models;
  }
}