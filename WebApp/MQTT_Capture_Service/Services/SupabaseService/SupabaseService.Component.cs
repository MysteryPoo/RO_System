
using Capture.DbRow;

public partial class SupabaseService {
  public async Task CreateOrUpdateComponentForDevice(string deviceId, string componentName, ConfigurationJson configuration) {
    var device = (await Client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Cannot add component to unknown device.");
    ComponentDbRow? existingComponent = (await Client.From<ComponentDbRow>().Where(c => c.DeviceId == device.Id && c.Name == componentName).Get()).Models.FirstOrDefault();
    ComponentDbRow componentDb = existingComponent is null ? await CreateComponentForDevice(device.Id, componentName, configuration) : await UpdateComponentForDevice(existingComponent, configuration);
    foreach(var option in configuration.Options) {
      await CreateOrUpdateOptionForComponent(componentDb.Id, option);
    }
  }

  public async Task<IEnumerable<ComponentDbRow>> GetComponentListForDevice(long deviceId) {
    return (await Client.From<ComponentDbRow>().Where(c => c.DeviceId == deviceId).Get()).Models;
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
}
