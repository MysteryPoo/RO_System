using DbRow;
using Supabase;

public class SupabaseService {
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

  public async Task CreateOrUpdateComponentForDevice(string deviceId, string componentName, ConfigurationJson configuration) {
    var device = (await Client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Cannot add component to unknown device.");
    ComponentDbRow? existingComponent = (await Client.From<ComponentDbRow>().Where(c => c.DeviceId == device.Id && c.Name == componentName).Get()).Models.FirstOrDefault();
    ComponentDbRow componentDb = existingComponent is null ? await CreateComponentForDevice(device.Id, componentName, configuration) : await UpdateComponentForDevice(existingComponent, configuration);
    foreach(var option in configuration.Options) {
      await CreateOrUpdateOptionForComponent(componentDb.Id, option);
    }
  }

  private async Task<ComponentDbRow> CreateComponentForDevice(long deviceId, string componentName, ConfigurationJson configuration) {
    var component = new ComponentDbRow() {
      Name = componentName,
      DeviceId = deviceId,
      Display = configuration.Display,
      Description = configuration.Description,
    };
    return (await Client.From<ComponentDbRow>().Insert(component)).Models.First();
  }

  private async Task<ComponentDbRow> UpdateComponentForDevice(ComponentDbRow component, ConfigurationJson configuration) {
    component.Display = configuration.Display;
    component.Description = configuration.Description;
    return (await Client.From<ComponentDbRow>().Update(component)).Models.First();
  }

  private async Task CreateOrUpdateOptionForComponent(long componentId, OptionJson option) {
    OptionDbRow? existingOption = (await Client.From<OptionDbRow>().Where(o => o.ComponentId == componentId && o.Name == option.Name).Get()).Models.FirstOrDefault();
    var optionDb = existingOption is null ? await CreateOptionForComponent(componentId, option) : await UpdateOptionForComponent(existingOption, option);
    if (optionDb is null) throw new Exception("Failed to insert Option.");
    switch(optionDb.Type) {
      case "number": {
        await CreateOrUpdateNumberOptionForOption(optionDb.Id, (NumberOption)option);
        break;
      }
      case "boolean": {
        await CreateOrUpdateBooleanOptionForOption(optionDb.Id, (BooleanOption)option);
        break;
      }
    }
  }

  private async Task<OptionDbRow> CreateOptionForComponent(long componentId, OptionJson option) {
    var optionDb = new OptionDbRow() {
      Name = option.Name,
      ComponentId = componentId,
      Type = option.Type,
    };
    return (await Client.From<OptionDbRow>().Insert(optionDb)).Models.First();
  }

  private async Task<OptionDbRow> UpdateOptionForComponent(OptionDbRow optionDb, OptionJson optionJson) {
    optionDb.Type = optionJson.Type;
    return (await Client.From<OptionDbRow>().Update(optionDb)).Models.First();
  }

  private async Task CreateOrUpdateNumberOptionForOption(long optionId, NumberOption option) {
    OptionNumberDbRow? existingOption = (await Client.From<OptionNumberDbRow>().Where(o => o.OptionId == optionId).Get()).Models.FirstOrDefault();
    OptionNumberDbRow optionDb = existingOption is null ? await CreateNumberOptionForOption(optionId, option) : await UpdateNumberOptionForOption(existingOption, option);
  }

  private async Task<OptionNumberDbRow> CreateNumberOptionForOption(long optionId, NumberOption option) {
    var optionNumberDb = new OptionNumberDbRow() {
      OptionId = optionId,
      Units = option.Units,
      Default = option.Default,
    };
    return (await Client.From<OptionNumberDbRow>().Insert(optionNumberDb)).Models.First();
  }

  private async Task<OptionNumberDbRow> UpdateNumberOptionForOption(OptionNumberDbRow optionDb, NumberOption optionJson) {
    optionDb.Units = optionJson.Units;
    optionDb.Default = optionJson.Default;
    return (await Client.From<OptionNumberDbRow>().Update(optionDb)).Models.First();
  }

  private async Task CreateOrUpdateBooleanOptionForOption(long optionId, BooleanOption option) {
    OptionBooleanDbRow? existingOption = (await Client.From<OptionBooleanDbRow>().Where(o => o.OptionId == optionId).Get()).Models.FirstOrDefault();
    OptionBooleanDbRow optionDb = existingOption is null ? await CreateBooleanOptionForOption(optionId, option) : await UpdateBooleanOptionForOption(existingOption, option);
  }

  private async Task<OptionBooleanDbRow> CreateBooleanOptionForOption(long optionId, BooleanOption option) {
    var optionBooleanDb = new OptionBooleanDbRow() {
      OptionId = optionId,
      Default = option.Default,
    };
    return (await Client.From<OptionBooleanDbRow>().Insert(optionBooleanDb)).Models.First();
  }

  private async Task<OptionBooleanDbRow> UpdateBooleanOptionForOption(OptionBooleanDbRow optionDb, BooleanOption optionJson) {
    optionDb.Default = optionJson.Default;
    return (await Client.From<OptionBooleanDbRow>().Update(optionDb)).Models.First();
  }

  public async Task WipeComponentsForDevice(string deviceId) {
    var device = (await Client.From<DeviceDbRow>().Select(d => new object[] { d.Id }).Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Cannot wipe components for unknown device.");
    await Client.From<ComponentDbRow>().Where(c => c.DeviceId == device.Id).Delete();
  }

  public async Task InsertHeartbeat(string deviceId, HeartbeatJson heartbeat) {
    var heartbeatToInsert = new HeartbeatDbRow() {
      Datetime = heartbeat.Datetime,
      MessageQueueSize = heartbeat.Data!.MessageQueueSize,
      Version = heartbeat.Data.Version,
      HeartbeatRate = heartbeat.Data.HeartbeatRate,
    };
    try {
      var heartbeatDb = await Client.From<HeartbeatDbRow>().Insert(heartbeatToInsert);
      await CreateWifiHeartbeat(heartbeatDb.Models.First().Id, heartbeat.Data.WiFi!);
      await CreateRoSystemHeartbeat(heartbeatDb.Models.First().Id, heartbeat.Data.RoSystem!);
      await CreateFloatSwitchHeartbeat(heartbeatDb.Models.First().Id, heartbeat.Data.FloatSwitch!);
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

  private async Task<List<DeviceDbRow>> GetDeviceListFromDatabase() {
    return (await Client.From<DeviceDbRow>().Get()).Models;
  }
}