const unauthorizedMessage = 'Unauthorized';
const deviceIdRequiredMessage = 'Device Id Required';

async function fetchDeviceList() {
  const response = await fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/deviceList`, {
    method: 'GET',
    headers: {
      Authorization: `Bearer ${window.localStorage.token}`,
    },
  });
  if (response.status === 200) {
    return response.json();
  }
  if (response.status === 401) {
    return Promise.reject(Error(unauthorizedMessage));
  }
  return Promise.reject(Error('Unknown error.'));
}

async function fetchStatus(deviceId) {
  if (deviceId !== null) {
    const response = await fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/devices/${deviceId}/status`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      return Promise.reject(Error(unauthorizedMessage));
    }
  }
  return Promise.reject(Error(deviceIdRequiredMessage));
}

async function fetchTick(
  deviceId,
  dateTo = new Date(),
  dateFrom = new Date((dateTo).getTime() - (7 * 24 * 60 * 60 * 1000)),
  resolution = 10,
) {
  if (deviceId !== null) {
    let uriString = `http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/devices/${deviceId}/ticks`;
    uriString += `?to=${dateTo.getTime()}`;
    uriString += `&from=${dateFrom.getTime()}`;
    uriString += `&resolution=${resolution}`;
    const response = await fetch(uriString, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      return Promise.reject(Error(unauthorizedMessage));
    }
    return Promise.reject(Error('Failed to retrieve devices.'));
  }
  return Promise.reject(Error(deviceIdRequiredMessage));
}

async function fetchLogs(deviceId) {
  if (deviceId !== null) {
    const response = await fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/devices/${deviceId}/logs`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      return Promise.reject(Error(unauthorizedMessage));
    }
  }
  return Promise.reject(Error(deviceIdRequiredMessage));
}

async function clearLog(deviceId, logId) {
  if (deviceId !== null) {
    const response = await fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/devices/${deviceId}/logs/${logId}`, {
      method: 'DELETE',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response;
    }
    if (response.status === 401) {
      return Promise.reject(Error(unauthorizedMessage));
    }
  }
  return Promise.reject(Error(deviceIdRequiredMessage));
}

async function fetchState(deviceId) {
  if (deviceId !== null) {
    const currentStateFetch = await fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/devices/${deviceId}/currentState`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    return currentStateFetch.json();
  }
  return Promise.reject(Error(deviceIdRequiredMessage));
}

async function fetchConfiguration(deviceId) {
  if (deviceId !== null) {
    const response = await fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/devices/${deviceId}/configuration`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      const configArray = await response.json();
      return configArray[0];
    }
    if (response.status === 401) {
      return Promise.reject(Error(unauthorizedMessage));
    }
  }
  return Promise.reject(Error(deviceIdRequiredMessage));
}

async function postConfiguration(deviceId, config) {
  if (deviceId !== null) {
    const response = await fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/devices/${deviceId}/configuration`, {
      method: 'POST',
      headers: {
        Accept: 'application/json',
        'Content-Type': 'application/json',
        Authorization: `Bearer ${window.localStorage.token}`,
      },
      body: JSON.stringify(config),
    });
    if (response.status === 200) {
      return response;
    }
    if (response.status === 401) {
      return Promise.reject(Error(unauthorizedMessage));
    }
  }
  return Promise.reject(Error(deviceIdRequiredMessage));
}

function login(formData) {
  return fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/auth/login`, {
    method: 'POST',
    headers: {
      Accept: 'application/json',
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(formData),
  });
}

function signup(formData) {
  return fetch(`http://${window.location.hostname}:${process.env.VUE_APP_API_PORT}/auth/signup`, {
    method: 'POST',
    headers: {
      Accept: 'application/json',
      'Content-Type': 'application/json',
    },
    body: JSON.stringify(formData),
  });
}

export default {
  fetchDeviceList,
  fetchStatus,
  fetchTick,
  fetchLogs,
  clearLog,
  fetchState,
  fetchConfiguration,
  postConfiguration,
  login,
  signup,
};
