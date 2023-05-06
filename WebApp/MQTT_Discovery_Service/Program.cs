
using System.Text.Json;

var root = Directory.GetCurrentDirectory();
var dotenv = Path.Combine(root, ".env");
DotEnv.Load(dotenv);

ManualResetEvent _quitEvent = new ManualResetEvent(false);

Console.CancelKeyPress += (sender, eArgs) => {
  _quitEvent.Set();
  eArgs.Cancel = true;
};

var connectionInfo = new MqttConnectionInfo {
  Username = Environment.GetEnvironmentVariable("MQTT_USERNAME") ?? "test_username",
  Password = Environment.GetEnvironmentVariable("MQTT_PASSWORD") ?? "test_password",
  Override = Environment.GetEnvironmentVariable("MQTT_OVERRIDE")
};

var server = new UdpServer();

Task.Factory.StartNew(async () => {
  Console.WriteLine($"Server is listening at port : {server.Port.ToString()}");
  Console.WriteLine($"Server ip : {server.Address.ToString()}");
  Console.WriteLine($"Server is IP4/IP6 : {server.Family}");
  while(true) {
    try {
      var message = await server.ReceiveAsync();
      Console.WriteLine($"Message received from: {message.Sender.ToString()}");
      Console.WriteLine($"Payload: {message.Payload}");
      if (message.Payload.Equals("Looking for MQTT server")) {
        Console.WriteLine("Sending connection information to client.");
        Console.WriteLine("Client info:");
        Console.WriteLine($"Address: {message.Sender.Address}");
        Console.WriteLine($"Port: {message.Sender.Port}");
        Console.WriteLine($"Override: {connectionInfo.Override}");
        string json = JsonSerializer.Serialize<MqttConnectionInfo>(connectionInfo);
        var bytes = server.Reply(json, message.Sender);
        Console.WriteLine($"Wrote {bytes} bytes.");
      }
    } catch (Exception e) {
      Console.WriteLine(e.Message);
    }
  }
});

_quitEvent.WaitOne();

server.Close();
Console.WriteLine("Server has shutdown.");
