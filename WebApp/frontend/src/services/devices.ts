
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
    if (deviceId !== null) {
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
    throw new DeviceRequiredException();
  }

  async getState(deviceId: string) {
    if (deviceId !== null) {
      const stateRequest = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/currentState`, {
        method: 'GET',
        headers: {
          Authorization: `Bearer ${window.localStorage.token}`,
        },
      });
      if (stateRequest.status === 200) {
        const response = await stateRequest.json();
        if (response.length > 0) {
          return response[0];
        }
      }
      if (stateRequest.status === 401) {
        throw new UnauthorizedException();
      }
    }
    throw new DeviceRequiredException();
  }

  async getStatesCount(deviceId: string) {
    if (deviceId !== null) {
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
    return 0;
  }

  async getStates(deviceId: string, skip = 0, rows = 10) {
    if (deviceId !== null) {
      const response = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/states?skip=${skip}&rows=${rows}`, {
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
    throw new DeviceRequiredException();
  }

  async getLogsCount(deviceId: string) {
    if (deviceId !== null) {
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
    return 0;
  }

  async getLogs(deviceId: string, skip = 0) {
    if (deviceId !== null) {
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
    throw new DeviceRequiredException();
  }

  async clearLog(deviceId: string, logId: string) {
    if (deviceId !== null) {
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
    throw new DeviceRequiredException();
  }

  async getTimeOfLastFlush(deviceId: string) {
    if (deviceId !== null) {
      const request = await fetch(`http://${window.location.hostname}:4000/devices/${deviceId}/flush`, {
        method: 'GET',
        headers: {
          Authorization: `Bearer ${window.localStorage.token}`,
        },
      });
      if (request.status === 200) {
        const response = await request.json();
        if (response.length > 0) {
          const flushEvent = response[0];
          if (flushEvent.data.success && flushEvent.data.state === 'FLUSH') {
            return flushEvent.datetime;
          } else {
            return null;
          }
        }
      }
      if (request.status === 401) {
        throw new UnauthorizedException();
      }
    } else {
      throw new DeviceRequiredException();
    }
  }

  async getRestarts(deviceId: string) {
    if (deviceId !== null) {
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
    } else {
      throw new DeviceRequiredException();
    }
  }

}

export function useDevicesApi() : DevicesApi {
  return new DevicesApi();
}
