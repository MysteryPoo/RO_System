
using System.Net.Http.Json;

public class ParticleService {
  private readonly SupabaseService _supabase;
  private System.Timers.Timer _refreshTimer = new System.Timers.Timer();
  private HttpClient _http = new HttpClient();
  private const string particleApiUri = "https://api.particle.io/v1/devices";
  public ParticleService(string? particleApiKey, SupabaseService supabase) {
    if (particleApiKey is null) {
      throw new Exception("PARTICLE_API_KEY is required.");
    }
    _supabase = supabase;
    _refreshTimer.Elapsed += OnRefreshTimer;
    _refreshTimer.Interval = 10000;
    _refreshTimer.AutoReset = true;
    _refreshTimer.Start();
    _http.DefaultRequestHeaders.Authorization = new System.Net.Http.Headers.AuthenticationHeaderValue("Bearer", particleApiKey);
  }

  private async void OnRefreshTimer(Object? source, System.Timers.ElapsedEventArgs elapsedEventArgs) {
    try {
      var response = await _http.GetAsync(particleApiUri);
      if (response.IsSuccessStatusCode) {
        var particleDeviceList = await response.Content.ReadFromJsonAsync<DeviceParticleModel[]>();
        if (particleDeviceList is not null) {
          await _supabase.UpdateDeviceList(particleDeviceList);
        }
      }
    } catch (Exception e) {
      Console.WriteLine(e.Message);
    }
  }
}
