async function fetchDeviceList() {
  // const prFetchList = await fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/deviceList?secret=${process.env.VUE_APP_API_SECRET}`);

  // return prFetchList.json();
  return (await fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/deviceList?secret=${process.env.VUE_APP_API_SECRET}`)).json();
}

async function fetchStatus(deviceId) {
  if (deviceId !== null) {
    const prFetchStatus = await fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/${deviceId}/status?secret=${process.env.VUE_APP_API_SECRET}`);
    return prFetchStatus.json();
  }
  return Promise.reject(Error('Device Id Required'));
}

async function fetchTick(deviceId) {
  if (deviceId !== null) {
    const promise = await fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/${deviceId}/lastTick?secret=${process.env.VUE_APP_API_SECRET}`);
    return promise.json();
  }
  return Promise.reject(Error('Device Id Required'));
}

async function fetchState(deviceId) {
  if (deviceId !== null) {
    const currentStateFetch = await fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/${deviceId}/currentState?secret=${process.env.VUE_APP_API_SECRET}`);
    return currentStateFetch.json();
  }
  return Promise.reject(Error('Device Id Required'));
}

async function fetchConfiguration(deviceId) {
  return (await (await fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/${deviceId}/configuration?secret=${process.env.VUE_APP_API_SECRET}`)).json())[0];
}

async function postConfiguration(deviceId, config) {
  return fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/${deviceId}/configuration?secret=${process.env.VUE_APP_API_SECRET}`, {
    method: 'POST',
    headers: {
      Accept: 'application/json',
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(config),
  });
}

export default {
  fetchDeviceList,
  fetchStatus,
  fetchTick,
  fetchState,
  fetchConfiguration,
  postConfiguration,
};
