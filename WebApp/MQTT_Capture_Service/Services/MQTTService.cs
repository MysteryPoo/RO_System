
using System.Text.Json;
using Capture.DbRow;
using MQTTnet;
using MQTTnet.Client;
//using Newtonsoft.Json;
using static Supabase.Client;

public class MQTTService {
  private MqttFactory _factory;
  private IMqttClient _client;
  private MqttClientOptions _options;
  private SupabaseService _supabase;
  private Supabase.Realtime.RealtimeChannel? _triggerChannel;
  private Supabase.Realtime.RealtimeBroadcast<TriggerJson>? _broadcast;

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
    // This stuff needs a refactor but it works for now
    _supabase.Client.From<OptionBooleanDbRow>().On(ChannelEventType.Update, async (sender, args) => {
      var option = args.Response!.Model<OptionBooleanDbRow>();
      var optionBase = (await _supabase.Client.From<OptionDbRow>().Where(o => o.Id == option!.OptionId).Get()).Models.First();
      var component = (await _supabase.Client.From<ComponentDbRow>().Where(c => c.Id == optionBase.ComponentId).Get()).Models.First();
      var device = (await _supabase.Client.From<DeviceDbRow>().Where(d => d.Id == component.DeviceId).Get()).Models.First();
      Console.WriteLine("Sending device config, because option_boolean_list table changed.");
      await SendDeviceConfiguration(device.DeviceId);
    });
    _supabase.Client.From<OptionNumberDbRow>().On(ChannelEventType.Update, async (sender, args) => {
      var option = args.Response!.Model<OptionNumberDbRow>();
      var optionBase = (await _supabase.Client.From<OptionDbRow>().Where(o => o.Id == option!.OptionId).Get()).Models.First();
      var component = (await _supabase.Client.From<ComponentDbRow>().Where(c => c.Id == optionBase.ComponentId).Get()).Models.First();
      var device = (await _supabase.Client.From<DeviceDbRow>().Where(d => d.Id == component.DeviceId).Get()).Models.First();
      Console.WriteLine("Sending device config, because option_number_list table changed.");
      await SendDeviceConfiguration(device.DeviceId);
    });
    _triggerChannel = _supabase.Client.Realtime.Channel("triggers");
    _broadcast = _triggerChannel.Register<TriggerJson>();
    _broadcast.OnBroadcast += async (sender, _) => {
      var data = _broadcast.Current();
      if (data is not null &&
          data.Payload is not null &&
          data.Event is not null &&
          data.Event.Equals("trigger")) {
        string topic = $"to/{data.Payload["device_id"]}/{data.Payload["component_name"]}/configuration";
        string payload = "{" + $"\"{data.Payload["trigger_name"]}\": true" + "}";
        var message = new MqttApplicationMessageBuilder()
          .WithTopic(topic)
          .WithPayload(payload)
          .Build();
        Console.WriteLine($"Sending topic: {topic} with payload: {payload}");
        await _client.PublishAsync(message);
      }
    };
  }

  public async Task StartAsync() {
    if (_client is null) return;
    if (_triggerChannel is not null) await _triggerChannel.Subscribe();
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

  // TODO: Refactor into Processor classes. Maybe use a dictionary to register the processor based on topic[2]
  private async Task ProcessMessage(string[] topic, string? payload) {
    if (!topic[0].Equals("from")) throw new Exception("This processor is for 'from' topics only.");
    string deviceId = topic[1];
    switch(topic[2]) {
      case "feature-list": {
        await ProcessFeatureList(topic, payload);
        break;
      }
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
      case "ro-system": {
        await ProcessRoSystem(topic, payload);
        break;
      }
      default:
        await ProcessUnknownMessage(topic, payload);
        break;
    }
  }

  private async Task ProcessFeatureList(string[] topic, string? payload) {
    await _supabase.PruneFeaturesForDevice(topic[1], JsonSerializer.Deserialize<string[]>(payload!)!);
  }

  private async Task ProcessStatus(string[] topic, string? payload) {
    string deviceId = topic[1];
    switch(payload) {
      case "online": {
        await _supabase.UpdateOnlineStatusForDevice(deviceId, true);
        break;
      }
      case "offline": {
        await _supabase.UpdateOnlineStatusForDevice(deviceId, false);
        break;
      }
      case "connected": {
        Console.WriteLine("Sending configuration, because device just connected.");
        await SendDeviceConfiguration(deviceId);
        break;
      }
      case "feature-refresh": {
        // This message is deprecated
        break;
      }
      default:
        await ProcessUnknownMessage(topic, payload);
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
        try {
          await ProcessHeartbeat(topic, payload);
        } catch (Exception e) {
          Console.WriteLine(e.Message);
        }
        break;
      }
    }
  }

  private async Task ProcessHeartbeat(string[] topic, string? payload) {
    if (payload is null) throw new Exception("No payload present for heartbeat.");
    string deviceId = topic[1];
    var heartbeat = JsonSerializer.Deserialize<HeartbeatJson>(payload);
    try {
      await _supabase.InsertHeartbeat(deviceId, heartbeat!);
    } catch (Exception e) {
      Console.WriteLine(e.Message);
    }
  }

  private async Task ProcessUnknownMessage(string[] topic, string? payload) {
    if (payload is null) throw new Exception("No payload for unknown message.");
    string deviceId = topic[1];
    string topicFull = String.Join('/', topic);
    await _supabase.InsertUnknownMessage(deviceId, topicFull, payload);
  }

  private async Task SendDeviceConfiguration(string deviceId) {
    // Get device
    var device = await _supabase.GetDevice(deviceId);
    if (device is null) throw new Exception("Send Device Configuration Error: Device does not exist.");
    // Get list of components
    var componentList = await _supabase.GetComponentListForDevice(device.Id);
    // For each component, get list of options
    foreach (var component in componentList) {
      string topic = $"to/{deviceId}/{component.Name}/configuration";
      string payload = "{";
      var optionList = await _supabase.GetOptionListForComponent(component.Id);
      // For each option, get value and send
      foreach (var optionBase in optionList) {
        switch(optionBase.Type) {
          case "number": {
            payload += $"\"{optionBase.Name}\": ";
            var option = await _supabase.GetNumberOptionFromOption(optionBase.Id);
            if (option is not null) {
              if (option.Value is not null) {
                payload += $"{option.Value}";
              } else {
                payload += $"{option.Default}";
              }
            }
            break;
          }
          case "boolean": {
            payload += $"\"{optionBase.Name}\": ";
            var option = await _supabase.GetBooleanOptionFromOption(optionBase.Id);
            if (option is not null) {
              if (option.Value is not null) {
                payload += (bool)option.Value ? "true" : "false";
              } else {
                payload += option.Default ? "true" : "false";
              }
            }
            break;
          }
          case "trigger": {
            // Do nothing
            break;
          }
          default: {
            throw new Exception("Unsupported option type.");
          }
        }
        payload += ',';
      }
      payload = payload.TrimEnd(',') + "}";
      var message = new MqttApplicationMessageBuilder()
        .WithTopic(topic)
        .WithPayload(payload)
        .Build();
      Console.WriteLine($"Sending topic: {topic} with payload: {payload}");
      await _client.PublishAsync(message);
    }
  }

  private async Task ProcessRoSystem(string[] topic, string? payload) {
    try {
      if (payload is null) return;
      string deviceId = topic[1];
      switch(topic[3]) {
        case "state-request": {
          StateRequestJson? json = JsonSerializer.Deserialize<StateRequestJson>(payload);
          if (json is not null) {
            await _supabase.InsertStateRequest(deviceId, json);
          }
          break;
        }
        default:
          await ProcessUnknownMessage(topic, payload);
          break;
      }
    } catch (Exception e) {
      Console.WriteLine(e.Message);
    }
  }
}
