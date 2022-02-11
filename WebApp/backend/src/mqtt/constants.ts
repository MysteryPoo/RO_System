import 'dotenv/config';

const username = process.env.MQTT_USERNAME ?? 'user';
const password = process.env.MQTT_PASSWORD ?? 'password';
const connectionString = process.env.MQTT_OVERRIDE ?? 'rabbitmq';

export const constants = {
  connectionString,
  username,
  password,
};
