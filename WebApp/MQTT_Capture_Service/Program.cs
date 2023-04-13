using MQTTnet;
using MQTTnet.Client;
using MQTTnet.Packets;
using Supabase;
using System.Net.Http.Json;

var root = Directory.GetCurrentDirectory();
var dotenv = Path.Combine(root, ".env");
DotEnv.Load(dotenv);

string? supabaseUrl = Environment.GetEnvironmentVariable("SUPABASE_URL");
string? supabaseKey = Environment.GetEnvironmentVariable("SUPABASE_KEY");
string? particleApiKey = Environment.GetEnvironmentVariable("PARTICLE_API_KEY");
string? mqttUsername = Environment.GetEnvironmentVariable("MQTT_USERNAME");
string? mqttPassword = Environment.GetEnvironmentVariable("MQTT_PASSWORD");
string? mqttUri = Environment.GetEnvironmentVariable("MQTT_OVERRIDE") ?? "rabbitmq";

ManualResetEvent _quitEvent = new ManualResetEvent(false);

Console.CancelKeyPress += (sender, eArgs) => {
  _quitEvent.Set();
  eArgs.Cancel = true;
};

SupabaseService supabase = new SupabaseService(supabaseUrl, supabaseKey);
await supabase.InitializeAsync();

ParticleService listenService = new ParticleService(particleApiKey, supabase);

MQTTService mqtt = new MQTTService(mqttUsername, mqttPassword, mqttUri, supabase);
await mqtt.StartAsync();

_quitEvent.WaitOne();


