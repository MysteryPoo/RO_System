using Capture.DbRow;

public partial class SupabaseService {
  public async Task InsertStateRequest(string deviceId, StateRequestJson json) {
    var device = (await _client.From<DeviceDbRow>().Where(d => d.DeviceId == deviceId).Get()).Models.FirstOrDefault();
    if (device is null) throw new Exception("Insert State Request Error: Device does not exist.");
    var stateRequest = new StateRequestDbRow() {
      DeviceId = device.Id,
      DateTime = json.DateTime,
      State = json.State,
      Success = json.Success,
      RequestReason = json.RequestReason,
      FailureReason = json.FailureReason,
    };
    try {
      await _client.From<StateRequestDbRow>().Insert(stateRequest);
    } catch (Exception e) {
      Console.WriteLine(e.Message);
    }
  }
}
