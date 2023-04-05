using MongoDB.Driver;

var databaseOptions = new {
  name = Environment.GetEnvironmentVariable("MONGODB_DB_NAME") ?? "test",
  username = Environment.GetEnvironmentVariable("MONGODB_USERNAME") ?? "admin",
  password = Environment.GetEnvironmentVariable("MONGODB_PASSWORD") ?? "admin",
  endpoint = Environment.GetEnvironmentVariable("MONGODB_URI") ?? "localhost",
};

var mqttOptions = new {
  endpoint = Environment.GetEnvironmentVariable("MQTT_OVERRIDE") ?? "rabbitmq",
};

var mongoClient = new MongoClient($"mongodb://{databaseOptions.username}:{databaseOptions.password}@{databaseOptions.endpoint}/{databaseOptions.name}?authSource=admin");

var database = mongoClient.GetDatabase(databaseOptions.name);

// Retrieve configuration settings for deviceId from database
IConfiguration GetConfiguration(string deviceId) {}

// Send configuration settings to the device over MQTT
void SendConfiguration(string deviceId) {}

// Store last status of device in database
void UpdateDeviceStatus(object device) {

}
interface IConfiguration {}
