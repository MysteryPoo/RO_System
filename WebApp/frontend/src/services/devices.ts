
class UnauthorizedException {
  code = 401;
  message = "Unauthorized";
}

class DeviceRequiredException {
  code = 422;
  message = "No device provided";
}

class DevicesApi {

  async getDeviceList() {
    const response = await fetch(`http://${window.location.hostname}:4000/deviceList`);
    if (response.status === 200) {
      return response.json();
    }
    return [];
  }

  async getStatus(deviceId: string) {
    if (deviceId === null) {
      throw new DeviceRequiredException();
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/status`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
  }

  async getTicks(deviceId: string, skip = 0, rows = 1) {
    if (deviceId === null) {
      return [];
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/ticks?skip=${skip}&rows=${rows}`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
  }

  async getStatesCount(deviceId: string) {
    if (deviceId === null) {
      return 0;
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/states?count=true`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
  }

  async getStates(deviceId: string, skip = 0, rows = 10, stateFilter = ['IDLE', 'FILL', 'FLUSH']) {
    if (deviceId === null) {
      throw new DeviceRequiredException();
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/states?states=${JSON.stringify(stateFilter)}&skip=${skip}&rows=${rows}`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
  }

  async getLogsCount(deviceId: string) {
    if (deviceId === null) {
      return 0;
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/logs?count=true`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
  }

  async getLogs(deviceId: string, skip = 0) {
    if (deviceId === null) {
      throw new DeviceRequiredException();
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/logs?skip=${skip}`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 200) {
      return response.json();
    }
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
  }

  async clearLog(deviceId: string, logId: string) {
    if (deviceId === null) {
      throw new DeviceRequiredException();
    }
    const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/logs/${logId}`, {
      method: 'DELETE',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (response.status === 401) {
      throw new UnauthorizedException();
    }
    return response;
  }

  async getRestarts(deviceId: string) {
    if (deviceId === null) {
      throw new DeviceRequiredException();
    }
    const request = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/restart`, {
      method: 'GET',
      headers: {
        Authorization: `Bearer ${window.localStorage.token}`,
      },
    });
    if (request.status === 200) {
      return request.json();
    }
    if (request.status === 401) {
      throw new UnauthorizedException();
    }
  }
}

export function useDevicesApi() : DevicesApi {
  return new DevicesApi();
}
