
using System.Text.Json;
using MQTTnet;
using MQTTnet.Client;

public class MQTTService {
  private MqttFactory _factory;
  private IMqttClient? _client;
  private MqttClientOptions _options;
  private SupabaseService _supabase;

  public MQTTService(string? mqttUsername, string? mqttPassword, string? endPoint, SupabaseService supabase) {
    if (mqttUsername is null || mqttPassword is null) {
      throw new Exception("MQTT Credentials are required.");
    }
    if (endPoint is null) {
      throw new Exception("MQTT Endpoint is required.");
    }
    _supabase = supabase;
    _factory = new MqttFactory();
    _client = _factory.CreateMqttClient();
    _options = new MqttClientOptionsBuilder().WithTcpServer("localhost").WithCredentials(mqttUsername, mqttPassword).WithClientId("Capture_Service").Build();
  }

  public async Task StartAsync() {
    if (_client is null) return;
    await Task.Factory.StartNew(async () => {
      Console.WriteLine("Starting MQTT connection.");
      _client.ApplicationMessageReceivedAsync += async delegate(MqttApplicationMessageReceivedEventArgs args) {
        args.ReasonCode = MqttApplicationMessageReceivedReasonCode.Success;
        Console.WriteLine($"{args.ApplicationMessage.Topic}: {System.Text.Encoding.Default.GetString(args.ApplicationMessage.Payload)}");
        await ProcessMessage(args.ApplicationMessage.Topic.Split('/'), System.Text.Encoding.Default.GetString(args.ApplicationMessage.Payload));
      };
      // Use proper cancellation and not while(true).
      while (true) {
        try {
          
          // This code will also do the very first connect! So no call to _ConnectAsync_ is required in the first place.
          if (!await _client.TryPingAsync()) {
            await _client.ConnectAsync(_options, CancellationToken.None);

            // Subscribe to topics when session is clean etc.
            Console.WriteLine("The MQTT client is connected.");

            var mqttSubscribeOptions = _factory.CreateSubscribeOptionsBuilder()
              .WithTopicFilter(
                f =>
                {
                    f.WithTopic("from/#");
                })
              .Build();

            var response = await _client.SubscribeAsync(mqttSubscribeOptions, CancellationToken.None);

            Console.WriteLine("MQTT client subscribed to topic.");

        }
        } catch (Exception e) {
          // Handle the exception properly (logging etc.).
          Console.WriteLine(e.Message);
        } finally {
          // Check the connection state every 5 seconds and perform a reconnect if required.
          await Task.Delay(TimeSpan.FromSeconds(5));
        }
      }
    });
  }

  private async Task ProcessMessage(string[] topic, string? payload) {
    if (!topic[0].Equals("from")) throw new Exception("This processor is for 'from' topics only.");
    string deviceId = topic[1];
    switch(topic[2]) {
      case "status": {
        await ProcessStatus(topic, payload);
        break;
      }
      case "configuration": {
        await ProcessConfiguration(topic, payload);
        break;
      }
      case "romcon": {
        await ProcessLogger(topic, payload);
        break;
      }
      default:
        Console.WriteLine($"No processor found for {topic[2]}");
        break;
    }
  }

  private async Task ProcessStatus(string[] topic, string? payload) {
    string deviceId = topic[1];
    switch(payload) {
      case "online": {
        await _supabase.UpdateOnlineStatusForDevice(deviceId, true);
        break;
      }
      case "feature-refresh": {
        // This message is deprecated
        break;
      }
      default:
        Console.WriteLine($"No subprocessor found for {payload}");
        break;
    }
    
  }

  private async Task ProcessConfiguration(string[] topic, string? payload) {
    if (payload is null) throw new Exception("Invalid configuration.");
    string deviceId = topic[1];
    try {
      var options = new JsonSerializerOptions();
      options.Converters.Add(new OptionConverterWithTypeDiscriminator());
      var configuration = JsonSerializer.Deserialize<ConfigurationJson>(payload, options);
      if (configuration is null) throw new Exception("Unable to deserialize configuration.");
      await _supabase.CreateOrUpdateComponentForDevice(deviceId, topic[3], configuration);
    } catch (Exception e) {
      Console.WriteLine(e.Message);
    }
  }

  private async Task ProcessLogger(string[] topic, string? payload) {
    switch(topic[3]) {
      case "system": {
        await ProcessSystem(topic, payload);
        break;
      }
    }
  }

  private async Task ProcessSystem(string[] topic, string? payload) {
    switch(topic[4]) {
      case "heartbeat": {
        await ProcessHeartbeat(topic, payload);
        break;
      }
    }
  }

  private async Task ProcessHeartbeat(string[] topic, string? payload) {
    if (payload is null) throw new Exception("No payload present for heartbeat.");
    string deviceId = topic[1];
    var heartbeat = JsonSerializer.Deserialize<HeartbeatJson>(payload);
    await _supabase.InsertHeartbeat(deviceId, heartbeat!);
  }
}
